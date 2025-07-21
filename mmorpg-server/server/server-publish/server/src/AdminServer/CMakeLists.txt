cmake_minimum_required(VERSION 3.16)
project(AdminServer)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 25)
#SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O1 -Wall -g -ggdb -std=c++11")
add_definitions("-Wall -MMD -O1 -std=c++11 -g")
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/../../include/" 
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/avalon/include/" 
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/zlib/include/"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/fmt/include/"
	)
link_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/../../lib/linux"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/avalon/lib/linux"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/zlib/lib/linux"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/fmt/lib/linux"
	)
message("dir is ${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(AdminServer ${DIR_SRCS})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/linux)
#install(TARGETS AdminServer DESTINATION ${PROJECT_SOURCE_DIR}/server_res/AdminServer)
target_link_libraries(AdminServer 
	CommonLib
	AvalonNetwork
	AvalonFramework
	AvalonDatabase 
	AvalonXml 
	AvalonLogger 
	AvalonThread 
	AvalonCore 
	AvalonCodec 
	AvalonGame
	AvalonScript
	pthread 
	mysqlclient 
	z
	)
