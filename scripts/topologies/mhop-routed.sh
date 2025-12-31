#!/bin/bash

set -e 

# HostA ─ Router1 ─ Router2 ─ HostB

for ns in hA r1 r2 hB; do
	ip netns add $ns
	ip netns exec $ns ip link set lo up
done

ip link add a_r1 type veth peer name r1_a
ip link add r1_r2 type veth peer name r2_r1
ip link add r2_b type veth peer name b_r2

ip link set a_r1 netns hA
ip link set r1_a netns r1
ip link set r1_r2 netns r1
ip link set r2_r1 netns r2
ip link set r2_b netns r2
ip link set b_r2 netns hB

ip netns exec hA ip addr add 10.0.1.2/24 dev a_r1
ip netns exec r1 ip addr add 10.0.1.1/24 dev r1_a
ip netns exec r1 ip addr add 10.0.2.1/24 dev r1_r2
ip netns exec r2 ip addr add 10.0.2.2/24 dev r2_r1
ip netns exec r2 ip addr add 10.0.3.1/24 dev r2_b
ip netns exec hB ip addr add 10.0.3.2/24 dev b_r2

for n in hA r1 r2 hB; do
  ip netns exec $n ip link set up dev $(ip -n $n link | awk -F: '/eth|veth/ {print $2}')
done

ip netns exec r1 sysctl -w net.ipv4.ip_forward=1
ip netns exec r2 sysctl -w net.ipv4.ip_forward=1

ip netns exec hA ip route add default via 10.0.1.1
ip netns exec hB ip route add default via 10.0.3.1
