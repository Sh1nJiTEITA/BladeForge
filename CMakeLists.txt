cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(-std=c++20) # optional to use c++11

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(
  GLOB SUBDIRECTORIES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*)
list(FILTER SUBDIRECTORIES EXCLUDE REGEX ".\\.h$")
list(FILTER SUBDIRECTORIES EXCLUDE REGEX ".\\.cpp$")
list(FILTER SUBDIRECTORIES EXCLUDE REGEX ".\\.hpp$")

foreach(subdir ${SUBDIRECTORIES})
  message("Subfolder found: ${subdir}")
  include_directories(include/${subdir})
endforeach()

include_directories(include)

if(WIN32)
  if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
    )
  endif()
endif(WIN32)
# VULKAN PATH
if(WIN32)
  set(VULKAN_DIR "C:/VulkanSDK/1.3.261.1")
  set(VULKAN_SHADER_COMPILER "${VULKAN_DIR}/Bin/glslc.exe")
endif(WIN32)

if(UNIX)
  set(VULKAN_DIR "VulkanSDK/1.3.261.1")
  set(VULKAN_SHADER_COMPILER "/usr/bin/glslc")
  add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
endif(UNIX)

# Project Name
set(PROJECT_NAME "BladeForge")
project(${PROJECT_NAME})

# Shaders (PRE-BUIL D) Shader files must be compiled from GLSL to SPIR-V before
# build is done.

set(SHADER_PACK_PATHS
    "${CMAKE_SOURCE_DIR}/shaders/basic"
    "${CMAKE_SOURCE_DIR}/shaders/thick_lines"
    "${CMAKE_SOURCE_DIR}/shaders/lines")

if(WIN32)
  if(EXISTS ${VULKAN_SHADER_COMPILER})
    set(SHADER_EXTENSIONS
        "*.vert"
        "*.frag"
        "*.geom"
        "*.glsl"
        "*.comp"
        "*.tesse"
        "*.tessc")

    set(ALL_SHADER_FILES "")
    foreach(SHADER_DIR_PATH ${SHADER_PACK_PATHS})

      set(ALL_LOCAL_SHADER_FILES "")

      foreach(SHADER_EXT ${SHADER_EXTENSIONS})

        file(GLOB_RECURSE SHADER_FILES_TEMP "${SHADER_DIR_PATH}/${SHADER_EXT}")
        list(APPEND ALL_LOCAL_SHADER_FILES ${SHADER_FILES_TEMP})

      endforeach()

      set(LOCAL_MESSAGE
          "[SHADER_PATH_SEARCH] Found shader files in dir ${SHADER_DIR_PATH}:")

      foreach(SHADER_FILE ${ALL_LOCAL_SHADER_FILES})
        # message(STATUS ${SHADER_FILE})
        get_filename_component(FILE_NAME ${SHADER_FILE} NAME)
        string(APPEND LOCAL_MESSAGE " ${FILE_NAME}")
      endforeach()

      message(${LOCAL_MESSAGE})
      list(APPEND ALL_SHADER_FILES ${ALL_LOCAL_SHADER_FILES})
    endforeach()

    foreach(SHADER_FILE ${ALL_SHADER_FILES})
      get_filename_component(NEW_FILE_EXT ${SHADER_FILE} EXT)
      string(REPLACE "." "" NEW_FILE_EXT ${NEW_FILE_EXT})
      get_filename_component(NEW_FILE_DIR ${SHADER_FILE} DIRECTORY)

      execute_process(
        COMMAND
          cmd /C
          "${VULKAN_SHADER_COMPILER} ${SHADER_FILE} -o ${NEW_FILE_DIR}/${NEW_FILE_EXT}.spv"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE command_result
        OUTPUT_VARIABLE command_output)
      if(command_result EQUAL 0)
        message(
          "[SHADER_COMPILE_SUCCESS] Shader file ${SHADER_FILE} was compiled to ${NEW_FILE_DIR}/${NEW_FILE_EXT}.spv"
        )
      else()
        message(
          "[SHADER_COMPILE_ERROR] Shader compile failed with error code: ${command_result}"
        )
        message("[SHADER_COMPILE_ERROR] Shader output: ${command_output}")
      endif()

      get_filename_component(LAST_FOLDER_NAME ${NEW_FILE_DIR} NAME)

      file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders/${LAST_FOLDER_NAME})
      file(COPY ${NEW_FILE_DIR}/${NEW_FILE_EXT}.spv
           DESTINATION ${CMAKE_BINARY_DIR}/shaders/${LAST_FOLDER_NAME})

    endforeach()
  else()
    message(
      FATAL_ERROR
        "Vulkan shader compiler wasn't found at path ${VULKAN_SHADER_COMPILER}")
  endif()
endif(WIN32)

if(UNIX)
  if(EXISTS ${VULKAN_SHADER_COMPILER})
    set(SHADER_EXTENSIONS
        "*.vert"
        "*.frag"
        "*.geom"
        "*.glsl"
        "*.comp"
        "*.tesse"
        "*.tessc")

    set(ALL_SHADER_FILES "")
    foreach(SHADER_DIR_PATH ${SHADER_PACK_PATHS})

      set(ALL_LOCAL_SHADER_FILES "")

      foreach(SHADER_EXT ${SHADER_EXTENSIONS})

        file(GLOB_RECURSE SHADER_FILES_TEMP "${SHADER_DIR_PATH}/${SHADER_EXT}")
        list(APPEND ALL_LOCAL_SHADER_FILES ${SHADER_FILES_TEMP})

      endforeach()

      set(LOCAL_MESSAGE
          "[SHADER_PATH_SEARCH] Found shader files in dir ${SHADER_DIR_PATH}:")

      foreach(SHADER_FILE ${ALL_LOCAL_SHADER_FILES})
        # message(STATUS ${SHADER_FILE})
        get_filename_component(FILE_NAME ${SHADER_FILE} NAME)
        string(APPEND LOCAL_MESSAGE " ${FILE_NAME}")
      endforeach()

      message(${LOCAL_MESSAGE})
      list(APPEND ALL_SHADER_FILES ${ALL_LOCAL_SHADER_FILES})
    endforeach()

    foreach(SHADER_FILE ${ALL_SHADER_FILES})
      get_filename_component(NEW_FILE_EXT ${SHADER_FILE} EXT)
      string(REPLACE "." "" NEW_FILE_EXT ${NEW_FILE_EXT})
      get_filename_component(NEW_FILE_DIR ${SHADER_FILE} DIRECTORY)

      execute_process(
        COMMAND
          sh -c
          "${VULKAN_SHADER_COMPILER} ${SHADER_FILE} -o ${NEW_FILE_DIR}/${NEW_FILE_EXT}.spv"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE command_result
        OUTPUT_VARIABLE command_output)
      if(command_result EQUAL 0)
        message(
          "[SHADER_COMPILE_SUCCESS] Shader file ${SHADER_FILE} was compiled to ${NEW_FILE_DIR}/${NEW_FILE_EXT}.spv"
        )
      else()
        message(
          "[SHADER_COMPILE_ERROR] Shader compile failed with error code: ${command_result}"
        )
        message("[SHADER_COMPILE_ERROR] Shader output: ${command_output}")
      endif()

      get_filename_component(LAST_FOLDER_NAME ${NEW_FILE_DIR} NAME)

      file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders/${LAST_FOLDER_NAME})
      file(COPY ${NEW_FILE_DIR}/${NEW_FILE_EXT}.spv
           DESTINATION ${CMAKE_BINARY_DIR}/shaders/${LAST_FOLDER_NAME})

    endforeach()
  else()
    message(
      FATAL_ERROR
        "Vulkan shader compiler wasn't found at path ${VULKAN_SHADER_COMPILER}")
  endif()
endif()

set(BF_BASE
    "include/base/bfVertex2.hpp"
    "include/base/bfBase.h"
    "src/base/bfBase.cpp"
    "include/base/bfBuffer.h"
    "src/base/bfBuffer.cpp"
    "include/base/bfDescriptor.h"
    "src/base/bfDescriptor.cpp"
    "include/base/bfPhysicalDevice.h"
    "src/base/bfPhysicalDevice.cpp"
    "include/base/bfSwapChain.h"
    "src/base/bfSwapChain.cpp"
    "include/base/bfUniforms.h"
    "src/base/bfUniforms.cpp"
    "include/base/bfHolder.h"
    "src/base/bfHolder.cpp"
    "include/base/bfWindow.h"
    "src/base/bfWindow.cpp")

set(BF_GEOMETRY
    "include/geometry/bfCurves.hpp"
    "include/geometry/bfCurves2.h"
    "src/geometry/bfCurves2.cpp"
    "include/geometry/bfCurves3.h"
    "src/geometry/bfCurves3.cpp"
    "include/geometry/bfBladeSection.h"
    "src/geometry/bfBladeSection.cpp")

set(BF_INTERACTION
    "include/interaction/bfConsole.h" "src/interaction/bfConsole.cpp"
    "include/interaction/bfEvent.h" "src/interaction/bfEvent.cpp"
    "include/interaction/bfGui.h" "src/interaction/bfGui.cpp")

set(BF_OTHER
    # "include/other/BfExecutionTime.hpp"
    "include/other/bfMatrix.hpp"
    "include/other/bfVariative.hpp"
    "include/other/bfMatrix2.h"
    "src/other/bfMatrix2.cpp"
    "include/other/stb_image.h"
    "include/other/stb_image_write.h"
    "include/other/bfMath.hpp"
    "src/other/bfMath.cpp"
    "include/other/bfTextureLoad.h"
    "src/other/bfTextureLoad.cpp"
    "include/other/bfIconsFontAwesome6.h"
    "include/other/bfConfigManager.h"
    "src/other/bfConfigManager.cpp")

set(BF_FORMS "include/other/config_forms/bfFormGui.h")

set(BF_DRAW
    "include/draw/bfDrawObject.h"
    "src/draw/bfDrawObject.cpp"
    "include/draw/bfLayerHandler.h"
    "src/draw/bfLayerHandler.cpp"
    "include/draw/bfDrawObjectDefineType.h"
    "include/draw/bfLayerKiller.h"
    "src/draw/bfLayerKiller.cpp")

# Headers/Sources
set(SOURCES "src/bfMain.cpp" "include/BfMain.h")

list(
  APPEND
  SOURCES
  ${BF_BASE}
  ${BF_GEOMETRY}
  ${BF_INTERACTION}
  ${BF_OTHER}
  ${BF_DRAW}
  ${BF_OTHER}
  ${BF_FORMS})

# Add source to this project's executable.
add_executable(${PROJECT_NAME} "main.h" "main.cpp" ${SOURCES})

file(COPY "${CMAKE_SOURCE_DIR}/resources" DESTINATION "${CMAKE_BINARY_DIR}/")
file(COPY "${CMAKE_SOURCE_DIR}/scripts" DESTINATION "${CMAKE_BINARY_DIR}/")

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

if(WIN32)
  set(glfw3_DIR "C:/Program Files (x86)/GLFW/lib/cmake/glfw3")
endif(WIN32)

# find_package(glfw3 REQUIRED) find_package(Vulkan REQUIRED)
#
# include_directories(${Vulkan_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})
# link_directories(${Vulkan_LIBRARY_DIRS} ${GLFW_LIBRARY_DIRS})
# target_link_libraries(BladeForge PRIVATE ${Vulkan_LIBRARIES})
#
# target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
# target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})

add_subdirectory(external)
target_link_libraries(${PROJECT_NAME} PUBLIC bfexternallib)

find_package(Catch2 REQUIRED)

# GLM
set(IMGUI_IMPL_VULKAN
    ON
    CACHE BOOL "" FORCE)

# add_subdirectory(external/glm)
find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

if(WIN32)
  set(VulkanMemoryAllocator_DIR
      "C:/Program Files (x86)/VMA/share/cmake/VulkanMemoryAllocator/")
endif(WIN32)

find_package(VulkanMemoryAllocator CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GPUOpen::VulkanMemoryAllocator)

# add_subdirectory(external/splines-lib) target_link_libraries(${PROJECT_NAME}
# PRIVATE splines-lib)

# add_subdirectory(external/imgui_doc) target_link_libraries(${PROJECT_NAME}
# PUBLIC imgui imgui-vulkan imgui-glfw)

# CATCH 2 add_subdirectory(external/catch2)

# Tests
# ~~~
add_executable(
  tests
  # "${CMAKE_SOURCE_DIR}/tests/own_api.cpp"
  # "${CMAKE_SOURCE_DIR}/tests/main_api.cpp"
  # "${CMAKE_SOURCE_DIR}/tests/draw_obj_test.cpp"
  # "${CMAKE_SOURCE_DIR}/tests/curves.cpp"
  # "${CMAKE_SOURCE_DIR}/tests/memory_test.cpp"
  # "${CMAKE_SOURCE_DIR}/tests/matrix.cpp"
  # "${CMAKE_SOURCE_DIR}/include/bfBase.h"
  # "${CMAKE_SOURCE_DIR}/include/BfEvent.h"
  # "${CMAKE_SOURCE_DIR}/src/bfEvent.cpp"
  # "${CMAKE_SOURCE_DIR}/include/bfVariative.hpp"
  # "${CMAKE_SOURCE_DIR}/src/bfBase.cpp"
  # "${CMAKE_SOURCE_DIR}/include/bfWindow.h"
  # "${CMAKE_SOURCE_DIR}/src/bfWindow.cpp"
  # "tests/bez_test.cpp"
  # "${CMAKE_SOURCE_DIR}/tests/ImGUItest.cpp"
  # "${CMAKE_SOURCE_DIR}/tests/matrix2.cpp" "include/other/bfMath.hpp"
  # "src/other/bfMath.cpp"
  "tests/sol2_lua_test.cpp"
  # "${CMAKE_SOURCE_DIR}/src/bfWindow.cpp"
  ${SOURCES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
# target_link_libraries(tests PRIVATE GPUOpen::VulkanMemoryAllocator)
# target_link_libraries(tests PRIVATE ${Vulkan_LIBRARIES} ${GLFW_LIBRARIES})
# target_link_libraries(tests PRIVATE imgui imgui-vulkan imgui-glfw)

target_link_libraries(tests PUBLIC bfexternallib)
target_include_directories(tests PUBLIC "C:/VulkanSDK/1.3.261.1/Include")
# target_link_libraries(tests PRIVATE splines-lib)

include_directories(external/eigen)
include_directories(external/splines-lib)
# target_link_libraries(tests PRIVATE glm)
target_link_libraries(tests PRIVATE glm::glm)
