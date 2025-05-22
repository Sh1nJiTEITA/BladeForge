cmake_minimum_required(VERSION 3.10)
project(bfexternal)

list(APPEND CMAKE_PREFIX_PATH "~/.local/share/cmake/")

# IMGUI ===================================================================
add_library(
  imgui STATIC
  ./imgui/imgui.cpp ./imgui/imgui_draw.cpp ./imgui/imgui_widgets.cpp
  ./imgui/imgui_tables.cpp ./imgui/backends/imgui_impl_vulkan.cpp
  ./imgui/backends/imgui_impl_glfw.cpp)

target_sources(imgui STATIC ./imgui/backends/imgui_impl_vulkan.h
                     ./imgui/backends/imgui_impl_glfw.h)

target_include_directories(imgui PUBLIC ./imgui/)

# IMPLOT ===================================================================
add_library(implot STATIC ./implot/implot.cpp ./implot/implot_items.cpp)
target_include_directories(implot PUBLIC ./implot/)
add_library(bfexternallib INTERFACE)

target_link_libraries(bfexternallib INTERFACE implot)
target_link_libraries(bfexternallib INTERFACE imgui)

# Единственная non-vcpkg
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/splines-lib)
target_link_libraries(bfexternallib INTERFACE splines-lib)
target_include_directories(bfexternallib
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/splines-lib)

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
# find_package(implot CONFIG REQUIRED)
find_package(imgui REQUIRED)
find_package(xtl REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nfd CONFIG REQUIRED)

# Проблемы с MINGW laplas решаются так
if(WIN32)
  find_package(OpenBLAS CONFIG REQUIRED)

  set(LAPACK_LINKER_FLAGS "-lopenblas")
  set(LAPACK_LIBRARIES OpenBLAS::OpenBLAS)
  set(LAPACK_FOUND TRUE)
  set(BLAS_LIBRARIES OpenBLAS::OpenBLAS)
  set(BLAS_FOUND TRUE)
  set(LAPACK_INCLUDE_DIRS "")
  set(LAPACK_INCLUDE_DIR "")
else()
  find_package(OpenBLAS CONFIG REQUIRED)
endif()

find_package(xtensor REQUIRED)
find_package(xtensor-blas REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS bimap)
find_path(SOL2_INCLUDE_DIRS "sol/abort.hpp")

target_include_directories(bfexternallib INTERFACE ${SOL2_INCLUDE_DIRS})

if(WIN32)
  set(LUA_INCLUDE_DIR "C:/msys64/mingw64/include")
  set(LUA_LIBRARIES "C:/msys64/mingw64/lib/liblua.dll.a")
elseif(UNIX)
  find_package(Lua REQUIRED)
endif()
target_include_directories(bfexternallib INTERFACE ${LUA_INCLUDE_DIR})

set(IMGUI_IMPL_VULKAN
    ON
    CACHE BOOL "" FORCE)

target_link_libraries(
  bfexternallib
  INTERFACE xtl
            glfw
            Boost::bimap
            ${LUA_LIBRARIES}
            # implot::implot
            imgui::imgui
            nfd::nfd
            fmt::fmt
            OpenBLAS::OpenBLAS
            glm::glm
            xtensor::optimize
            xtensor-blas
            ${Vulkan_LIBRARIES}
            ${GLFW_LIBRARIES}
            GPUOpen::VulkanMemoryAllocator)
