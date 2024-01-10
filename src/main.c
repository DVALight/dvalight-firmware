#include "stm32f4xx_hal.h"
#include "uart.h"
#include <stdio.h>

int main(void)
{
  HAL_Init();
  
  UART_Init();

  printf("Hello World!\r\n");

  while (1) {}
}
