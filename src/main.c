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

volatile uint8_t g_PktIF = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_1)
  {
    enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIE, EIE_INTIE);

    uint8_t eir = enc28j60ReadOp(ENC28J60_READ_CTRL_REG, EIR);
    if (eir & EIR_PKTIF)
    {
      g_PktIF = 1;
      enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_PKTIF);
    }

    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE);
  }
}

int main(void)
{
  HAL_Init();
  
  if (UART_Init())
  {
    Failure_Hang_Loop();
  }

  // EXTI1 line
  HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef) {
    .Pin = GPIO_PIN_1,
    .Mode = GPIO_MODE_IT_FALLING,
    .Pull = GPIO_NOPULL
  });
  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

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
    if (g_PktIF)
    {
      printf("g_PktIf\r\n");

      while (enc28j60Read(EPKTCNT)) // process all packets
      {
        uint16_t pos = packetloop_icmp_tcp(g_NetBuf,
          ES_enc28j60PacketReceive(sizeof(g_NetBuf), g_NetBuf));
        printf("pos %u\r\n", pos);
      }
      
      g_PktIF = 0;
    }
  }
}
