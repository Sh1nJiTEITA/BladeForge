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

struct BfBase {
	static BfWindow*				window;
	static VkInstance				instance;
	static VkSurfaceKHR				surface;
	static BfPhysicalDevice*		physical_device;
	static VkDevice					device;
	static VkSwapchainKHR			swap_chain;
	static VkDebugUtilsMessengerEXT debug_messenger;
};

// Main functions
BfEvent bfCreateInstance();
BfEvent bfCreateDebugMessenger();
BfEvent bfCreateSurface();
BfEvent bfCreatePhysicalDevice();
BfEvent bfCreateLogicalDevice();
// Populate
void bfPopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

#endif
