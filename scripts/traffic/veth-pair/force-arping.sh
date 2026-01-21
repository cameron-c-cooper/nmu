#!/bin/bash

set -euo pipefail

cleanup() {
	echo "Stopping pings..."
	kill "${ARPING_A_PID}" "${ARPING_B_PID}" 2>/dev/null || true
	wait 2>/dev/null || true
	exit 0
}

trap cleanup SIGINT SIGTERM
sudo ip netns exec nsA arping -c 1 -I vethA 10.0.0.2 &
ARPING_A_PID=$!
sudo ip netns exec nsB arping -c 1 -I vethB 10.0.0.1 &
ARPING_B_PID=$!
echo "Pings running (Ctrl-C to stop both)"
wait
