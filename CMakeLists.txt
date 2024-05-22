cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(ReBeLGGP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(propnet)
add_subdirectory(agents)
add_subdirectory(rebel)

add_executable(play play.cpp setup.cpp)
target_link_libraries(play PUBLIC propnet)
target_link_libraries(play PUBLIC agents)
target_link_libraries(play PUBLIC rebel)

if(MSVC)
  target_compile_options(play PRIVATE /W4 /WX)
else()
  target_compile_options(play PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(train train.cpp)

if(MSVC)
  target_compile_options(train PRIVATE /W4 /WX)
else()
  target_compile_options(train PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
