#ifndef BF_DESCRIPTOR_H
#define BF_DESCRIPTOR_H

#include <vector>
#include <map>
#include <string>

#include "bfVariative.hpp"
#include "bfBuffer.h"

struct BfBase;

// DESCRIPTOR USAGE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
enum BfEnDescriptorUsage {
	BfDescriptorViewDataUsage,
	BfDescriptorModelMtxUsage,
	BfDescriptorPosPickUsage,
};

extern std::map<BfEnDescriptorUsage, std::string> BfEnDescriptorUsageStr;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


enum BfEnDescriptorSetLayoutType {
	BfDescriptorSetGlobal = 0,
	BfDescriptorSetMain = 1,
};

struct BfDescriptorCreateInfo {
	// Buffer createuion 
	BfEnDescriptorUsage			usage;
	VmaAllocator				vma_allocator;
	size_t						single_buffer_element_size;
	size_t						elements_count;
	VkBufferUsageFlags			vk_buffer_usage_flags;
	VmaMemoryUsage				vma_memory_usage;
	VmaAllocationCreateFlags	vma_alloc_flags;
	// Binding 
	VkDescriptorType			type; // VkDescriptorType : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER...
	VkShaderStageFlags			shader_stages; // VK_SHADER_STAGE_VERTEX_BIT
	uint32_t					binding; // Binding in shader: 0, 1, 2 ...
	
	// DescriptorSetLayout
	BfEnDescriptorSetLayoutType layout_binding;
};

struct BfDescriptorLayoutPack {
	std::vector<VkDescriptorSet> desc_sets;
	VkDescriptorSetLayout desc_set_layout;
};


class BfDescriptor {

	unsigned int __frames_in_flight;
	
	// 
	bool __is_descriptor_mutable = true;
	std::forward_list<BfDescriptorCreateInfo> __desc_create_info_list;

	std::map<BfEnDescriptorUsage, std::vector<BfAllocatedBuffer>> __desc_buffers_map;
	std::map<BfEnDescriptorSetLayoutType, BfDescriptorLayoutPack> __desc_layout_packs_map;

	VkDescriptorPool __desc_pool;
	
public:
	BfDescriptor();


	BfEvent add_descriptor_create_info(BfDescriptorCreateInfo info);
	BfEvent add_descriptor_create_info(std::vector<BfDescriptorCreateInfo> info);

	BfEvent allocate_desc_buffers();
	BfEvent deallocate_desc_buffers();

	BfEvent create_desc_pool(VkDevice device, std::vector<VkDescriptorPoolSize> sizes);
	BfEvent destroy_desc_pool(VkDevice device);

	BfEvent create_desc_set_layouts(VkDevice device);
	BfEvent destroy_desc_set_layouts(VkDevice device);

	BfEvent allocate_desc_sets(VkDevice device);

	BfEvent update_desc_sets(VkDevice device);
	
	void map_descriptor(BfEnDescriptorUsage usage, unsigned int frame_index, void* data);
	void unmap_descriptor(BfEnDescriptorUsage usage, unsigned int frame_index);

	void set_frames_in_flight(unsigned int in);
private:
	VkWriteDescriptorSet __write_desc_buffer(BfDescriptorCreateInfo create_info,
											 VkDescriptorSet set,
											 BfAllocatedBuffer* buffer,
											 VkDescriptorBufferInfo* bufferInfo);
	
	VkDescriptorSetLayoutBinding  __get_desc_set_layout_binding(VkDescriptorType type, 
																VkShaderStageFlags stage_flags, 
																uint32_t binding);
};


#endif