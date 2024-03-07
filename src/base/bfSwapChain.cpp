#include "bfSwapChain.h"

BfEvent bfGetSwapChainSupport(VkPhysicalDevice physical_device, VkSurfaceKHR surface,BfSwapChainSupport& support)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &support.capabilities);
	
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &formatCount, nullptr);


	if (formatCount != 0) {
		support.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &formatCount, support.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		support.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &presentModeCount, support.presentModes.data());
	}
	

	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
	event.action = BfEnActionType::BF_ACTION_TYPE_GET_SWAPCHAIN_SUPPORT;
	return BfEvent(event);
}

BfEvent bfGetSwapSurfaceFormat(BfSwapChainSupport& swapchain_support, VkSurfaceFormatKHR& surface_format)
{
	VkSurfaceFormatKHR return_format{};
	bool is_chosen = false;

	for (const auto& availableFormat : swapchain_support.formats) {
		if (
			availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
			) {
			is_chosen = true;
			return_format = availableFormat;
		}
	}

	BfSingleEvent event{};
	if (is_chosen) {
		surface_format = return_format;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_GET_SWAPCHAIN_SURFACE_FORMAT;
		event.info = "Needed format (by VVK_FORMAT_B8G8R8A8_SRGB &"
					 "VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) was found among formats";
	}
	else {
		surface_format = swapchain_support.formats[0];
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_GET_SWAPCHAIN_SURFACE_FORMAT;
		event.info = "Needed format (by VVK_FORMAT_B8G8R8A8_SRGB &"
					 "VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) wasn't found among formats, "
					 "chosen format is the first available";
	}

	
	return BfEvent(event);
}

BfEvent bfGetSwapPresentMode(BfSwapChainSupport& swapchain_support, VkPresentModeKHR& present_mode)
{
	VkPresentModeKHR return_present_mode;
	bool is_chosen = false;

	for (const auto& presentMode : swapchain_support.presentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return_present_mode = presentMode;
			is_chosen = true;
		}
	}
	
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
	event.action = BfEnActionType::BF_ACTION_TYPE_GET_SWAPCHAIN_SURFACE_FORMAT;

	if (is_chosen) {
		present_mode = return_present_mode;
		event.info = "Needed swapchain present mode was found"
					 "(VK_PRESENT_MODE_MAILBOX_KHR)";
	} 
	else {
		present_mode = VK_PRESENT_MODE_FIFO_KHR;
		event.info = "Need swapchain present mode wasn't found, "
					 "using present mode (VK_PRESENT_MODE_FIFO_KHR)";
	}

	return BfEvent(event);
}

BfEvent bfGetSwapExtent(BfSwapChainSupport& swapchain_support, GLFWwindow* window, VkExtent2D& extent)
{
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
	event.action = BfEnActionType::BF_ACTION_TYPE_GET_SWAPCHAIN_EXTENT;

	if (swapchain_support.capabilities.currentExtent.width != UINT32_MAX) {
		extent = swapchain_support.capabilities.currentExtent;
		std::stringstream ss;
		ss << "Current swapchain extent is OK, using extent = ("
		   << swapchain_support.capabilities.currentExtent.width << ", "
		   << swapchain_support.capabilities.currentExtent.height 
		   << ")";
		event.info = ss.str();
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height),
		};
		actualExtent.width = std::clamp(
			actualExtent.width,
			swapchain_support.capabilities.minImageExtent.width,
			swapchain_support.capabilities.maxImageExtent.width
		);
		actualExtent.height = std::clamp(
			actualExtent.height,
			swapchain_support.capabilities.minImageExtent.height,
			swapchain_support.capabilities.maxImageExtent.height
		);

		extent = actualExtent;

		std::stringstream ss;
		ss << "Current swapchain extent is not Ok, using fresh window extent = ("
			<< actualExtent.width << ", "
			<< actualExtent.height
			<< ")";
		event.info = ss.str();
	}
	
	return BfEvent(event);
}
