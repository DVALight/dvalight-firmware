#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "network.h"
#include "uart.h"
#include "EtherShield.h"

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

  while (1)
  {
    //NET_SendUDP(g_RemoteIP, 61337, 1337, (const uint8_t*)"Hello World!\r\n", 15);
    //HAL_Delay(500);
    uint16_t len = ES_enc28j60PacketReceive(sizeof(g_NetBuf), g_NetBuf);
    if (!len) continue;

    printf("len %u\r\n", len);
    UART_HexDump(g_NetBuf, len, 16);
  }
}
