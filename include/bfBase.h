#ifndef BF_BASE_H
#define BF_BASE_H



#include "bfVariative.hpp"
#include "bfEvent.h"
#include "bfWindow.h"
#include "bfPhysicalDevice.h"
#include "bfHolder.h"
#include "bfVertex.h"

//static std::vector<BfPhysicalDevice> bfPhysicalDeviceHolder{};
//static std::vector<BfWindow> bfWindowHolder{};

struct BfImagePack {
	VkImage* pImage;
	VkImageView* pImage_view;
	VkFramebuffer* pStandart_Buffer;
	VkFramebuffer* pGUI_buffer;
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
							 
	VkRenderPass			 standart_render_pass;
	VkRenderPass			 gui_render_pass;
	VkDescriptorSetLayout	 descriptor_set_layout;
	VkPipeline				 triangle_pipeline;
	VkPipelineLayout		 triangle_pipeline_layout;
	VkPipeline				 line_pipeline;
	VkPipelineLayout		 line_pipeline_layout;

	VkCommandPool			 command_pool;
};

// Main functions
BfEvent bfCreateInstance(BfBase& base);
BfEvent bfCreateDebugMessenger(BfBase& base);
BfEvent bfCreateSurface(BfBase& base);
BfEvent bfCreatePhysicalDevice(BfBase& base);
BfEvent bfCreateLogicalDevice(BfBase& base);
BfEvent bfCreateSwapchain(BfBase& base);
BfEvent bfCreateImageViews(BfBase& base);
BfEvent bfCreateStandartRenderPass(BfBase& base);
BfEvent bfCreateGUIRenderPass(BfBase& base);
BfEvent bfCreateDescriptorSetLayout(BfBase& base);
BfEvent bfCreateGraphicsPipelines(BfBase& base, std::string vert_shader_path, std::string frag_shader_path);
BfEvent bfCreateStandartFrameBuffers(BfBase& base);
BfEvent bfCreateGUIFrameBuffers(BfBase& base);
BfEvent bfCreateCommandPool(BfBase& base);


// Populate
void bfPopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);


// Additional
BfEvent bfaReadFile(std::vector<char>& data, const std::string& filename);
BfEvent bfaCreateShaderModule(VkShaderModule& module, VkDevice device, const std::vector<char>& data);
BfEvent bfaCreateGraphicsPipelineLayouts(BfBase& base);

#endif
