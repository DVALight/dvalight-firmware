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

  // first loop iteration to init everything up
  packetloop_icmp_tcp(g_NetBuf, 0);

  printf("Init OK\r\n");

  while (1)
  {
    uint16_t pos = packetloop_icmp_tcp(g_NetBuf,
      ES_enc28j60PacketReceive(sizeof(g_NetBuf), g_NetBuf));
    if (!pos) continue;
    printf("pos %u\r\n", pos);
  }
}
