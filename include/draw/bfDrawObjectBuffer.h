#ifndef BF_DRAW_OBJECT_BUFFER_H
#define BF_DRAW_OBJECT_BUFFER_H

#include <cstring>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "bfAllocator.h"
#include "bfBase.h"
#include "bfVariative.hpp"

namespace obj
{

class BfBuffer
{
public:
   BfBuffer(
       size_t allocSize,
       VkBufferUsageFlags usage,
       VmaMemoryUsage memoryUsage,
       VmaAllocationCreateFlags flags = 0
   );
   virtual ~BfBuffer();

   VmaAllocation& allocation() { return m_alloc; }
   VkBuffer& raw() { return m_buffer; }

   void* map();
   void unmap();
   void clear();

   VkBuffer& handle() { return m_buffer; }

protected:
   VkBuffer m_buffer;
   VmaAllocation m_alloc;
   VmaAllocationInfo m_allocinfo;
   bool m_isalloc;
   size_t m_size;
   void* m_pData;
};

class BfImage
{
public:
   BfImage(
       size_t width,
       size_t height,
       VkImageUsageFlags usage,
       VmaMemoryUsage memoryUsage,
       VmaAllocationCreateFlags flags = 0
   )
   {
      fmt::print("============== CREATING IMAGE ==============\n");
      VkImageCreateInfo image_info{
          .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
          .imageType = VK_IMAGE_TYPE_2D,
          .format = VK_FORMAT_R8G8B8A8_SRGB,
          .extent =
              {.width = static_cast< uint32_t >(width),
               .height = static_cast< uint32_t >(height),
               .depth = 1},
          .mipLevels = 1,
          .arrayLayers = 1,
          .samples = VK_SAMPLE_COUNT_1_BIT,
          .tiling = VK_IMAGE_TILING_OPTIMAL,
          .usage = usage,
          .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
          .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      };
      VmaAllocationCreateInfo vmaAllocInfo{
          .flags = flags,
          .usage = memoryUsage,
      };
      auto res = vmaCreateImage(
          BfAllocator::get(),
          &image_info,
          &vmaAllocInfo,
          &m_image,
          &m_alloc,
          &m_allocinfo
      );

      if (res == VK_SUCCESS)
      {
         m_isalloc = true;
         m_width = width;
         m_height = height;
      }
      else
      {
         throw std::runtime_error("VkImage was not created. Aborting...\n");
         abort();
      }
   }

   ~BfImage() { vmaDestroyImage(BfAllocator::get(), m_image, m_alloc); }

   size_t width() noexcept { return m_width; }
   size_t height() noexcept { return m_height; }

   VkImage& handle() { return m_image; }

private:
   VkImage m_image;
   VmaAllocation m_alloc;
   VmaAllocationInfo m_allocinfo;
   bool m_isalloc;
   size_t m_width;
   size_t m_height;
};

class BfImageView
{
public:
   BfImageView(const VkImageViewCreateInfo& ci)
   {
      auto& base = *bfGetBase();
      if (vkCreateImageView(base.device, &ci, nullptr, &m_view) != VK_SUCCESS)
      {
         throw std::runtime_error("Cant create VkImageView");
      }
   }
   ~BfImageView()
   {
      auto& base = *bfGetBase();
      vkDestroyImageView(base.device, m_view, nullptr);
   }
   VkImageView& handle() noexcept { return m_view; }

private:
   VkImageView m_view;
};

// BufferCreateInfo

class BfObjectBuffer
{
public:
   BfObjectBuffer(size_t vertex_size, size_t max_vertex, size_t max_obj);

   BfBuffer& vertex() { return m_vertexBuffer; }
   BfBuffer& index() { return m_indexBuffer; }

   // clang-format on
private:
   BfBuffer m_vertexBuffer;
   BfBuffer m_indexBuffer;
};

} // namespace obj

#endif
