/**
 * NeighAdv example application
 * ================================
 * This application sending an ARP or NA packet with various parameters to a target IP address.
 * Its basic input is the target IP address and the interface name/IP to send the ARP or ICMPv6 NA packet from.
 */

#include <iostream>
#include <MacAddress.h>
#include <IpAddress.h>
#include <Logger.h>
#include <PcapLiveDeviceList.h>
#include <PcapLiveDevice.h>
#include <getopt.h>
#include <SystemUtils.h>
#include "NeighAdvSendPkt.h"
#include "NeighAdvCmd.h"

/**
 * main method of the application
 */
int main(const int argc, char* argv[])
{
	pcpp::AppName::init(argc, argv);

	int maxTries = DEFAULT_MAX_TRIES;
	std::string ifaceNameOrIP;
	pcpp::NeighAdvProtoConfParams params;
	int optionIndex = 0;
	int opt = 0;

	while ((opt = getopt_long(argc, argv, "i:s:d:t:o:p:S:D:T:F:U:c:hvl:", pcpp::getNeighborAdvOptions(), &optionIndex)) != -1)
	{
		switch (opt)
		{
		case 0:
			break;
		case 'i':
			ifaceNameOrIP = optarg;
			// verify that interface name or IP were provided
			if (ifaceNameOrIP.empty())
				EXIT_WITH_ERROR("You must provide at least interface name or interface IP (-i switch)");
			break;
		case 's':
			try
			{
				params.sourceMac = pcpp::MacAddress(optarg);
			}
			catch (std::exception&)
			{
				EXIT_WITH_ERROR("Source MAC address is not valid");
			}
			break;
		case 'd':
			try
			{
				params.dstMac = pcpp::MacAddress(optarg);
			}
			catch (std::exception&)
			{
				EXIT_WITH_ERROR("Destination MAC address is not valid");
			}
			break;
		case 't':
			try
			{
				params.targetMac = pcpp::MacAddress(optarg);
			}
			catch (std::exception&)
			{
				EXIT_WITH_ERROR("Target MAC address is not valid");
			}
			break;
		case 'o':
			params.arpOperCode = atoi(optarg);
			break;
		case 'p':
			params.ipVersion = atoi(optarg);
			break;
		case 'S':
			try
			{
				params.sourceIP = pcpp::IPAddress(optarg);
			}
			catch (const std::exception&)
			{
				EXIT_WITH_ERROR("Source IP address is not valid");
			}
			break;
		case 'D':
			try
			{
				params.dstIP = pcpp::IPAddress(optarg);
			}
			catch (const std::exception&)
			{
				EXIT_WITH_ERROR("Destination IP address is not valid");
			}
			break;
		case 'T':
			try
			{
				params.targetIP = pcpp::IPAddress(optarg);
			}
			catch (const std::exception&)
			{
				EXIT_WITH_ERROR("Target IP is not valid");
			}
			break;
		case 'F':
			params.naFlagsStr = optarg;
			// verify that NA flags were provided
			if (ifaceNameOrIP.empty())
				EXIT_WITH_ERROR("You must provide at least flags of Neighbor Advertisement (-F switch)");
			break;
		case 'U':
			params.unsolicited = true;
			break;
		case 'c':
			maxTries = atoi(optarg);
			break;
		case 'h':
			pcpp::printUsage();
			exit(0);
		case 'v':
			pcpp::printAppVersion();
			break;
		case 'l':
			pcpp::listInterfaces();
			break;
		default:
			pcpp::printUsage();
			exit(-1);
		}
	}

	// check if all required parameters were provided
	pcpp::NeighAdvCmdParamCheck(params);

	// open device in promiscuous mode
	pcpp::PcapLiveDevice* dev = pcpp::getDevByIfNameOrIP(ifaceNameOrIP);
	if (!dev->open())
		EXIT_WITH_ERROR("Couldn't open interface device '" << dev->getName() << "'");

	// set default values for Neighbor Advertisement packets if not provided by user
	pcpp::NeighAdvSetDefaultValues(dev, params);

	// suppressing errors to avoid cluttering stdout
	pcpp::Logger::getInstance().suppressLogs();
	// make sure the app closes the device upon termination
	bool shouldStop = false;
	pcpp::ApplicationEventHandler::getInstance().onApplicationInterrupted(pcpp::onApplicationInterrupted, &shouldStop);

	// send Neighbor Advertisement packets
	int i = 1;
	while (i <= maxTries && !shouldStop)
	{
		NeighAdvSendPacket(dev, params);
		i++;
	}

	// close device
	dev->close();
}
