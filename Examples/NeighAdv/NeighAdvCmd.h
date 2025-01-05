#pragma once
#include <getopt.h>

namespace pcpp
{
	#define EXIT_WITH_ERROR(reason)                                                                                        \
	do                                                                                                                 \
	{                                                                                                                  \
		pcpp::printUsage();                                                                                                  \
		std::cout << std::endl << "ERROR: " << reason << std::endl << std::endl;                                       \
		exit(1);                                                                                                       \
	} while (0);

	option* getNeighborAdvOptions();
	void NeighAdvCmdParamCheck(const NeighAdvProtoConfParams& params);
	void printUsage();
	void printAppVersion();
	void onApplicationInterrupted(void* cookie);
	void listInterfaces();
};