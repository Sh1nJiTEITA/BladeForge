#include "bfHolder.h"

BfHolder* BfHolder::__bfpHolder = nullptr;
BfGeometryHolder* BfGeometryHolder::__bfpGeometryHolder = nullptr;

BfEvent bfBindHolder(BfHolder* holder)
{
	BfHolder::__bfpHolder = holder;

	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_HOLDER_EVENT;
	event.action = BfEnActionType::BF_ACTION_TYPE_BIND_HOLDER;
	return BfEvent(event);
}

BfHolder::BfHolder()
	//windows{},
	//physical_devices{}
{
	__bfpHolder = this;
}

//void bfAddToHolder(BfWindow window, int& index)
//{
//	BfHolder::__bfpHolder->windows.push_back(window);
//	BfHolder::__bfpHolder->windows.back().holder_index = BfHolder::__bfpHolder->windows.size() - 1;
//}
//
//void bfAddToHolder(BfPhysicalDevice physical_device, int& index)
//{
//	BfHolder::__bfpHolder->physical_devices.push_back(physical_device);
//	//BfHolder::__bfpHolder->physical_devices.back().holder_index = BfHolder::__bfpHolder->windows.size() - 1;
//}

BfHolder* bfGetpHolder()
{
	return BfHolder::__bfpHolder;
}

BfEvent bfBindGeometryHolder(BfGeometryHolder* geometry_holder)
{
	BfGeometryHolder::__bfpGeometryHolder = geometry_holder;

	BfSingleEvent event{};
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CURVE_HOLDER_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_BIND_CURVE_HOLDER;
	}
	return BfEvent(event);
}

BfEvent bfAllocateGeometrySet(BfeGeometrySetType type, size_t elements_count)
{
	BfGeometryHolder* pGeometryHolder = bfGetpGeometryHolder();

	pGeometryHolder->allocated_geometries++;
	pGeometryHolder->geometry_sets.resize(pGeometryHolder->allocated_geometries);

	BfGeometrySet* pGeometrySet = &pGeometryHolder->geometry_sets[pGeometryHolder->allocated_geometries-1];
	
	pGeometrySet->set_up(type, elements_count, pGeometryHolder->outside_allocator);

	BfSingleEvent event{};
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_GEOMETRY_HOLDER_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_GEOMETRY_SET_SUCCESS;
		event.info = " BfGeometrySet of type = " + std::to_string(type) + " was added to holder";
	}
	return BfEvent(event);
}

BfEvent bfBindGraphicsPipeline(BfeGeometrySetType type, VkPipeline* pipeline, BfePipelineType ptype)
{
	BfGeometryHolder* pGeometryHolder = bfGetpGeometryHolder();
	BfGeometrySet* pGeometrySet = pGeometryHolder->get_geometry_set(type);

	pGeometrySet->pPipeline = pipeline;
	pGeometrySet->pipeline_type = ptype;

	BfSingleEvent event{};
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_HOLDER_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_BIND_GRAPHICS_PIPELINE_TO_GEOMETRY_SET;
		event.info = "Geometry set of type = " + std::to_string(type);
	}

	return event;
}



BfGeometryHolder* bfGetpGeometryHolder()
{
	return BfGeometryHolder::__bfpGeometryHolder;
}



BfEvent bfHoldWindow(BfWindow*& window) {
	if (window == nullptr) {
		BfHolder::__bfpHolder->windows.push_back(BfWindow());
		BfHolder::__bfpHolder->windows.back().holder_index = BfHolder::__bfpHolder->windows.size() - 1;
		window = &BfHolder::__bfpHolder->windows.back();
	}
	else {
		BfHolder::__bfpHolder->windows.push_back(*window);
		BfHolder::__bfpHolder->windows.back().holder_index = BfHolder::__bfpHolder->windows.size() - 1;
		window = &BfHolder::__bfpHolder->windows.back();
	}

	BfSingleEvent event{};
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_HOLDER_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_HOLDER_ADD_BF_WINDOW;
	}
	return BfEvent(event);
}

BfEvent bfHoldPhysicalDevice(BfPhysicalDevice*& physical_device) {
	if (physical_device == nullptr) {
		BfHolder::__bfpHolder->physical_devices.push_back(BfPhysicalDevice());
		BfHolder::__bfpHolder->physical_devices.back().holder_index = BfHolder::__bfpHolder->physical_devices.size() - 1;
		physical_device = &BfHolder::__bfpHolder->physical_devices.back();
	}
	else {
		BfHolder::__bfpHolder->physical_devices.push_back(*physical_device);
		BfHolder::__bfpHolder->physical_devices.back().holder_index = BfHolder::__bfpHolder->physical_devices.size() - 1;
		physical_device = &BfHolder::__bfpHolder->physical_devices.back();
	}
	
	BfSingleEvent event{};
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_HOLDER_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_HOLDER_ADD_BF_PHYSICAL_DEVICE;
	}
	return BfEvent(event);
}

BfGeometryHolder::BfGeometryHolder() :
	BfGeometryHolder(nullptr) {}

BfGeometryHolder::BfGeometryHolder(VmaAllocator _outside_allocator)
{
	this->outside_allocator = _outside_allocator;
	this->allocated_geometries = 0;
	this->geometry_sets = std::vector<BfGeometrySet>();
}

BfEvent bfBindGeometryHolderOutsideAllocator(VmaAllocator _outside_allocator)
{
	BfGeometryHolder* pGeometryHolder = bfGetpGeometryHolder();
	pGeometryHolder->outside_allocator = _outside_allocator;

	BfSingleEvent event{};
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CURVE_HOLDER_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_BIND_OUTSIDE_ALLOCATOR_FOR_CURVE_HOLDER;
	}
	return BfEvent(event);
}

BfGeometrySet* BfGeometryHolder::get_geometry_set(BfeGeometrySetType type)
{
	if (this->allocated_geometries == 0) {
		throw std::runtime_error("Can't get BfGeometrySet -> BfGeometryHolder holds 0 BfGeometrySet's");
	}
	BfGeometrySet* pGeometrySet = nullptr;
	
	for (size_t i = 0; i < this->geometry_sets.size(); i++) {
		if (this->geometry_sets[i].type == type) {
			pGeometrySet = &this->geometry_sets[i];
			break;
		}
	}
	
	if (pGeometrySet == nullptr) {
		throw std::runtime_error("BfGeometryHolder doesn't hold needed type of BfGeometrySet");
	}
	
	return pGeometrySet;
}

BfGeometrySet* BfGeometryHolder::get_geometry_set_by_index(size_t i)
{
	return &this->geometry_sets[i];
}

void BfGeometryHolder::update_obj_data(VmaAllocation allocation)
{
	size_t obj_data_count = 0;

	for (size_t i = 0; i < this->allocated_geometries; i++) {
		BfGeometrySet* pGeometrySet = this->get_geometry_set_by_index(i);
		obj_data_count += pGeometrySet->ready_elements_count;
	}
	
	std::vector<BfObjectData> obj_data(obj_data_count);
	
	size_t last_obj_data_index = 0;
	for (size_t i = 0; i < this->allocated_geometries; i++) {
		BfGeometrySet* pGeometrySet = this->get_geometry_set_by_index(i);

		for (size_t j = 0; j < pGeometrySet->ready_elements_count; j++) {
			obj_data[last_obj_data_index + j] = pGeometrySet->datas[j].obj_data;
		}
		last_obj_data_index += pGeometrySet->ready_elements_count;
	}
	
	void* pobjects_data;
	vmaMapMemory(this->outside_allocator, allocation, &pobjects_data);
	{
		memcpy(pobjects_data, obj_data.data(), sizeof(BfObjectData) * obj_data.size());
	}
	vmaUnmapMemory(this->outside_allocator, allocation);
}

void BfGeometryHolder::draw_indexed(VkCommandBuffer command_buffer)
{
	size_t geometry_sets_count = this->allocated_geometries;
	
	std::vector<BfGeometrySet*> line_pipeline_sets;
	std::vector<uint32_t> line_offsets;
	std::vector<BfGeometrySet*> triangle_pipeline_sets;
	std::vector<uint32_t> triangle_offsets;

	uint32_t c_offset = 0;
	for (size_t i = 0; i < geometry_sets_count; i++) {
		BfGeometrySet* pGeometrySet = get_geometry_set_by_index(i);
		
		if (pGeometrySet->pPipeline == nullptr) {
			throw std::runtime_error("Graphics pipeline wasn't bound to BfGeometry set, which was going to draw");
		}

		if (pGeometrySet->pipeline_type == BF_GRAPHICS_PIPELINE_LINES) {
			line_pipeline_sets.push_back(pGeometrySet);
			line_offsets.push_back(c_offset);
			c_offset += pGeometrySet->ready_elements_count;
		}
		else if (pGeometrySet->pipeline_type == BF_GRAPHICS_PIPELINE_TRIANGLE) {
			triangle_pipeline_sets.push_back(pGeometrySet);
			triangle_offsets.push_back(c_offset);
			c_offset += pGeometrySet->ready_elements_count;
		}
		else {
			throw std::runtime_error("Graphics pipeline type is not supported or wasn't specified");
		}
	}

	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *line_pipeline_sets[0]->pPipeline);
	for (size_t i = 0; i < line_pipeline_sets.size(); i++) {
		line_pipeline_sets[i]->draw_indexed(command_buffer, line_offsets[i]);
	}

	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *triangle_pipeline_sets[0]->pPipeline);
	for (size_t i = 0; i < triangle_pipeline_sets.size(); i++) {
		triangle_pipeline_sets[i]->draw_indexed(command_buffer, triangle_offsets[i]);
	}
	

	/*uint32_t index_offset = 0;

	for (size_t i = 0; i < geometry_sets_count; i++) {
		BfGeometrySet* pGeometrySet = get_geometry_set_by_index(i);

		pGeometrySet->draw_indexed(command_buffer, index_offset);

		index_offset += pGeometrySet->ready_elements_count;
	}*/
}



void bfAddToHolder(const BfLine& o, const BfObjectData& obj_data)
{
	BfGeometryHolder* pHolder = bfGetpGeometryHolder();
	BfGeometrySet* pSet = pHolder->get_geometry_set(BF_GEOMETRY_TYPE_CURVE_LINEAR);

	std::vector<BfVertex3> line_vertices{
		o.get_start_point(),
		o.get_finish_point()
	};

	std::vector<uint16_t> line_indices{
		0,
		1
	};

	pSet->add_data(line_vertices, line_indices, obj_data);
}

void bfAddToHolder(const BfBezier& o, const BfObjectData& obj_data)
{
	BfGeometryHolder* pHolder = bfGetpGeometryHolder();
	
	BfGeometrySet* pCurveSet = pHolder->get_geometry_set(BF_GEOMETRY_TYPE_CURVE_BEZIER);
	BfGeometrySet* pHandleSet = pHolder->get_geometry_set(BF_GEOMETRY_TYPE_HANDLE_CURVE_BEZIER);
	BfGeometrySet* pCarcassSet = pHolder->get_geometry_set(BF_GEOMETRY_TYPE_CARCASS_CURVE_BEZIER);

	// --- Curve -----------------------------------------------------------------------//
	const size_t cvert_count = o.get_cvertices().size();
	std::vector<uint16_t> line_indices(cvert_count);

	for (size_t i = 0; i < cvert_count; i++) {
		line_indices[i] = i;
	}
	pCurveSet->add_data(o.get_cvertices(), line_indices, obj_data);

	// --- Handle ----------------------------------------------------------------------//
	std::vector<BfVertex3> handle_vertices = o.get_handles_vertices(0.05f, {0.0f, 1.0f, 0.0f});
	std::vector<uint16_t> handle_indices = o.get_handles_indices();
	pHandleSet->add_data(handle_vertices, handle_indices, obj_data);


	// --- Carcass ---------------------------------------------------------------------//
	std::vector<BfVertex3> carcass_vertices = o.get_carcass_vertices();
	std::vector<uint16_t> carcass_indices = o.get_carcass_indices();
	pCarcassSet->add_data(carcass_vertices, carcass_indices, obj_data);
}

