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
