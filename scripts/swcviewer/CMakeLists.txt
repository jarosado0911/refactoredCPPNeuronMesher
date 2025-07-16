# scripts/swcviewer/CMakeLists.txt

# create executable from source files
add_executable(swc_viewer 
	       swc_viewer.cpp
	       ${CMAKE_SOURCE_DIR}/src/tinyfiledialogs.c
)

target_compile_options(swc_viewer PRIVATE
	$<$<CONFIG:Release>:-O3 -march=native -ffast-math>
)

# include top-level include directory
target_include_directories(swc_viewer PRIVATE ${CMAKE_SOURCE_DIR}/include/externals)

# add any needed libraries ...
target_link_libraries(swc_viewer 
	              glfw 
		      GLU 
		      GL
)  # Plus tinyfiledialogs, etc.

set_target_properties(swc_viewer PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin
)

