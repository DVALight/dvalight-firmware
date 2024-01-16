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

// returns udp data length if its udp packet
uint16_t packetloop()
{
  uint16_t plen = NET_ReceivePacket();
  if (eth_type_is_arp_and_my_ip(NET_BUF, plen))
  {
    if (NET_BUF[ETH_ARP_OPCODE_L_P] == ETH_ARP_OPCODE_REQ_L_V)
    {
      make_arp_answer_from_request(NET_BUF);
    }
  }

  if (eth_type_is_ip_and_my_ip(NET_BUF, plen))
  {
    // ICMP
    if (NET_BUF[IP_PROTO_P] == IP_PROTO_ICMP_V 
      && NET_BUF[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V)
    {
      make_echo_reply_from_request(NET_BUF, plen);
    }
    else if (NET_BUF[IP_PROTO_P] == IP_PROTO_UDP_V)
    {
      return get_udp_data_len(NET_BUF);
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

  // our goals for this firmware
  // 1. send UDP request server
  // 2. wait for response
  // 3. process response, perform actions described in response
  // 4. repeat

  NET_SendUDP(g_RemoteIP, DVA_SPORT, DVA_DPORT, (const uint8_t*)"Hello World!\r\n", 15);

  while (1)
  {
    uint16_t len = packetloop();
    if (!len) continue;

    printf("len %u\r\n", len);
  }
}
