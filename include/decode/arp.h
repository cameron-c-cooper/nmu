#pragma once
#include "decode/eth.h"
#include <stdint.h>

struct arp_metadata {
	/* 
	 * TODO: Decide if this struct should contain the dst/src values. It
	 * *technically* is not located in the arp packet, but A) dst and src
	 * values are very important to the arp protocol for device discovery.
	 * Besides, the arp_metadata object should only exist as long as the
	 * frames metadata. This means that the dst and src values should just
	 * be set to the pointer of the src and dst values within the parent
	 * ethe
	 */

	uint16_t dst[6];
	uint16_t src[6];
	uint16_t prototype;
};

int decode_arp_payload(const uint8_t* data, size_t len, struct arp_metadata* md, struct ethernet_metadata* m_md);
