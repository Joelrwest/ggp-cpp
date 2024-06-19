cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(ReBeLGGP)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

set(BOOST_INCLUDE_LIBRARIES program_options)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.7z
  USES_TERMINAL_DOWNLOAD TRUE
  DOWNLOAD_NO_EXTRACT FALSE
)
FetchContent_MakeAvailable(Boost)

add_subdirectory(propnet)
add_subdirectory(agents)
add_subdirectory(rebel)

add_executable(play src/play.cpp src/setup.cpp)
target_link_libraries(play PUBLIC propnet)
target_link_libraries(play PUBLIC agents)
target_link_libraries(play PUBLIC rebel)

if(MSVC)
  target_compile_options(play PRIVATE /W4 /WX)
else()
  target_compile_options(play PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# if(${CMAKE_BUILD_TYPE} EQUAL Release) # TODO: Figure out more compiler options
#   if(MSVC)
#     target_compile_options(play PRIVATE /GL /LTCG)
#   else()
#     target_compile_options(play PRIVATE -flto)
#   endif()
# endif()

add_executable(train src/train.cpp src/setup.cpp)
target_link_libraries(train PUBLIC propnet)
target_link_libraries(train PUBLIC rebel)
target_link_libraries(train PUBLIC Boost::program_options)

if(MSVC)
  target_compile_options(train PRIVATE /W4 /WX)
else()
  target_compile_options(train PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
