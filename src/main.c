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

void EXTI1_IRQHandler(void)
{
  printf("EXTI1_IRQHandler\r\n");

  // disable enc interrupts
  enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIE, EIE_INTIE);

  uint8_t eir = enc28j60ReadOp(ENC28J60_READ_CTRL_REG, EIR);
  if (eir & EIR_PKTIF)
  {
    printf("EXTI1: EIR_PKTIF - packet received\r\n");
    printf("EPKTCNT: %u\r\n", enc28j60Read(EPKTCNT));
  }

  // enable enc interrupts
  enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE);
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

  // EXTI1 line
  HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef) {
    .Pin = GPIO_PIN_1,
    .Mode = GPIO_MODE_IT_FALLING,
    .Pull = GPIO_NOPULL
  });
  // BUG: such priority could shadow SysTick interrupt causing SPI HAL not working
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

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
