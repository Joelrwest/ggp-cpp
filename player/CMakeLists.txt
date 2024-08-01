cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

set(Torch_DIR "~/libtorch/share/cmake/Torch")
find_package(Torch REQUIRED)

add_library(player STATIC src/search.cpp src/misc.cpp src/random_sampler.cpp src/model.cpp)

target_include_directories(player PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

include(FetchContent)

FetchContent_Declare(caches URL https://github.com/vpetrigo/caches/archive/refs/tags/v0.1.0.tar.gz)
set(BUILD_TEST false)
FetchContent_MakeAvailable(caches)

target_link_libraries(player PUBLIC propnet)
target_link_libraries(player PUBLIC caches)
target_link_libraries(player PUBLIC agents)
target_link_libraries(player PRIVATE torch)
target_compile_options(player PRIVATE -Wall -Wextra -Wpedantic -Werror)
