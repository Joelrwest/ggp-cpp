cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_library(propnet STATIC basenet.cpp nodes.cpp propnet.cpp state.cpp state_data.cpp role.cpp basenet_parser.cpp)
target_include_directories(propnet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(propnet PUBLIC nlohmann_json::nlohmann_json)
