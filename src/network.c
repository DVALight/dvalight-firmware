#include "network.h"
#include "stm32f4xx_hal.h"

SPI_HandleTypeDef hspi1 = {
  .Instance = SPI1,
  .Init.Mode = SPI_MODE_MASTER,
  .Init.Direction = SPI_DIRECTION_2LINES,
  .Init.DataSize = SPI_DATASIZE_8BIT,
  .Init.CLKPolarity = SPI_POLARITY_LOW,
  .Init.CLKPhase = SPI_PHASE_1EDGE,
  .Init.NSS = SPI_NSS_SOFT,
  .Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64,
  .Init.FirstBit = SPI_FIRSTBIT_MSB,
  .Init.TIMode = SPI_TIMODE_DISABLE,
  .Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE
};

uint8_t g_LocalMAC[6];
uint8_t g_LocalIP[4];

int NET_Init(void)
{
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
    return -1;

  return 0;
}

void NET_ParseMAC(uint8_t* mac, const char* macStr)
{
  sscanf(macStr, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
    &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
}

void NET_ParseIP(uint8_t* ip, const char* ipStr)
{
  sscanf(ipStr, "%hhd.%hhd.%hhd.%hhd",
    &ip[0], &ip[1], &ip[2], &ip[3]);
}
