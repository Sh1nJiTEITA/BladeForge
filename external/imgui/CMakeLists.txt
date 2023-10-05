cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(imgui)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/backends)

add_library(imgui STATIC
    #imgui.h
    imgui.cpp
    #backends/imgui_impl_glfw.h
    backends/imgui_impl_glfw.cpp
    #backends/imgui_impl_vulkan.h
    backends/imgui_impl_vulkan.cpp
)


#target_include_directories(imgui PUBLIC
#    ${CMAKE_CURRENT_BINARY_DIR}/
#    ${CMAKE_CURRENT_BINARY_DIR}/backends/
#)

#target_include_directories(glad PUBLIC include/)
