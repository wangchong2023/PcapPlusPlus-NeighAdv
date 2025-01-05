#include "MacAddress.h"
#include "IpAddress.h"
#include "Logger.h"
#include "Ethlayer.h"
#include "IPv6layer.h"
#include "NDPlayer.h"
#include "NeighAdvPktBuild.h"

namespace pcpp
{
	int NeighAdvBuildArpPacket(const MacAddress& sourceMacAddr, const MacAddress& dstMacAddr, const MacAddress& targetMacAddr,
						const IPv4Address& senderIpAddr, const IPv4Address& targetIP, ArpOpcode arpOperCode, Packet& advPacket)
	{
		// Ethernet layer
		EthLayer ethLayer(sourceMacAddr, dstMacAddr);
		if (!advPacket.addLayer(&ethLayer))
		{
			PCPP_LOG_ERROR("Couldn't build Eth layer for ARP packet");
			return 1;
		}

		//ARP layer
		ArpLayer arpLayer(arpOperCode, sourceMacAddr, targetMacAddr, senderIpAddr, targetIP);
		if (!advPacket.addLayer(&arpLayer))
		{
			PCPP_LOG_ERROR("Couldn't build ARP layer for ARP packet");
			return 1;
		}

		advPacket.computeCalculateFields();

		return 0;
	}

	int NeighAdvBuildNdpPacket(const MacAddress& senderMacAddr, const MacAddress& dstMacAddr, const MacAddress& targetMacAddr,
			                   const IPv6Address& sourceIP, const IPv6Address& dstIP, const IPv6Address& targetIP,
			                   const std::string& naFlagsStr, Packet& advPacket)
	{
		// Ethernet layer
		EthLayer ethLayer((MacAddress(senderMacAddr)), MacAddress(dstMacAddr));
		advPacket.addLayer(&ethLayer);

		// IPv6 layer
		auto* ipv6AdvLayer = new IPv6Layer(IPv6Address(sourceIP), IPv6Address(dstIP));
		advPacket.addLayer(ipv6AdvLayer, true);

		// NDP Neighbor Advertisement layer
		NeighAdvNAFlags naFlags;
		NeighAdvTranslateNAFlags(naFlagsStr, naFlags);
		auto* ndpAdvLayer = new NDPNeighborAdvertisementLayer(
										0, IPv6Address(targetIP), MacAddress(targetMacAddr),
											 naFlags.routerFlag, naFlags.solicitedFlags, naFlags.overrideFlag);
		advPacket.addLayer(ndpAdvLayer, true);

		advPacket.computeCalculateFields();

		return 0;
	}

	int NeighAdvBuildPacket(const NeighAdvProtoConfParams& params, Packet& advPacket)
	{
		const IPAddress senderIP = params.unsolicited ? params.sourceIP : params.targetIP;
		if (params.targetIP.getType() == IPAddress::IPv4AddressType)
		{
			return NeighAdvBuildArpPacket(params.sourceMac, params.dstMac, params.targetMac, senderIP.getIPv4(),
									      params.targetIP.getIPv4(), static_cast<ArpOpcode>(params.arpOperCode), advPacket);
		}

		return NeighAdvBuildNdpPacket(params.sourceMac, params.dstMac, params.targetMac, senderIP.getIPv6(),
		                                    params.dstIP.getIPv6(), params.targetIP.getIPv6(), params.naFlagsStr, advPacket);
	}

	int NeighAdvSendPacket(PcapLiveDevice* dev, const NeighAdvProtoConfParams& params)
	{
		Packet advPacket(100);
		if (NeighAdvBuildPacket(params, advPacket) != 0)
		{
			return 1;
		}

		if (!dev->sendPacket(&advPacket))
		{
			return 1;
		}

		return 0;
	}
}