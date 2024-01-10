#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "network.h"
#include "uart.h"

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

  printf("Hello World!\r\n");

  while (1) {}
}
