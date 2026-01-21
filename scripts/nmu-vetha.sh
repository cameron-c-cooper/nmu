#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"

NS=nsA
IFACE=vethA
MONITOR="$SCRIPT_DIR/../build/nmu"

sudo ip netns exec $NS $MONITOR
