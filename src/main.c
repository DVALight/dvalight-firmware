#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "network.h"
#include "uart.h"
#include "enc28j60.h"
#include "EtherShield.h"
#include "ip_arp_udp_tcp.h"

void Failure_Hang_Loop(void)
{
  while (1) {
    __NOP();
  }
}

uint16_t packetloop(uint8_t* buf, uint16_t plen, uint16_t* dlen)
{
  if (eth_type_is_arp_and_my_ip(buf, plen))
  {
    if (buf[ETH_ARP_OPCODE_L_P] == ETH_ARP_OPCODE_REQ_L_V)
    {
      make_arp_answer_from_request(buf);
    }
  }

  if (eth_type_is_ip_and_my_ip(buf, plen))
  {
    // ICMP
    if (buf[IP_PROTO_P] == IP_PROTO_ICMP_V 
      && buf[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V)
    {
      make_echo_reply_from_request(buf, plen);
    }
    else if (buf[IP_PROTO_P] == IP_PROTO_UDP_V)
    {
      *dlen = get_udp_data_len(buf);
      return IP_HEADER_LEN + 8 + 14;
    }
  }

  return 0;
}

int main(void)
{
  HAL_Init();
  
  if (UART_Init())
  {
    Failure_Hang_Loop();
  }

  if (NET_Init())
  {
    printf("Failed to initialize network\r\n");
    Failure_Hang_Loop();
  }

  printf("Init OK\r\n");

  while (1)
  {
    uint16_t len = 0;
    uint16_t pos = packetloop(g_NetBuf,
      ES_enc28j60PacketReceive(sizeof(g_NetBuf), g_NetBuf), &len);
    if (!pos) continue;
    printf("udp pos %u len %u\r\n", pos, len);
    UART_HexDump(&g_NetBuf[pos], len, 8);
  }
}
