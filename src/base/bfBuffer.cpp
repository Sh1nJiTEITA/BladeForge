#include "bfBuffer.h"

//BfEvent bfCreateBuffer(VmaAllocator allocator,
//					   VkDeviceSize size, 
//					   VkBuffer& buffer)
//{
//	
//	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
//	bufferInfo.size = 65536;
//	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
//
//	VmaAllocationCreateInfo allocInfo = {};
//	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
//
//
//	VmaAllocation allocation;
//	vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
//
//
//	return BfEvent();
//}




BfEvent bfCreateBuffer(BfAllocatedBuffer*		allocatedBuffer,
					   VmaAllocator				allocator,
					   size_t					allocSize,
					   VkBufferUsageFlags		usage,
					   VmaMemoryUsage			memoryUsage,
					   VmaAllocationCreateFlags flags)
{
	allocatedBuffer->allocator = allocator;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size  = allocSize;
	bufferInfo.usage = usage;
	bufferInfo.pNext = nullptr;

	VmaAllocationCreateInfo vmaAllocInfo{};
	vmaAllocInfo.usage = memoryUsage;
	vmaAllocInfo.flags = flags;

	BfSingleEvent event{};
	if (vmaCreateBuffer(allocator, 
						&bufferInfo, 
						&vmaAllocInfo, 
						&allocatedBuffer->buffer, 
						&allocatedBuffer->allocation, 
						&allocatedBuffer->allocation_info) == VK_SUCCESS) {
		
		allocatedBuffer->is_allocated = true;
		allocatedBuffer->size = allocSize;

		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_BUFFER_SUCCESS;
		event.success = true;
	}
	else {
		allocatedBuffer->is_allocated = false;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_BUFFER_FAILURE;
		event.success = false;
	}

	return BfEvent(event);
}

BfEvent bfDestroyBuffer(BfAllocatedBuffer* allocatedBuffer)
{
	vmaDestroyBuffer(allocatedBuffer->allocator, 
					 allocatedBuffer->buffer, 
					 allocatedBuffer->allocation);
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
	event.action = BfEnActionType::BF_ACTION_TYPE_DESTROY_BUFFER;
	
	return event;
}

BfEvent bfCreateImage(BfAllocatedImage* allocatedImage, 
					  VmaAllocator allocator, 
					  VkImageCreateInfo* image_create_info, 
					  VmaAllocationCreateInfo* alloc_create_info)
{
	BfSingleEvent event{};
	event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

	allocatedImage->allocator = allocator;

	if (vmaCreateImage(allocator,
					   image_create_info,
					   alloc_create_info,
					  &allocatedImage->image,
					  &allocatedImage->allocation,
					  &allocatedImage->allocation_info) != VK_SUCCESS)
	{
		event.action = BF_ACTION_TYPE_CREATE_IMAGE_FAILURE;
		event.success = false;
	}
	else
	{
		event.action = BF_ACTION_TYPE_CREATE_IMAGE_SUCCESS;
		event.success = true;
	}
	
	return event;
}

BfEvent bfDestroyImage(BfAllocatedImage* allocatedImage)
{
	BfSingleEvent event{};
	event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
	event.action = BF_ACTION_TYPE_DESTROY_IMAGE;

	vmaDestroyImage(allocatedImage->allocator, allocatedImage->image, allocatedImage->allocation);
	
	return event;
}

BfEvent bfCreateImageView(BfAllocatedImage* allocatedImage, 
						  VkDevice device, 
						  VkImageViewCreateInfo* imageViewInfo)
{
	BfSingleEvent event{};
	event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

	if (vkCreateImageView(device, 
						  imageViewInfo, 
						  nullptr, 
						 &allocatedImage->view) != VK_SUCCESS) 
	{
		event.action = BF_ACTION_TYPE_CREATE_IMAGE_VIEW_FAILURE;
		event.success = false;
	}
	else
	{
		event.action = BF_ACTION_TYPE_CREATE_IMAGE_VIEW_SUCCESS;
		event.success = true;
	}
	
	return event;
}

BfEvent bfDestroyImageView(BfAllocatedImage* allocatedImage, VkDevice device)
{
	BfSingleEvent event{};
	event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
	event.action = BF_ACTION_TYPE_DESTROY_IMAGE_VIEW;
	
	vkDestroyImageView(device, allocatedImage->view, nullptr);
	
	return event;
}


BfLayerBuffer::BfLayerBuffer(VmaAllocator allocator, 
							 size_t single_vertex_size,
							 size_t max_vertex_count, 
							 size_t max_obj_count)
	: __max_obj_size{ single_vertex_size * max_vertex_count }
	, __max_obj_count{ max_obj_count }
{
	size_t vertex_size = __max_obj_size * __max_obj_count;
	BfEvent v_event = bfCreateBuffer(
		&__vertex_buffer,
		allocator,
		vertex_size,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		//VMA_MEMORY_USAGE_CPU_TO_GPU 
		VMA_MEMORY_USAGE_AUTO,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT
	);
	if (!v_event.single_event.success) {
		throw std::runtime_error("vertex buffer wasn't created");
	}

	size_t index_size = sizeof(uint16_t) * max_vertex_count * max_obj_count;
	BfEvent i_event = bfCreateBuffer(
		&__index_buffer,
		allocator,
		index_size,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		//VMA_MEMORY_USAGE_CPU_TO_GPU);
		VMA_MEMORY_USAGE_AUTO,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT
	);
	if (!i_event.single_event.success) {
		throw std::runtime_error("index buffer wasn't created");
	}

}

BfLayerBuffer::~BfLayerBuffer()
{
	BfEvent v_event = bfDestroyBuffer(&__vertex_buffer);
	BfEvent i_event = bfDestroyBuffer(&__index_buffer);
}

const size_t BfLayerBuffer::get_vertex_capacity() const
{
	return __vertex_buffer.size;
}

const size_t BfLayerBuffer::get_index_capacity() const
{
	return __index_buffer.size;
}

void BfLayerBuffer::clear_vertex_buffer() {
	void* p_vertex_data = this->map_vertex_memory();
	std::memset(p_vertex_data, 0, 0);
	this->unmap_vertex_memory();
}

void BfLayerBuffer::clear_index_buffer() {
	void* p_index_data = this->map_index_memory();
	std::memset(p_index_data, 0, 0);
	this->unmap_index_memory();
}


void* BfLayerBuffer::map_vertex_memory()
{
	vmaMapMemory(__vertex_buffer.allocator, __vertex_buffer.allocation, &__p_vertex_data);
	return __p_vertex_data;
}

void* BfLayerBuffer::map_index_memory()
{
	vmaMapMemory(__index_buffer.allocator, __index_buffer.allocation, &__p_index_data);
	return __p_index_data;
}

void BfLayerBuffer::unmap_vertex_memory()
{
	vmaUnmapMemory(__vertex_buffer.allocator, __vertex_buffer.allocation);
}

void BfLayerBuffer::unmap_index_memory()
{
	vmaUnmapMemory(__index_buffer.allocator, __index_buffer.allocation);
}

VmaAllocationInfo BfLayerBuffer::get_vertex_allocation_info()
{
	return __vertex_buffer.allocation_info;
}

VmaAllocationInfo BfLayerBuffer::get_index_allocation_info()
{
	return __index_buffer.allocation_info;
}

VkBuffer* BfLayerBuffer::get_p_vertex_buffer()
{
	return &__vertex_buffer.buffer;
}

VkBuffer* BfLayerBuffer::get_p_index_buffer()
{
	return &__index_buffer.buffer;
}
