// get eth type
// handle VLAN stacking
// find L3 start offset
// verify length

#include <netinet/in.h>
#include <string.h>
#include <stdint.h>
#include "decode/eth.h"

int decode_eth(struct packet_view *pv, struct eth_info *out) {
	if (!pv_has(pv, 14)) return -1;
	const uint8_t* p = pv_ptr(pv);
	uint16_t ethertype;
	memcpy(&ethertype, p + 12, sizeof(ethertype));
	ethertype = ntohs(ethertype);
	size_t offset = 14;

	// TODO: make these macros or constants
	while (ethertype == 0x8100 || ethertype == 0x88a8) {
		if (!pv_has(pv, offset + 4)) return -1;
		memcpy(&ethertype, p + offset + 2, sizeof(ethertype));
		ethertype = ntohs(ethertype);
		offset += 4;
	}

	out -> ethertype = ethertype;
	out -> l3_offset = offset;
	return 1;
}
