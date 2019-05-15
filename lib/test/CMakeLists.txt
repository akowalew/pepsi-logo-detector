add_executable(detector_test
	blobs_test.cpp
	tests_main.cpp
	moments_test.cpp
)

target_link_libraries(detector_test
	PRIVATE
		detector
		${CONAN_LIBS}
)

set_target_properties(detector_test
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
)

if(CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(detector_test
    	PRIVATE
			-Wall -Wextra -pedantic
    )
endif()
if(MSVC)
    target_compile_options(detector_test
    	PRIVATE
    		/W4
    )
endif()

