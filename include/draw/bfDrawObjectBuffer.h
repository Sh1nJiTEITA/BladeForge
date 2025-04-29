#ifndef BF_DRAW_OBJECT_BUFFER_H
#define BF_DRAW_OBJECT_BUFFER_H

#include <cstring>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "bfAllocator.h"
#include "bfSingle.h"
#include "bfVariative.hpp"

namespace obj
{

struct BfBuffer
{
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
   size_t size() noexcept { return m_size; }

protected:
   VkBuffer m_buffer;
   VmaAllocation m_alloc;
   VmaAllocationInfo m_allocinfo;
   bool m_isalloc;
   size_t m_size;
   void* m_pData;
};

struct BfImage
{
   BfImage(
       size_t width,
       size_t height,
       VkImageUsageFlags usage,
       VmaMemoryUsage memoryUsage,
       VmaAllocationCreateFlags flags = 0,
       VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED
   );

   ~BfImage();
   size_t width() noexcept { return m_width; }
   size_t height() noexcept { return m_height; }
   VkImage& handle() { return m_image; }
   VkImageLayout& layout() noexcept { return m_layout; }

private:
   VkImageLayout m_layout;
   VkImage m_image;
   VmaAllocation m_alloc;
   VmaAllocationInfo m_allocinfo;
   bool m_isalloc;
   size_t m_width;
   size_t m_height;
};

struct BfImageView
{
   BfImageView(const VkImageViewCreateInfo& ci);
   ~BfImageView();

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
