# Set header files

set(SHARED_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(TARGET shared)

message("shared is already defined")

else()

include_directories(${SHARED_INCLUDE_DIR})

set(SHARED_HEADER_FILES
	engine.h
	pfspinstance.h
	pfsp_state.h
	pfsp_problem.h	
	support_functions.h
	ii_engine.h
	iga_engine.h
  )
  
  # Set header files
set(SHARED_SOURCE_FILES
	${SHARED_HEADER_FILES}
	engine.cpp
	pfspinstance.cpp	
	pfsp_state.cpp
	pfsp_problem.cpp
	support_functions.cpp
	ii_engine.cpp
	iga_engine.cpp
  )

add_library(shared STATIC ${SHARED_SOURCE_FILES})

endif()
