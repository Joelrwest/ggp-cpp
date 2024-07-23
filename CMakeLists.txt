cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(GGP)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(BOOST_INCLUDE_LIBRARIES program_options asio beast)
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

add_library(setup src/setup.cpp)
target_link_libraries(setup PUBLIC propnet)
target_link_libraries(setup PUBLIC agents)
target_link_libraries(setup PUBLIC player)
target_link_libraries(setup PUBLIC Boost::program_options)
target_link_libraries(setup PUBLIC "${TORCH_LIBRARIES}")

add_executable(play src/play.cpp)
target_link_libraries(play PUBLIC setup)

target_compile_options(play PRIVATE -Wall -Wextra -Wpedantic -Werror)

add_executable(train src/train.cpp)
target_link_libraries(train PUBLIC setup)

target_compile_options(train PRIVATE -Wall -Wextra -Wpedantic -Werror)

add_executable(debug src/debug.cpp)
target_link_libraries(debug PUBLIC setup)

target_compile_options(debug PRIVATE -Wall -Wextra -Wpedantic -Werror)

add_executable(benchmark src/benchmark.cpp)
target_link_libraries(benchmark PUBLIC setup)

target_compile_options(benchmark PRIVATE -Wall -Wextra -Wpedantic -Werror)

add_executable(play-external src/play_external.cpp)
target_link_libraries(play-external PUBLIC setup)
target_link_libraries(play-external PUBLIC Boost::asio Boost::beast)

target_compile_options(play-external PRIVATE -Wall -Wextra -Wpedantic -Werror)
