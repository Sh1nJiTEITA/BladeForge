#ifndef BF_BUFFER_H
#define BF_BUFFER_H

#include "bfVariative.hpp"
#include "bfEvent.h"


struct BfAllocatedBuffer {
	VkBuffer buffer;
	VmaAllocator allocator;
	VmaAllocation allocation;
	VmaAllocationInfo allocation_info;

	bool is_allocated;
	size_t size;
};

class BfLayerBuffer {
private:
	BfAllocatedBuffer __vertex_buffer;
	BfAllocatedBuffer __index_buffer;

	size_t __max_obj_size;
	size_t __max_obj_count;

	void* __p_vertex_data = nullptr;
	void* __p_index_data = nullptr;

public:

	BfLayerBuffer(VmaAllocator allocator, 
				  size_t single_vertex_size,
				  size_t max_vertex_count, 
				  size_t max_obj_count);

	const size_t get_vertex_capacity() const;
	const size_t get_index_capacity() const;
	
	void* map_vertex_memory();
	void* map_index_memory();
	void unmap_vertex_memory();
	void unmap_index_memory();

	VkBuffer* get_p_vertex_buffer();
	VkBuffer* get_p_index_buffer();
};

struct BfAllocatedUniformBuffer : BfAllocatedBuffer {
	void* data;
	VkDescriptorSet* descriptor_set;
};

struct BfAllocatedImage {
	VkImage image;
	VmaAllocation allocation;
};



BfEvent bfCreateBuffer(BfAllocatedBuffer* allocatedBuffer, 
					   VmaAllocator allocator, 
					   size_t allocSize, 
					   VkBufferUsageFlags usage, 
					   VmaMemoryUsage memoryUsage,
					   VmaAllocationCreateFlags flags = 0);





#endif // !BF_BUFFER_H
