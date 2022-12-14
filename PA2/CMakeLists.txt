# CMake version
cmake_minimum_required (VERSION 3.12)

# Project name
project(fp VERSION 1.0 LANGUAGES CXX)

MESSAGE(STATUS "CMAKE_ROOT: " ${CMAKE_ROOT})
MESSAGE(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
set(FM_3RD_PARTY_DIR ${PROJECT_SOURCE_DIR}/3rd-party)

add_library(error_settings INTERFACE)

add_executable(fp ${CMAKE_CURRENT_SOURCE_DIR}/src/fp.cpp)

target_include_directories(fp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

include(CTest)
set(CMAKE_CTEST_ARGUMENTS "--output-on-failure")

add_subdirectory(unittest)

add_custom_command(
  TARGET ${PROJECT_NAME}  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/run.sh
          ${CMAKE_CURRENT_SOURCE_DIR}/build/run.sh)

add_custom_command(
  TARGET ${PROJECT_NAME}  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_SOURCE_DIR}/checker.py
          ${CMAKE_CURRENT_SOURCE_DIR}/build/checker.py)

#add_custom_command(
#  TARGET ${PROJECT_NAME} POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E copy
#          ${CMAKE_CURRENT_SOURCE_DIR}/unittest/test.dat
#          ${CMAKE_CURRENT_SOURCE_DIR}/build/test.dat)
