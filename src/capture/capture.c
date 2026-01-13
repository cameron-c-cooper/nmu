#include <arpa/inet.h>
#include <linux/net_tstamp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <time.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "capture/capture.h"
#include "capture/packet.h"

const size_t buffer_size = 2048;

int capture_start(struct capture_ctx* ctx, const char* iface) {
	memset(ctx, 0, sizeof(*ctx));
	ctx -> running = 1;
	ctx -> fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (ctx -> fd < 0)  {
		fprintf(stderr, "Failed to open socket, check permissions");
		return -1;
	}
	int val = SOF_TIMESTAMPING_RX_SOFTWARE |
          SOF_TIMESTAMPING_SOFTWARE |
          SOF_TIMESTAMPING_RAW_HARDWARE;
	setsockopt(ctx -> fd, SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(val));
	int ver = TPACKET_V3;
	setsockopt(ctx -> fd, SOL_PACKET, PACKET_VERSION, &ver, sizeof(ver));
	ctx -> req = (struct tpacket_req3) {
		.tp_block_size = 1 << 20, // 20 MiB
		.tp_frame_size = buffer_size,
		.tp_block_nr = 64,
		.tp_retire_blk_tov = 60, //ms
	};
	ctx -> req.tp_frame_nr = (ctx -> req.tp_block_size * ctx -> req.tp_block_nr) / ctx -> req.tp_frame_size;
	if (ctx -> req.tp_frame_size < TPACKET_ALIGNMENT)
		abort();
	setsockopt(ctx -> fd, SOL_PACKET, PACKET_RX_RING, &ctx -> req, sizeof(ctx -> req));
	ctx -> ring = mmap(
			NULL,
			ctx -> req.tp_block_size * ctx -> req.tp_block_nr,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			ctx -> fd,
			0
	);
	if (ctx -> ring == MAP_FAILED) {
		fprintf(stderr, "mmap call failed.\n");
		return -1;
	}
	struct sockaddr_ll sll = {
		.sll_family 	= AF_PACKET,
		.sll_protocol	= htons(ETH_P_ALL),
		.sll_ifindex	= if_nametoindex(iface)
	};
	if (!sll.sll_ifindex || bind(ctx -> fd, (struct sockaddr *) &sll, sizeof(sll)) < 0) {
		fprintf(stderr, "if_nametoindex or binding socket failed. Potentially invalid interface.\n");
		return -1;
	}
	return 1;
}

int capture_run(struct capture_ctx* ctx, packet_decoder decoder) {
	struct pollfd pfd = {
		.fd = ctx -> fd,
		.events = POLLIN
	};
	uint8_t* base = ctx -> ring;
	uint32_t block_num = 0;
	while (ctx -> running) {
		struct tpacket_block_desc* block =
			(void*)(base + block_num * ctx -> req.tp_block_size);
		if (!(block -> hdr.bh1.block_status & TP_STATUS_USER)) {
			poll(&pfd, 1, 100);
			continue;
		}
		struct tpacket3_hdr* pkt = 
			(void*)((uint8_t *)block +
					block -> hdr.bh1.offset_to_first_pkt);
		for (uint32_t i = 0; i < block -> hdr.bh1.num_pkts; i++) {
			struct packet p = {
				.data = (uint8_t*) pkt + pkt->tp_mac,
				.len = pkt -> tp_snaplen,
				.ts = {
					.tv_sec = pkt -> tp_sec,
					.tv_nsec = pkt -> tp_nsec,
				}
			};
			decoder(&p);
			pkt = (void*)((uint8_t*)pkt + pkt -> tp_next_offset);
		}
		block -> hdr.bh1.block_status = TP_STATUS_KERNEL;
		block_num = (block_num + 1) % ctx -> req.tp_block_nr;
	}
	return 1;
}

void capture_close(struct capture_ctx* ctx) {
	if (ctx -> ring)
		munmap(ctx -> ring, ctx -> ring_len);
	if (ctx -> fd >= 0)
		close(ctx -> fd);
}
