#include <iostream>
#include "NeighAdvComm.h"
#include "NeighAdvCmd.h"

namespace pcpp
{
	PcapLiveDevice* getDevByIfNameOrIP(const std::string& ifaceNameOrIP)
	{
		PcapLiveDevice* dev = nullptr;

		// Search interface by name or IP
		if (!ifaceNameOrIP.empty())
		{
			dev = PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIpOrName(ifaceNameOrIP);
			if (dev == nullptr)
				EXIT_WITH_ERROR("Couldn't find interface by provided IP address or name");
		}
		else
			EXIT_WITH_ERROR("Interface name or IP empty");

		return dev;
	}

	MacAddress getSourceMacAddr(const PcapLiveDevice* dev, MacAddress sourceMac)
	{
		// Use the interface MAC address
		sourceMac = dev->getMacAddress();

		// if source MAC is still invalid, it means it couldn't be extracted from interface
		if (sourceMac == MacAddress::Zero)
			EXIT_WITH_ERROR("MAC address couldn't be extracted from interface");

		return sourceMac;
	}

	std::string& getDevIPAddrStr(std::string& ipAddrStr, const PcapLiveDevice* dev)
	{
		const std::vector<IPAddress> ipAddr =  dev->getIPAddresses();
		if (ipAddr.empty())
		{
			return ipAddrStr;
		}

		std::vector<std::string> ipv4AddrStr;
		std::vector<std::string> ipv6AddrStr;
		for (const auto & ipAddrItem : ipAddr)
		{
			if (ipAddrItem.isIPv4())
			{
				ipv4AddrStr.push_back(ipAddrItem.toString());
			}
			if (ipAddrItem.isIPv6())
			{
				ipv6AddrStr.push_back(ipAddrItem.toString());
			}
		}

		std::sort(ipv4AddrStr.begin(), ipv4AddrStr.end());
		std::sort(ipv6AddrStr.begin(), ipv6AddrStr.end());

		for (const auto & ipAddrStrItem : ipv4AddrStr)
		{
			ipAddrStr.append(ipAddrStrItem).append(", ");
		}
		for (const auto & ipAddrStrItem : ipv6AddrStr)
		{
			ipAddrStr.append(ipAddrStrItem).append(", ");
		}

		if (!ipAddrStr.empty())
		{
			ipAddrStr = ipAddrStr.substr(0, ipAddrStr.length() - 2);
		}

		return ipAddrStr;
	}

	IPAddress getDevFirstSourceIPAddr(const PcapLiveDevice* dev, int ipVer)
	{
		IPAddress sourceIP;
		if (ipVer == IP_VERSION4)
		{
			sourceIP = dev->getIPv4Address();
			if (sourceIP == IPv4Address::Zero)
				EXIT_WITH_ERROR("Source IPv4 address wasn't supplied and couldn't be retrieved from interface");
			return sourceIP;
		}
		else if (ipVer == IP_VERSION6)
		{
			sourceIP = dev->getIPv6Address();
			if (sourceIP == IPv6Address::Zero)
				EXIT_WITH_ERROR("Source IPv4 address wasn't supplied and couldn't be retrieved from interface");
		}
		else
		{
			EXIT_WITH_ERROR("IP Version is not valid");
		}

		return sourceIP;
	}

	void NeighAdvSetDefaultValuesIPv4(NeighAdvProtoConfParams& params)
	{
		// Set default destination MAC address if not provided
		if (params.dstMac == MacAddress::Zero)
		{
			params.dstMac = MacAddress(MAC_ADDR_IPV4_BROADCAST);
		}
		// Set default IPv4 source IP address and ARP operator code if not provided
		if (params.arpOperCode == ARP_OPER_CODE_INVALID)
		{
			params.arpOperCode = ArpOpcode::ARP_REQUEST;
		}
	}

	void NeighAdvSetDefaultValuesIPv6(NeighAdvProtoConfParams& params)
	{
		// Set default destination MAC address if not provided
		if (params.dstMac == MacAddress::Zero)
		{
			params.dstMac = MacAddress(MAC_ADDR_IPV6_ALL_NODE);
		}
		// Set default IPv6 Neighbor Advertisement flag if not provided
		if (params.naFlagsStr.empty())
		{
			params.naFlagsStr = std::string(NDP_NA_FLAGS_OVERRIDE).append(NDP_NA_FLAGS_SOLICITED);
		}
	}

	void NeighAdvSetDefaultValues(const PcapLiveDevice* dev, NeighAdvProtoConfParams& params)
	{
		// Set default source MAC address if not provided
		if (params.sourceMac == MacAddress::Zero)
		{
			params.sourceMac = getSourceMacAddr(dev, params.sourceMac);
		}
		// Set default target MAC address if not provided
		if (params.targetMac == MacAddress::Zero)
		{
			params.targetMac = params.sourceMac;
		}
		// Set default values by IP version
		if (params.ipVersion == IP_VERSION4)
		{
			NeighAdvSetDefaultValuesIPv4(params);
		}
		else
		{
			NeighAdvSetDefaultValuesIPv6(params);
		}
		// Set default source IP address if not provided
		if (params.sourceIP == IPv6Address::Zero)
		{
			params.sourceIP = getDevFirstSourceIPAddr(dev, params.ipVersion);
		}
	}

	void NeighAdvTranslateNAFlags(const std::string& naFlagsStr, NeighAdvNAFlags& naFlags)
	{
		//Translate router flags if exist in the flags string
		if (naFlagsStr.find(std::string(NDP_NA_FLAGS_ROUTER)) != std::string::npos)
		{
			naFlags.routerFlag = false;
		}
		//Translate override flags if exist in the flags string
		if (naFlagsStr.find(std::string(NDP_NA_FLAGS_OVERRIDE)) != std::string::npos)
		{
			naFlags.overrideFlag = false;
		}
		//Translate solicited flags if exist in the flags string
		if (naFlagsStr.find(std::string(NDP_NA_FLAGS_SOLICITED)) != std::string::npos)
		{
			naFlags.solicitedFlags = false;
		}
	}
}