#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>

#include "capture/packet.h"

void handler(const struct packet* pkt);
void handler(const struct packet* pkt) {
	printf("Packet length: %zu\n", pkt -> len);
}

int main(void) {
	printf("Initializing nmu...\n");
	capture_start("vethA", handler);
	printf("Closing nmu...\n");
	return 0;
}
