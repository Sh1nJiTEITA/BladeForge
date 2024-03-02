# CMakeList.txt : CMake project for ${PROJECT_NAME}, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.13 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 20)
include_directories(include)
link_directories(include)
#include_directories(external/implot/)
#add_subdirectory(external/implot)
#include_directories(src)


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

# Shaders (PRE-BUIL D) 
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
	#"src/BladeForgeBin.cpp"
	#"src/bfVertex.cpp"
	

	"src/bfBase.cpp"
	"src/bfEvent.cpp" 
	"src/bfWindow.cpp" 
	"src/bfPhysicalDevice.cpp"
	"src/bfSwapChain.cpp"
	"src/bfConsole.cpp"
	"src/bfHolder.cpp"
	"src/bfBuffer.cpp"
	"src/bfUniforms.cpp" 
	"src/bfMain.cpp"
	"src/bfCurves2.cpp"
	"src/bfDrawObject.cpp"
	"src/bfDescriptor.cpp"
	"src/bfLayerHandler.cpp" 
	"src/bfGUI.cpp" 
	"src/bfCurves3.cpp"
	#"external/implot/implot.cpp"
    #"external/implot/implot_demo.cpp"
    #"external/implot/implot_items.cpp"
)

set(HEADERS
	
	
	"include/stb_image.h"
	"include/stb_image_write.h"

	#"include/BladeForgeBin.h"	
	"include/bfVariative.hpp"
	"include/bfVertex2.hpp"
	"include/BfExecutionTime.hpp"
	"include/BfCurves.hpp"
	"include/bfMatrix.hpp"
	#"include/bfVertex.h"
	"include/bfBase.h"
	"include/bfEvent.h" 
	"include/bfWindow.h"
	"include/bfPhysicalDevice.h"
	"include/bfSwapChain.h"
	"include/bfConsole.h"
	"include/bfHolder.h"
	"include/bfBuffer.h"
	"include/bfUniforms.h"
	"include/BfMain.h"
	"include/bfCurves2.h"
	"include/bfDrawObject.h"
	"include/bfDescriptor.h"
	"include/bfLayerHandler.h"
	"include/bfGUI.h" 
	"include/bfCurves3.h"
	"main.h"
)
# Add source to this project's executable.
add_executable(${PROJECT_NAME} 
	"main.cpp"

	#"external/implot/implot.cpp"
    #"external/implot/implot_demo.cpp"
    #"external/implot/implot_items.cpp"
	#"external/implot/implot.h"
    #"external/implot/implot_internal.h"

		
	

	${SOURCES}
	${HEADERS}
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()


# Copies allShaderFiles to binary.


file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
file(GLOB SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.spv)
foreach(shaderi ${SHADERS})
	file(COPY ${shaderi} DESTINATION ${CMAKE_BINARY_DIR}/shaders/)
	message("-- [SHADER-COPY] File ${shaderi} is copied to binary dir.")
endforeach()


### External libraries ### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Vulkan
set(ENV{VULKAN_SDK} ${VULKAN_DIR})
find_package(Vulkan REQUIRED)
#target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC "${VULKAN_DIR}/Include")



# GLFW LIBRARY
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
#target_link_libraries(${PROJECT_NAME} glfw)

# GLM
set(IMGUI_IMPL_VULKAN ON CACHE BOOL "" FORCE)

add_subdirectory(external/glm)
#target_link_libraries(${PROJECT_NAME} glm)

# imgui
#TODO: Add another implementation
#set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui/)
#include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)
#set(imgui-glfw imglfw CACHE STRING "" FORCE)
add_subdirectory(external/VulkanMemoryAllocator)
target_link_libraries(${PROJECT_NAME} PRIVATE VulkanMemoryAllocator)


#add_subdirectory(external/FreeImage)

# if you want RGB color order instead of BGR
#target_compile_definitions(FreeImage PUBLIC FREEIMAGE_COLORORDER=1)

# ...

#target_link_libraries(${PROJECT_NAME} PUBLIC FreeImage)
#target_include_directories(${PROJECT_NAME} PUBLIC external/FreeImage/Source)
# implot
#add_subdirectory(external/implot)
#target_link_libraries(${PROJECT_NAME} PUBLIC implot)



add_subdirectory(external/imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui imgui-vulkan imgui-glfw)

# CATCH 2
add_subdirectory(external/catch2)

#add_subdirectory(external/simple_matrix)
#target_link_libraries(${PROJECT_NAME} PUBLIC SimpleMatrix)
#target_link_libraries(${PROJECT_NAME} PUBLIC armadillo)

#add_subdirectory(external/eigen-3.4.0)
#find_package(Eigen3 3.3 REQUIRED NO_MODULE)
 
#target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)

#target_link_libraries(${PROJECT_NAME} PUBLIC Eigen)

#add_subdirectory(external/armadillo)
#target_link_libraries(${PROJECT_NAME} PUBLIC Eigen)

#find_package(catch2)
#add_executable(tests ${CMAKE_SOURCE_DIR}/tests/bez_test.cpp)


#target_link_libraries(${PROJECT_NAME} catch2)


### Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_executable(tests
	#"${CMAKE_SOURCE_DIR}/tests/own_api.cpp"
	#"${CMAKE_SOURCE_DIR}/tests/main_api.cpp"
	#"${CMAKE_SOURCE_DIR}/tests/draw_obj_test.cpp"
	#"${CMAKE_SOURCE_DIR}/tests/curves.cpp"
	#"${CMAKE_SOURCE_DIR}/tests/memory_test.cpp"
	#"${CMAKE_SOURCE_DIR}/include/bfBase.h"
	#"${CMAKE_SOURCE_DIR}/include/BfEvent.h"
	#"${CMAKE_SOURCE_DIR}/src/bfEvent.cpp"
	#"${CMAKE_SOURCE_DIR}/include/bfVariative.hpp"
	#"${CMAKE_SOURCE_DIR}/src/bfBase.cpp"
	
	#"${CMAKE_SOURCE_DIR}/include/bfWindow.h"
	#"${CMAKE_SOURCE_DIR}/src/bfWindow.cpp"
	#"tests/bez_test.cpp"
	#"${CMAKE_SOURCE_DIR}/tests/ImGUItest.cpp"

		
	


	${SOURCES}
	${INCLUDES}
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE VulkanMemoryAllocator)
#find_package(Vulkan REQUIRED)
#target_link_libraries(tests PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(tests PRIVATE imgui imgui-vulkan imgui-glfw)
#target_link_libraries(tests PUBLIC implot)
target_include_directories(tests PUBLIC "C:/VulkanSDK/1.3.261.1/Include")
#target_link_libraries(tests PUBLIC SimpleMatrix)

#target_link_libraries(tests PUBLIC Eigen3::Eigen)
#target_link_libraries(tests PUBLIC armadillo)

target_link_libraries(tests PRIVATE glm)
#target_link_libraries(tests PRIVATE glfw)
#target_link_libraries(tests PRIVATE imgui)
