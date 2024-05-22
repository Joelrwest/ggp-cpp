cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

add_library(rebel STATIC naive_sampler.cpp training_sampler.cpp)
target_include_directories(rebel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(rebel PUBLIC propnet)

if(MSVC)
  target_compile_options(rebel PRIVATE /W4 /WX)
else()
  target_compile_options(rebel PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
