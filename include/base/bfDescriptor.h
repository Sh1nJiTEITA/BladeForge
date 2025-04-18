#ifndef BF_DESCRIPTOR_H
#define BF_DESCRIPTOR_H

#include <algorithm>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "bfBuffer.h"
#include "bfTextureLoad.h"
#include "bfVariative.hpp"

struct BfBase;

// DESCRIPTOR USAGE
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
enum BfEnDescriptorUsage
{
   BfDescriptorViewDataUsage,
   BfDescriptorModelMtxUsage,
   BfDescriptorPosPickUsage,
};

extern std::map< BfEnDescriptorUsage, std::string > BfEnDescriptorUsageStr;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// In shader: set = ...?
enum BfEnDescriptorSetLayoutType
{
   BfDescriptorSetGlobal = 0,
   BfDescriptorSetMain = 1,
   BfDescriptorSetTexture = 2,
};

struct BfDescriptorImageCreateInfo
{
   VkImageCreateInfo image_create_info;
   VkImageViewCreateInfo view_create_info;
   VmaAllocationCreateInfo alloc_create_info;
   bool is_image_view;
   size_t count;
};

struct BfDescriptorBufferCreateInfo
{
   size_t single_buffer_element_size;
   size_t elements_count;
   VkBufferUsageFlags vk_buffer_usage_flags;
   VmaMemoryUsage vma_memory_usage;
   VmaAllocationCreateFlags vma_alloc_flags;
};

struct BfDescriptorCreateInfo
{
   BfEnDescriptorUsage usage;
   VmaAllocator vma_allocator;

   BfAllocatedImage* pImage = nullptr;
   BfAllocatedBuffer* pBuffer = nullptr;

   BfDescriptorImageCreateInfo* pImage_info = nullptr;
   BfDescriptorBufferCreateInfo* pBuffer_info = nullptr;
   // Binding
   // VkDescriptorType : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER...
   VkDescriptorType type;
   VkShaderStageFlags shader_stages; // VK_SHADER_STAGE_VERTEX_BIT
   uint32_t binding;                 // Binding in shader: 0, 1, 2 ...

   // DescriptorSetLayout
   BfEnDescriptorSetLayoutType layout_binding;
};

struct BfDescriptorLayoutPack
{
   std::vector< VkDescriptorSet > desc_sets;
   VkDescriptorSetLayout desc_set_layout;
};

class BfDescriptor
{
   unsigned int __frames_in_flight;

   //
   bool __is_descriptor_mutable = true;
   std::forward_list< BfDescriptorCreateInfo > __desc_create_info_list;

   std::map< BfEnDescriptorUsage, std::vector< BfAllocatedBuffer > >
       __desc_buffers_map;
   std::map< BfEnDescriptorUsage, std::vector< BfAllocatedImage > >
       __desc_image_map;
   std::map< BfEnDescriptorSetLayoutType, BfDescriptorLayoutPack >
       __desc_layout_packs_map;

   std::vector< BfTexture* > __textures;

   VkDescriptorPool __desc_pool;
   VkDevice __device;

public:
   BfDescriptor();
   ~BfDescriptor();
   void kill();
   void set_frames_in_flight(unsigned int in);
   void bind_device(VkDevice device);

   std::vector< VkDescriptorSetLayout > getAllLayouts() const;

   BfEvent add_descriptor_create_info(BfDescriptorCreateInfo info);
   BfEvent
   add_descriptor_create_info(std::vector< BfDescriptorCreateInfo > info);

   // BfEvent add_texture(std::vector<std::shared_ptr<BfTexture>> textures);
   BfEvent add_texture(BfTexture* texture);

   BfEvent allocate_desc_buffers();
   BfEvent deallocate_desc_buffers();
   BfEvent allocate_desc_images();
   BfEvent deallocate_desc_images();

   BfEvent create_desc_pool(std::vector< VkDescriptorPoolSize > sizes);
   BfEvent destroy_desc_pool();

   BfEvent create_desc_set_layouts();
   BfEvent destroy_desc_set_layouts();
   BfEvent create_texture_desc_set_layout();

   BfEvent allocate_desc_sets();
   // BfEvent allocate_texture_desc_sets();

   BfEvent update_desc_sets();

   // void map_textures();
   void map_descriptor(
       BfEnDescriptorUsage usage, unsigned int frame_index, void** data
   );
   void unmap_descriptor(BfEnDescriptorUsage usage, unsigned int frame_index);

   void bind_desc_sets(
       BfEnDescriptorSetLayoutType type,
       uint32_t frame_index,
       VkCommandBuffer command_buffer,
       VkPipelineLayout pipeline_layout,
       uint32_t set_index
   );

   bool is_usage(BfEnDescriptorUsage usage);

   BfAllocatedBuffer*
   get_buffer(BfEnDescriptorUsage usage, uint32_t frame_index);

   BfAllocatedImage* get_image(BfEnDescriptorUsage usage, uint32_t index);

private:
   VkWriteDescriptorSet __write_desc_buffer(
       BfDescriptorCreateInfo& create_info,
       VkDescriptorSet set,
       BfAllocatedBuffer* buffer,
       VkDescriptorBufferInfo* bufferInfo
   );

   VkWriteDescriptorSet __write_desc_image(
       BfDescriptorCreateInfo& create_info,
       VkDescriptorSet set,
       BfAllocatedImage* image,
       VkDescriptorImageInfo* imageInfo
   );

   VkDescriptorSetLayoutBinding __get_desc_set_layout_binding(
       VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t binding
   );
   friend BfEvent bfaCreateGraphicsPipelineLayouts(BfBase& base);
};

#endif
