cmake_minimum_required(VERSION 3.10)

project(NeuronGraph)

# Get the correct Python extension suffix for the platform
execute_process(
    COMMAND python3 -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))"
    OUTPUT_VARIABLE PYTHON_SUFFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Global include for project headers
include_directories(${PROJECT_SOURCE_DIR}/include/project)

# === SHARED SOURCE FILES ===
set(SHARED_SOURCES
    src/neurongraph.cpp
    src/utils.cpp
    src/ugxobject.cpp
    src/neuronpft.cpp
)

# === MAIN EXECUTABLE ===
add_executable(main
    scripts/main.cpp
    ${SHARED_SOURCES}
)

set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/"
)

target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/include/project)
target_link_libraries(main PRIVATE tinyxml2)

# === SPLITREFINE EXECUTABLE ===
add_executable(splitrefine
    scripts/splitrefine.cpp
    ${SHARED_SOURCES}
)

set_target_properties(splitrefine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/"
)

target_include_directories(splitrefine PRIVATE ${PROJECT_SOURCE_DIR}/include/project)
target_link_libraries(splitrefine PRIVATE tinyxml2)

# === SPLITREFINE SET EXECUTABLE ===
add_executable(splitrefineset
    scripts/splitrefineSET.cpp
    ${SHARED_SOURCES}
)

set_target_properties(splitrefineset PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/"
)

target_include_directories(splitrefineset PRIVATE ${PROJECT_SOURCE_DIR}/include/project)
target_link_libraries(splitrefineset PRIVATE tinyxml2)

# === Extracttrunks EXECUTABLE ===
add_executable(extracttrunks
    scripts/extracttrunks.cpp
    ${SHARED_SOURCES}
)

set_target_properties(extracttrunks PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/"
)

target_include_directories(extracttrunks PRIVATE ${PROJECT_SOURCE_DIR}/include/project)
target_link_libraries(extracttrunks PRIVATE tinyxml2)

# === UGX Main EXECUTABLE ===
add_executable(ugxmain
    scripts/ugxmain.cpp
    ${SHARED_SOURCES}
)

set_target_properties(ugxmain PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin/"
)

target_include_directories(ugxmain PRIVATE ${PROJECT_SOURCE_DIR}/include/project)
target_link_libraries(ugxmain PRIVATE tinyxml2)

# === TESTING ===
add_subdirectory(tests)

# === NeuronViewer ===
add_subdirectory(scripts/app_neuronviewer)

# === SWCViewer ===
add_subdirectory(scripts/swcviewer)

# === Pybind11 ===
# Fetch pybind11
include(FetchContent)
FetchContent_Declare(
	pybind11
	GIT_REPOSITORY https://github.com/pybind/pybind11.git
	GIT_TAG	       v2.12.0
)
FetchContent_MakeAvailable(pybind11)

add_library(neurongraph SHARED
	src/bindings.cpp
	src/neurongraph.cpp
)

set_target_properties(neurongraph PROPERTIES
    PREFIX ""                                     # no lib prefix
    SUFFIX "${PYTHON_SUFFIX}"                    # correct .so suffix
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/python_package"  # place it in ../python_package
)

target_include_directories(neurongraph PRIVATE .)
target_link_libraries(neurongraph PRIVATE pybind11::module tinyxml2)
