#pragma once
#include "decode/eth.h"

struct packet_metadata {
	struct ethernet_metadata eth_data;
};

int decode_packet(struct packet_view* pv, struct packet_metadata* pmd);

int decode_payload(
		enum ethertype et,
		const uint8_t* data,
		size_t len,
		struct ethernet_payload_metadata* eth_md
);
