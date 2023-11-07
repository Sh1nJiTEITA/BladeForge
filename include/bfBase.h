#ifndef BF_BASE_H
#define BF_BASE_H



#include "bfVariative.hpp"
#include "bfEvent.h"
#include "bfWindow.h"
#include "bfPhysicalDevice.h"
#include "bfHolder.h"

//static std::vector<BfPhysicalDevice> bfPhysicalDeviceHolder{};
//static std::vector<BfWindow> bfWindowHolder{};

struct BfImagePack {
	VkImage* pImage;
	VkImageView* pImage_view;
};



struct BfBase {
	BfWindow*				 window;
	BfPhysicalDevice*		 physical_device;

	VkInstance				 instance;
	VkSurfaceKHR			 surface;
	VkDevice				 device;
	VkDebugUtilsMessengerEXT debug_messenger;

	VkSwapchainKHR			 swap_chain;
	VkFormat				 swap_chain_format;
	VkExtent2D				 swap_chain_extent;
	
	std::vector<BfImagePack> image_packs;
	uint32_t				 image_pack_count;

};

// Main functions
BfEvent bfCreateInstance(BfBase& base);
BfEvent bfCreateDebugMessenger(BfBase& base);
BfEvent bfCreateSurface(BfBase& base);
BfEvent bfCreatePhysicalDevice(BfBase& base);
BfEvent bfCreateLogicalDevice(BfBase& base);
BfEvent bfCreateSwapchain(BfBase& base);
BfEvent bfCreateImageViews(BfBase& base);

// Populate
void bfPopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

#endif
