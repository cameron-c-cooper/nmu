=================
Decoding Ethernet
=================
--------
Overview
--------
Entered via ``decode_ethernet()``, the function writes the ethernet metadata
into a ``ethernet_metadata`` struct that only looks at Layer 2 framing. This
means that it should follow some simple principles.

* Validate that the packet is long enough for an ethernet header
* Parsing the Ethernet II header
* Identifying the payload protocol
* Advance an offset into the packet buffer
* Decide whether further decoding is possible

There are some things that I should avoid, such as inspecting IP headers,
assuming IPv4 vs IPv6, tracking flows, or reassembling anything. Its like
opening the christmas card before the present, you know?

Additionally the function will return the pointer to the payload of the
ethernet frame to allow the calling function to discover where the proper
'location' to start reading data from is.

The EtherType is what will determine if there is additional processing within
the ``decode_ethernet()`` as that contains information on the payload. The main
area where it may change something is if something is tagged with VLAN as the
0x8100 EtherType value. This offsets the EtherType/size below it as demonstrated
in table :ref:`table-1`. Simply checking the value that exists at offset 20 into
the ethernet packet/frame.

Just as a matter of convenience, we will only be focusing on Ethernet Type II
frames, it makes dealing with this much simpler and it is what the Internet
Protocol uses so it is very widespread already. Another quick note, as I will
forget this is that the linux kernel will strip the preamble, SFD, FCS, and IPG
from the packet to form the full frame so I do not need to read just values 
based on anything from there.

------------
VLAN Effects
------------
Some quick and simple VLAN effects exist. The first and foremost is that it 
shifts the EtherType/Size 4 bytes and takes its place. As such, within the
frame, the tag would exist at octet 12 and the EtherType would be located at
octet 16. The tag structure is in :ref:`table-2`, it demonstrates the location
of the different fields. A description of the fields is given at the bottom of
this section. IEEE 802.1ad also introduces the concept of double tagging. In 
this case, the first 802.1Q header will have TPID 0x88A8 and the next will be 
0x8100. It is also important to note that the EtherType field will still work
as originally intended, merely shifted.

TPID (Tag protocol identifier):
  * A 16-bit field set to a value of 0x8100 in order to identify the frame as an
  IEEE 802.1Q-tagged frame. This field is located at the same position as the
  EtherType field in untagged frames, and is thus used to distinguish the frame
  from untagged frames.

TCI (Tag control information):
  * A 16-bit field containing the following sub-fields:
    * PCP is a 3 bit field refering to the 802.1p class of service and maps to
      the frame priority level. Different PCP values prioritize different
      classes of traffic
    * The DEI indicator is a 1 bit field, it may be used seperately or in
      conjunction with PCP to indicate frames that are fine to be dropped
      given the presence of network congestion
    * VID identifier is a 12 bit field specifying the VLAN to which the frame
      belongs. 0x000 and 0xfff are reserved. All other values are good to be
      used as VLAN identifiers, allowing for up to 4094 VLANs. 0x000 indicates
      no VLAN ID, and just a priority. On bridges, 0x001, or the default, is
      usually reserved for network management VLAN, vendor-specific.


------------
Tables
------------
.. _table-1:
+-------------------+----------+-----+----------+------------+------------+---------------------+---------+--------------+-----+
| Layer             | Preamble | SFD | MAC Dest | MAC Source | 802.1 tag  | Ethertype (ETH II)  | Payload | FCS          | IPG |
|                   |          |     |          |            | (optional) | or len (IEEE 802.3) |         | (32-bit CRC) |     |
+===================+==========+=====+==========+============+============+=====================+=========+==============+=====+
| Length (octets)   |    7     |  1  |    6     |     6      |     (4)    |          2          | 42-1500 |       4      | 12  | 
+-------------------+----------+-----+----------+------------+------------+---------------------+---------+--------------+-----+
| Layer 2 Eth Frame | (not in frame) |                          <- 64-1522 octets ->                                     | NIF | 
+-------------------+----------+-----+----------+------------+------------+---------------------+---------+--------------+-----+
| Eth Packet & IPG  |                                           <- 72-1530 octets ->                                     | 12  | 
+-------------------+----------+-----+----------+------------+------------+---------------------+---------+--------------+-----+

.. _table-2:
+---------+--------+-------+---------+
| 16 bits | 3 bits | 1 bit | 12 bits |
+=========+========+=======+=========+
|  TPID   |           TCI            |
+         +--------+-------+---------+
|         |  PCP   |  DEI  |   VID   |
+---------+--------+-------+---------+


---------
Resources
---------
* RFC 894 - A Standard for the Transmission of IP Datagrams over Ethernet Networks
* https://en.wikipedia.org/wiki/Ethernet_frame
* https://github.com/torvalds/linux/blob/master/include/uapi/linux/if_ether.h
* https://github.com/torvalds/linux/blob/master/net/ethernet/eth.c
* https://en.wikipedia.org/wiki/EtherType
* https://en.wikipedia.org/wiki/IEEE_802.1Q
