cmake_minimum_required(VERSION 3.16)
project(CommonLib)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
message("dir is ${CMAKE_CURRENT_SOURCE_DIR}")
#add_executable(AdminServer ${DIR_SRCS})
#target_link_libraries( CommonLib
#	pthread 
#	fmt
#	)
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 25)
add_library(CommonLib STATIC ${DIR_SRCS})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../lib/linux)
target_link_libraries( CommonLib
	pthread 
	fmt
	)
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED True)
add_definitions("-Wall -MMD -O1 -std=c++11 -g")
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/../../include" 
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/avalon/include" 
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/zlib/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/fmt/include"
	)
link_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/../../dep/fmt/lib/linux/"
	)
