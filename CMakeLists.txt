cmake_minimum_required(VERSION 2.8)
project(HFT_Orderbook)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/hftlob.h
        src/datastructs.c
        src/limits.c
        src/orders.c
        src/bst.c
        src/utils.c
        src/main.c
        src/benchmark.c
        src/binance.cpp
        src/CuTest.h
        src/CuTest.c
        src/testCases.c)

add_executable(HFT_Orderbook ${SOURCE_FILES})
target_link_libraries(HFT_Orderbook curl)

set(CMAKE_BUILD_TYPE Debug)