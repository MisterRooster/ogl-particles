cmake_minimum_required(VERSION 3.9.0)
project(ogl_particles)

# Set CXX standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Output
if(CMAKE_BUILD_TYPE MATCHES Debug)
	set(APP_BUILD_TYPE "Debug")
else()
	set(APP_BUILD_TYPE "Release")
endif()

# Add .lib files
link_directories(
	"${CMAKE_SOURCE_DIR}/dependencies/glew/lib"
	"${CMAKE_SOURCE_DIR}/dependencies/glfw/lib"
)

# Find source files
file(GLOB_RECURSE APP_SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.cpp
	${CMAKE_SOURCE_DIR}/src/*.glsl
	${CMAKE_SOURCE_DIR}/src/*.vert
	${CMAKE_SOURCE_DIR}/src/*.frag)
	
# Find header files
file(GLOB_RECURSE APP_HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp
	${CMAKE_SOURCE_DIR}/src/*.inc)

# Setup output directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Target
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_UNICODE /DUNICODE")
add_link_options(
	$<$<CONFIG:DEBUG>:/SUBSYSTEM:CONSOLE>
	$<$<NOT:$<CONFIG:DEBUG>>:/SUBSYSTEM:WINDOWS>
	$<$<NOT:$<CONFIG:DEBUG>>:/ENTRY:mainCRTStartup>)
add_executable(${PROJECT_NAME} ${APP_HEADER_FILES} ${APP_SOURCE_FILES})

# Preprocessor definesS
if (WIN32)
	target_compile_definitions(
		${PROJECT_NAME} PRIVATE   
		# Prevents Windows.h from adding unnecessary includes    
		WIN32_LEAN_AND_MEAN  
		# Prevents Windows.h from defining min/max as macros 
		NOMINMAX
		_CRT_SECURE_NO_WARNINGS
		GLEW_STATIC)   
endif()

# Compiler flags
#if (MSVC)
#    # warning level 4 and all warnings as errors
#	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
#else()
#    # lots of warnings and all warnings as errors
#	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wformat -Wcast-align)
#endif()

#######################################
# External libraries! 				  #
#######################################

## OpenGL
find_package(OpenGL REQUIRED)

## Put all libraries into a variable
set(APP_LIBS glfw3 ${OPENGL_LIBRARY} glew32s ${CMAKE_DL_LIBS})

# Define the include DIRs
include_directories(
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/dependencies/glew/include"
	"${CMAKE_SOURCE_DIR}/dependencies/glfw/include"
	"${CMAKE_SOURCE_DIR}/dependencies/glm/include"
	"${CMAKE_SOURCE_DIR}/dependencies/imgui"
)

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${APP_LIBS})

# Create virtual folders to make it look nicer in VS
if(MSVC_IDE)
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	# Run macro
	GroupSources(src)
endif()

# Copy dlls and shaders
if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/src/shaders"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders")
endif()

# Install executable to the bin directory
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
