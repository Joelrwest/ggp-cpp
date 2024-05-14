cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(ReBeLGGP)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(propnet)
add_subdirectory(agents)

add_executable(play play.cpp setup.cpp)
target_link_libraries(play PUBLIC propnet)
target_link_libraries(play PUBLIC agents)

add_executable(train train.cpp)
