# scripts/neuronviewer/CMakeLists.txt

# create executable from source files
add_executable(neuronviewer 
	       neuronviewer.cpp
	       ${CMAKE_SOURCE_DIR}/src/tinyfiledialogs.c
		   ${CMAKE_SOURCE_DIR}/src/neurongraph.cpp
    	   ${CMAKE_SOURCE_DIR}/src/utils.cpp
		   ${CMAKE_SOURCE_DIR}/src/viewercpp/callbacks.cpp
		   ${CMAKE_SOURCE_DIR}/src/viewercpp/globals.cpp
		   ${CMAKE_SOURCE_DIR}/src/viewercpp/draw_utils.cpp
		   ${CMAKE_SOURCE_DIR}/src/viewercpp/opgl_utils.cpp
)

target_compile_options(neuronviewer PRIVATE
	$<$<CONFIG:Release>:-O3 -march=native -ffast-math>
)

# include top-level include directory
target_include_directories(neuronviewer PRIVATE 
                           ${CMAKE_SOURCE_DIR}/include/externals
						   ${CMAKE_SOURCE_DIR}/include/viewer)

# add any needed libraries ...
target_link_libraries(neuronviewer 
	          	glfw 
		      	GLU 
		      	GL
			  	tinyxml2
)  # Plus tinyfiledialogs, etc.

set_target_properties(neuronviewer PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin
)
