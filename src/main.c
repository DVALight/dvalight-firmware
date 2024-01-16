#include <stdio.h>
#include <string.h>
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
  if (eth_type_is_arp_and_my_ip(NET, plen))
  {
    if (NET_ARP_IS_REQUEST())
    {
      make_arp_answer_from_request(NET);
    }
  }

  if (eth_type_is_ip_and_my_ip(NET, plen))
  {
    // ICMP
    if (NET_PROTO_IS(IP_PROTO_ICMP_V) && NET[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V)
    {
      make_echo_reply_from_request(NET, plen);
    }
    else if (NET_PROTO_IS(IP_PROTO_UDP_V))
    {
      return get_udp_data_len(NET);
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

  memcpy(NET_UDP, "Hello World!\r\n", 15);
  NET_SendUDP(15);

  while (1)
  {
    uint16_t len = packetloop();
    if (!len) continue;

    printf("len %u\r\n", len);
  }
}
