add_subdirectory(./../shared shared)
  
# Set header files
set(SOURCE_FILES
	flowshop_iga.cpp
  )

include_directories(${SHARED_INCLUDE_DIR})  
  
add_executable(
	flowshop_iga
	${SOURCE_FILES}
	)
	
target_link_libraries(
	flowshop_iga
	shared
	${ARMADILLO_LIBRARIES}
	${Boost_LIBRARIES})