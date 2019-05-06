cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(pepsi-logo-detector VERSION 0.1.0 LANGUAGES CXX)

# Project options
option(DETECTOR_TESTS "Enable unit tests for detector" ON)

# Find necessary libraries
find_package(OpenCV REQUIRED)

# Main executable
add_executable(detector
	src/main.cpp
)

target_link_libraries(detector
	PRIVATE
		${OpenCV_LIBRARIES}
)

target_include_directories(detector
	PUBLIC
		include/
	PRIVATE
		src/
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

# Unit tests
if(DETECTOR_TESTS)
	add_subdirectory(test)
endif()
