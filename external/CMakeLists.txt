cmake_minimum_required(VERSION 3.10)
project(bfexternal)

set(BF_IMGUI_DIR_NAME "imgui")

add_library(
  bfexternallib
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui_demo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui_tables.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/imgui_widgets.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/backends/imgui_impl_vulkan.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/backends/imgui_impl_glfw.cpp)

target_include_directories(
  bfexternallib
  INTERFACE ${Vulkan_INCLUDE_DIRS}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${BF_IMGUI_DIR_NAME}/backends/
  PUBLIC C:/VulkanSDK/1.4.304.0/Include
)

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




if(WIN32)
  set(LUA_INCLUDE_DIR "C:/msys64/mingw64/include")
  set(LUA_LIBRARIES "C:/msys64/mingw64/lib/liblua.dll.a")
elseif(UNIX) 
  find_package(Lua REQUIRED)
endif()


target_link_libraries(bfexternallib PUBLIC sol2::sol2 ${LUA_LIBRARIES})
target_include_directories(bfexternallib PUBLIC ${LUA_INCLUDE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sol2)

set(IMGUI_IMPL_VULKAN
    ON
    CACHE BOOL "" FORCE)

find_package(glm CONFIG REQUIRED)
target_link_libraries(bfexternallib  PRIVATE glm::glm)

if(WIN32)
  set(VulkanMemoryAllocator_DIR
      "C:/Program Files (x86)/VMA/share/cmake/VulkanMemoryAllocator/")
endif(WIN32)

find_package(VulkanMemoryAllocator CONFIG REQUIRED)
target_link_libraries(bfexternallib PRIVATE GPUOpen::VulkanMemoryAllocator)
