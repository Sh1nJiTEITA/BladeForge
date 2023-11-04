#ifndef BF_SWAPCHAIN_H
#define BF_SWAPCHAIN_H

#include "bfVariative.hpp"
#include "bfEvent.h"


struct BfSwapChainSupport {
	VkSurfaceCapabilitiesKHR		capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>	presentModes;
};

BfEvent bfGetSwapChainSupport(VkPhysicalDevice physical_device, VkSurfaceKHR surface, BfSwapChainSupport& support);

#endif