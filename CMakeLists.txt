cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(ReBeLGGP)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(propnet)

add_executable(play play.cpp)
target_link_libraries(play PUBLIC propnet)
