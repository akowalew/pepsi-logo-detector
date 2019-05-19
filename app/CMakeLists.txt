# Main executable
add_executable(find_pepsi
	src/main.cpp
	src/Application.cpp src/Application.hpp
)

target_link_libraries(find_pepsi
	PRIVATE
		detector
		${CONAN_LIBS}
)

target_include_directories(find_pepsi
	PUBLIC
		include/
	PRIVATE
		src/
)

set_target_properties(find_pepsi
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
)

if(CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(find_pepsi
    	PRIVATE
			-Wall -Wextra -pedantic
    )
endif()
if(MSVC)
    target_compile_options(find_pepsi
    	PRIVATE
    		/W4
    )
endif()
