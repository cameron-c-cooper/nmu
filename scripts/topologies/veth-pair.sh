#!/bin/bash

# nsA <---> veth <---> nsB

set -e

ip netns add nsA
ip netns add nsB

ip link add vethA type veth peer name vethB
ip link set vethA netns nsA
ip link set vethB netns nsB

ip netns exec nsA ip addr add 10.0.0.1/24 dev vethA
ip netns exec nsB ip addr add 10.0.0.2/24 dev vethB

ip netns exec nsA ip link set vethA up
ip netns exec nsB ip link set vethB up
ip netns exec nsA ip link set lo up
ip netns exec nsB ip link set lo up
