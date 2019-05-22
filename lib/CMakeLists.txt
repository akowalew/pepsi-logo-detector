# Main library
add_library(detector
	src/blobs.cpp include/blobs.hpp
	src/core.cpp include/core.hpp
	src/drawing.cpp include/drawing.hpp
	src/format.cpp include/format.hpp
	src/imglog.cpp include/imglog.hpp
	src/moments.cpp include/moments.hpp
	src/morpho.cpp include/morpho.hpp
	src/PepsiDetector.cpp src/PepsiDetectorConfig.cpp include/PepsiDetector.hpp src/PepsiDetectorImpl.hpp
	src/points.cpp include/points.hpp
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

# Enable tests
add_subdirectory(test)
