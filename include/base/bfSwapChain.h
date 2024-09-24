#ifndef BF_SWAPCHAIN_H
#define BF_SWAPCHAIN_H

#include "bfEvent.h"
#include "bfVariative.hpp"

#include <algorithm>

struct BfSwapChainSupport {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

BfEvent bfGetSwapChainSupport(VkPhysicalDevice physical_device,
                              VkSurfaceKHR surface,
                              BfSwapChainSupport &support);
BfEvent bfGetSwapSurfaceFormat(BfSwapChainSupport &swapchain_support,
                               VkSurfaceFormatKHR &surface_format);
BfEvent bfGetSwapPresentMode(BfSwapChainSupport &swapchain_support,
                             VkPresentModeKHR &present_mode);
BfEvent bfGetSwapExtent(BfSwapChainSupport &swapchain_support,
                        GLFWwindow *window, VkExtent2D &extent);

#endif
