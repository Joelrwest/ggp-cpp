cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(ReBeLGGP)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(lru_cache GIT_REPOSITORY https://github.com/nitnelave/lru_cache GIT_TAG 48f9cd9f1713ee63172f2a28af6824bbf8161e5c)
FetchContent_MakeAvailable(lru_cache)

add_subdirectory(propnet)
add_subdirectory(agents)

add_executable(play play.cpp setup.cpp)
target_link_libraries(play PUBLIC propnet)
target_link_libraries(play PUBLIC agents)

add_executable(train train.cpp)
