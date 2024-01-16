#include "network.h"
#include "stm32f4xx_hal.h"
#include "EtherShield.h"
#include <string.h>

SPI_HandleTypeDef hspi1 = {
  .Instance = SPI1,
  .Init.Mode = SPI_MODE_MASTER,
  .Init.Direction = SPI_DIRECTION_2LINES,
  .Init.DataSize = SPI_DATASIZE_8BIT,
  .Init.CLKPolarity = SPI_POLARITY_LOW,
  .Init.CLKPhase = SPI_PHASE_1EDGE,
  .Init.NSS = SPI_NSS_SOFT,
  .Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64,
  .Init.FirstBit = SPI_FIRSTBIT_MSB,
  .Init.TIMode = SPI_TIMODE_DISABLE,
  .Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE
};

uint8_t g_LocalMAC[6], g_LocalIP[4];
uint8_t g_GatewayMAC[6], g_GatewayIP[4];

uint8_t g_RemoteIP[4];

void NET_StringToMAC(uint8_t* mac, const char* macStr)
{
  sscanf(macStr, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
    &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
}

void NET_StringToIP(uint8_t* ip, const char* ipStr)
{
  sscanf(ipStr, "%hhd.%hhd.%hhd.%hhd",
    &ip[0], &ip[1], &ip[2], &ip[3]);
}

const char* NET_MACToString(const uint8_t* mac)
{
  //00:00:00:00:00:00.
  static char macStr[18];
  sprintf(macStr, "%02x:%02x:%02x:%02x:%02x:%02x",
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return macStr;
}

const char* NET_IPToString(const uint8_t* ip)
{
  //000.000.000.000.
  static char ipStr[16];
  sprintf(ipStr, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
  return ipStr;
}

void ES_PingCallback(void)
{
  printf("ES_PingCallback\r\n");
}

int NET_Init(void)
{
  HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef) {
    .Pin = ETHERNET_CS_PIN | ETHERNET_LED_PIN,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Speed = GPIO_SPEED_LOW
  });

  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    return -1;
  }

  NET_StringToMAC(g_LocalMAC, DVA_LOCAL_MAC);
  NET_StringToIP(g_LocalIP, DVA_LOCAL_IP);

  NET_StringToIP(g_GatewayIP, DVA_GATEWAY_IP);
  NET_StringToIP(g_RemoteIP, DVA_REMOTE_IP);

  ES_enc28j60SpiInit(&hspi1);
  ES_enc28j60Init(g_LocalMAC);

  ES_init_ip_arp_udp_tcp(g_LocalMAC, g_LocalIP, 80);

  printf("flushing: ");
  while (NET_ReceivePacket())
  {
    printf(".");
  }
  printf(" - done!\r\n");

  return 0;
}

uint8_t NET[DVA_NETBUF_SIZE];

void NET_SendUDP(uint16_t len)
{
  ES_send_udp_data2(NET, g_GatewayMAC, len, DVA_SPORT, g_RemoteIP, DVA_DPORT);
}

// 1. get gateway mac
// 2. 

static enum {
  NET_INIT,
  NET_GATEWAY_ARP_WAITING,
  NET_READY,
} NET_State = NET_INIT;

// returns udp data length if its udp packet
uint16_t NET_PacketLoop()
{
  uint16_t plen = NET_ReceivePacket();
  if (!plen)
  {
    if (NET_State == NET_INIT)
    {
      client_arp_whohas(NET, g_GatewayIP);
      NET_State = NET_GATEWAY_ARP_WAITING;
    }
  }

  // ARP
  if (eth_type_is_arp_and_my_ip(NET, plen))
  {
    if (NET_ARP_IS_REQUEST())
    {
      printf("[ARP] request\r\n");
      make_arp_answer_from_request(NET);
    }
    else if (NET_ARP_IS_REPLY())
    {
      printf("[ARP] reply\r\n");
      if (NET_State == NET_GATEWAY_ARP_WAITING && !memcmp(&NET[ETH_ARP_SRC_IP_P], g_GatewayIP, 4))
      {
        memcpy(g_GatewayMAC, &NET[ETH_ARP_SRC_MAC_P], 6);
        printf("gateway - %s\r\n", NET_MACToString(g_GatewayMAC));
        NET_State = NET_READY;
      }
    }
  }

  // IP
  if (eth_type_is_ip_and_my_ip(NET, plen))
  {
    // ICMP
    if (NET_PROTO_IS(IP_PROTO_ICMP_V) && NET[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V)
    {
      ES_PingCallback();
      make_echo_reply_from_request(NET, plen);
    }
    // UDP
    else if (NET_PROTO_IS(IP_PROTO_UDP_V))
    {
      if (NET_State == NET_READY)
      {
        return get_udp_data_len(NET);
      }
    }
  }

  return 0;
}

uint8_t NET_IsReady()
{
  return NET_State == NET_READY;
}
