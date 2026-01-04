#include <stdio.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <time.h>

#include "capture/packet.h"

// NOTE: For whatever reason, when using ping,
// it is printing the packet twice
void handler(const struct packet* pkt);
void handler(const struct packet* pkt) {
	printf("Packet Length: %zu\n", pkt -> len);
	printf("Packet TS: %s\n", asctime(localtime(&pkt -> ts.tv_sec)));
	printf("\n");
}

int main(void) {
	printf("Initializing nmu...\n");
	void* ring = NULL;
	capture_start("vethA", ring, handler);
	munmap(ring, (1<<20)*64); // manually calc tp_block_size * tp_block_nbr
	printf("Closing nmu...\n");
	return 0;
}
