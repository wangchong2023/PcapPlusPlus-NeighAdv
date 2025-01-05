#include <iostream>
#include "PcapLiveDeviceList.h"
#include "PcapPlusPlusVersion.h"
#include <getopt.h>
#include "SystemUtils.h"
#include "NeighAdvComm.h"
#include "NeighAdvCmd.h"

namespace pcpp
{
	// clang-format off
	option NeighAdvOptions[] = {
		{ "interface",     optional_argument, nullptr, 'i' },
		{ "source-mac",    optional_argument, nullptr, 's' },
		{ "dest-mac",      optional_argument, nullptr, 'd' },
		{ "target-mac",    optional_argument, nullptr, 't' },
		{ "arp-oper-code", optional_argument, nullptr, 'o' },
		{ "ip-version",	optional_argument, nullptr, 'p' },
		{ "source-ip",		optional_argument, nullptr, 'S' },
		{ "dest-ip",		optional_argument, nullptr, 'D' },
		{ "target-ip",		optional_argument, nullptr, 'T' },
		{ "na-flags",	    optional_argument, nullptr, 'F' },
		{ "unsolicited",	optional_argument, nullptr, 'U' },
		{ "count",			optional_argument, nullptr, 'c' },
		{ "version",		no_argument,       nullptr, 'v' },
		{ "list",			optional_argument, nullptr, 'l' },
		{ "help",			optional_argument, nullptr, 'h' },
		{ nullptr,			0,                 nullptr,  0  }
	};

	option* getNeighborAdvOptions()
	{
		return NeighAdvOptions;
	}

	/**
	 * Print application usage
	 */
	void printUsage()
	{
		std::cout
		    << std::endl
		    << "Usage:" << std::endl
		    << "------" << std::endl
		    << AppName::get() << " [-hvl] [-c count] [-s mac_addr] [-d mac_addr] [-t mac_addr] [-o arp_oper_code] [-t mac_addr] [-p ip_ver] [-S ip_addr] [-D ip_addr] [-F na_flags] [-U] -T ip_addr -i interface"
		    << std::endl
		    << std::endl
		    << "Options:" << std::endl
		    << std::endl
		    << "    -h               : Displays this help message and exits" << std::endl
		    << "    -v               : Displays the current version and exists" << std::endl
		    << "    -l               : Print the list of interfaces and exists" << std::endl
		    << "    -c count         : Send 'count' requests" << std::endl
		    << "    -i interface	 : Use the specified interface. Can be interface name (e.g eth0) or interface IPv4 address"
		    << std::endl
		    << "    -s mac_addr		 : Set source MAC address" << std::endl
			<< "    -d mac_addr      : Set Destination MAC address" << std::endl
			<< "    -o arp_oper_code : Set operator code(1 or 2) of ARP packet" << std::endl
			<< "    -p ip_ver        : Set IP version(4 or 6)" << std::endl
		    << "    -S ip_addr		 : Set source IP address" << std::endl
			<< "    -D ip_addr		 : Set Destination IP address" << std::endl
		    << "    -T ip_addr		 : Set target IP address" << std::endl
			<< "    -F na_flags		 : Set flags(ros) of IPv6 Neighbor Advertisement" << std::endl
			<< "    -U unsolicited   : Send gratuitous ARP or IPv6 unsolicited Neighbor Advertisement packet" << std::endl
		    << std::endl;
	}

	/**
	 * Print application version
	 */
	void printAppVersion()
	{
		std::cout << AppName::get() << " " << getPcapPlusPlusVersionFull() << std::endl
		          << "Built: " << getBuildDateTime() << std::endl
		          << "Built from: " << getGitInfo() << std::endl;
		exit(0);
	}

	/**
	 * Go over all interfaces and output their names
	 */
	void listInterfaces()
	{
		const std::vector<PcapLiveDevice*>& devList = PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();

		std::cout << std::endl << "Network interfaces:" << std::endl;
		for (const auto& dev : devList)
		{
			std::string ipAddrStr;
			std::cout << "    -> Name: '" << dev->getName() << "'   IP address: " << getDevIPAddrStr(ipAddrStr, dev) << std::endl;
			ipAddrStr.clear();
		}

		exit(0);
	}

	/**
	 * The callback to be called when application is terminated by ctrl-c. Stops the endless while loop
	 */
	void onApplicationInterrupted(void* cookie)
	{
		const auto shouldStop = static_cast<bool*>(cookie);
		*shouldStop = true;
	}

	void NeighAdvCmdParamIPv4Check(const NeighAdvProtoConfParams& params)
	{
		if (!params.sourceIP.isIPv4())
		{
			EXIT_WITH_ERROR("You must provide IPv4 source IP address to match with IP version");
		}
		if (!params.dstIP.isIPv4())
		{
			EXIT_WITH_ERROR("You must provide IPv4 destination IP address to match with IP version");
		}
		if (!params.targetIP.isIPv4())
		{
			EXIT_WITH_ERROR("You must provide IPv4 target IP address to match with IP version");
		}
		if (params.targetIP.isZero())
		{
			EXIT_WITH_ERROR("You must not provide IPv4 target zero IP Address");
		}
		if (!params.naFlagsStr.empty())
		{
			EXIT_WITH_ERROR("You must not provide Neighbor Advertisement flags with IP version 4");
		}
		if (params.arpOperCode > ARP_REQUEST)
		{
			EXIT_WITH_ERROR("You must provide valid ARP operator code(1-2) with IP version 4");
		}
	}

	void NeighAdvCmdParamIPv6Check(const NeighAdvProtoConfParams& params)
	{
		if ((!params.sourceIP.isZero()) && (!params.sourceIP.isIPv6()))
		{
			EXIT_WITH_ERROR("You must provide IPv6 source IP address to match with IP version");
		}
		if ((!params.dstIP.isZero()) && (!params.dstIP.isIPv6()))
		{
			EXIT_WITH_ERROR("You must provide IPv6 destination IP address to match with IP version");
		}
		if (!params.targetIP.isIPv6())
		{
			EXIT_WITH_ERROR("You must provide IPv6 target IP address to match with IP version");
		}
		if (params.targetIP.isZero())
		{
			EXIT_WITH_ERROR("You must not provide IPv4 target zero IP Address");
		}
		if (params.arpOperCode != ARP_OPER_CODE_INVALID)
		{
			EXIT_WITH_ERROR("You must not provide ARP operator code with IP version 6");
		}
		if (params.naFlagsStr.length() > ARP_OPER_CODE_LENGTH)
		{
			EXIT_WITH_ERROR("You must provide valid Neighbor Advertisement flags length(1-3) with IP version 6");
		}
	}

	void NeighAdvCmdParamCheck(const NeighAdvProtoConfParams& params)
	{
		if (params.ipVersion == IP_VERSION4)
		{
			NeighAdvCmdParamIPv4Check(params);
		}
		else if (params.ipVersion == IP_VERSION6)
		{
			NeighAdvCmdParamIPv6Check(params);
		}
		else
		{
			EXIT_WITH_ERROR("You must provide valid IP version (4 or 6)");
		}
	}
}