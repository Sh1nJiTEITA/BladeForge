# CMakeList.txt : CMake project for ${PROJECT_NAME}, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.13 FATAL_ERROR)

include_directories(include)
#include_directories(src)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# Project Name
set(PROJECT_NAME "BladeForge")
project(${PROJECT_NAME})

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

### External libraries ### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Vulkan
set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.261.1")
find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC "C:/VulkanSDK/1.3.261.1/Include")



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



### Tests ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_executable(tests
	"tests/bez_test.cpp"
	
	${SOURCES}
	${INCLUDES}
)
target_include_directories(tests PUBLIC "C:/VulkanSDK/1.3.261.1/Include")
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
#target_link_libraries(tests PRIVATE Catch2::Catch2)
target_link_libraries(tests PRIVATE glm)
target_link_libraries(tests PRIVATE glfw)
#target_link_libraries(tests PRIVATE imgui)
