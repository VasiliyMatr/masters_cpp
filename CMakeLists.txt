cmake_minimum_required(VERSION 3.22)

project(masters_cpp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Default C++ settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra")

# Add CPM
include(cmake/CPM.cmake)

# Add third party
include(cmake/third_party.cmake)

enable_testing()

add_subdirectory(common)

add_subdirectory(t01_string)
