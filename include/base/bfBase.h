#ifndef BF_BASE_H
#define BF_BASE_H

#include <filesystem>

#include "bfDescriptor.h"
#include "bfEvent.h"
#include "bfHolder.h"
#include "bfIconsFontAwesome6.h"
#include "bfLayerHandler.h"
#include "bfLayerKiller.h"
#include "bfPhysicalDevice.h"
#include "bfTextureLoad.h"
#include "bfUniforms.h"
#include "bfVariative.hpp"
#include "bfVertex2.hpp"
#include "bfWindow.h"

#define MAX_UNIQUE_DRAW_OBJECTS 10000

static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

struct BfImagePack
{
   VkImage *pImage;
   VkImageView *pImage_view;
   VkImage *pImage_id;
   VkImageView *pImage_view_id;
   VkFramebuffer *pStandart_Buffer;
   VkFramebuffer *pGUI_buffer;
};

struct BfFramePack
{
   // Discriptors
   BfAllocatedBuffer *model_matrix_buffer;  // For object model matrix
   BfAllocatedBuffer *pos_pick_buffer;      // For picking object

   VkCommandBuffer *standart_command_buffer;
   VkCommandBuffer *gui_command_buffer;

   VkSemaphore *available_image_semaphore;
   VkSemaphore *finish_render_image_semaphore;
   VkFence *frame_in_flight_fence;
};

struct BfBase
{
   BfWindow *window;
   BfPhysicalDevice *physical_device;

   VkInstance instance;
   VkSurfaceKHR surface;
   VkDevice device;
   VkDebugUtilsMessengerEXT debug_messenger;

   VkSwapchainKHR swap_chain;
   VkFormat swap_chain_format;
   VkExtent2D swap_chain_extent;

   BfAllocatedBuffer id_image_buffer;
   BfAllocatedImage depth_image;
   VkFormat depth_format;

   std::vector<BfImagePack> image_packs;
   std::vector<BfFramePack> frame_pack;
   uint32_t image_pack_count;
   uint32_t current_image;
   uint32_t current_frame;

   VkRenderPass standart_render_pass;
   VkRenderPass gui_render_pass;

   VkPipeline tline_pipeline;
   VkPipelineLayout tline_pipeline_layout;
   VkPipeline triangle_pipeline;
   VkPipelineLayout triangle_pipeline_layout;
   VkPipeline line_pipeline;
   VkPipelineLayout line_pipeline_layout;

   BfDescriptor descriptor;

   VkCommandPool command_pool;

   VkDescriptorPool gui_descriptor_pool;
   VkSampler sampler;
   BfTextureLoader texture_loader;

   BfLayerHandler layer_handler;
   BfLayerKiller layer_killer;

   BfDrawLayer *section_layer = nullptr;

   VmaAllocator allocator;
   bool is_resized;
   uint32_t pos_id;

   BfTexture *t_texture;
};

// Main functions
BfEvent bfCreateInstance(BfBase &base);
BfEvent bfDestroyInstance(BfBase &base);

BfEvent bfCreateDebugMessenger(BfBase &base);
BfEvent bfDestroyDebufMessenger(BfBase &base);

BfEvent bfCreateSurface(BfBase &base);
BfEvent bfDestroySurface(BfBase &base);

BfEvent bfCreatePhysicalDevice(BfBase &base);

BfEvent bfCreateLogicalDevice(BfBase &base);
BfEvent bfDestroyLogicalDevice(BfBase &base);

BfEvent bfCreateSwapchain(BfBase &base);
BfEvent bfDestroySwapchain(BfBase &base);

BfEvent bfCreateImageViews(BfBase &base);
BfEvent bfDestroyImageViews(BfBase &base);

BfEvent bfCreateStandartRenderPass(BfBase &base);
BfEvent bfDestroyStandartRenderPass(BfBase &base);

BfEvent bfCreateGUIRenderPass(BfBase &base);
BfEvent bfDestroyGUIRenderPass(BfBase &base);

BfEvent bfInitOwnDescriptors(BfBase &base);
BfEvent bfDestroyOwnDescriptors(BfBase &base);

BfEvent bfCreateGraphicsPipelines(BfBase &base);
BfEvent bfDestroyGraphicsPipelines(BfBase &base);

BfEvent bfCreateStandartFrameBuffers(BfBase &base);
BfEvent bfDestroyStandartFrameBuffers(BfBase &base);

BfEvent bfCreateGUIFrameBuffers(BfBase &base);
BfEvent bfDestroyGUIFrameBuffers(BfBase &base);

BfEvent bfCreateCommandPool(BfBase &base);
BfEvent bfDestroyCommandPool(BfBase &base);

BfEvent bfCreateGUIDescriptorPool(BfBase &base);
BfEvent bfDestroyGUIDescriptorPool(BfBase &base);

BfEvent bfCreateStandartCommandBuffers(BfBase &base);
BfEvent bfDestroyStandartCommandBuffers(BfBase &base);

BfEvent bfCreateGUICommandBuffers(BfBase &base);
BfEvent bfDestroyGUICommandBuffers(BfBase &base);

BfEvent bfCreateSyncObjects(BfBase &base);
BfEvent bfDestorySyncObjects(BfBase &base);

BfEvent bfInitImGUI(BfBase &base);
BfEvent bfPostInitImGui(BfBase &base);
BfEvent bfDestroyImGUI(BfBase &base);

BfEvent bfCreateDepthBuffer(BfBase &base);
BfEvent bfDestroyDepthBuffer(BfBase &base);

BfEvent bfCreateIDMapImage(BfBase &base);
BfEvent bfDestroyIDMapImage(BfBase &base);

BfEvent bfCreateAllocator(BfBase &base);
BfEvent bfDestroyAllocator(BfBase &base);

BfEvent bfCreateTextureLoader(BfBase &base);
BfEvent bfDestroyTextureLoader(BfBase &base);

BfEvent bfLoadTextures(BfBase &base);

BfEvent bfBindAllocatorToLayerHandler(BfBase &base);
BfEvent bfBindTrianglePipelineToLayerHandler(BfBase &base);
BfEvent bfBindLinePipelineToLayerHandler(BfBase &base);

// void bfUploadMesh(BfBase &base, BfMesh &mesh);
// void bfUploadVertices(BfBase &base, BfMesh &mesh);
// void bfUploadIndices(BfBase &base, BfMesh &mesh);

// Populate
void bfPopulateMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo
);

// Additional
BfEvent bfaReadFile(std::vector<char> &data, const std::string &filename);
BfEvent bfaCreateShaderModule(
    VkShaderModule &module, VkDevice device, const std::vector<char> &data
);

BfEvent bfaCreateGraphicsPipelineLayouts(BfBase &base);
BfEvent bfDestoryGraphicsPipelineLayouts(BfBase &base);
// BfEvent
BfEvent bfaRecreateSwapchain(BfBase &base);

BfEvent bfaCreateShaderCreateInfos(
    VkDevice device,
    std::string path,
    std::vector<VkShaderModule> &modules,
    std::vector<VkPipelineShaderStageCreateInfo> &out
);

void bfBeginSingleTimeCommands(BfBase &base, VkCommandBuffer &commandBuffer);
void bfEndSingleTimeCommands(BfBase &base, VkCommandBuffer &commandBuffer);
void bfDrawFrame(BfBase &base);
void bfMainRecordCommandBuffer(BfBase &base);
uint32_t bfGetCurrentObjectId(BfBase &base);
void bfUpdateImGuiPlatformWindows();

// Uniform buffers
size_t bfPadUniformBufferSize(
    const BfPhysicalDevice *physical_device, size_t original_size
);
VkDescriptorSetLayoutBinding bfGetDescriptorSetLayoutBinding(
    VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t binding
);
VkWriteDescriptorSet bfWriteDescriptorBuffer(
    VkDescriptorType type,
    VkDescriptorSet dstSet,
    VkDescriptorBufferInfo *bufferInfo,
    uint32_t binding
);

// Depth buffers
VkImageCreateInfo bfPopulateDepthImageCreateInfo(
    VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent
);
VkImageViewCreateInfo bfPopulateDepthImageViewCreateInfo(
    VkFormat format, VkImage image, VkImageAspectFlags aspect_flags
);
VkPipelineDepthStencilStateCreateInfo bfPopulateDepthStencilStateCreateInfo(
    bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp
);

// CleanUp's
BfEvent bfCleanUpSwapchain(BfBase &base);

void bfUpdateUniformBuffer(BfBase &base);

void bfCreateSampler(BfBase &base);
void bfDestorySampler(BfBase &base);

// Binding

static BfBase *__pBase = nullptr;
void bfBindBase(BfBase *);
BfBase *bfGetBase();

#endif
