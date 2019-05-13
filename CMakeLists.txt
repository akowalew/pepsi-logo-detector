cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(pepsi-logo-detector VERSION 0.1.0 LANGUAGES CXX)

# Project options
option(DETECTOR_TESTS "Enable unit tests for detector" ON)

# Conan setup
find_file(CONANBUILDINFO_FILE conanbuildinfo.cmake HINTS ${CMAKE_BINARY_DIR})
include(${CONANBUILDINFO_FILE})
conan_basic_setup()

# Other libraries
find_package(OpenCV REQUIRED)

# Copy assets to build dir
message(STATUS "Copying assets to build dir")
execute_process(COMMAND
	${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/assets
                                       ${CMAKE_BINARY_DIR}/assets
)

# Add targets
add_subdirectory(lib)
add_subdirectory(app)
