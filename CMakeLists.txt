# CMakeLists.txt for the packet capture project

cmake_minimum_required(VERSION 3.5)
project(PacketCapture)

# Set the C++ standard to C++11 or higher
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)



# Find the libpcap library
find_package(PkgConfig REQUIRED)
pkg_search_module(PCAP REQUIRED libpcap)

# Include directories
include_directories(${PCAP_INCLUDE_DIRS})

# Add the executable
add_executable(packet_capture src/preprocessing/packet_parser.cpp)

# Link libraries
target_link_libraries(packet_capture ${PCAP_LIBRARIES})
