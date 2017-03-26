cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-lboost_program_options")

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_executable(main test.cpp)
target_link_libraries( main ${Boost_LIBRARIES} )