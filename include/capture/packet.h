#pragma once

#include <stdint.h>
#include <stddef.h>
#include <time.h>

struct packet {
	const uint8_t* data; // data from packet should never change.
	size_t len;
	struct timespec ts;
};

struct packet_view {
	const uint8_t* data;
	size_t len;
	size_t offset;
};

typedef void (*packet_handler)(const struct packet *pkt);

static inline int pv_has(const struct packet_view* pv, size_t n) {
	return pv -> offset + n <= pv -> len;
}

static inline const void* pv_ptr(const struct packet_view *pv) {
	return pv -> data + pv -> offset;
}

static inline void pv_advance(struct packet_view *pv, size_t n) {
	pv -> offset += n;
}
