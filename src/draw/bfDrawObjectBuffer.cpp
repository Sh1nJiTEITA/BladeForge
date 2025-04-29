#include "bfDrawObjectBuffer.h"
#include <vulkan/vulkan_core.h>

namespace obj
{

BfBuffer::BfBuffer(
    size_t allocSize,
    VkBufferUsageFlags usage,
    VmaMemoryUsage memoryUsage,
    VmaAllocationCreateFlags flags
)
{
   fmt::print("============== CREATING BUFFER ==============\n");
   VkBufferCreateInfo bufferInfo{
       .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
       .pNext = nullptr,
       .size = allocSize,
       .usage = usage
   };
   VmaAllocationCreateInfo vmaAllocInfo{
       .flags = flags,
       .usage = memoryUsage,
   };

   auto res = vmaCreateBuffer(
       BfAllocator::get(),
       &bufferInfo,
       &vmaAllocInfo,
       &m_buffer,
       &m_alloc,
       &m_allocinfo
   );

   if (res == VK_SUCCESS)
   {
      m_isalloc = true;
      m_size = allocSize;
   }
   else
   {
      throw std::runtime_error("buffer was not created. Aborting...\n");
      abort();
   }

   // clang-format on
}

BfBuffer::~BfBuffer()
{
   vmaDestroyBuffer(BfAllocator::get(), m_buffer, m_alloc);

   fmt::print("============== DESTRUCTING BUFFER ==============\n");
}

void*
BfBuffer::map()
{
   vmaMapMemory(BfAllocator::get(), m_alloc, &m_pData);
   return m_pData;
}

void
BfBuffer::unmap()
{
   vmaUnmapMemory(BfAllocator::get(), m_alloc);
}

void
BfBuffer::clear()
{
   void* pdata = map();
   std::memset(pdata, 0, 0);
   unmap();
};

/* BfImage */

BfImage::BfImage(
    size_t width,
    size_t height,
    VkImageUsageFlags usage,
    VmaMemoryUsage memoryUsage,
    VmaAllocationCreateFlags flags,
    VkImageLayout layout
)
    : m_layout{layout}
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

BfImage::~BfImage() { vmaDestroyImage(BfAllocator::get(), m_image, m_alloc); }

/* BfImageView */
BfImageView::BfImageView(const VkImageViewCreateInfo& ci)
{
   if (vkCreateImageView(base::g::device(), &ci, nullptr, &m_view) !=
       VK_SUCCESS)
   {
      throw std::runtime_error("Cant create VkImageView");
   }
}

BfImageView::~BfImageView()
{
   vkDestroyImageView(base::g::device(), m_view, nullptr);
}

/*  BfObjectBuffer     */

BfObjectBuffer::BfObjectBuffer(
    size_t vertex_size, size_t max_vertex, size_t max_obj
)
    // clang-format off
   : m_vertexBuffer {
        vertex_size * max_vertex * max_obj,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT
   }
   , m_indexBuffer{
        static_cast<size_t>(sizeof(uint32_t) * max_vertex * max_obj * 1.2f), 
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT
   }
{ 
   fmt::print("============== CREATING OBJECT BUFFER ==============\n");
}
   





















}  // namespace obj
