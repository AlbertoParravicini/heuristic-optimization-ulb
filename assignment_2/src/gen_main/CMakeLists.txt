add_subdirectory(./../shared shared)
  
# Set header files
set(SOURCE_FILES
	flowshop_gen.cpp
  )
  
include_directories(${SHARED_INCLUDE_DIR})

add_executable(
	flowshop_gen
	${SOURCE_FILES}
	)
	
target_link_libraries(
	flowshop_gen
	shared
	${ARMADILLO_LIBRARIES}
	${Boost_LIBRARIES})