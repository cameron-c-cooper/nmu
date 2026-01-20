#include <stdio.h>
#include <errno.h>
#include "decode/decode.h"
#include "capture/packet.h"
#include "decode/arp.h"
#include "decode/eth.h"

int decode_packet(struct packet_view* pv, struct packet_metadata* pmd) {
	if (decode_ethernet(pv, &pmd -> eth_data) < 0) {
		fprintf(stderr, "Failed to decode ethernet frame");
		return -1;
	}
	char buffer[1024];
	ethernet_fmt(buffer, sizeof(buffer), &pmd -> eth_data);
	printf("Ethernet Metadata from packet: %s\n",buffer);
	return 1;
}

int decode_payload(
		enum ethertype et,
		const uint8_t *data,
		size_t len, 
		struct ethernet_payload_metadata *eth_md
) {
	switch (et) {
	case ARP:
		return decode_arp_payload(data, len, &eth_md -> arpmd);
	default:
		return -ENOTSUP;
	}
}
