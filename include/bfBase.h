#ifndef BF_BASE_H
#define BF_BASE_H



#include "bfVariative.hpp"
#include "bfEvent.h"
#include "bfWindow.h"
#include "bfPhysicalDevice.h"
/*
	void BladeForge::run()
{
	initWindow();
	initVulkan();
	initImGUI();
	mainLoop();
	cleanup();
}


void BladeForge::initVulkan()
{
	createInstance();
 	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createImGUIRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();

	createFramebuffers();
	createImGUIFrameBuffers();

	createCommandPool();
	createVertexbuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createImGUICommandBuffers();
	createSyncObjects();
}


*/
static std::vector<BfPhysicalDevice> bfPhysicalDeviceHolder{};
static std::vector<BfWindow> bfWindowHolder{};




struct BfBase {
	BfWindow*				 window;
	VkInstance				 instance;
	VkSurfaceKHR			 surface;
	BfPhysicalDevice*		 physical_device;
	VkDevice				 device;
	VkSwapchainKHR			 swap_chain;
	VkDebugUtilsMessengerEXT debug_messenger;
};

// Main functions
BfEvent bfCreateInstance(BfBase& base);
BfEvent bfCreateDebugMessenger(BfBase& base);
BfEvent bfCreateSurface(BfBase& base);
BfEvent bfCreatePhysicalDevice(BfBase& base);
BfEvent bfCreateLogicalDevice();
// Populate
void bfPopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

#endif
