#!/bin/bash

set -e 

# Delete known namespaces (safe even if they don't exist)
for ns in hA hB hC r1 r2 r3 nsA nsB; do
    ip netns del $ns 2>/dev/null || true
done

echo "[*] Removing bridges..."
for br in br0 br1; do
    ip link set $br down 2>/dev/null || true
    ip link del $br 2>/dev/null || true
done

echo "[*] Removing stray veth interfaces..."
for iface in $(ip link show | awk -F: '/veth/ {print $2}' | tr -d ' '); do
    ip link del "$iface" 2>/dev/null || true
done

echo "[*] Clearing tc rules..."
for iface in $(ip link show | awk -F: '/veth|br/ {print $2}' | tr -d ' '); do
    tc qdisc del dev "$iface" root 2>/dev/null || true
done
