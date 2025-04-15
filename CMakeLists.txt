cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(WIN32)
  # Проект точно собирается под x64-mingw-static - поэтому форсим всем
  # зависимости использовать именно его

  # TODO: добавить поддержку MSVC - это реально, но пока что не нужно
  set(VCPKG_TARGET_TRIPLET "x64-mingw-static")

  # Так как lapack не работает из коробки с MINGW, то используется OpenBLAS, но
  # так как в xtensor-blas - жестко прописан lapack - то таким образом можно
  # изменить это поведение, так как OpenBLAS содержит в себе lapack
  set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
                        ${CMAKE_MODULE_PATH})
endif()

set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")
set(CMAKE_SUPPRESS_REGENERATION TRUE)

set(CMAKE_CXX_FLAGS "-g -O0")
set(CMAKE_CXX_STANDARD 20)
add_compile_options(-std=c++20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(IS_BUILD_TESTS "build tests" ON)

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
  set(VULKAN_DIR "C:/VulkanSDK/1.4.304.0")
  set(VULKAN_SHADER_COMPILER "${VULKAN_DIR}/Bin/glslc.exe")
  message("Complier path: ${VULKAN_SHADER_COMPILER}")
endif(WIN32)

if(UNIX)
  set(VULKAN_DIR "VulkanSDK/1.4.304.0")
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
    "${CMAKE_SOURCE_DIR}/shaders/lines"
    "${CMAKE_SOURCE_DIR}/shaders/triangle"
    "${CMAKE_SOURCE_DIR}/shaders/axis")

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

      # Используем execute_process для компиляции шейдеров
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

      # Проверяем существование директории и создаем её
      set(TARGET_SHADER_DIR ${CMAKE_BINARY_DIR}/shaders/${LAST_FOLDER_NAME})
      file(MAKE_DIRECTORY ${TARGET_SHADER_DIR})
      message("Created dir: ${TARGET_SHADER_DIR}")

      # Копируем шейдеры в директорию
      file(COPY ${NEW_FILE_DIR}/${NEW_FILE_EXT}.spv
           DESTINATION ${TARGET_SHADER_DIR})
      message("Copied shader to: ${TARGET_SHADER_DIR}/${NEW_FILE_EXT}.spv")

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

add_subdirectory(external)

set(BF_BASE
    "include/base/bfVertex2.hpp"
    "src/base/bfBase.cpp"
    "src/base/bfBuffer.cpp"
    "src/base/bfDescriptor.cpp"
    "src/base/bfPhysicalDevice.cpp"
    "src/base/bfSwapChain.cpp"
    "src/base/bfUniforms.cpp"
    "src/base/bfHolder.cpp"
    "src/base/bfWindow.cpp"
    "src/base/bfPipeline.cpp"
    "src/base/bfCamera.cpp")
add_library(bfbaselib STATIC ${BF_BASE})
target_link_libraries(bfbaselib PUBLIC bfexternallib)

set(BF_GEOMETRY
    "include/geometry/bfCurves.hpp"
    "src/geometry/bfCurves3.cpp"
    "src/geometry/bfCurves4.cpp"
    "src/geometry/bfCurves3_gui.cpp"
    "src/geometry/bfBladeSection.cpp"
    "src/geometry/bfBladeSection2.cpp"
    "src/geometry/bfAxis.cpp"
    "src/geometry/bfObjectMath.cpp")

add_library(bfgeometrylib STATIC ${BF_GEOMETRY})
target_link_libraries(bfgeometrylib PUBLIC bfexternallib)

set(BF_INTERACTION
    "src/interaction/bfConsole.cpp"
    "src/interaction/bfEvent.cpp"
    "src/interaction/bfGui.cpp"
    "src/interaction/bfGuiFileDialog.cpp"
    "src/interaction/bfGuiLuaInteraction.cpp"
    "src/interaction/bfGuiCreateWindow.cpp"
    "src/interaction/bfGuiCreateWindowContainer.cpp"
    "src/interaction/bfGuiCreateWindowBladeSection.cpp"
    "src/interaction/bfGuiCreateWindowBladeBase.cpp"
    "src/interaction/bfGuiSmartLayerObserverWindow.cpp"
    "src/interaction/bfGuiCreateWindowSingleLine.cpp"
    "src/interaction/bfGuiCreateWindowCircleWithHandles.cpp"
    "src/interaction/bfGuiWindow.cpp"
    "src/interaction/bfGuiCamera.cpp")
add_library(bfinteractionlib STATIC ${BF_INTERACTION})
target_link_libraries(bfinteractionlib PUBLIC bfexternallib)
target_link_libraries(bfinteractionlib PUBLIC bfgeometrylib)

set(BF_OTHER
    # "include/other/BfExecutionTime.hpp"
    "include/other/bfMatrix.hpp"
    "include/other/bfVariative.hpp"
    "src/other/bfMatrix2.cpp"
    "include/other/bfMath.hpp"
    "src/other/bfMath.cpp"
    "src/other/bfTextureLoad.cpp"
    "src/other/bfConfigManager.cpp")

add_library(bfotherlib STATIC ${BF_OTHER})
target_link_libraries(bfotherlib PUBLIC bfexternallib)

set(BF_DRAW
    "src/draw/bfDrawObject.cpp" "src/draw/bfLayerHandler.cpp"
    "src/draw/bfLayerKiller.cpp" "src/draw/bfDrawObjectBuffer.cpp"
    "src/draw/bfDrawObject2.cpp")

add_library(bfdrawlib STATIC ${BF_DRAW})
target_link_libraries(bfdrawlib PUBLIC bfexternallib)

# Add source to this project's executable.
add_executable(${PROJECT_NAME} "main.h" "main.cpp" "src/bfMain.cpp"
                               "include/BfMain.h")

target_link_libraries(${PROJECT_NAME} PUBLIC bfbaselib)
target_link_libraries(${PROJECT_NAME} PUBLIC bfgeometrylib)
target_link_libraries(${PROJECT_NAME} PUBLIC bfotherlib)
target_link_libraries(${PROJECT_NAME} PUBLIC bfdrawlib)
target_link_libraries(${PROJECT_NAME} PUBLIC bfinteractionlib)
# target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES})

file(COPY "${CMAKE_SOURCE_DIR}/resources" DESTINATION "${CMAKE_BINARY_DIR}/")
file(COPY "${CMAKE_SOURCE_DIR}/scripts" DESTINATION "${CMAKE_BINARY_DIR}/")

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

if(WIN32)
  set(glfw3_DIR "C:/Program Files (x86)/GLFW/lib/cmake/glfw3")
endif(WIN32)

# CATCH 2 add_subdirectory(external/catch2) Tests
find_package(Catch2 REQUIRED)

add_library(bfgeometrytestlib STATIC ${BF_GEOMETRY})
target_compile_definitions(bfgeometrytestlib PUBLIC BF_CURVES4_NO_PIPELINE)
target_link_libraries(bfgeometrytestlib PUBLIC bfexternallib)

add_executable(
  tests #
  # "${CMAKE_SOURCE_DIR}/tests/own_api.cpp" #
  # "${CMAKE_SOURCE_DIR}/tests/main_api.cpp" #
  # "${CMAKE_SOURCE_DIR}/tests/draw_obj_test.cpp" #
  # "${CMAKE_SOURCE_DIR}/tests/curves.cpp" #
  # "${CMAKE_SOURCE_DIR}/tests/memory_test.cpp" #
  # "${CMAKE_SOURCE_DIR}/tests/matrix.cpp" #
  # "${CMAKE_SOURCE_DIR}/include/bfBase.h" #
  # "${CMAKE_SOURCE_DIR}/include/BfEvent.h" #
  # "${CMAKE_SOURCE_DIR}/src/bfEvent.cpp" #
  # "${CMAKE_SOURCE_DIR}/include/bfVariative.hpp" #
  # "${CMAKE_SOURCE_DIR}/src/bfBase.cpp" #
  # "${CMAKE_SOURCE_DIR}/include/bfWindow.h" #
  # "${CMAKE_SOURCE_DIR}/src/bfWindow.cpp" # "tests/bez_test.cpp" #
  # "${CMAKE_SOURCE_DIR}/tests/ImGUItest.cpp" #
  # "${CMAKE_SOURCE_DIR}/tests/matrix2.cpp"
  "include/other/bfMath.hpp" "src/other/bfMath.cpp" "./tests/object.cpp"
  "./tests/curves4.cpp" #
  "./tests/xtensor.cpp"
  # "tests/filedialog_test.cpp" # "${CMAKE_SOURCE_DIR}/src/bfWindow.cpp" )
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PUBLIC bfgeometrytestlib)
target_link_libraries(tests PUBLIC bfotherlib)
target_link_libraries(tests PUBLIC bfdrawlib)
target_link_libraries(tests PUBLIC bfinteractionlib)
# target_link_libraries(tests LINK_PUBLIC ${Boost_LIBRARIES})
target_link_libraries(tests PUBLIC bfexternallib)
target_link_libraries(tests PUBLIC bfbaselib)

foreach(subdir ${SUBDIRECTORIES})
  target_include_directories(tests PUBLIC include/${subdir})
endforeach()

# endif(${IS_BUILD_TESTS})
