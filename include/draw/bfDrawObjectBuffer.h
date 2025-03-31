#ifndef BF_DRAW_OBJECT_BUFFER_H
#define BF_DRAW_OBJECT_BUFFER_H

#include <cstring>
#include <stdexcept>

#include "bfAllocator.h"
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

protected:
   VkBuffer m_buffer;
   VmaAllocation m_alloc;
   VmaAllocationInfo m_allocinfo;
   bool m_isalloc;
   size_t m_size;
   void* m_pData;
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

}  // namespace obj

#endif
