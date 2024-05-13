cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

add_library(agents STATIC agent.cpp factory.cpp)
target_include_directories(propnet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(agents PUBLIC propnet)
