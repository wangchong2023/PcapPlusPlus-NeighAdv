#pragma once
#include "IpAddress.h"
#include "MacAddress.h"
#include "PcapLiveDeviceList.h"

#define DEFAULT_MAX_TRIES 1000000
#define IP_VERSION4 4
#define IP_VERSION6 6
#define MAC_ADDR_IPV4_BROADCAST "ff:ff:ff:ff:ff:ff"
#define MAC_ADDR_IPV6_ALL_NODE  "ff:02:00:00:00:01"
#define ARP_OPER_CODE_INVALID   0
#define ARP_OPER_CODE_LENGTH    3
#define NDP_NA_FLAGS_ROUTER    "r"
#define NDP_NA_FLAGS_OVERRIDE  "o"
#define NDP_NA_FLAGS_SOLICITED "s"

namespace pcpp
{
  struct NeighAdvProtoConfParams
  {
    int ipVersion = IP_VERSION4;
    int arpOperCode = ARP_OPER_CODE_INVALID;
    bool unsolicited = false;
    std::string naFlagsStr;
    MacAddress sourceMac = MacAddress::Zero;
    MacAddress dstMac = MacAddress::Zero;
    MacAddress targetMac = MacAddress::Zero;
    IPAddress sourceIP = IPv4Address::Zero;
    IPAddress dstIP = IPv4Address::Zero;
    IPAddress targetIP = IPv4Address::Zero;
  };

  struct NeighAdvNAFlags
  {
    bool routerFlag = false;
    bool solicitedFlags = true;
    bool overrideFlag = false;
  };

  PcapLiveDevice* getDevByIfNameOrIP(const std::string& ifaceNameOrIP);
  MacAddress getSourceMacAddr(const PcapLiveDevice* dev, MacAddress sourceMac);
  IPAddress getSourceIPAddr(const PcapLiveDevice* dev, IPAddress sourceIP);
  void NeighAdvSetDefaultValues(const PcapLiveDevice* dev, NeighAdvProtoConfParams& params);
  void NeighAdvTranslateNAFlags(const std::string& naFlagsStr, NeighAdvNAFlags& naFlags);
  std::string&  getDevIPAddrStr(std::string& ipAddrStr, const PcapLiveDevice* dev);
}