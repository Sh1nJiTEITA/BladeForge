cmake_minimum_required(VERSION 3.10)
project(bfexternal)

add_library(bfexternallib INTERFACE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/splines-lib)
target_link_libraries(bfexternallib INTERFACE splines-lib)
target_include_directories(bfexternallib
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/splines-lib)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)
find_package(implot REQUIRED)

find_path(SOL2_INCLUDE_DIRS "sol/abort.hpp")
target_include_directories(bfexternallib INTERFACE ${SOL2_INCLUDE_DIRS})

# target_include_directories(bfexternallib INTERFACE ${Vulkan_INCLUDE_DIRS}
# ${GLFW_INCLUDE_DIRS}) target_link_directories(bfexternallib INTERFACE
# ${Vulkan_LIBRARY_DIRS} ${GLFW_LIBRARY_DIRS})

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

find_package(glm CONFIG REQUIRED)
find_package(xtensor CONFIG REQUIRED)
find_package(xtensor-blas CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS bimap)
find_library(BLAS_LIB NAMES openblas)

target_link_libraries(
  bfexternallib
  INTERFACE glfw
            Boost::bimap
            # sol2::sol2
            ${LUA_LIBRARIES}
            implot::implot
            imgui::imgui
            glm::glm
            xtensor::optimize
            xtensor-blas
            ${BLAS_LIB}
            ${Vulkan_LIBRARIES}
            ${GLFW_LIBRARIES}
            GPUOpen::VulkanMemoryAllocator)

# if(WIN32) set(VulkanMemoryAllocator_DIR "C:/Program Files
# (x86)/VMA/share/cmake/VulkanMemoryAllocator/") endif(WIN32)
