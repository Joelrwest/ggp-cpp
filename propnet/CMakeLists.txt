cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_library(propnet STATIC src/nodes.cpp src/input_set.cpp src/propnet.cpp src/vector_state.cpp src/persistent_array_state.cpp src/role.cpp src/parser.cpp)
target_include_directories(propnet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(propnet PUBLIC nlohmann_json::nlohmann_json)
target_compile_options(propnet PRIVATE -O3 -Wall -Wextra -Wpedantic -Werror)
