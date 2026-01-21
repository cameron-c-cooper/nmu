#include <string.h>
#include <arpa/inet.h>
#include "decode/arp.h"

static inline uint16_t load_be16(const void *p) {
	const uint8_t* b = (const uint8_t*) p;
	return ((uint16_t) b[0] << 8) | b[1];
}

int decode_arp_payload(const uint8_t *data, size_t len, struct arp_metadata *md) {
	if (len < 28) return -1;
	// md -> htype = ntohs(*(const uint16_t*)(const void*)(data + 0));
	// md -> ptype = ntohs(*(const uint16_t*)(const void*)(data+ 2));
	md -> htype = load_be16(data + 0);
	md -> ptype = load_be16(data + 2);
	md -> hlen = *(data+4);
	md -> plen = *(data+5);
	md -> oper = load_be16(data+6);
	// data get overriden if not copied
	// TODO: Check assembly output from compiler
	memcpy(md -> s_hw_addr, data+8, md -> hlen);
	memcpy(md -> s_proto_addr, data+8 + md -> hlen, md -> plen);
	memcpy(md -> t_hw_addr, data+8 + md -> hlen + md -> plen, md -> hlen);
	memcpy(md -> t_proto_addr, data+8 + 2 * md -> hlen + md -> plen, md -> plen);

	return 1;
}
