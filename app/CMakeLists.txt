# Main executable
add_executable(find_logos
	src/main.cpp
	src/Application.cpp src/Application.hpp
)

target_link_libraries(find_logos
	PRIVATE
		detector
		${CONAN_LIBS}
		stdc++fs
)

target_include_directories(find_logos
	PUBLIC
		include/
	PRIVATE
		src/
)

set_target_properties(find_logos
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
)

if(CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(find_logos
    	PRIVATE
			-Wall -Wextra -pedantic
    )
endif()
if(MSVC)
    target_compile_options(find_logos
    	PRIVATE
    		/W4
    )
endif()
