cmake_minimum_required(VERSION 3.6)
project(DNSCache)

# comment C++11 tpo enable catch testing on MAC OSX
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -D_GNU_SOURCE")

set(SRC_DIR src)
set(SOURCE_FILES main.cpp ${SRC_DIR}/DNSCache.cpp)
add_executable(demo ${SOURCE_FILES})

set(TEST_DIR tests)
set(TEST_FILES ${TEST_DIR}/DNSCache_tests.cpp ${SRC_DIR}/DNSCache.cpp)
add_executable(dns_test ${TEST_FILES})
