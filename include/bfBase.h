#ifndef BF_BASE_H
#define BF_BASE_H



#include "bfVariative.hpp"
#include "bfEvent.h"
#include "bfWindow.h"
#include "bfPhysicalDevice.h"
#include "bfHolder.h"
#include "bfVertex2.hpp"
#include "bfUniforms.h"


//static std::vector<BfPhysicalDevice> bfPhysicalDeviceHolder{};
//static std::vector<BfWindow> bfWindowHolder{};

static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

struct BfImagePack {
	VkImage*				pImage;
	VkImageView*			pImage_view;
	VkFramebuffer*			pStandart_Buffer;
	VkFramebuffer*			pGUI_buffer;
};

struct BfFramePack {
	BfAllocatedBuffer*		uniform_view_buffer;
	void*					uniform_view_data;
	
	VkCommandBuffer*		standart_command_buffer;
	VkCommandBuffer*		gui_command_buffer;

	VkSemaphore*			available_image_semaphore;
	VkSemaphore*			finish_render_image_semaphore;
	VkFence*				frame_in_flight_fence;
	
	VkDescriptorSet*		global_descriptor_set;
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
	std::vector<BfFramePack> frame_pack;
	uint32_t				 image_pack_count;
	uint32_t				 current_image;
	uint32_t				 current_frame;

	VkRenderPass			 standart_render_pass;
	VkRenderPass			 gui_render_pass;
	VkDescriptorSetLayout	 global_set_layout;
	VkPipeline				 triangle_pipeline;
	VkPipelineLayout		 triangle_pipeline_layout;
	VkPipeline				 line_pipeline;
	VkPipelineLayout		 line_pipeline_layout;

	VkCommandPool			 command_pool;
	VkDescriptorPool		 standart_descriptor_pool;
	VkDescriptorPool		 gui_descriptor_pool;

	VmaAllocator			 allocator;
	
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
BfEvent bfInitDescriptors(BfBase& base); //
BfEvent bfCreateGraphicsPipelines(BfBase& base, std::string vert_shader_path, std::string frag_shader_path);
BfEvent bfCreateStandartFrameBuffers(BfBase& base);
BfEvent bfCreateGUIFrameBuffers(BfBase& base);
BfEvent bfCreateCommandPool(BfBase& base);
BfEvent bfCreateUniformBuffers(BfBase& base);
BfEvent bfCreateStandartDescriptorPool(BfBase& base);
BfEvent bfCreateGUIDescriptorPool(BfBase& base);
BfEvent bfCreateDescriptorSets(BfBase& base);
BfEvent bfCreateStandartCommandBuffers(BfBase& base);
BfEvent bfCreateGUICommandBuffers(BfBase& base);
BfEvent bfCreateSyncObjects(BfBase& base);
BfEvent bfInitImGUI(BfBase& base);

void bfCreateAllocator(BfBase& base);
void bfUploadMesh(BfBase& base, BfMesh& mesh);
void bfUploadVertices(BfBase& base, BfMesh& mesh);
void bfUploadIndices(BfBase& base, BfMesh& mesh);

// Populate
void bfPopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);


// Additional
BfEvent bfaReadFile(std::vector<char>& data, const std::string& filename);
BfEvent bfaCreateShaderModule(VkShaderModule& module, VkDevice device, const std::vector<char>& data);
BfEvent bfaCreateGraphicsPipelineLayouts(BfBase& base);
BfEvent bfaRecreateSwapchain(BfBase& base);

void bfBeginSingleTimeCommands(BfBase& base, VkCommandBuffer& commandBuffer);
void bfEndSingleTimeCommands(BfBase& base, VkCommandBuffer& commandBuffer);
void bfDrawFrame(BfBase& base, BfMesh& mesh);
void bfMainRecordCommandBuffer(BfBase& base, BfMesh& mesh);

// CleanUp's
BfEvent bfCleanUpSwapchain(BfBase& base);

void bfUpdateUniformBuffer(BfBase& base);


#endif
