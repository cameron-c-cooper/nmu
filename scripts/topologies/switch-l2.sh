#!/bin/bash

set -e 

# HOST1-+
#		|-bridge0- HOST2
# HOST3-+

ip link add br0 type bridge
ip link set br0 up

for i in 1 2 3; do
	ip netns add h$i
	ip link add veth$i type veth peer name br$i
  	ip link set veth$i netns h$i
  	ip link set br$i master br0
  	ip link set br$i up

	ip netns exec h$i ip addr add 10.0.0.$i/24 dev veth$i
  	ip netns exec h$i ip link set veth$i up
  	ip netns exec h$i ip link set lo up
done

echo "Broadcast domain ready (ARP, flooding, etc)"
