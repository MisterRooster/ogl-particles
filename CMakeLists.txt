cmake_minimum_required(VERSION 3.9.0)
project(ogl-particles)

# Set CXX standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Output
if(CMAKE_BUILD_TYPE MATCHES Debug)
	set(APP_BUILD_TYPE "Debug")
else()
	set(APP_BUILD_TYPE "Release")
endif()

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

# Preprocessor defines
if (WIN32)
	target_compile_definitions(
		${PROJECT_NAME} PRIVATE   
		# Prevents Windows.h from adding unnecessary includes    
		WIN32_LEAN_AND_MEAN  
		# Prevents Windows.h from defining min/max as macros 
		NOMINMAX
		_CRT_SECURE_NO_WARNINGS
		GLEW_STATIC
		# customize ImGui implementation
		IMGUI_USER_CONFIG="thirdparty/oglp_imgui_config.h")   
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
# External libraries
#######################################

# OpenGL
find_package(OpenGL REQUIRED)

# Download all submodules
find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	# Update submodules as needed
	option(GIT_SUBMODULE "Check submodules during build" ON)
	if (GIT_SUBMODULE)
		message(STATUS "Submodule update")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
										WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
										RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if (NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		endif()
	endif()
endif()

# Check all the submodules
if (NOT EXISTS "${PROJECT_SOURCE_DIR}/dependencies/glfw/CMakeLists.txt")
	message(FATAL_ERROR "The glfw submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if (NOT EXISTS "${PROJECT_SOURCE_DIR}/dependencies/glew-cmake/CMakeLists.txt")
	message(FATAL_ERROR "The glew submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if (NOT EXISTS "${PROJECT_SOURCE_DIR}/dependencies/glm/CMakeLists.txt")
	message(FATAL_ERROR "The glm submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if (NOT EXISTS "${PROJECT_SOURCE_DIR}/dependencies/imgui/imgui.h")
	message(FATAL_ERROR "The imgui submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

# GLFW
set( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )
set( GLFW_INSTALL OFF CACHE BOOL "" FORCE )
add_subdirectory( ${PROJECT_SOURCE_DIR}/dependencies/glfw )

# GLEW
set( glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE )
set( glew-cmake_BUILD_STATIC ON CACHE BOOL "" FORCE )
add_subdirectory( ${PROJECT_SOURCE_DIR}/dependencies/glew-cmake )

# GLM
add_subdirectory( ${PROJECT_SOURCE_DIR}/dependencies/glm )

# Put all libraries into a variable
set(APP_LIBS glfw ${OPENGL_LIBRARY} libglew_static glm::glm ${CMAKE_DL_LIBS})

if(WIN32)
	set(APP_LIBS ${APP_LIBS} "Dwmapi.lib")
endif()

# Define the include DIRs
target_include_directories(${PROJECT_NAME}
	PRIVATE "${CMAKE_SOURCE_DIR}/src"
	PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/glfw/include"
	PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/glm"
	PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/imgui"
)

# Define the link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${APP_LIBS})

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
		"${PROJECT_SOURCE_DIR}/data"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>/data")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/src/shaders"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>/data/shaders")
endif()

# Install executable to the bin directory
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
