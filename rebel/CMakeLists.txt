cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

add_library(rebel STATIC naive_sampler.cpp training_sampler.cpp)
target_include_directories(rebel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(rebel PUBLIC propnet)
