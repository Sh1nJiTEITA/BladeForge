#ifndef BF_PHYSICAL_DEVICE
#define BF_PHYSICAL_DEVICE

#include "bfVariative.hpp"
#include "bfEvent.h"
#include "bfSwapChain.h"


#include <map>
#include <optional>
#include <vector>
#include <algorithm>


struct BfPhysicalDevice {
	VkPhysicalDevice physical_device;
	std::map<BfvEnQueueType, std::optional<uint32_t>> queue_family_indices;
	std::map<BfvEnQueueType, VkQueue> queues;

	VkPhysicalDeviceProperties properties;
	int holder_index;
};

BfEvent bfIsQueueFamilyIndicesCorrect(const BfPhysicalDevice* physical_device, bool& is_suitable);
BfEvent bfCheckDeviceExtensionSupport(BfPhysicalDevice* physical_device, bool& is_suitable);
BfEvent bfFindQueueFamilyIndices(BfPhysicalDevice* bf_physical_device, VkSurfaceKHR surface);
BfEvent bfIsDeviceSuitable(BfPhysicalDevice* bf_physical_device, VkSurfaceKHR surface, bool& is_suitable);

#endif
