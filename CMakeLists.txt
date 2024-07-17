cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(GGP)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

set(BOOST_INCLUDE_LIBRARIES program_options)
set(BOOST_ENABLE_CMAKE ON)

set(Torch_DIR "~/libtorch/share/cmake/Torch")
find_package(Torch REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.7z
  USES_TERMINAL_DOWNLOAD TRUE
  DOWNLOAD_NO_EXTRACT FALSE
)
FetchContent_MakeAvailable(Boost)

add_subdirectory(propnet)
add_subdirectory(agents)
add_subdirectory(player)

add_executable(play src/play.cpp src/setup.cpp)
target_link_libraries(play PUBLIC propnet)
target_link_libraries(play PUBLIC agents)
target_link_libraries(play PUBLIC player)
target_link_libraries(play PRIVATE torch)

if(MSVC)
  target_compile_options(play PRIVATE /W4 /WX)
else()
  target_compile_options(play PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(train src/train.cpp src/setup.cpp)
target_link_libraries(train PUBLIC propnet)
target_link_libraries(train PUBLIC player)
target_link_libraries(train PUBLIC Boost::program_options)
target_link_libraries(train PRIVATE torch)

if(MSVC)
  target_compile_options(train PRIVATE /W4 /WX)
else()
  target_compile_options(train PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(debug src/debug.cpp src/setup.cpp)
target_link_libraries(debug PUBLIC propnet)
target_link_libraries(debug PUBLIC player)
target_link_libraries(debug PRIVATE torch)

if(MSVC)
  target_compile_options(debug PRIVATE /W4 /WX)
else()
  target_compile_options(debug PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(benchmark src/benchmark.cpp src/setup.cpp)
target_link_libraries(benchmark PUBLIC propnet)
target_link_libraries(benchmark PUBLIC player)
target_link_libraries(benchmark PRIVATE torch)

if(MSVC)
  target_compile_options(benchmark PRIVATE /W4 /WX)
else()
  target_compile_options(benchmark PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
