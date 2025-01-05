#pragma once
#include "NeighAdvComm.h"

#include <IpAddress.h>
#include <PcapLiveDevice.h>

namespace pcpp
{
	int buildGratuitousArpPacket(const MacAddress& sourceMacAddr, const MacAddress& dstMacAddr, const IPv4Address& targetIP, int arpOperCode);
	int buildUnsolicitedNdpPacket(const MacAddress& senderMacAddr, const MacAddress& dstMacAddr,
								  const IPAddress& dstIP, const IPv6Address& targetIP, const std::string& naFlags);
	int NeighAdvBuildPacket(const NeighAdvProtoConfParams& params);
	int NeighAdvSendPacket(PcapLiveDevice* dev, const NeighAdvProtoConfParams& params);
}