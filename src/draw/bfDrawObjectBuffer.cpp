#include "bfDrawObjectBuffer.h"

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
