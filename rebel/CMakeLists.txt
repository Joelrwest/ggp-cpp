cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

add_library(rebel STATIC src/bogo_sampler.cpp src/misc.cpp src/naive_sampler.cpp src/training_sampler.cpp)
target_include_directories(rebel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(FetchContent)

FetchContent_Declare(caches URL https://github.com/vpetrigo/caches/archive/refs/tags/v0.1.0.tar.gz)
set(BUILD_TEST false)
FetchContent_MakeAvailable(caches)

target_link_libraries(rebel PUBLIC propnet)
target_link_libraries(rebel PUBLIC caches)

if(MSVC)
  target_compile_options(rebel PRIVATE /W4 /WX)
else()
  target_compile_options(rebel PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
