#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "network.h"
#include "uart.h"
#include "EtherShield.h"
#include "ip_arp_udp_tcp.h"

void Failure_Hang_Loop()
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

  uint32_t lastSendTick = HAL_GetTick();
  const uint32_t sendDelayTicks = 1000;

  while (1)
  {
    if (HAL_GetTick() > lastSendTick + sendDelayTicks)
    {
      NET_SendUDP(g_RemoteIP, 61337, 1337, (uint8_t*)"Hello World!\r\n", 15);
      lastSendTick = HAL_GetTick();
    }

    uint16_t pos = packetloop_icmp_tcp(g_NetBuf,
      ES_enc28j60PacketReceive(sizeof(g_NetBuf), g_NetBuf));
    if (!pos) continue;
    printf("pos %u\n", pos);
  }
}
