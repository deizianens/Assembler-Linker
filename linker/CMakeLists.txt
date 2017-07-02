cmake_minimum_required(VERSION 3.6)
project(ligador)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp geradorMIF.h geradorMIF.cpp)
add_executable(ligador ${SOURCE_FILES})