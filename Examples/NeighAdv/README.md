NeighAdv
======

This application sending an ARP or NA packet with various parameters to a target IP address.
Its basic input is the target IP address and the interface name/IP to send the ARP or ICMPv6 NA packet from.

Using the utility
-----------------
	Basic usage:
	------
	NeighAdv [-hvl] [-c count] [-s mac_addr] [-d mac_addr] [-t mac_addr] [-o 
	arp_oper_code] [-t mac_addr] [-p ip_ver] [-S ip_addr] [-D ip_addr] [-F 
	na_flags] [-U] -T ip_addr -i interface
	
	Options:
	
	    -h               : Displays this help message and exits
	    -v               : Displays the current version and exists
	    -l               : Print the list of interfaces and exists
	    -c count         : Send 'count' requests
	    -i interface	 : Use the specified interface. Can be interface name (e.g 
	eth0) or interface IPv4 address
	    -s mac_addr		 : Set source MAC address
	    -d mac_addr      : Set Destination MAC address
	    -o arp_oper_code : Set operator code(1 or 2) of ARP packet
	    -p ip_ver        : Set IP version(4 or 6)
	    -S ip_addr		 : Set source IP address
	    -D ip_addr		 : Set Destination IP address
	    -T ip_addr		 : Set target IP address
	    -F na_flags		 : Set flags(ros) of IPv6 Neighbor Advertisement
	    -U unsolicited   : Send gratuitous ARP or IPv6 unsolicited Neighbor 
	Advertisement packet

Limitations
-----------
- Doesn't support send ARP or ICMPv6 NA packet with multi interfaces at the same time.
