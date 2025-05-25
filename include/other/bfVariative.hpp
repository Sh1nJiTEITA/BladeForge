#ifndef BF_VARIATIVE_HPP
#define BF_VARIATIVE_HPP
// Defines
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// //
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "implot.h"
// #include "implot_internal.h"
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __linux__
#define VK_USE_PLATFORM_XLIB_KHR
#define GLFW_EXPOSE_NATIVE_X11
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// #include "GLFW/glfw3native.h"

#define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
// #define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/printf.h>

#include <fstream> // Read shaderfiles
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <set>
#include <vector>

// #include "vma/vk_mem_alloc.h"
// #include "vk_mem_alloc.h"

// #define VMA_IMPLEMENTATION
#ifdef _WIN32
#include "vma/vk_mem_alloc.h"
#elif defined(__unix__) || defined(__linux__)
// #ifndef VMA_LOCAL_
// #define VMA_LOCAL_
//
// #define VMA_DEBUG_INITIALIZE_ALLOCATIONS 1
#include "vk_mem_alloc.h"
// #endif
#endif

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Defines //

// Vulkan properties
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

// Store device extensions
const std::vector< const char* > bfvDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,

    // GL_KHR_vulkan_glsl
};

const std::vector< VkValidationFeatureEnableEXT > bfvValidationFeatures = {
    VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Vulkan
// properties //

// Debug properties
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// Store validation layers here:
const std::vector< const char* > bfvValidationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL
bfvDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
)
{
   char* severity_type{};
   fmt::color color;

   switch (static_cast< int >(messageSeverity))
   {
   case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      severity_type = const_cast< char* >("VKINFO");
      color = fmt::color::green;
      break;
   case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      severity_type = const_cast< char* >("VKVERB");
      color = fmt::color::blue_violet;
      break;
   case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      severity_type = const_cast< char* >("VKWARN");
      color = fmt::color::yellow;
      break;
   case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      severity_type = const_cast< char* >("VKERR");
      color = fmt::color::indian_red;
      break;
   }

   fmt::print(
       stderr,
       fmt::emphasis::bold | fmt::fg(fmt::color::medium_orchid),
       "[{:%H:%M:%S}]=>[{}] ",
       fmt::localtime(std::time(nullptr)),
       severity_type
   );
   fmt::print(stderr, "{}\n", pCallbackData->pMessage);

   // fmt::print(
   //     stderr,
   //     "{} Validation layer: {}\n",
   //     severity_type,
   //     pCallbackData->pMessage
   // );

   return VK_FALSE;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Debug
// properties //

// Window properties
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

inline void
bfvSetGLFWProperties()
{
   glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
   glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
   glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
   glfwWindowHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
   glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Window
// properties //

// Queues
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// //
enum BfvEnQueueType
{
   BF_QUEUE_GRAPHICS_TYPE,
   BF_QUEUE_COMPUTE_TYPE,
   BF_QUEUE_PRESENT_TYPE,
   BF_QUEUE_TRANSFER_TYPE
};

const std::set< BfvEnQueueType > bfvEnabledQueueTypes{
    BF_QUEUE_GRAPHICS_TYPE,
    // BF_QUEUE_COMPUTE_TYPE,
    BF_QUEUE_PRESENT_TYPE,
    BF_QUEUE_TRANSFER_TYPE
};
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Queues //

static void
check_vk_result(VkResult err)
{
   if (err == 0)
      return;
   fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
   if (err < 0)
      abort();
};

#endif
