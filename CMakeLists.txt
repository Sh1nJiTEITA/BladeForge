# CMakeList.txt : CMake project for ${PROJECT_NAME}, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.13 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 20)
include_directories(include)
#include_directories(src)
add_custom_target(shader_dir 
	ALL
	COMMAND ${CMAKE_COMMAND} 
	-E make_directory ${directory}
)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# VULKAN PATH
set(VULKAN_DIR
	"C:/VulkanSDK/1.3.261.1"
)

# Project Name
set(PROJECT_NAME "BladeForge")
project(${PROJECT_NAME})

# Shaders (PRE-BUILD) 
# Shader files must be compiled from GLSL to SPIR-V before build is done.
if (WIN32)
	execute_process(
		#COMMAND cmd /C ${CMAKE_SOURCE_DIR}/compile.bat
		COMMAND cmd /C "${VULKAN_DIR}/Bin/glslc.exe shaders/basic.vert -o shaders/vert.spv"
		COMMAND cmd /C "${VULKAN_DIR}/Bin/glslc.exe shaders/basic.frag -o shaders/frag.spv"

		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		RESULT_VARIABLE command_result
		OUTPUT_VARIABLE command_output
	)
	if (command_result EQUAL 0)
		message(STATUS "All shaders is correctly compiled.")
	else()
		message(STATUS "Shader compile failed with error code: ${command_result}")
		message(STATUS "Shader output: ${command_output}")
	endif()
endif()


# Headers/Sources
set(SOURCES
	"src/BladeForgeBin.cpp"
)

set(HEADERS
	"include/BladeForgeBin.h"	
)

# Add source to this project's executable.
add_executable(${PROJECT_NAME} 
	"main.cpp"
	"include/BladeForgeBin.h"
	"src/BladeForgeBin.cpp"
	${SOURCES}
	${HEADERS}
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()


# Copies allShaderFiles to binary.



file(GLOB SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.spv)
foreach(shaderi ${SHADERS})
	configure_file(${shaderi} ${CMAKE_CURRENT_BINARY_DIR}/shaders/ COPYONLY)
	message("-- [SHADER-COPY] File ${shaderi} is copied to binary dir.")
endforeach()


### External libraries ### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Vulkan
set(ENV{VULKAN_SDK} ${VULKAN_DIR})
find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC "${VULKAN_DIR}/Include")



# GLFW LIBRARY
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

# GLM
add_subdirectory(external/glm)
target_link_libraries(${PROJECT_NAME} glm)

# imgui
#TODO: Add another implementation
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui/)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)


# CATCH 2
add_subdirectory(external/catch2)
#find_package(catch2)
add_executable(tests ${CMAKE_SOURCE_DIR}/tests/bez_test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

#target_link_libraries(${PROJECT_NAME} catch2)


### Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#add_executable(tests
#	"tests/bez_test.cpp"
	
	#${SOURCES}
	#${INCLUDES}
#)

find_package(Vulkan REQUIRED)
target_link_libraries(tests PRIVATE ${Vulkan_LIBRARIES})
target_include_directories(tests PUBLIC "C:/VulkanSDK/1.3.261.1/Include")


target_link_libraries(tests PRIVATE glm)
target_link_libraries(tests PRIVATE glfw)
#target_link_libraries(tests PRIVATE imgui)
