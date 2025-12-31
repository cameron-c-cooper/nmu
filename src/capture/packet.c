#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <time.h>
#include <string.h>
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
	struct sockaddr_ll sll = {
		.sll_family 	= AF_PACKET,
		.sll_protocol	= htons(ETH_P_ALL),
		.sll_ifindex	= if_nametoindex(iface)
	};
	if (bind(fd, (struct sockaddr *)&sll, sizeof(sll)) < 0)
		return -1;
	while (1) {
		uint8_t buf[buffer_size];
		// ssize_t is used to also indicate the potential presence of an error
		ssize_t n = recv(fd, buf, sizeof(buf), 0);
		if (n <= 0) continue;
		uint8_t* copy = (uint8_t *) malloc(n * sizeof(uint8_t));
		memcpy(copy, buf, n);
		struct packet pkt = {
			.data = copy,
			.len = (size_t) n,
			.timestamp = time(NULL)
		};
		// handler must free the copy
		handler(&pkt);
		free(copy);
	}
	return 1;
}
