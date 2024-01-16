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
    uint16_t len = NET_PacketLoop();
    if (!len) continue;

    printf("len %u\r\n", len);
  }
}
