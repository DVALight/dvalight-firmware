#include "stm32f4xx_hal.h"
#include "uart.h"
#include <stdio.h>

void UART_Hang_Loop()
{
  while (1) {
    __NOP();
  }
}

int main(void)
{
  HAL_Init();
  
  if (UART_Init()) {
    UART_Hang_Loop();
  }

  printf("Hello World!\r\n");

  while (1) {}
}
