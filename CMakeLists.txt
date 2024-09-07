cmake_minimum_required(VERSION 3.22)

project(masters_cpp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Default C++ settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra")

# Find Google test
find_package(GTest QUIET)
if (NOT GTest_FOUND)
  message(WARNING "Google test package not found. Tests will not be build")
else()
  enable_testing()
endif ()

add_subdirectory(common)


