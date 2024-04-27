#ifndef BF_BUFFER_H
#define BF_BUFFER_H


#include "bfVariative.hpp"
#include "bfEvent.h"
#include <cstring>

struct BfAllocatedBuffer {
	VkBuffer buffer;
	VmaAllocator allocator;
	VmaAllocation allocation;
	VmaAllocationInfo allocation_info;

	bool is_allocated;
	size_t size;
};

struct BfAllocatedImage {
	VkImage image;
	VkImageView view;
	VmaAllocator allocator;
	VmaAllocation allocation;
	VmaAllocationInfo allocation_info;

	bool is_view;
};

class BfLayerBuffer {
private:
	BfAllocatedBuffer __vertex_buffer;
	BfAllocatedBuffer __index_buffer;

	size_t __max_obj_size;
	size_t __max_obj_count;

	void* __p_vertex_data = nullptr;
	void* __p_index_data = nullptr;

	bool __is_nested;
public:

	BfLayerBuffer(VmaAllocator allocator, 
				  size_t single_vertex_size,
				  size_t max_vertex_count, 
				  size_t max_obj_count,
				  bool is_nested);

	~BfLayerBuffer();

	bool is_nested() const noexcept;

	const size_t get_vertex_capacity() const;
	const size_t get_index_capacity() const;
	
	void clear_vertex_buffer();
	void clear_index_buffer();
	void* map_vertex_memory();
	void* map_index_memory();
	void unmap_vertex_memory();
	void unmap_index_memory();
	
	VmaAllocationInfo get_vertex_allocation_info();
	VmaAllocationInfo get_index_allocation_info();

	VkBuffer* get_p_vertex_buffer();
	VkBuffer* get_p_index_buffer();
};

struct BfAllocatedUniformBuffer : BfAllocatedBuffer {
	void* data;
	VkDescriptorSet* descriptor_set;
};



BfEvent bfCreateBuffer(BfAllocatedBuffer* allocatedBuffer, 
					   VmaAllocator allocator, 
					   size_t allocSize, 
					   VkBufferUsageFlags usage, 
					   VmaMemoryUsage memoryUsage,
					   VmaAllocationCreateFlags flags = 0);

BfEvent bfDestroyBuffer(BfAllocatedBuffer* allocatedBuffer);


BfEvent bfCreateImage(BfAllocatedImage* allocatedImage, 
					  VmaAllocator allocator,
					  VkImageCreateInfo* image_create_info,
					  VmaAllocationCreateInfo* alloc_create_info);

BfEvent bfDestroyImage(BfAllocatedImage* allocatedImage);

BfEvent bfCreateImageView(BfAllocatedImage* allocatedImage,
						  VkDevice device, 
						  VkImageViewCreateInfo* imageViewInfo);

BfEvent bfDestroyImageView(BfAllocatedImage* allocatedImage,
						   VkDevice device);


#endif // !BF_BUFFER_H
