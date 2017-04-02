# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)
#Set C++11
set (CMAKE_CXX_STANDARD 11)

# Project's name
project(assignment_1)

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

# Set header files
set(HEADER_FILES
	#state.h
	#problem.h
	engine.h
	pfspinstance.h
	pfsp_state.h
	pfsp_problem.h	
	support_functions.h
	ii_engine.h
	vnd_engine.h
  )
  
  # Set header files
set(SOURCE_FILES
	${HEADER_FILES}
	flowshop.cpp
	engine.cpp
	#state.cpp
	#problem.cpp
	pfspinstance.cpp	
	pfsp_state.cpp
	pfsp_problem.cpp
	support_functions.cpp
	ii_engine.cpp
	vnd_engine.cpp
  )

add_executable(flowshop 
	${SOURCE_FILES}
	)
	
target_link_libraries(flowshop 
	${ARMADILLO_LIBRARIES}
	${Boost_LIBRARIES})
