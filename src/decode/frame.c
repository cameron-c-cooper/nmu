// get eth type
// handle VLAN stacking
// find L3 start offset
// verify length

#include <netinet/in.h>
#include <string.h>
#include <stdint.h>
#include "capture/packet.h"
#include "decode/eth.h"

int decode_ethernet(struct packet_view *pv, struct ethernet_metadata *out) {
	// validate length
	if (!pv_has(pv, 14)) return -1;
	// obtains pointer to the data
	const uint8_t* p = pv_ptr(pv);
	memcpy(out -> mac_dst, p, sizeof(out -> mac_dst));
	memcpy(out -> mac_src, p + 6, sizeof(out -> mac_src));
	uint16_t ethertype;
	memcpy(&ethertype, p + 12, sizeof(ethertype));
	ethertype = ntohs(ethertype);
	size_t offset = 14;
	// handles both vlan tags
	while (ethertype == 0x8100 || ethertype == 0x88a8) {
		if (!pv_has(pv, offset + 4)) return -1;
		memcpy(&ethertype, p + offset + 2, sizeof(ethertype));
		ethertype = ntohs(ethertype);
		// offset now exists marks proper offset into the packet
		offset += 4;
	}
	out -> et = ethertype;
	pv_advance(pv, offset);
	return 1;
}
