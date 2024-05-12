cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

add_library(propnet STATIC basenet.cpp)
target_include_directories(propnet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
