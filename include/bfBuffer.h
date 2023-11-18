#ifndef BF_BUFFER_H
#define BF_BUFFER_H

#include "bfVariative.hpp"
#include "bfEvent.h"







struct BfAllocatedBuffer {
	VkBuffer buffer;
	VmaAllocation allocation;
	
	bool is_allocated;
	size_t size;
};

struct BfAllocatedUniformBuffer : BfAllocatedBuffer {
	void* data;
	VkDescriptorSet* descriptor_set;
};


BfEvent bfCreateBuffer(BfAllocatedBuffer* allocatedBuffer, 
					   VmaAllocator allocator, 
					   size_t allocSize, 
					   VkBufferUsageFlags usage, 
					   VmaMemoryUsage memoryUsage);





#endif // !BF_BUFFER_H
