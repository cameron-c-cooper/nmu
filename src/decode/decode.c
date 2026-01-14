#include <stdio.h>
#include "decode/decode.h"
#include "capture/packet.h"
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
