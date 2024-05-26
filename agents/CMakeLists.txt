cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

add_library(agents STATIC agent.cpp factory.cpp non_seeing.cpp simple.cpp human.cpp)
target_include_directories(agents PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(agents PUBLIC propnet)

if(MSVC)
  target_compile_options(agents PRIVATE /W4 /WX)
else()
  target_compile_options(agents PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
