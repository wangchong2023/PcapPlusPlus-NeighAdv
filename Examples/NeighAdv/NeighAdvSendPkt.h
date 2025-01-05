#pragma once
#include "NeighAdvComm.h"
#include "IpAddress.h"
#include "PcapLiveDevice.h"

namespace pcpp
{
	int buildGratuitousArpPacket(PcapLiveDevice* dev, const MacAddress& sourceMacAddr, const MacAddress& dstMacAddr, const IPv4Address& targetIP,
								 int arpOperCode, Packet& advPacket);
	int buildUnsolicitedNdpPacket(PcapLiveDevice* dev, const MacAddress& senderMacAddr, const MacAddress& dstMacAddr,
								  const IPAddress& dstIP, const IPv6Address& targetIP, const std::string& naFlags,
								  Packet& advPacket);
	int NeighAdvSendPacket(PcapLiveDevice* dev, const NeighAdvProtoConfParams& params);
}