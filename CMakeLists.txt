cmake_minimum_required(VERSION 2.8)

# Generate config.h
include(CheckCSourceCompiles)

check_c_source_compiles("
#include <pwd.h>
int main() { return 0; }
" HAVE_PWD_H)

check_c_source_compiles("
#include <sys/time.h>
int main() { return 0; }
" HAVE_SYS_TIME_H)

check_c_source_compiles("
#include <unistd.h>
int main() { return 0; }
" HAVE_UNISTD_H)

configure_file(${CMAKE_CURRENT_LIST_DIR}/config.h.cmakein
               ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DHAVE_CONFIG_H)

# Special flags depending on the compiler
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(--std=c++11)
  add_definitions(-DHAVE_TIME_H)
endif()

file(GLOB sundown_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  sundown/*.c)
file(GLOB bake_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  *.cpp)

add_library(sundown OBJECT ${sundown_sources})
add_executable(bake $<TARGET_OBJECTS:sundown> ${bake_sources})
