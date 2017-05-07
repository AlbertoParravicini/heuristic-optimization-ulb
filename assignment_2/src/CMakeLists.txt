# Specify the minimum version for CMake
cmake_minimum_required(VERSION 2.8)
#Set C++11
set (CMAKE_CXX_STANDARD 11)

# Project's name
project(assignment_2)

find_package(Boost)
include_directories (${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/../build/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

add_subdirectory(shared)

add_subdirectory(iga_main)
add_subdirectory(gen_main)
