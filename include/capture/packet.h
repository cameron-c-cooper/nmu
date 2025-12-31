#pragma once

#include <stdint.h>
#include <stddef.h>
#include <time.h>

struct packet {
	const uint8_t* data; // data from packet should never change.
	size_t len;
	time_t timestamp;
};

typedef void (*packet_handler)(const struct packet *pkt);
int capture_start(const char* iface, packet_handler handler);
