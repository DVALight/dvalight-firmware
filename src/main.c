#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "network.h"
#include "uart.h"
#include "dva.h"

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

  // setup LED
  __HAL_RCC_GPIOD_CLK_ENABLE();
  HAL_GPIO_Init(GPIOD, &(GPIO_InitTypeDef) {
    .Pin = GPIO_PIN_0,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_PULLUP,
    .Speed = GPIO_SPEED_LOW
  });

  uint32_t lastDVARequest = HAL_GetTick();
  while (1)
  {
    uint16_t len = NET_PacketLoop();
    if (len)
    {
      struct DVAResponse* res = DVA_ReadResponse();
      if (!res) continue;
      
      printf("DVA response: state %u\r\n", res->state);

      // set LED according to state
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0,
        res->state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
    else if (NET_IsReady())
    {
      uint32_t tick = HAL_GetTick();
      if (tick > lastDVARequest + DVA_DELAY)
      {
        DVA_MakeRequest();
        lastDVARequest = tick;
      }
    }
  }
}
