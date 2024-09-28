cmake_minimum_required(VERSION 3.10)
project(bfexternal)

set(BF_IMGUI_DIR_NAME "imgui")

add_library(
  bfexternallib
  # ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imconfig.h
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui.h
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui_demo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui_draw.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui_internal.h
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui_tables.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui_widgets.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imstb_rectpack.h
  # ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imstb_textedit.h
  # ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imstb_truetype.h
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/backends/imgui_impl_vulkan.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/backends/imgui_impl_glfw.cpp)

target_include_directories(
  bfexternallib
  INTERFACE ${Vulkan_INCLUDE_DIRS}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/backends/)

set(BF_IMPLOT_DIR_NAME "implot")

target_include_directories(
  bfexternallib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMPLOT_DIR_NAME}/)

target_sources(
  bfexternallib
  INTERFACE # "implot/implot.h" "implot/implot_internal.h"
            ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMPLOT_DIR_NAME}/implot.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMPLOT_DIR_NAME}/implot_demo.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMPLOT_DIR_NAME}/implot_items.cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/splines-lib)
target_link_libraries(bfexternallib PUBLIC splines-lib)
target_include_directories(bfexternallib
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/splines-lib)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

target_include_directories(bfexternallib PUBLIC ${Vulkan_INCLUDE_DIRS}
                                                ${GLFW_INCLUDE_DIRS})
target_link_directories(bfexternallib PUBLIC ${Vulkan_LIBRARY_DIRS}
                        ${GLFW_LIBRARY_DIRS})
target_link_libraries(bfexternallib PUBLIC ${Vulkan_LIBRARIES}
                                           ${GLFW_LIBRARIES})

target_link_libraries(bfexternallib PUBLIC glfw)

find_package(Lua REQUIRED)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sol2)

target_link_libraries(bfexternallib PUBLIC sol2::sol2 ${LUA_LIBRARIES})
target_include_directories(bfexternallib PUBLIC ${LUA_INCLUDE_DIR})
