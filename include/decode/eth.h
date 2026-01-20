#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "capture/packet.h"
#include "decode/arp.h"

enum ethertype : uint16_t {
	IPv4 = 0x0800,
	Chaosnet = 0x0804,
	ARP = 0x0806,
	WoL = 0x0842,
	SRP = 0x22ea,
	AVTP = 0x22f0,
	IETFTRILL = 0x22F3,
	DECMOPRC = 0x6002,
	DECnetP4DNAR = 0x6003,
	DECLAT = 0x6004,
	RARP = 0x8035,
	EtherTalk = 0x809b,
	LLCPDU = 0x80d5, // preceeded by 2 bytes length and 1 byte padding
	AARP = 0x80f3,
	VLAN_TAG = 0x8100,
	SLPP = 0x8102,
	VLACP = 0x8103,
	IPX = 0x8137,
	QNXQnet = 0x8204,
	IPv6 = 0x86dd,
	EFC = 0x8808,
	ESP = 0x8809,
	COBRANET = 0x8819,
	MPLS_UNI = 0x8847,
	MPLS_MULTI = 0x8848,
	PPPoEDISC = 0x8863,
	PPPoESESS = 0x8864,
	HP10MME = 0x887b,
	EAPoLAN = 0x888e,
	PROFINET = 0x8892,
	HYPERSCSI = 0x889a,
	ATAoE = 0x88a2,
	ETHERCAT = 0x88a4,
	VLAN_DUP = 0x88a8,
	ETHERNET_POWERLINK = 0x88ab,
	GOOSE = 0x88b8,
	GSE = 0x88b9,
	SV = 0x88ba,
	MIKTROTIKRoMON = 0x88bf,
	LLDP = 0x88cc,
	SERCOSIII = 0x88cd,
	HOMEPLUGGREENPHY = 0x88e1,
	MRP = 0x88e3,
	MACSEC = 0x88e5,
	PBB = 0x88e7,
	PIP = 0x88f7,
	NCSI = 0x88f8,
	PRP = 0x88fb,
	CFM = 0x8902,
	FCoE = 0x8906,
	FCoE_INIT = 0x8914,
	RoCE = 0x8915,
	TTE = 0x891d,
	IEEP1905 = 0x893a,
	HSR = 0x892f,
	ECTP = 0x9000,
	REDUNDANCY_TAG = 0xf1c1
};

struct ethernet_metadata {
	enum ethertype et;
	uint8_t mac_dst[6];
	uint8_t mac_src[6];
};

/* I really fucking hate that this include cant be at the top */
struct ethernet_payload_metadata {
	enum ethertype et;
	union {
		struct arp_metadata arpmd;
		/* IPv4, ARP, etc */
	};
};

int decode_ethernet(struct packet_view* pv, struct ethernet_metadata *out);

static inline int ethernet_fmt(char* buf, size_t buflen, const struct ethernet_metadata* emd) {
	return snprintf(buf, buflen,
			"ethertype=0x%04x dst=%02x:%02x:%02x:%02x:%02x:%02x src=%02x:%02x:%02x:%02x:%02x:%02x",
			emd -> et,
			emd -> mac_dst[0], emd -> mac_dst[1], emd -> mac_dst[2],
			emd -> mac_dst[3], emd -> mac_dst[4], emd -> mac_dst[5],
			emd -> mac_src[0], emd -> mac_src[1], emd -> mac_src[2],
			emd -> mac_src[3], emd -> mac_src[4], emd -> mac_src[5]
			);
}
