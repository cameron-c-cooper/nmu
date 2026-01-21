#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

// This is the most common arp ipv4 setup
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
	uint16_t htype;
	uint16_t ptype;
	uint8_t hlen;
	uint8_t plen;
	uint16_t oper;
	uint8_t s_hw_addr[6];
	uint8_t s_proto_addr[4];
	uint8_t t_hw_addr[6];
	uint8_t t_proto_addr[4];
};

int decode_arp_payload(
		const uint8_t* data,
		size_t len,
		struct arp_metadata* md
);

static inline int arp_fmt(char* buf, size_t buflen, const struct arp_metadata* amd) {
	return snprintf(buf, buflen,
			// should already know addresses
			"htype=0x%04x ptype=0x%04x hlen=%d plen=%d oper=0x%04x",
			amd -> htype, amd -> ptype, amd -> hlen, amd -> plen, amd -> oper
			);
}
