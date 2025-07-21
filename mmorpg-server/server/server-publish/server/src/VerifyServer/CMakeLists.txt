cmake_minimum_required(VERSION 3.16)
project(VerifyServer)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 25)
add_definitions("-Wall -MMD -Wattributes -fpermissive -O1 -std=c++11 -g")
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/../../include/" 
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/avalon/include/" 
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/fmt/include/" 
	"/usr/include/mono-2.0/" 
	"${CMAKE_CURRENT_SOURCE_DIR}/"
	)
link_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/../../lib/linux"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/avalon/lib/linux"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/fmt/lib/linux/"
	"/usr/lib64/mysql/"
	)
message("dir is ${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(VerifyServer ${DIR_SRCS})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/linux)
target_link_libraries(VerifyServer
	CommonLib
	AvalonNetwork
	AvalonFramework
	AvalonGame 
	AvalonScript 
	AvalonXml 
	AvalonLogger 
	AvalonThread 
	AvalonCore 
	AvalonCodec 
	pthread 
	fmt 
	mono-2.0
	)
