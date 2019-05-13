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

# Main executable
add_executable(detector
	src/main.cpp
)

target_link_libraries(detector
	PRIVATE
		${CONAN_LIBS}
		${OpenCV_LIBRARIES}
)

target_include_directories(detector
	PUBLIC
		include/
	PRIVATE
		src/
		${OpenCV_INCLUDE_DIRS}
)

set_target_properties(detector
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
)

if(CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(detector
    	PRIVATE
			-Wall -Wextra -pedantic
    )
endif()
if(MSVC)
    target_compile_options(detector
    	PRIVATE
    		/W4
    )
endif()

# Copy assets to build dir
message(STATUS "Copying assets to build dir")
execute_process(COMMAND
	${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/assets
                                       ${CMAKE_BINARY_DIR}/assets
)

# Unit tests
if(DETECTOR_TESTS)
	add_subdirectory(test)
endif()
