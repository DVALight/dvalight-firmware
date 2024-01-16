#ifndef __NETWORK_H
#define __NETWORK_H

#include <stdint.h>
#include "ip_arp_udp_tcp.h"

extern uint8_t g_LocalMAC[6], g_LocalIP[4];
extern uint8_t g_RemoteMAC[6], g_RemoteIP[4];

void NET_ParseMAC(uint8_t* mac, const char* macStr);
void NET_ParseIP(uint8_t* ip, const char* ipStr);

int NET_Init(void);

extern uint8_t NET[DVA_NETBUF_SIZE];
#define NET_UDP               (&NET[UDP_DATA_P])

#define NET_PROTO_IS(proto)   (NET[IP_PROTO_P] == (proto))
#define NET_ARP_IS_REQUEST()  (NET[ETH_ARP_OPCODE_L_P] == ETH_ARP_OPCODE_REQ_L_V)

#define NET_ReceivePacket() ES_enc28j60PacketReceive(DVA_NETBUF_SIZE, NET)
void NET_SendUDP(uint8_t* ip, uint16_t sport, uint16_t dport,
  const uint8_t* payload, unsigned size);

#endif