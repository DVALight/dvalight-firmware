#ifndef __NETWORK_H
#define __NETWORK_H

#include <stdint.h>

extern uint8_t g_LocalMAC[6], g_LocalIP[4];
extern uint8_t g_RemoteMAC[6], g_RemoteIP[4];

void NET_ParseMAC(uint8_t* mac, const char* macStr);
void NET_ParseIP(uint8_t* ip, const char* ipStr);

int NET_Init(void);

extern uint8_t g_NetBuf[DVA_NETBUF_SIZE];

void NET_SendUDP(uint8_t* ip, uint16_t sport, uint16_t dport,
  const uint8_t* payload, unsigned size);

#endif