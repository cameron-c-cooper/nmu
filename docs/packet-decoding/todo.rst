=====================
Packet Decoding TODOs
=====================

----------
Objectives
----------
1. [ ] Ethernet II
2. [ ] IPv4 (including fragmentation) 
3. [ ] ICMP (echo, dest ureachable, time exceeded)
4. [ ] UDP (easy)
5. [ ] TCP:
    * [ ] Flags
    * [ ] Seq/Ack tracking
    * [ ] State transition (SYN -> Established -> FIN)

ICMP may be implemented earlier than what is written, it is something that is
extremely easy to test before I have full traffic generation set up.

---------
Resources
---------
* https://wiki.wireshark.org/InternetProtocolFamily
