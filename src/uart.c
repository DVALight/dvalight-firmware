#include "uart.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

UART_HandleTypeDef huart1 = {
  .Instance = USART1,
  .Init.BaudRate = 115200,
  .Init.WordLength = UART_WORDLENGTH_8B,
  .Init.StopBits = UART_STOPBITS_1,
  .Init.Parity = UART_PARITY_NONE,
  .Init.Mode = UART_MODE_TX_RX,
  .Init.HwFlowCtl = UART_HWCONTROL_NONE,
  .Init.OverSampling = UART_OVERSAMPLING_16
};

// stdout/stdin syscalls for UART
int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart1, (const uint8_t*)&ch, 1, -1);
  return ch;
}

int __io_getchar(void)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, -1);
  return ch;
}

int UART_Init(void)
{
  return HAL_UART_Init(&huart1) == HAL_OK ? 0 : -1;
}

void UART_HexDump(const uint8_t* data, uint16_t len, unsigned cols)
{
  for (uint16_t i = 0; i < len; i++)
  {
    if (i && i % cols == 0)
    {
      printf("\r\n");
    }

    printf("%02x ", data[i]);
  }
  printf("\r\n");
}
