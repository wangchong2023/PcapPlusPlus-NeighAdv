add_executable(NeighAdv main.cpp NeighAdvComm.cpp NeighAdvPktBuild.cpp NeighAdvCmd.cpp)

target_link_libraries(NeighAdv PUBLIC PcapPlusPlus::Pcap++)

if(MSVC)
  # This executable requires getopt.h not available on VStudio
  target_link_libraries(NeighAdv PRIVATE Getopt-for-Visual-Studio)
endif()

set_target_properties(NeighAdv PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PCAPPP_BINARY_EXAMPLES_DIR}")

if(PCAPPP_INSTALL)
  install(
    TARGETS NeighAdv
    EXPORT PcapPlusPlusTargets
    RUNTIME DESTINATION ${PCAPPP_INSTALL_BINDIR})
endif()
