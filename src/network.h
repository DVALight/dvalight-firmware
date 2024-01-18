#ifndef __NETWORK_H
#define __NETWORK_H

#include "net.h"
#include "ip_arp_udp_tcp.h"
#include <stdint.h>

void NET_StringToMAC(uint8_t* mac, const char* macStr);
void NET_StringToIP(uint8_t* ip, const char* ipStr);

const char* NET_MACToString(const uint8_t* mac);
const char* NET_IPToString(const uint8_t* ip);

int NET_Init(void);

extern uint8_t NET[DVA_NETBUF_SIZE];
#define NET_UDP               (&NET[UDP_DATA_P])

#define NET_PROTO_IS(proto)   (NET[IP_PROTO_P] == (proto))
#define NET_ARP_IS_REQUEST()  (NET[ETH_ARP_OPCODE_L_P] == ETH_ARP_OPCODE_REQ_L_V)
#define NET_ARP_IS_REPLY()    (NET[ETH_ARP_OPCODE_L_P] == ETH_ARP_OPCODE_REPLY_L_V)

extern uint16_t g_LocalDPort, g_RemoteSPort;

#define NET_ReceivePacket() ES_enc28j60PacketReceive(DVA_NETBUF_SIZE, NET)
void NET_SendUDP(uint16_t len);
void NET_ReplyUDP(uint16_t len);

// returns udp data length if its udp packet
uint16_t NET_PacketLoop();
uint8_t NET_IsReady();

#endif