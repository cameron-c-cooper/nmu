#include <stdio.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <time.h>
#include <signal.h>

#include "capture/capture.h"
#include "capture/packet.h"

// NOTE: For whatever reason, when using ping,
// it is printing the packet twice
void handler(const struct packet* pkt);
void handler(const struct packet* pkt) {
	printf("Packet Length: %zu\n", pkt -> len);
	printf("Packet TS: %s\n", asctime(localtime(&pkt -> ts.tv_sec)));
	printf("\n");
}

static struct capture_ctx* global_ctx;

void handle_sigterm(int signo);
void handle_sigterm(int signo) {
	(void) signo;
	if (global_ctx)
		global_ctx -> running = 0;
}

int main(void) {
	struct capture_ctx ctx;
	global_ctx = &ctx;
	signal(SIGINT, handle_sigterm);
	signal(SIGTERM, handle_sigterm);
	printf("Initializing nmu...\n");
	capture_start(&ctx, "vethA");
	capture_run(&ctx, handler);
	capture_close(&ctx);
	printf("Closing nmu...\n");
	return 0;
}
