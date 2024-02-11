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


BfLayerBuffer::BfLayerBuffer(VmaAllocator allocator, 
							 size_t single_vertex_size,
							 size_t max_vertex_count, 
							 size_t max_obj_count)
	: __max_obj_size{ single_vertex_size * max_vertex_count }
	, __max_obj_count{ max_obj_count }
{
	size_t vertex_size = __max_obj_size * __max_obj_count;
	bfCreateBuffer(&__vertex_buffer,
				   allocator,
				   vertex_size,
				   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				   VMA_MEMORY_USAGE_CPU_TO_GPU);
	
	size_t index_size = sizeof(uint16_t) * max_vertex_count * max_obj_count;
	bfCreateBuffer(&__index_buffer,
				   allocator,
				   index_size,
				   VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				   VMA_MEMORY_USAGE_CPU_TO_GPU);

}

const size_t BfLayerBuffer::get_vertex_capacity() const
{
	return __vertex_buffer.size;
}

const size_t BfLayerBuffer::get_index_capacity() const
{
	return __index_buffer.size;
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

VkBuffer* BfLayerBuffer::get_p_vertex_buffer()
{
	return &__vertex_buffer.buffer;
}

VkBuffer* BfLayerBuffer::get_p_index_buffer()
{
	return &__index_buffer.buffer;
}
