cmake_minimum_required(VERSION 3.6)
project(DNSCache)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -pthread -fPIC -std=c++11")

set(SRC_DIR src)
set(SOURCE_FILES demo.cpp ${SRC_DIR}/DNSCache.cpp)
add_executable(demo ${SOURCE_FILES})

set(TEST_DIR tests)
set(TEST_FILES ${TEST_DIR}/DNSCache_tests.cpp ${SRC_DIR}/DNSCache.cpp)
add_executable(dns_test ${TEST_FILES})

add_library(DNSCache SHARED ${SRC_DIR}/DNSCache.cpp)

