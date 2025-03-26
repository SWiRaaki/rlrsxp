cmake_minimum_required( VERSION 3.0 )
project( rlrsxp C )

# Field Definitions
set(
	CMAKE_EXPORT_COMPILE_COMMANDS ON
)

set(
	SOURCEFILES
	src/resource.c
)

# Main Executable
add_executable(
	rlrsxp
	main.c
	${SOURCEFILES}
)

add_subdirectory(
	external/raylib
)

target_link_libraries(
	rlrsxp
	raylib
	${OS_LIBS}
)

target_include_directories(
	rlrsxp PRIVATE
	inc
)

set_property(
	TARGET rlrsxp
	PROPERTY C_STANDARD 99
)

include_directories(
	inc
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# using Clang
	target_compile_options(
		rlrsxp PRIVATE
		-Werror
		-Wall
		-Wextra
		-Wconversion
		-Wreturn-type
		-pedantic
		-Wno-multichar
	)
	
	set(
		OS_LIBS
		m
	)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# using GCC
	target_compile_options(
		rlrsxp PRIVATE
		-Werror
		-Wall
		-Wextra
		-Wconversion
		-Wreturn-type
		-pedantic
		-Wno-multichar
	)
	
	set(
		OS_LIBS
		m
	)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	# using Intel C++
	target_compile_options(
		rlrsxp PRIVATE
		-Werror
		-Wall
		-Wextra
		-Wconversion
		-Wreturn-type
		-pedantic
		-Wno-multichar
	)

	set(
		OS_LIBS
		m
	)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	# using Visual Studio C++
	target_compile_options(
		rlrsxp PRIVATE
		/WX
		/W4
		/we4244
		/we4267
		/we4716
		/permissive
		/wd4125
	)

	set(
		OS_LIBS
		""
	)
endif()

if( WIN32 )
	add_definitions( -DPLATFORM_WINDOWS )
elseif( UNIX )
	add_definitions( -DPLATFORM_UNIX )
endif()
