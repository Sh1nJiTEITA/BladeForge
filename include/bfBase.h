 #ifndef BF_BASE_H
#define BF_BASE_H



#include "bfVariative.hpp"
#include "bfEvent.h"
#include "bfWindow.h"
#include "bfPhysicalDevice.h"
#include "bfHolder.h"
#include "bfVertex2.hpp"
#include "bfUniforms.h"
#include "bfExecutionTime.hpp"

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
	void*					uniform_view_data;

	BfAllocatedBuffer*		uniform_view_buffer;
	BfAllocatedBuffer*		bezier_points_buffer;
	BfAllocatedBuffer*		model_matrix_buffer;

	VkCommandBuffer*		standart_command_buffer;
	VkCommandBuffer*		gui_command_buffer;

	VkSemaphore*			available_image_semaphore;
	VkSemaphore*			finish_render_image_semaphore;
	VkFence*				frame_in_flight_fence;
	
	VkDescriptorSet*		global_descriptor_set;
	VkDescriptorSet*		main_descriptor_set;
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

	BfAllocatedImage		 depth_image;
	VkImageView				 depth_image_view;
	VkFormat				 depth_format;

	std::vector<BfImagePack> image_packs;
	std::vector<BfFramePack> frame_pack;
	uint32_t				 image_pack_count;
	uint32_t				 current_image;
	uint32_t				 current_frame;

	VkRenderPass			 standart_render_pass;
	VkRenderPass			 gui_render_pass;
	VkPipeline				 triangle_pipeline;
	VkPipelineLayout		 triangle_pipeline_layout;
	VkPipeline				 line_pipeline;
	VkPipelineLayout		 line_pipeline_layout;

	VkDescriptorSetLayout	 global_set_layout;
	VkDescriptorSetLayout    main_set_layout;

	VkCommandPool			 command_pool;
	VkDescriptorPool		 standart_descriptor_pool;
	VkDescriptorPool		 gui_descriptor_pool;

	BfAllocatedBuffer		 dynamic_vertex_buffer;
	BfAllocatedBuffer		 dynamic_index_buffer;
	BfAllocatedBuffer		 bezier_properties_uniform_buffer;
	char*					 bezier_data;

	VmaAllocator			 allocator;
	
	float					 x_scale;
	float					 y_scale;
	float					 xyz_scale = 1; 

	float					 px;
	float				     py;
	float					 pz;

	std::vector<BfStorageBezierPoints> storage;

	uint32_t				 vert_number;

	//BfCurveSet* pBezier_set;
	//BfCurveSet* pLinear_set;

	bool is_resized;
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
BfEvent bfCreateDepthBuffer(BfBase& base);

void bfCreateAllocator(BfBase& base);
void bfAllocateBuffersForDynamicMesh(BfBase& base);
void bfUploadDynamicMesh(BfBase& base, BfMesh& mesh);
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
void bfDrawFrame(BfBase& base, BfMesh& mesh, BfMeshHandler& handler);
void bfMainRecordCommandBuffer(BfBase& base, BfMesh& mesh, BfMeshHandler& handler);

// Uniform buffers
size_t bfPadUniformBufferSize(const BfPhysicalDevice* physical_device, size_t original_size);
VkDescriptorSetLayoutBinding bfGetDescriptorSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t binding);
VkWriteDescriptorSet bfWriteDescriptorBuffer(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo, uint32_t binding);

// Depth buffers
VkImageCreateInfo bfPopulateDepthImageCreateInfo(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent);
VkImageViewCreateInfo bfPopulateDepthImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags);
VkPipelineDepthStencilStateCreateInfo bfPopulateDepthStencilStateCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);

// CleanUp's
BfEvent bfCleanUpSwapchain(BfBase& base);

void bfUpdateUniformBuffer(BfBase& base);


#endif
