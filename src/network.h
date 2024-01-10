#ifndef __NETWORK_H
#define __NETWORK_H

#include <stdint.h>

extern uint8_t g_LocalMAC[6];
extern uint8_t g_LocalIP[4];

int NET_Init(void);

void NET_ParseMAC(uint8_t* mac, const char* macStr);
void NET_ParseIP(uint8_t* ip, const char* ipStr);

#endif