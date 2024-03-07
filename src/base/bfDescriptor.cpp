#include "bfDescriptor.h"

std::map<BfEnDescriptorUsage, std::string> BfEnDescriptorUsageStr = {
	{BfDescriptorViewDataUsage, "Descriptor for view data usage"},
	{BfDescriptorModelMtxUsage, "Descriptor for model matrix usage"},
	{BfDescriptorPosPickUsage, "Descriptor for cursor position picking usage"}
};

VkWriteDescriptorSet BfDescriptor::__write_desc_buffer(BfDescriptorCreateInfo& create_info, 
													   VkDescriptorSet set, 
													   BfAllocatedBuffer* buffer,
													   VkDescriptorBufferInfo* bufferInfo)
{	
	/*VkDescriptorBufferInfo bufferInfo{};*/
	bufferInfo->buffer = buffer->buffer;
	bufferInfo->offset = 0;	// From start
	// Length in bites of data
	bufferInfo->range = create_info.pBuffer_info->elements_count * create_info.pBuffer_info->single_buffer_element_size;  

	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;

	write.dstBinding = create_info.binding;
	write.dstSet = set;
	write.descriptorCount = 1;
	write.descriptorType = create_info.type;
	write.pBufferInfo = bufferInfo;

	return write;
}

VkWriteDescriptorSet BfDescriptor::__write_desc_image(BfDescriptorCreateInfo& create_info, 
													  VkDescriptorSet set, 
													  BfAllocatedImage* image,
													  VkDescriptorImageInfo* imageInfo)
{
	imageInfo->imageView = image->view;
	imageInfo->imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;

	write.dstBinding = create_info.binding;
	write.dstSet = set;
	write.descriptorCount = 1;
	write.descriptorType = create_info.type;
	write.pImageInfo = imageInfo;
	
	return write;
}



void BfDescriptor::unmap_descriptor(BfEnDescriptorUsage usage, unsigned int frame_index)
{
	BfAllocatedBuffer* buffer = &__desc_buffers_map[usage][frame_index];
	vmaUnmapMemory(buffer->allocator, buffer->allocation);
}

void BfDescriptor::map_descriptor(BfEnDescriptorUsage usage, unsigned int frame_index, void** data)
{
	BfAllocatedBuffer* buffer = &__desc_buffers_map[usage][frame_index];
	if (vmaMapMemory(buffer->allocator, buffer->allocation, data) != VK_SUCCESS) {
		throw std::runtime_error("Couldn't map memory for BfDescriptor");
	}
}


VkDescriptorSetLayoutBinding BfDescriptor::__get_desc_set_layout_binding(VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t binding)
{
	VkDescriptorSetLayoutBinding setbind = {};
	setbind.binding = binding;
	setbind.descriptorCount = 1;
	setbind.descriptorType = type;
	setbind.pImmutableSamplers = nullptr;
	setbind.stageFlags = stage_flags;

	return setbind;
}

void BfDescriptor::bind_desc_sets(BfEnDescriptorSetLayoutType type,
								  uint32_t frame_index,
								  VkCommandBuffer command_buffer, 
								  VkPipelineLayout pipeline_layout,
								  uint32_t set_index)
{
	vkCmdBindDescriptorSets(
		command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline_layout,
		set_index,
		1,
		&__desc_layout_packs_map[type].desc_sets[frame_index],//base.frame_pack[base.current_frame].global_descriptor_set,//base.frame_pack[base.current_frame].uniform_view_buffer->descriptor_set,
		0,//1,
		nullptr//&uniform_offset
	);
	

}

bool BfDescriptor::is_usage(BfEnDescriptorUsage usage)
{
	return __desc_buffers_map.contains(usage);
}

BfEvent BfDescriptor::create_desc_pool(std::vector<VkDescriptorPoolSize> sizes)
{
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.flags = 0;
	poolInfo.maxSets = 10;
	poolInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
	poolInfo.pPoolSizes = sizes.data();

	std::stringstream ss;

	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
	if (vkCreateDescriptorPool(__device,
		&poolInfo,
		nullptr,
		&this->__desc_pool) == VK_SUCCESS) {
		
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_DESCRIPTOR_POOL_SUCSESS;
		ss << "VkDescriptorPool for "
			<< poolInfo.maxSets
			<< " sets was created successfully";
	}
	else {
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_DESCRIPTOR_POOL_FAILURE;
		ss << "VkDescriptorPool for "
			<< poolInfo.maxSets
			<< " sets wasn't created";
	}
	event.info = ss.str();
	return event;
}

BfEvent BfDescriptor::create_desc_set_layouts()
{
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
	
	std::vector<BfEnDescriptorSetLayoutType> unique;
	// Find all used Layouts
	for (auto& it : __desc_create_info_list) {
		if (std::find(unique.begin(), unique.end(), it.layout_binding) == unique.end()) {
			unique.push_back(it.layout_binding);
		}
	}

	// Fill map with chosen layout types 
	for (auto& unique_layout_type : unique) {
		__desc_layout_packs_map.emplace(
			unique_layout_type, BfDescriptorLayoutPack()
		);
		// For each layout create bindings (bindings from create info) 
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		for (auto& create_info : __desc_create_info_list) {
			if (create_info.layout_binding == unique_layout_type) {
				auto binding = __get_desc_set_layout_binding(create_info.type,
															 create_info.shader_stages,
															 create_info.binding);
				bindings.push_back(binding);
			}
		}
		
		VkDescriptorSetLayoutCreateInfo desc_set_layout_create_info{};
		desc_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		desc_set_layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
		desc_set_layout_create_info.pNext = nullptr;
		desc_set_layout_create_info.flags = 0;
		desc_set_layout_create_info.pBindings = bindings.data();


		if (!vkCreateDescriptorSetLayout(
				__device,
				&desc_set_layout_create_info,
				nullptr,
				&__desc_layout_packs_map[unique_layout_type].desc_set_layout) == VK_SUCCESS)
		{
			std::stringstream ss;
			ss << "VkDescriptroSetLayout for BfDescriptor for layout type: "
				<< (int)unique_layout_type << " wasn't created";
			event.action = BfEnActionType::BF_ACTION_TYPE_INIT_DESCRIPTOR_SET_LAYOUT_FAILURE;
			event.info = ss.str();
			event.success = false;
			return event;
		}
	}
	event.action = BfEnActionType::BF_ACTION_TYPE_INIT_DESCRIPTOR_SET_LAYOUT_SUCCESS;
	event.success = true;
	return event;
}

BfDescriptor::BfDescriptor() {}

BfDescriptor::~BfDescriptor()
{
	this->destroy_desc_set_layouts();
	this->destroy_desc_pool();
	this->deallocate_desc_images();
	this->deallocate_desc_buffers();
}



BfEvent BfDescriptor::add_descriptor_create_info(BfDescriptorCreateInfo info)
{
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

	if (info.pBuffer_info == nullptr && info.pImage_info == nullptr) {
		event.action = BfEnActionType::BF_ACTION_TYPE_ADD_DESCRIPTOR_CREATE_INFO_FAILURE;
		event.info = "Descriptor pInfos are nullptr's";
		return event;
	}


	for (auto& create_info : __desc_create_info_list) {
		if ((create_info.usage == info.usage) &&
			(create_info.layout_binding == info.layout_binding)) {
			event.action = BfEnActionType::BF_ACTION_TYPE_ADD_DESCRIPTOR_CREATE_INFO_FAILURE;
			event.info = "BfDescriptorCreateInfo wasn not added to BfDescriptor: usage and layout_binding are the same";
			event.success = false;
			return event;
		}
	}

	if (__is_descriptor_mutable) {
		__desc_create_info_list.push_front(info);
		event.action = BfEnActionType::BF_ACTION_TYPE_ADD_DESCRIPTOR_CREATE_INFO_SUCCESS;
	}
	else {
		event.action = BfEnActionType::BF_ACTION_TYPE_ADD_DESCRIPTOR_CREATE_INFO_FAILURE;
		event.info = "BfDescriptor was not mutable during this call";
	}
	
	return event;
}

BfEvent BfDescriptor::add_descriptor_create_info(std::vector<BfDescriptorCreateInfo> info)
{
	BfEvent event{};
	for (auto& it : info) {
		event = this->add_descriptor_create_info(it);
	}

	return event;
}

BfEvent BfDescriptor::destroy_desc_pool()
{
	vkDestroyDescriptorPool(__device, __desc_pool, nullptr);
	
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
	event.action = BfEnActionType::BF_ACTION_TYPE_DESTROY_DESCRIPTOR_POOL;
	return event;
}

BfEvent BfDescriptor::allocate_desc_sets()
{
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
	std::stringstream ss;

	for (size_t i = 0; i < __frames_in_flight; i++) {
		for (auto& __desc_layout_pack : __desc_layout_packs_map) {
			__desc_layout_pack.second.desc_sets.push_back(VkDescriptorSet());
			
			VkDescriptorSetAllocateInfo desc_set_alloc_info{};
			desc_set_alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			desc_set_alloc_info.pNext = nullptr;
			desc_set_alloc_info.descriptorPool = __desc_pool;
			desc_set_alloc_info.descriptorSetCount = 1; // 1 for each frame
			// 1 layout for each frame the same, but not for type
			desc_set_alloc_info.pSetLayouts = &__desc_layout_pack.second.desc_set_layout;
			

			if (vkAllocateDescriptorSets(
				__device,
				&desc_set_alloc_info,
				&__desc_layout_pack.second.desc_sets.at(i)) == VK_SUCCESS) 
			{
				ss  << "Global VkDescriptorsSet for desc_type_layout = " 
					<< (int)__desc_layout_pack.first 
					<< " for frame = " << i << "was allocated;";
			}
			else {
				ss	<< "Global VkDescriptorsSet for desc_type_layout = "
					<< (int)__desc_layout_pack.first
					<< " for frame = " << i << "wasn't allocated;";
				event.action = BfEnActionType::BF_ACTION_TYPE_ALLOCATE_DESCRIPTOR_SETS_FAILURE;
				event.info = ss.str();
				event.success = false;
				return event;
			}
		}
	}
	event.action = BfEnActionType::BF_ACTION_TYPE_ALLOCATE_DESCRIPTOR_SETS_SUCCESS;
	event.info = ss.str();
	return event;
}

BfEvent BfDescriptor::destroy_desc_set_layouts()
{
	for (auto& pack : this->__desc_layout_packs_map) {
		vkDestroyDescriptorSetLayout(__device, pack.second.desc_set_layout, nullptr);
	}
	return BfEvent();
}

// TODO: UPDATE FUNCTION FOR IMAGE FUNCTIONALITY
BfEvent BfDescriptor::update_desc_sets()
{
	for (size_t i = 0; i < __frames_in_flight; i++) {
		std::vector<VkWriteDescriptorSet> writes;
		writes.reserve(std::distance(__desc_create_info_list.begin(), __desc_create_info_list.end()));
		
		std::vector<VkDescriptorBufferInfo> buffer_infos;
		buffer_infos.reserve(std::distance(__desc_create_info_list.begin(), __desc_create_info_list.end()));
		
		std::vector<VkDescriptorImageInfo> image_infos;
		image_infos.reserve(std::distance(__desc_create_info_list.begin(), __desc_create_info_list.end()));

		size_t buffer_j = 0;
		size_t image_j = 0;
		for (auto& create_info : __desc_create_info_list) {
			VkDescriptorSet desc_set = 
				__desc_layout_packs_map[create_info.layout_binding].desc_sets[i];


			if (create_info.pBuffer_info != nullptr)
			{
				buffer_infos.emplace_back();
				BfAllocatedBuffer* buffer = 
					&__desc_buffers_map[create_info.usage][i];
				writes.emplace_back(__write_desc_buffer(create_info, desc_set, buffer, &buffer_infos[buffer_j]));
				buffer_j++;
			}
			
			if (create_info.pImage_info != nullptr)
			{
				image_infos.emplace_back();
				BfAllocatedImage* image =
					&__desc_image_map[create_info.usage][i];
				
				writes.emplace_back(__write_desc_image(create_info, desc_set, image, &image_infos[image_j]));
				image_j++;
			}

		}

		vkUpdateDescriptorSets(__device, 
							   static_cast<uint32_t>(writes.size()), 
							   writes.data(), 
							   0, 
							   nullptr);
	}
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
	event.action = BfEnActionType::BF_ACTION_TYPE_UPDATE_DESCRIPTOR_SETS;
	return event;
}

BfEvent BfDescriptor::allocate_desc_buffers()
{
	BfSingleEvent whole_event{};
	whole_event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

	// Allocate buffer holders
	// Create buffers
	for (auto& create_info : __desc_create_info_list) {
		if (create_info.pBuffer_info == nullptr) continue;
		
		// Create storage for buffers
		__desc_buffers_map.emplace(
			create_info.usage, std::vector<BfAllocatedBuffer>()
		);		
		// Reserve vector for buffers by number of frames in flight
		__desc_buffers_map[create_info.usage].reserve(__frames_in_flight * 2);
		
		// For each frame in flight create buffer for current usage
		for (size_t frame_index = 0; frame_index < __frames_in_flight; frame_index++) {
			// Add empty buffer to storage
			__desc_buffers_map[create_info.usage].emplace_back();

			// Create buffer
			BfEvent event = bfCreateBuffer(
				&__desc_buffers_map[create_info.usage][frame_index],
				create_info.vma_allocator,
				create_info.pBuffer_info->elements_count * create_info.pBuffer_info->single_buffer_element_size,
				create_info.pBuffer_info->vk_buffer_usage_flags,
				create_info.pBuffer_info->vma_memory_usage,
				create_info.pBuffer_info->vma_alloc_flags
			);
			
			if (!event.single_event.success) {
				std::stringstream ss;
				ss  << "Buffer for type " << (int)create_info.usage
					<< " (use: " << BfEnDescriptorUsageStr[create_info.usage]
					<< ") wasn't created";
				whole_event.action = BfEnActionType::BF_ACTION_TYPE_ALLOCATE_DESCRIPTOR_BUFFERS_FAILURE;
				whole_event.success = false;
				return whole_event;
			}
		}
	}
	whole_event.action = BfEnActionType::BF_ACTION_TYPE_ALLOCATE_DESCRIPTOR_BUFFERS_SUCCESS;
	return whole_event;
}

BfEvent BfDescriptor::deallocate_desc_buffers()
{
	BfEvent event{};
	for (auto it = __desc_buffers_map.begin(); it != __desc_buffers_map.end(); it++) {
		for (size_t index = 0; index < it->second.size(); index++) {
			event = bfDestroyBuffer(&it->second.at(index));
		}
	}
	return event;
}

BfEvent BfDescriptor::allocate_desc_images()
{
	BfSingleEvent whole_event{};
	whole_event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

	// For each pImage in create_info
	for (auto& create_info : __desc_create_info_list) {
		if (create_info.pImage_info == nullptr) continue;

		// Create storage for images
		__desc_image_map.emplace(
			create_info.usage, std::vector<BfAllocatedImage>()
		);
		// Reserve vector for buffers by number of frames in flight
		__desc_image_map[create_info.usage].reserve(create_info.pImage_info->count * 2);

		// For each frame in flight create buffer for current usage
		for (size_t frame_index = 0; frame_index < create_info.pImage_info->count; frame_index++) {
			// Add empty buffer to storage
			__desc_image_map[create_info.usage].emplace_back();

			// Create image
			BfEvent res_image = bfCreateImage(
				&__desc_image_map[create_info.usage][frame_index],
				create_info.vma_allocator,
				&create_info.pImage_info->image_create_info,
				&create_info.pImage_info->alloc_create_info
			);


			/*VkResult res_image = vmaCreateImage(
				 create_info.vma_allocator,
				&create_info.pImage_info->image_create_info,
				&create_info.pImage_info->alloc_create_info,
				&__desc_image_map[create_info.usage][frame_index].image,
				&__desc_image_map[create_info.usage][frame_index].allocation,
				&__desc_image_map[create_info.usage][frame_index].allocation_info
			);*/

			BfEvent res_view{}; res_view.single_event.success = true;
			if (create_info.pImage_info->is_image_view) {
				create_info.pImage_info->view_create_info.image = 
					__desc_image_map[create_info.usage][frame_index].image;
				res_view = bfCreateImageView(
					&__desc_image_map[create_info.usage][frame_index],
					 __device,
					&create_info.pImage_info->view_create_info);
			}


			if (!res_image.single_event.success || !res_view.single_event.success) {
				std::stringstream ss;
				ss << "Image for type " << (int)create_info.usage
					<< " (use: " << BfEnDescriptorUsageStr[create_info.usage]
					<< ") wasn't created";
				whole_event.action = BfEnActionType::BF_ACTION_TYPE_ALLOCATE_DESCRIPTOR_BUFFERS_FAILURE;
				whole_event.success = false;
				return whole_event;
			}
		}
	}
	whole_event.action = BfEnActionType::BF_ACTION_TYPE_ALLOCATE_DESCRIPTOR_BUFFERS_SUCCESS;
	return whole_event;
}

BfEvent BfDescriptor::deallocate_desc_images()
{
	BfSingleEvent event{};
	event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
	event.action = BF_ACTION_TYPE_DESTORY_DESCRIPTOR_IMAGES;
	event.success = true;

	for (auto& create_info : __desc_image_map) {
		for (auto& image : create_info.second) {
			bfDestroyImage(&image);
			if (image.is_view) {
				bfDestroyImageView(&image, __device);
			}
		}
	}
	
	return event;
}


void BfDescriptor::set_frames_in_flight(unsigned int in) {
	__frames_in_flight = in;
}

void BfDescriptor::bind_device(VkDevice device)
{
	__device = device;
}

BfAllocatedBuffer* BfDescriptor::get_buffer(BfEnDescriptorUsage usage, uint32_t frame_index)
{
	return &__desc_buffers_map[usage][frame_index];
}

BfAllocatedImage* BfDescriptor::get_image(BfEnDescriptorUsage usage, uint32_t index)
{
	return &__desc_image_map[usage][index];
}
