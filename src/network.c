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
uint8_t g_RemoteMAC[6], g_RemoteIP[4];

void NET_ParseMAC(uint8_t* mac, const char* macStr)
{
  sscanf(macStr, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
    &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
}

void NET_ParseIP(uint8_t* ip, const char* ipStr)
{
  sscanf(ipStr, "%hhd.%hhd.%hhd.%hhd",
    &ip[0], &ip[1], &ip[2], &ip[3]);
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

  NET_ParseMAC(g_LocalMAC, LOCAL_MAC_STR);
  NET_ParseIP(g_LocalIP, LOCAL_IP_STR);

  NET_ParseMAC(g_RemoteMAC, REMOTE_MAC_STR);
  NET_ParseIP(g_RemoteIP, REMOTE_IP_STR);

  ES_enc28j60SpiInit(&hspi1);
  ES_enc28j60Init(g_LocalMAC);

  ES_init_ip_arp_udp_tcp(g_LocalMAC, g_LocalIP, 80);

  uint8_t gwip[4];
  NET_ParseIP(gwip, "192.168.100.1");
  client_set_gwip(gwip);

  return 0;
}

uint8_t g_NetBuf[DVA_NETBUF_SIZE];

void NET_SendUDP(uint8_t* ip, uint16_t sport, uint16_t dport,
  const uint8_t* payload, unsigned size)
{
  memcpy(&g_NetBuf[UDP_DATA_P], payload, size);

  ES_send_udp_data2(g_NetBuf, g_RemoteMAC, size, sport, ip, dport);
}
