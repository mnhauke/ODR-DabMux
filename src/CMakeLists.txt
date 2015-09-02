########################################################################
# Project setup
########################################################################

cmake_minimum_required(VERSION 2.8)
project(ODR-DabMux C CXX)

# Select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)


########################################################################
# Version information
########################################################################

set(VERSION_INFO_MAJOR  0)
set(VERSION_INFO_MINOR  7)
set(VERSION_INFO_PATCH  5)

if(NOT DEFINED VERSION_INFO_EXTRA)
    set(VERSION_INFO_EXTRA "git")
endif()
include(Version)

if(NOT DEFINED VERSION)
    #set(VERSION "\"${VERSION_INFO_MAJOR}.${VERSION_INFO_MINOR}.${VERSION_INFO_PATCH}\"")
    set(VERSION "\"${VERSION_INFO}\"")
endif()

set(GITVERSION "\"${VERSION_INFO}\"")
set(PACKAGE_VERSION "\"${VERSION_INFO}\"")
set(PACKAGE_NAME "\"ODR-DabMux\"")
set(PACKAGE "\"ODR-DabMux\"")

#add_definitions(-DVERSION="${VERSION_INFO}")
#add_definitions(-DGITVERSION="${VERSION_INFO}")
#add_definitions(-DPACKAGE="odr-dabmux")
#add_definitions(-DPACKAGE_NAME="ODR-DabMux")


########################################################################
# Compiler specific setup
########################################################################

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|x86")
    set(USE_SIMD "SSE2" CACHE STRING "Use SIMD instructions")
else()
    set(USE_SIMD "no" CACHE STRING "Use SIMD instructions")
endif()
    set(USE_SIMD_VALUES "no" "SSE2" "AVX")
    #set(USE_SIMD_VALUES "no" "SSE2")
    set_property(CACHE USE_SIMD PROPERTY STRINGS ${USE_SIMD_VALUES})
    list(FIND USE_SIMD_VALUES ${USE_SIMD} USE_SIMD_INDEX)
    if(${USE_SIMD_INDEX} EQUAL -1)
        message(FATAL_ERROR "Option ${USE_SIMD} not supported, valid entries are ${USE_SIMD_VALUES}")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    add_definitions(-Wall)
elseif(MSVC)
    # TODO
endif()


########################################################################
# Find build dependencies
########################################################################

find_package(PkgConfig)
# Threads
find_package(Threads REQUIRED)
# ZMQ
pkg_check_modules(ZMQ libzmq>=4.0)
if(NOT ZMQ_FOUND)
    message(FATAL_ERROR "libzmq required to compile ODR-DabMux\n")
else()
    include_directories(${ZMQ_INCLUDE_DIRS})
endif()
# Fec
find_package(Fec REQUIRED)
if(NOT FEC_FOUND)
    message(FATAL_ERROR "libfec required to compile ODR-DabMux\n")
else()
include_directories(${FEC_INCLUDE_DIRS})
endif()
# Boost
find_package(Boost 1.41 REQUIRED COMPONENTS system thread)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost required to compile ODR-DabMux\n")
else()
    include_directories(${Boost_INCLUDE_DIRS})
endif()


########################################################################
# config.h
########################################################################

add_definitions(-DHAVE_CONFIG_H)

##check system for includes
#include(CheckIncludeFile)
#check_include_file("arpa/inet.h"         HAVE_ARPA_INET_H)
#check_include_file("limits.h"            HAVE_LIMITS_H)
#check_include_file("dlfcn.h"             HAVE_DLFCN_H)
#check_include_file("inttypes.h"          HAVE_INTTYPES_H)
#check_include_file("netinet/in.h"        HAVE_NETINET_IN_H)
#check_include_file("memory.h"            HAVE_MEMORY_H)
#check_include_file("netdb.h"             HAVE_NETDB_H)
#check_include_file("stdbool.h"           HAVE_STDBOOL_H)
#check_include_file("stddef.h"            HAVE_STDDEF_H)
#check_include_file("stdint.h"            HAVE_STDINT_H)
#check_include_file("stdlib.h"            HAVE_STDLIB_H)
#check_include_file("strings.h"           HAVE_STRINGS_H)
#check_include_file("string.h"            HAVE_STRING_H)
#check_include_file("sys/ioctl.h"         HAVE_SYS_IOCTL_H)
#check_include_file("sys/socket.h"        HAVE_SYS_SOCKET_H)
#check_include_file("sys/stat.h"          HAVE_SYS_STAT_H)
#check_include_file("sys/time.h"          HAVE_SYS_TIME_H)
#check_include_file("sys/types.h"         HAVE_SYS_TYPES_H)
#check_include_file("sys/wait.h"          HAVE_SYS_WAIT_H)
#check_include_file("unistd.h"            HAVE_UNISTD_H)
#
##Functions
#include(CheckFunctionExists)
#include(CheckSymbolExists)
#check_function_exists("bzero"            HAVE_BZERO)
#check_function_exists("gethostbyname"    HAVE_GETHOSTBYNAME)
#check_function_exists("gettimeofday"     HAVE_GETTIMEOFDAY)
#check_function_exists("inet_ntoa"        HAVE_INET_NTOA)
#check_function_exists("malloc"           HAVE_MALLOC)
#check_function_exists("memchr"           HAVE_MEMCHR)
#check_function_exists("memmove"          HAVE_MEMMOVE)
#check_function_exists("memset"           HAVE_MEMSET)
#check_function_exists("realloc"          HAVE_REALLOC)
#check_function_exists("socket"           HAVE_SOCKET)
#check_function_exists("strchr"           HAVE_STRCHR)
#check_function_exists("strdup"           HAVE_STRDUP)
#check_function_exists("strerror"         HAVE_STRERROR)
#check_function_exists("strrchr"          HAVE_STRRCHR)
#check_function_exists("strstr"           HAVE_STRSTR)
#check_function_exists("strtol"           HAVE_STRTOL)
#check_function_exists("strtoul"          HAVE_STRTOUL)
#check_function_exists("vprintf"          HAVE_VPRINTF)

configure_file ("${PROJECT_SOURCE_DIR}/cmakeconfig.h.in" "${PROJECT_BINARY_DIR}/config.h" )
include_directories(${PROJECT_BINARY_DIR})


########################################################################
# Options
########################################################################

# Formats
option(ENABLE_FORMAT_BRIDGE     "Enable BRIDGE format" OFF)
option(ENABLE_FORMAT_DABPLUS    "Enable DABPLUS format" ON)
option(ENABLE_FORMAT_DMB        "Enable DMB format" ON)
option(ENABLE_FORMAT_EPM        "Enable EPM format" ON)
option(ENABLE_FORMAT_MPEG       "Enable MPEG format" ON)
option(ENABLE_FORMAT_PACKET     "Enable PACKET format" ON)
option(ENABLE_FORMAT_RAW        "Enable RAW format" ON)
# Inputs
option(ENABLE_INPUT_FILE        "Enable FILE input" ON)
option(ENABLE_INPUT_FIFO        "Enable FIFO input" ON)
option(ENABLE_INPUT_ZEROMQ      "Enable ZEROMQ input" ON)
option(ENABLE_INPUT_PRBS        "Enable PRBS input" OFF)
option(ENABLE_INPUT_SLIP        "Enable SLIP input" OFF)
option(ENABLE_INPUT_TEST        "Enable TEST input" OFF)
option(ENABLE_INPUT_UDP         "Enable UDP input" OFF)
# Outputs
option(ENABLE_OUTPUT_FILE       "Enable FILE output" ON)
option(ENABLE_OUTPUT_FIFO       "Enable FIFO output" ON)
option(ENABLE_OUTPUT_SIMUL      "Enable SIMUL output" ON)
option(ENABLE_OUTPUT_TCP        "Enable TCP output" ON)
option(ENABLE_OUTPUT_UDP        "Enable UDP output" ON)
option(ENABLE_OUTPUT_ZEROMQ     "Enable ZEROMQ output" ON)
option(ENABLE_OUTPUT_RAW        "Enable RAW output" ON)

#
if(ENABLE_FORMAT_BRIDGE)
    add_definitions(-DHAVE_FORMAT_BRIDGE)
    list(APPEND list_formats "bridge")
endif()

if(ENABLE_FORMAT_DABPLUS)
    add_definitions(-DHAVE_FORMAT_DABPLUS)
    list(APPEND list_formats "dabplus")
endif()

if(ENABLE_FORMAT_DMB)
    add_definitions(-DHAVE_FORMAT_DMB)
    list(APPEND list_formats "dmb")
endif()

if(ENABLE_FORMAT_EPM)
    add_definitions(-DHAVE_FORMAT_EPM)
    list(APPEND list_formats "epm")
endif()

if(ENABLE_FORMAT_MPEG)
    add_definitions(-DHAVE_FORMAT_MPEG)
    list(APPEND list_formats "mpeg")
endif()

if(ENABLE_FORMAT_PACKET)
    add_definitions(-DHAVE_FORMAT_PACKET)
    list(APPEND list_formats "packet")
endif()

if(ENABLE_FORMAT_RAW)
    add_definitions(-DHAVE_FORMAT_RAW)
    list(APPEND list_formats "raw")
endif()

if(ENABLE_INPUT_FILE)
    add_definitions(-DHAVE_INPUT_FILE)
    list(APPEND list_inputs "file")
endif()

if(ENABLE_INPUT_FIFO)
    add_definitions(-DHAVE_INPUT_FIFO)
    list(APPEND list_inputs "fifo")
endif()

if(ENABLE_INPUT_ZEROMQ)
    if(NOT ZMQ_FOUND)
        message(FATAL_ERROR "libzmq required to compile ODR-DabMux with option ENABLE_INPUT_ZEROMQ\n")
    endif()
    add_definitions(-DHAVE_INPUT_ZEROMQ)
    list(APPEND list_inputs "zmq")
endif()

if(ENABLE_INPUT_PRBS)
    add_definitions(-DHAVE_INPUT_PRBS)
    list(APPEND list_inputs "prbs")
endif()

if(ENABLE_INPUT_SLIP)
    add_definitions(-DHAVE_INPUT_SLIP)
    list(APPEND list_inputs "slip")
endif()

if(ENABLE_INPUT_TEST)
    add_definitions(-DHAVE_INPUT_TEST)
    list(APPEND list_inputs "test")
endif()

if(ENABLE_INPUT_UDP)
    add_definitions(-DHAVE_INPUT_UDP)
    list(APPEND list_inputs "udp")
endif()

if(ENABLE_OUTPUT_FILE)
    add_definitions(-DHAVE_OUTPUT_FILE)
    list(APPEND list_outputs "file")
endif()

if(ENABLE_OUTPUT_FIFO)
    add_definitions(-DHAVE_OUTPUT_FIFO)
    list(APPEND list_outputs "fifo")
endif()

if(ENABLE_OUTPUT_SIMUL)
    add_definitions(-DHAVE_OUTPUT_SIMUL)
    list(APPEND list_outputs "simul")
endif()

if(ENABLE_OUTPUT_TCP)
    add_definitions(-DHAVE_OUTPUT_TCP)
    list(APPEND list_outputs "tcp")
endif()

if(ENABLE_OUTPUT_UDP)
    add_definitions(-DHAVE_OUTPUT_UDP)
    list(APPEND list_outputs "udp")
endif()

if(ENABLE_OUTPUT_ZEROMQ)
    if(NOT ZMQ_FOUND)
        message(FATAL_ERROR "libzmq required to compile ODR-DabMux with option ENABLE_INPUT_ZEROMQ\n")
    endif()
    add_definitions(-DHAVE_OUTPUT_ZEROMQ)
    list(APPEND list_outputs "zmq")
endif()

if(ENABLE_OUTPUT_RAW)
    add_definitions(-DHAVE_OUTPUT_RAW)
    list(APPEND list_outputs "raw")
endif()

########################################################################
# Setup apps
########################################################################

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
# FIXME
include_directories(../lib/farsync/linux)
include_directories(.)
include_directories(dabOutput)
include_directories(dabOutput/edi)
# FIXME end

list(APPEND odr-dabmux_sources
    DabMux.cpp
    #
    dabInput.cpp
    dabInputBridgeUdp.cpp
    dabInputDabplusFifo.cpp
    dabInputDabplusFile.cpp
    dabInputDmbFile.cpp
    dabInputDmbUdp.cpp
    dabInputEnhancedFifo.cpp
    dabInputEnhancedPacketFile.cpp
    dabInputFifo.cpp
    dabInputFile.cpp
    dabInputMpegFifo.cpp
    dabInputMpegFile.cpp
    dabInputPacketFile.cpp
    dabInputPrbs.cpp
    dabInputRawFile.cpp
    dabInputRawFifo.cpp
    dabInputSlip.cpp
    dabInputTest.cpp
    dabInputUdp.cpp
    dabInputZmq.cpp
    #
    dabOutput/dabOutputFile.cpp
    dabOutput/dabOutputFifo.cpp
    dabOutput/dabOutputRaw.cpp
    dabOutput/dabOutputSimul.cpp
    dabOutput/dabOutputTcp.cpp
    dabOutput/dabOutputUdp.cpp
    dabOutput/dabOutputZMQ.cpp
    dabOutput/edi/AFPacket.cpp
    dabOutput/edi/TagItems.cpp
    dabOutput/edi/TagPacket.cpp
    dabOutput/edi/PFT.cpp
    #
    utils.cpp
    MuxElements.cpp
    RemoteControl.cpp
    ParserCmdline.cpp
    ConfigParser.cpp
    Eti.cpp
    Log.cpp
    UdpSocket.cpp
    InetAddress.cpp
    prbs.c
    crc.c
    dabUtils.cpp
    Dmb.cpp
    Interleaver.cpp
    ReedSolomon.cpp
    mpeg.c
    ManagementServer.cpp
    TcpServer.cpp
    TcpSocket.cpp
    )

list(APPEND odr_bridgetest_sources
    bridge.c
    crc.c
    )

list(APPEND zmqinput_keygen_sources
    zmqinput-keygen.c
    )


set_source_files_properties(${odr-dabmux_sources} PROPERTIES LANGUAGE "CXX")
add_executable(odr-dabmux ${odr-dabmux_sources})
target_link_libraries(odr-dabmux ${ZMQ_LIBRARIES} ${Boost_LIBRARIES} ${FEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(odr_bridgetest ${odr_bridgetest_sources})
set_source_files_properties(${odr_bridgetest_sources} PROPERTIES LANGUAGE "C")
set_target_properties(odr_bridgetest PROPERTIES COMPILE_FLAGS "-DBRIDGE_TEST")

add_executable(zmqinput_keygen ${zmqinput_keygen_sources})
target_link_libraries(zmqinput_keygen ${ZMQ_LIBRARIES})


install(TARGETS
            odr-dabmux
            odr_bridgetest
            zmqinput_keygen
        DESTINATION bin)


########################################################################
# Print Summary
########################################################################
message(STATUS "")
message(STATUS "##########################################################")
message(STATUS "## Building for version: ${VERSION}")
message(STATUS "## Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "## Enabled Formats:      ${list_formats}")
message(STATUS "## Enabled Inputs:       ${list_inputs}")
message(STATUS "## Enabled Outputs:      ${list_outputs}")
message(STATUS "##########################################################")
message(STATUS "")

