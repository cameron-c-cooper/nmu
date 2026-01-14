#include <stdio.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <signal.h>
#include "decode/decode.h"
#include "capture/capture.h"
#include "capture/packet.h"

// handler should construct the packet view and
// then pass it to the decoder
void handler(const struct packet* pkt);
void handler(const struct packet* pkt) {
	// not really sure if this function even needs pkt? i will say though that
	// it could help out with timestamps in the future when I dont need the
	// timestamps to decode the packet. thoughts for the future.
	struct packet_view pv = {
		.data = pkt -> data,
		.len = pkt -> len,
		.offset = 0
	};
	struct packet_metadata pmd;
	decode_packet(&pv, &pmd);
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
