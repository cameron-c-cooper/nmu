#pragma once

#include <stdint.h>
#include <stddef.h>
#include "capture/packet.h"

struct eth_info {
	uint16_t ethertype;
	size_t l3_offset;
};

int decode_eth(struct packet_view* pv, struct eth_info *out);
