#include "MacAddress.h"
#include "IpAddress.h"
#include "Logger.h"
#include "Ethlayer.h"
#include "IPv6layer.h"
#include "NDPlayer.h"
#include "NeighAdvPktBuild.h"
#include "NeighAdvComm.h"

namespace pcpp
{
	int NeighAdvBuildArpPacket(const MacAddress& sourceMacAddr, const MacAddress& dstMacAddr, const MacAddress& targetMacAddr,
						const IPv4Address& senderIpAddr, const IPv4Address& targetIP, ArpOpcode opCode)
	{
		EthLayer ethLayer(sourceMacAddr, dstMacAddr);
		ArpLayer arpLayer(opCode, sourceMacAddr, targetMacAddr, senderIpAddr, targetIP);
		Packet arpPacket(sizeof(ArpLayer) + sizeof(EthLayer));

		if (!arpPacket.addLayer(&ethLayer))
		{
			PCPP_LOG_ERROR("Couldn't build Eth layer for ARP packet");
			return 1;
		}

		if (!arpPacket.addLayer(&arpLayer))
		{
			PCPP_LOG_ERROR("Couldn't build ARP layer for ARP packet");
			return 1;
		}

		arpPacket.computeCalculateFields();

		return 0;
	}

	int NeighAdvBuildNdpPacket(const MacAddress& senderMacAddr, const MacAddress& dstMacAddr, const MacAddress& targetMacAddr,
	                   const IPv6Address& sourceIP, const IPv6Address& dstIP, const IPv6Address& targetIP, const std::string& naFlagsStr)
	{
		EthLayer ethLayer((MacAddress(senderMacAddr)), MacAddress(dstMacAddr));
		auto* ipv6AdvLayer = new IPv6Layer(IPv6Address(sourceIP), IPv6Address(dstIP));

		NeighAdvNAFlags naFlags;
		NeighAdvTranslateNAFlags(naFlagsStr, naFlags);
		auto* ndpAdvLayer = new NDPNeighborAdvertisementLayer(
										0, IPv6Address(targetIP), MacAddress(targetMacAddr),
											 naFlags.routerFlag, naFlags.solicitedFlags, naFlags.overrideFlag);

		Packet neighAdvPacket(100);
		neighAdvPacket.addLayer(&ethLayer);
		neighAdvPacket.addLayer(ipv6AdvLayer, true);
		neighAdvPacket.addLayer(ndpAdvLayer, true);
		neighAdvPacket.computeCalculateFields();

		return 0;
	}

	int NeighAdvBuildPacket(const NeighAdvProtoConfParams& params)
	{
		const IPAddress senderIP = params.unsolicited ? params.sourceIP : params.targetIP;
		if (params.targetIP.getType() == IPAddress::IPv4AddressType)
		{
			return NeighAdvBuildArpPacket(params.sourceMac, params.dstMac, params.targetMac, senderIP.getIPv4(),
			                                    params.targetIP.getIPv4(), static_cast<ArpOpcode>(params.arpOperCode));
		}

		return NeighAdvBuildNdpPacket(params.sourceMac, params.dstMac, params.targetMac, senderIP.getIPv6(),
		                                    params.dstIP.getIPv6(), params.targetIP.getIPv6(), params.naFlagsStr);
	}

	int NeighAdvSendPacket(PcapLiveDevice* dev, const NeighAdvProtoConfParams& params)
	{
		Packet arpPacket(100);
		if (NeighAdvBuildPacket(params) != 0)
		{
			return 1;
		}

		if (!dev->sendPacket(&arpPacket))
		{
			return 1;
		}

		return 0;
	}
}