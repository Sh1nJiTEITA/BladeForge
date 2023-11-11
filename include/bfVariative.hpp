#ifndef BF_VARIATIVE_HPP
#define BF_VARIATIVE_HPP
// Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include <fstream> // Read shaderfiles





// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines //

// Vulkan properties ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

// Store device extensions
const std::vector<const char*> bfvDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	//GL_KHR_vulkan_glsl
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Vulkan properties //


// Debug properties ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// Store validation layers here:
const std::vector<const char*> bfvValidationLayers = {
	"VK_LAYER_KHRONOS_validation"
};


static VKAPI_ATTR VkBool32 VKAPI_CALL bfvDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
)
{
	char* severity_type{};
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		severity_type = const_cast<char*>("[INFO]"); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		severity_type = const_cast<char*>("[VERB]"); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		severity_type = const_cast<char*>("[WARN]"); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		severity_type = const_cast<char*>("[ERR]"); break;
	}

	std::cerr << severity_type
		<< " Validation layer: "
		<< pCallbackData->pMessage
		<< std::endl;


	return VK_FALSE;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Debug properties //



// Window properties ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

inline void bfvSetGLFWProperties() {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Window properties //

// Queues ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
enum BfvEnQueueType {
	BF_QUEUE_GRAPHICS_TYPE,
	BF_QUEUE_COMPUTE_TYPE,
	BF_QUEUE_PRESENT_TYPE,
	BF_QUEUE_TRANSFER_TYPE
};

const std::set<BfvEnQueueType> bfvEnabledQueueTypes{
	BF_QUEUE_GRAPHICS_TYPE,
	//BF_QUEUE_COMPUTE_TYPE,
	BF_QUEUE_PRESENT_TYPE,
	BF_QUEUE_TRANSFER_TYPE
};
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Queues //





#endif