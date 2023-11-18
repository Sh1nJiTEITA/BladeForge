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

BfEvent bfCreateBuffer(BfAllocatedBuffer*	allocatedBuffer, 
					   VmaAllocator			allocator, 
					   size_t				allocSize, 
					   VkBufferUsageFlags	usage, 
					   VmaMemoryUsage		memoryUsage)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size  = allocSize;
	bufferInfo.usage = usage;
	bufferInfo.pNext = nullptr;

	VmaAllocationCreateInfo vmaAllocInfo{};
	vmaAllocInfo.usage = memoryUsage;

	BfSingleEvent event{};
	if (vmaCreateBuffer(allocator, 
						&bufferInfo, 
						&vmaAllocInfo, 
						&allocatedBuffer->buffer, 
						&allocatedBuffer->allocation, 
						nullptr) == VK_SUCCESS) {
		
		allocatedBuffer->is_allocated = true;
		allocatedBuffer->size = allocSize;

		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_BUFFER_SUCCESS;
	}
	else {
		allocatedBuffer->is_allocated = false;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_BUFFER_FAILURE;
	}

	return BfEvent(event);
}
