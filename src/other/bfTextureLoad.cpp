#include "bfTextureLoad.h"

#include <vulkan/vulkan_core.h>

#include "bfBuffer.h"
#include "bfVariative.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

BfTextureLoader::BfTextureLoader() {}

BfTextureLoader::BfTextureLoader(VkDevice* device, VmaAllocator* allocator)
    : __pDevice{device}, __pAllocator{allocator}
{
   __create_sampler();
}

BfTextureLoader::~BfTextureLoader() {}

void BfTextureLoader::__create_sampler()
{
   VkSamplerCreateInfo sampler_info{};
   sampler_info.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
   sampler_info.magFilter               = VK_FILTER_LINEAR;
   sampler_info.minFilter               = VK_FILTER_LINEAR;
   sampler_info.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
   sampler_info.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
   sampler_info.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
   sampler_info.anisotropyEnable        = VK_TRUE;
   sampler_info.maxAnisotropy           = 16;
   sampler_info.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
   sampler_info.unnormalizedCoordinates = VK_FALSE;
   sampler_info.compareEnable           = VK_FALSE;
   sampler_info.compareOp               = VK_COMPARE_OP_ALWAYS;
   sampler_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
   sampler_info.mipLodBias              = 0.0f;
   sampler_info.minLod                  = 0.0f;
   sampler_info.maxLod                  = 0.0f;

   if (vkCreateSampler(*__pDevice, &sampler_info, nullptr, &__sampler) !=
       VK_SUCCESS)
   {
      throw std::runtime_error("VkSampler was not created");
   }
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

BfTexture BfTextureLoader::load(const std::string& path)
{
   BfTexture texture(path);
   texture.open();

   BfAllocatedBuffer buffer;
   bfCreateBuffer(&buffer,
                  *__pAllocator,
                  texture.size(),
                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VMA_MEMORY_USAGE_AUTO,
                  VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                      VMA_ALLOCATION_CREATE_MAPPED_BIT);

   VmaAllocation buffer_allocation;

   void* data;
   vmaMapMemory(buffer.allocator, buffer.allocation, &data);
   {
      memcpy(data, texture.data(), texture.size());
   }
   vmaUnmapMemory(buffer.allocator, buffer.allocation);

   stbi_image_free(texture.data());

   VkImageCreateInfo imageInfo{};
   imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
   imageInfo.imageType     = VK_IMAGE_TYPE_2D;
   imageInfo.extent.width  = texture.width();
   imageInfo.extent.height = texture.height();
   imageInfo.extent.depth  = 1;
   imageInfo.mipLevels     = 1;
   imageInfo.arrayLayers   = 1;
   imageInfo.format        = VK_FORMAT_R8G8B8A8_SRGB;
   imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
   imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
   imageInfo.usage =
       VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
   imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
   imageInfo.samples     = VK_SAMPLE_COUNT_1_BIT;
   imageInfo.flags       = 0;

   VmaAllocationCreateInfo alloc_info;
   alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

   bfCreateImage(texture.image(), *__pAllocator, &imageInfo, &alloc_info);
}

int BfTexture::width() const { return __width; }
int BfTexture::height() const { return __height; }
int BfTexture::channels() const { return __channels; }

void BfTexture::open()
{
   __data = stbi_load(__path.c_str(), &__width, &__height, &__channels, 4);
   if (!__data)
   {
      throw std::runtime_error("Texture was not loaded via stbimage");
   }
}

uint8_t* BfTexture::data() { return __data; }

void BfTexture::load(VmaAllocator allocator, VkDevice device)
{
   VkImageCreateInfo image_info{
       .sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
       .pNext       = nullptr,
       .flags       = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT,
       .imageType   = VK_IMAGE_TYPE_2D,
       .format      = VK_FORMAT_R8G8B8A8_SRGB,
       .extent      = VkExtent3D{(uint32_t)__width, (uint32_t)__height, 1},
       .mipLevels   = 1,
       .arrayLayers = 1,
       .samples     = VK_SAMPLE_COUNT_1_BIT,
       .tiling      = VK_IMAGE_TILING_OPTIMAL,
       .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
       .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
       .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

   VmaAllocationCreateInfo alloc_info{};
   alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
   // alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
   // VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

   bfCreateImage(&__image, allocator, &image_info, &alloc_info);

   VkImageViewCreateInfo image_view_info{
       .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
       .image    = __image.image,
       .viewType = VK_IMAGE_VIEW_TYPE_2D,
       .format   = VK_FORMAT_R8G8B8A8_SRGB};

   image_view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
   image_view_info.subresourceRange.baseMipLevel   = 0;
   image_view_info.subresourceRange.levelCount     = 1;
   image_view_info.subresourceRange.baseArrayLayer = 0;
   image_view_info.subresourceRange.layerCount     = 1;

   bfCreateImageView(&__image, device, &image_view_info);
}

bool BfTexture::is_ok() const
{
   return __image.allocator && __image.view && __image.allocation && __data;
}

uint32_t BfTexture::id() const { return __id; }

const std::string& BfTexture::path() const { return __path; };

VmaAllocator BfTexture::allocator() const { return __image.allocator; }

uint32_t BfTexture::size() const { return __width * __height * __channels; }

BfAllocatedImage* BfTexture::image() { return &__image; }
