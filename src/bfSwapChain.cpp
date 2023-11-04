#include "bfSwapChain.h"

BfEvent bfGetSwapChainSupport(VkPhysicalDevice physical_device, VkSurfaceKHR surface,BfSwapChainSupport& support)
{
	std::stringstream ss;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &support.capabilities);
	
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &formatCount, nullptr);

	ss << " Formats (" << formatCount << ")"

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
	



}
