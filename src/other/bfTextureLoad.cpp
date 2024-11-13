#include "bfTextureLoad.h"

#include <vulkan/vulkan_core.h>

#include "bfBuffer.h"
#include "bfVariative.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

BfTextureLoader::BfTextureLoader() {}

BfTextureLoader::BfTextureLoader(
    BfPhysicalDevice* physical_device,
    VkDevice* device,
    VmaAllocator* allocator,
    VkCommandPool* pool
)
    : __pPhysicalDevice{physical_device}
    , __pDevice{device}
    , __pAllocator{allocator}
    , __pCommandPool{pool}
{
   __create_sampler();
}

void
BfTextureLoader::kill()
{
   vkDestroySampler(*__pDevice, __sampler, nullptr);
}
BfTextureLoader::~BfTextureLoader() {}

void
BfTextureLoader::__create_sampler()
{
   VkSamplerCreateInfo sampler_info{};
   sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
   sampler_info.magFilter = VK_FILTER_LINEAR;
   sampler_info.minFilter = VK_FILTER_LINEAR;
   sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
   sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
   sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
   sampler_info.anisotropyEnable = VK_FALSE;
   sampler_info.maxAnisotropy = 16;
   sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
   sampler_info.unnormalizedCoordinates = VK_FALSE;
   sampler_info.compareEnable = VK_FALSE;
   sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
   sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
   sampler_info.mipLodBias = 0.0f;
   sampler_info.minLod = 0.0f;
   sampler_info.maxLod = 0.0f;

   if (vkCreateSampler(*__pDevice, &sampler_info, nullptr, &__sampler) !=
       VK_SUCCESS)
   {
      throw std::runtime_error("VkSampler was not created");
   }
}

void
BfTextureLoader::__begin_single_time_command()
{
   VkCommandBufferAllocateInfo allocInfo{};
   allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
   allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
   allocInfo.commandPool = *__pCommandPool;
   allocInfo.commandBufferCount = 1;

   vkAllocateCommandBuffers(*__pDevice, &allocInfo, &__commandBuffer);

   VkCommandBufferBeginInfo beginInfo{};
   beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
   beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

   vkBeginCommandBuffer(__commandBuffer, &beginInfo);
}

void
BfTextureLoader::__end_single_time_command()
{
   vkEndCommandBuffer(__commandBuffer);

   VkSubmitInfo submitInfo{};
   submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
   submitInfo.commandBufferCount = 1;
   submitInfo.pCommandBuffers = &__commandBuffer;

   vkQueueSubmit(
       __pPhysicalDevice->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE],
       1,
       &submitInfo,
       VK_NULL_HANDLE
   );
   vkQueueWaitIdle(
       __pPhysicalDevice->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE]
   );

   vkFreeCommandBuffers(*__pDevice, *__pCommandPool, 1, &__commandBuffer);
}

void
BfTextureLoader::__transition_image(
    BfTexture* texture, VkImageLayout o, VkImageLayout n
)
{
   __begin_single_time_command();

   VkImageMemoryBarrier barrier{};
   barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
   barrier.oldLayout = o;
   barrier.newLayout = n;
   barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
   barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
   barrier.image = texture->__image.image;
   barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
   barrier.subresourceRange.baseMipLevel = 0;
   barrier.subresourceRange.levelCount = 1;
   barrier.subresourceRange.baseArrayLayer = 0;
   barrier.subresourceRange.layerCount = 1;

   VkPipelineStageFlags sourceStage;
   VkPipelineStageFlags destinationStage;

   if (o == VK_IMAGE_LAYOUT_UNDEFINED &&
       n == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
   {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
   }
   else if (o == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            n == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)

   {
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
   }
   else
   {
      throw std::invalid_argument("unsupported layout transition!");
   }

   vkCmdPipelineBarrier(
       __commandBuffer,
       sourceStage,
       destinationStage,
       0,
       0,
       nullptr,
       0,
       nullptr,
       1,
       &barrier
   );

   __end_single_time_command();
}

void
BfTextureLoader::__copy_buffer_to_image(
    BfAllocatedBuffer* buffer, BfAllocatedImage* image, uint32_t w, uint32_t h
)
{
   __begin_single_time_command();

   VkBufferImageCopy region{};
   region.bufferOffset = 0;
   region.bufferRowLength = 0;
   region.bufferImageHeight = 0;
   region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
   region.imageSubresource.mipLevel = 0;
   region.imageSubresource.baseArrayLayer = 0;
   region.imageSubresource.layerCount = 1;
   region.imageOffset = {0, 0, 0};
   region.imageExtent = {w, h, 1};

   vkCmdCopyBufferToImage(
       __commandBuffer,
       buffer->buffer,
       image->image,
       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
       1,
       &region
   );

   __end_single_time_command();
}

void
BfTextureLoader::__create_texture_image_view(BfTexture* texture)
{
   VkImageViewCreateInfo viewInfo{};
   viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
   viewInfo.image = texture->__image.image;
   viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
   viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
   viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
   viewInfo.subresourceRange.baseMipLevel = 0;
   viewInfo.subresourceRange.levelCount = 1;
   viewInfo.subresourceRange.baseArrayLayer = 0;
   viewInfo.subresourceRange.layerCount = 1;

   bfCreateImageView(&texture->__image, *__pDevice, &viewInfo);
}

BfTexture::BfTexture(std::string path)
    : __path{std::move(path)}
{
}

BfTexture::~BfTexture()
{
   if (!__data) stbi_image_free(__data);
   bfDestroyImage(&__image);
}

// TODO: ADD BUFFER CREATION!!!!
void
BfTextureLoader::__create_temp_buffer(
    BfAllocatedBuffer* buffer, BfTexture* texture
)
{
   bfCreateBuffer(
       buffer,
       buffer->allocator,
       texture->size() * 4,
       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
       VMA_MEMORY_USAGE_AUTO,
       VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
           VMA_ALLOCATION_CREATE_MAPPED_BIT
   );
}

void
BfTextureLoader::__map_temp_buffer(
    BfAllocatedBuffer* buffer, BfTexture* texture
)
{
   void* data;
   vmaMapMemory(buffer->allocator, buffer->allocation, &data);
   {
      memcpy(data, texture->__data, texture->size());
   }
   vmaUnmapMemory(buffer->allocator, buffer->allocation);
}

void
BfTextureLoader::__create_texture_image(BfTexture* texture)
{
   VkImageCreateInfo image_info{
       .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
       .pNext = nullptr,
       .flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT,
       .imageType = VK_IMAGE_TYPE_2D,
       .format = VK_FORMAT_R8G8B8A8_SRGB,
       .extent =
           VkExtent3D{
               (uint32_t)texture->width(),
               (uint32_t)texture->height(),
               1
           },
       .mipLevels = 1,
       .arrayLayers = 1,
       .samples = VK_SAMPLE_COUNT_1_BIT,
       .tiling = VK_IMAGE_TILING_OPTIMAL,
       .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
       .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
       .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
   };

   VmaAllocationCreateInfo alloc_info{};
   alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
   // alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
   // VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

   bfCreateImage(texture->image(), *__pAllocator, &image_info, &alloc_info);
}

uint32_t
BfTextureLoader::load(const std::string& path)
{
   // Create output textre
   // BfTexture texture(path);
   // Create temp buffer

   __textures.emplace_back(path);

   BfTexture* texture = &(*__textures.rbegin());
   texture->__sampler = &__sampler;

   BfAllocatedBuffer buffer;
   buffer.allocator = *__pAllocator;

   // Load image-info inside buffer
   texture->open();
   {
      __create_temp_buffer(&buffer, texture);
      __map_temp_buffer(&buffer, texture);
   }
   texture->close();  // free memory

   __create_texture_image(texture);
   __transition_image(
       texture,
       VK_IMAGE_LAYOUT_UNDEFINED,
       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
   );

   __copy_buffer_to_image(
       &buffer,
       &texture->__image,
       texture->__width,
       texture->__height
   );

   __transition_image(
       texture,
       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
   );

   __create_texture_image_view(texture);

   bfDestroyBuffer(&buffer);

   return texture->id();
}

BfTexture*
BfTextureLoader::get(uint32_t id)
{
   for (auto it = __textures.begin(); it != __textures.end(); ++it)
   {
      if (it->id() == id)
      {
         return &(*it);
      }
   }
   return nullptr;
}

void
BfTextureLoader::create_imgui_descriptor_pool()
{
   VkDescriptorPoolSize poolSizes[] = {
       {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        // static_cast<uint32_t>(__textures.size())}
        100}
   };

   VkDescriptorPoolCreateInfo poolInfo{};
   poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
   poolInfo.poolSizeCount = 1;
   poolInfo.pPoolSizes = poolSizes;
   // poolInfo.maxSets       = static_cast<uint32_t>(__textures.size());
   poolInfo.maxSets = 100;

   if (vkCreateDescriptorPool(
           *__pDevice,
           &poolInfo,
           nullptr,
           &__imguiDescriptorPool
       ) != VK_SUCCESS)
   {
      throw std::runtime_error("failed to create descriptor pool for ImGui!");
   }
}

void
BfTextureLoader::create_imgui_descriptor_set_layout()
{
   VkDescriptorSetLayoutBinding layoutBinding{};
   layoutBinding.binding = 0;
   layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
   layoutBinding.descriptorCount = 1;
   layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
   layoutBinding.pImmutableSamplers = nullptr;

   VkDescriptorSetLayoutCreateInfo layoutInfo{};
   layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
   layoutInfo.bindingCount = 1;
   layoutInfo.pBindings = &layoutBinding;

   if (vkCreateDescriptorSetLayout(
           *__pDevice,
           &layoutInfo,
           nullptr,
           &__desc_set_layout
       ) != VK_SUCCESS)
   {
      throw std::runtime_error("failed to create descriptor set layout!");
   }
}

void
BfTextureLoader::create_imgui_descriptor_set(BfTexture* texture)
{
   VkDescriptorSetAllocateInfo allocInfo{};
   allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
   allocInfo.descriptorPool = __imguiDescriptorPool;
   allocInfo.descriptorSetCount = 1;
   allocInfo.pSetLayouts = &__desc_set_layout;

   if (vkAllocateDescriptorSets(*__pDevice, &allocInfo, texture->set()) !=
       VK_SUCCESS)
   {
      throw std::runtime_error("failed to allocate descriptor set!");
   }

   VkDescriptorImageInfo imageInfo{};
   imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
   imageInfo.imageView = texture->__image.view;
   imageInfo.sampler = __sampler;

   VkWriteDescriptorSet descriptorWrite{};
   descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   descriptorWrite.dstSet = *texture->set();
   descriptorWrite.dstBinding = 0;
   descriptorWrite.dstArrayElement = 0;
   descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
   descriptorWrite.descriptorCount = 1;
   descriptorWrite.pImageInfo = &imageInfo;

   vkUpdateDescriptorSets(*__pDevice, 1, &descriptorWrite, 0, nullptr);

   // texture->imgui_id =
   //     ImGui_ImplVulkan_AddTexture(__sampler,
   //                                 texture->__image.view,
   //                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

BfAllocatedImage*
BfTexture::image()
{
   return &__image;
}

int
BfTexture::width() const
{
   return __width;
}
int
BfTexture::height() const
{
   return __height;
}
int
BfTexture::channels() const
{
   return __channels;
}

void
BfTexture::open()
{
   __data = stbi_load(__path.c_str(), &__width, &__height, &__channels, 4);
   if (!__data)
   {
      throw std::runtime_error("Texture was not loaded via stbimage");
   }
}

void
BfTexture::close()
{
   stbi_image_free(__data);
}

void
BfTexture::load(VmaAllocator allocator, VkDevice device)
{
   VkImageCreateInfo image_info{
       .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
       .pNext = nullptr,
       .flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT,
       .imageType = VK_IMAGE_TYPE_2D,
       .format = VK_FORMAT_R8G8B8A8_SRGB,
       .extent = VkExtent3D{(uint32_t)__width, (uint32_t)__height, 1},
       .mipLevels = 1,
       .arrayLayers = 1,
       .samples = VK_SAMPLE_COUNT_1_BIT,
       .tiling = VK_IMAGE_TILING_OPTIMAL,
       .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
       .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
       .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
   };

   VmaAllocationCreateInfo alloc_info{};
   alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
   // alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
   // VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

   bfCreateImage(&__image, allocator, &image_info, &alloc_info);

   VkImageViewCreateInfo image_view_info{
       .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
       .image = __image.image,
       .viewType = VK_IMAGE_VIEW_TYPE_2D,
       .format = VK_FORMAT_R8G8B8A8_SRGB
   };

   image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
   image_view_info.subresourceRange.baseMipLevel = 0;
   image_view_info.subresourceRange.levelCount = 1;
   image_view_info.subresourceRange.baseArrayLayer = 0;
   image_view_info.subresourceRange.layerCount = 1;

   bfCreateImageView(&__image, device, &image_view_info);
}

bool
BfTexture::is_ok() const
{
   return __image.allocator && __image.view && __image.allocation && __data;
}

uint32_t
BfTexture::id() const
{
   return __id;
}

const std::string&
BfTexture::path() const
{
   return __path;
};

VmaAllocator
BfTexture::allocator() const
{
   return __image.allocator;
}

uint32_t
BfTexture::size() const
{
   return __width * __height * __channels;
}

VkSampler*
BfTexture::sampler() const
{
   return __sampler;
}

VkDescriptorSet*
BfTexture::set()
{
   return &__desc_set;
}
