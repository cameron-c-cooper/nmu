#include <arpa/inet.h>
#include <linux/net_tstamp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <time.h>
#include <poll.h>
#include <string.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "capture/packet.h"

const size_t buffer_size = 2048;

int capture_start(const char *iface, packet_handler handler) {
	int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (fd < 0)  {
		fprintf(stderr, "Failed to open socket, check permissions");
		return -1;
	}
	int val = SOF_TIMESTAMPING_RX_SOFTWARE |
          SOF_TIMESTAMPING_SOFTWARE |
          SOF_TIMESTAMPING_RAW_HARDWARE;
	setsockopt(fd, SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(val));
	int ver = TPACKET_V3;
	setsockopt(fd, SOL_PACKET, PACKET_VERSION, &ver, sizeof(ver));
	struct tpacket_req3 req = {
		.tp_block_size = 1 << 20, // 20 MiB
		.tp_frame_size = buffer_size,
		.tp_block_nr = 64,
		.tp_retire_blk_tov = 60, //ms
	};
	req.tp_frame_nr = (req.tp_block_size * req.tp_block_nr) / req.tp_frame_size;
	if (req.tp_frame_size < TPACKET_ALIGNMENT)
		abort();
	setsockopt(fd, SOL_PACKET, PACKET_RX_RING, &req, sizeof(req));
	void* ring = mmap(
			NULL,
			req.tp_block_size * req.tp_block_nr,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd,
			0
	);
	if (ring == MAP_FAILED) {
		fprintf(stderr, "mmap call failed.\n");
		return -1;
	}
	struct sockaddr_ll sll = {
		.sll_family 	= AF_PACKET,
		.sll_protocol	= htons(ETH_P_ALL),
		.sll_ifindex	= if_nametoindex(iface)
	};
	if (sll.sll_ifindex == 0) {
		munmap(ring, req.tp_block_size * req.tp_block_nr);
		fprintf(stderr, "if_nametoindex failed. Potentially invalid interface.\n");
		return -1;
	}
	if (bind(fd, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
		munmap(ring, req.tp_block_size * req.tp_block_nr);
		fprintf(stderr, "Binding the socket failed.\n");
		return -1;
	}
	struct pollfd pfd = {
		.fd = fd,
		.events = POLLIN
	};
	struct tpacket_block_desc* block;
	uint8_t* base = ring;
	uint32_t block_num = 0;
	for(;;) {
		// black magic double cast bc fuck this shit
		block = (struct tpacket_block_desc*)(void*)
			(base + (block_num * req.tp_block_size));
		_Static_assert(alignof(struct tpacket_block_desc) <= 8,
               "Unexpected alignment requirement");
		if (!(block -> hdr.bh1.block_status & TP_STATUS_USER)) {
			poll(&pfd, 1, -1);
			continue;
		}
		struct tpacket3_hdr* pkt = (struct tpacket3_hdr*)(void*)
			((uint8_t*) block + block->hdr.bh1.offset_to_first_pkt);
		for (uint32_t i = 0; i < block->hdr.bh1.num_pkts; i++) {
			uint8_t* data = (uint8_t*) pkt + pkt -> tp_mac;
			uint32_t len = pkt -> tp_snaplen;
			struct packet p = {
				.data = data,
				.len = len,
				.ts = {
					.tv_sec = pkt -> tp_sec,
					.tv_nsec= pkt -> tp_nsec,
				}
			};
			handler(&p);
			pkt = (struct tpacket3_hdr*)(void*)
				((uint8_t*) pkt + pkt->tp_next_offset);
		}
		block -> hdr.bh1.block_status = TP_STATUS_KERNEL;
		block_num = (block_num + 1) % req.tp_block_nr;
	}
	return 1;
}
