cmake_minimum_required(VERSION 3.10)

project(NeuronGraph)
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

# === TESTING ===
add_subdirectory(tests)

