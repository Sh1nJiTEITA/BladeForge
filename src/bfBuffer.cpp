#include "bfBuffer.h"

BfEvent bfCreateBuffer(VmaAllocator allocator,
					   VkDeviceSize size, 
					   VkBuffer& buffer)
{
	
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = 65536;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;


	VmaAllocation allocation;
	vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);


	return BfEvent();
}
