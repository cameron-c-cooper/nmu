#pragma once
#include "decode/eth.h"

struct packet_metadata {
	struct ethernet_metadata eth_data;
};

int decode_packet(struct packet_view* pv, struct packet_metadata* pmd);
