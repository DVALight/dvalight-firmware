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

TIM_HandleTypeDef htim2 = {
  .Instance = TIM2,
  .Init.Prescaler = 0,
  .Init.CounterMode = TIM_COUNTERMODE_UP,
  .Init.Period = 4294967295,
  .Init.ClockDivision = TIM_CLOCKDIVISION_DIV1,
  .Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE
};

extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);

void PWM_Init()
{
  // PWM
  HAL_TIM_PWM_Init(&htim2);
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &(TIM_MasterConfigTypeDef) {
    .MasterOutputTrigger = TIM_TRGO_RESET,
    .MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE
  });
  
  const TIM_OC_InitTypeDef ocConfig = {
    .OCMode = TIM_OCMODE_PWM1,
    .Pulse = 0,
    .OCPolarity = TIM_OCPOLARITY_HIGH,
    .OCFastMode = TIM_OCFAST_DISABLE
  };
  HAL_TIM_PWM_ConfigChannel(&htim2, &ocConfig, TIM_CHANNEL_1); // PA0 - TIM2_CH1
  HAL_TIM_PWM_ConfigChannel(&htim2, &ocConfig, TIM_CHANNEL_2); // PB3 - TIM2_CH2
  HAL_TIM_PWM_ConfigChannel(&htim2, &ocConfig, TIM_CHANNEL_3); // PA2 - TIM2_CH3

  HAL_TIM_MspPostInit(&htim2);

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
}

void RGB_Write(uint16_t r, uint16_t g, uint16_t b)
{
  TIM2->CCR1 = r << 8;
  TIM2->CCR2 = g << 8;
  TIM2->CCR3 = b << 8;
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

  PWM_Init();
  RGB_Write(0, 0, 0);


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
      if (res->state)
      {
        RGB_Write((res->color >> 16) & 0xFF, (res->color >> 8) & 0xFF, res->color & 0xFF);  
      }
      else
      {
        RGB_Write(0, 0, 0);
      }
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
