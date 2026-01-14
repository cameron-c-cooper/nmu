#!/bin/bash

set -euo pipefail

cleanup() {
	echo "Stopping pings..."
	kill "${PING_A_PID}" "${PING_B_PID}" 2>/dev/null || true
	wait 2>/dev/null || true
	exit 0
}

trap cleanup SIGINT SIGTERM
sudo ip netns exec nsA ping 10.0.0.2 &
PING_A_PID=$!
sudo ip netns exec nsB ping 10.0.0.1 &
PING_B_PID=$!
echo "Pings running (Ctrl-C to stop both)"
wait
