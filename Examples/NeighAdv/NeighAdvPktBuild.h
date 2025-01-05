#pragma once
#include "NeighAdvComm.h"
#include "IpAddress.h"
#include "PcapLiveDevice.h"

namespace pcpp
{
	int buildGratuitousArpPacket(const MacAddress& sourceMacAddr, const MacAddress& dstMacAddr, const IPv4Address& targetIP,
								 int arpOperCode, Packet& advPacket);
	int buildUnsolicitedNdpPacket(const MacAddress& senderMacAddr, const MacAddress& dstMacAddr,
								  const IPAddress& dstIP, const IPv6Address& targetIP, const std::string& naFlags,
								  Packet& advPacket);
	int NeighAdvBuildPacket(const NeighAdvProtoConfParams& params, Packet& advPacket);
	int NeighAdvSendPacket(PcapLiveDevice* dev, const NeighAdvProtoConfParams& params);
}