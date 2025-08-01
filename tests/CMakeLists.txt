# ==== Add the doctest-based test executable for neurongraph
add_executable(doctest 
    test_neurongraph_doctest.cpp
    ${PROJECT_SOURCE_DIR}/src/neurongraph.cpp
    ${PROJECT_SOURCE_DIR}/src/utils.cpp
)

target_include_directories(doctest PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
set_target_properties(doctest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/"
)

target_link_libraries(doctest PRIVATE tinyxml2)
add_test(NAME RunDocTest COMMAND doctest)
# ==========================================================

# ==== Add the doctest-based test executable for ugxobject
add_executable(ugxobject_doctest 
    test_ugxobject_doctest.cpp
    ${PROJECT_SOURCE_DIR}/src/ugxobject.cpp
    ${PROJECT_SOURCE_DIR}/src/neurongraph.cpp
    ${PROJECT_SOURCE_DIR}/src/utils.cpp
)

target_include_directories(ugxobject_doctest PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(ugxobject_doctest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/"
)

target_link_libraries(ugxobject_doctest PRIVATE tinyxml2)
add_test(NAME RunDocTestUGX COMMAND ugxobject_doctest)

# ==========================================================
add_executable(test_neurongraph 
	       test_neurongraph.cpp
	       ${PROJECT_SOURCE_DIR}/src/neurongraph.cpp
)

target_include_directories(test_neurongraph PRIVATE 
	                   ${PROJECT_SOURCE_DIR}/include 
)

# set output directory same as main
set_target_properties(test_neurongraph PROPERTIES 
	 RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/"
)

target_link_libraries(test_neurongraph PRIVATE tinyxml2)

add_test(NAME RunNeuronTests COMMAND test_neurongraph)

# Find all .swc files in tests/data/SWC
file(GLOB SWC_FILES "${PROJECT_SOURCE_DIR}/data/SWC/*.swc")

# Loop over each SWC file
foreach(SWC_FILE ${SWC_FILES})
    get_filename_component(BASENAME ${SWC_FILE} NAME_WE)

    # Set output UGX file
    set(UGX_FILE "${PROJECT_SOURCE_DIR}/output/UGX/${BASENAME}.ugx")

    # Create a unique test source file from template
    set(TEST_SRC "${CMAKE_CURRENT_BINARY_DIR}/test_swc_to_ugx_${BASENAME}.cpp")
    configure_file(${PROJECT_SOURCE_DIR}/tests/test_swc_to_ugx.cpp ${TEST_SRC})

    # Create executable
    add_executable(test_swc_to_ugx_${BASENAME} ${TEST_SRC}
	    	   ${PROJECT_SOURCE_DIR}/src/neurongraph.cpp)
    target_include_directories(test_swc_to_ugx_${BASENAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(test_swc_to_ugx_${BASENAME} PRIVATE tinyxml2)

    # Register with CTest
    add_test(NAME RunSWCtoUGX_${BASENAME}
             COMMAND test_swc_to_ugx_${BASENAME})
endforeach()

