#pragma once

#include <stddef.h>
#include <linux/if_packet.h>
#include <signal.h>

#include "capture/packet.h"

struct capture_ctx {
	int fd;
	void* ring;
	size_t ring_len;

	struct tpacket_req3 req;
	volatile sig_atomic_t running;
};

int capture_start(struct capture_ctx* ctx, const char* iface);
int capture_run(struct capture_ctx* ctx, packet_decoder decoder);
void capture_close(struct capture_ctx* ctx);
