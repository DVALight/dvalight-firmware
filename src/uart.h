#ifndef __UART_H
#define __UART_H

#include <stdint.h>

int UART_Init(void);
void UART_HexDump(const uint8_t* data, uint16_t len, unsigned cols);

#endif