#include "bfHolder.h"

BfHolder* BfHolder::__bfpHolder = nullptr;
BfCurveHolder* BfCurveHolder::__bfpCurveHolder = nullptr;

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

BfEvent bfBindCurveHolder(BfCurveHolder* curve_holder)
{
	BfCurveHolder::__bfpCurveHolder = curve_holder;

	BfSingleEvent event{};
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CURVE_HOLDER_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_BIND_CURVE_HOLDER;
	}
	return BfEvent(event);
}

BfEvent bfAllocateCurveSet(BfeCurveType type, size_t elements_count)
{
	BfCurveHolder* pCurveHolder = bfGetpCurveHolder();

	pCurveHolder->allocated_curves++;
	pCurveHolder->curve_sets.resize(pCurveHolder->allocated_curves);

	BfCurveSet* pCurveSet = &pCurveHolder->curve_sets[pCurveHolder->allocated_curves-1];
	
	pCurveSet->set_up(type, elements_count, pCurveHolder->outside_allocator);

	BfSingleEvent event{};
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CURVE_HOLDER_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_CURVE_SET_SUCCESS;
		event.info = " BfCurveSet of type = " + std::to_string(type) + " was added to holder";
	}
	return BfEvent(event);
}



BfCurveHolder* bfGetpCurveHolder()
{
	return BfCurveHolder::__bfpCurveHolder;
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

BfCurveHolder::BfCurveHolder() :
	BfCurveHolder(nullptr) {}

BfCurveHolder::BfCurveHolder(VmaAllocator _outside_allocator)
{
	this->outside_allocator = _outside_allocator;
	this->allocated_curves = 0;
	this->curve_sets = std::vector<BfCurveSet>();
}

BfEvent bfBindCurveHolderOutsideAllocator(VmaAllocator _outside_allocator)
{
	BfCurveHolder* pCurveHolder = bfGetpCurveHolder();
	pCurveHolder->outside_allocator = _outside_allocator;

	BfSingleEvent event{};
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CURVE_HOLDER_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_BIND_OUTSIDE_ALLOCATOR_FOR_CURVE_HOLDER;
	}
	return BfEvent(event);
}

BfCurveSet* BfCurveHolder::get_curve_set(BfeCurveType type)
{
	if (allocated_curves == 0) {
		throw std::runtime_error("BfCurveHolder holds 0 BfCurveSet's, can't be");
	}
	BfCurveSet* pCurveSet = nullptr;
	
	for (size_t i = 0; i < this->curve_sets.size(); i++) {
		if (curve_sets[i].type == type) {
			pCurveSet = &curve_sets[i];
			break;
		}
	}
	
	if (pCurveSet == nullptr) {
		throw std::runtime_error("BfCurveHolder doesn't hold needed type of BfCurveSet");
	}
	
	return pCurveSet;
}

BfCurveSet* BfCurveHolder::get_curve_set_by_index(size_t i)
{
	return &this->curve_sets[i];
}

void BfCurveHolder::update_obj_data(VmaAllocation allocation)
{
	size_t obj_data_count = 0;

	for (size_t i = 0; i < this->allocated_curves; i++) {
		BfCurveSet* pCurveSet = this->get_curve_set_by_index(i);
		obj_data_count += pCurveSet->ready_elements_count;
	}
	
	std::vector<BfObjectData> obj_data(obj_data_count);
	
	size_t last_obj_data_index = 0;
	for (size_t i = 0; i < this->allocated_curves; i++) {
		BfCurveSet* pCurveSet = this->get_curve_set_by_index(i);

		for (size_t j = 0; j < pCurveSet->ready_elements_count; j++) {
			obj_data[last_obj_data_index + j] = pCurveSet->curve_datas[j].obj_data;
		}
		last_obj_data_index += pCurveSet->ready_elements_count;
	}
	
	void* pobjects_data;
	vmaMapMemory(this->outside_allocator, allocation, &pobjects_data);
	{
		memcpy(pobjects_data, obj_data.data(), sizeof(BfObjectData) * obj_data.size());
	}
	vmaUnmapMemory(this->outside_allocator, allocation);
}

void BfCurveHolder::draw_indexed(VkCommandBuffer command_buffer)
{
	size_t curve_sets_count = this->allocated_curves;

	uint32_t index_offset = 0;

	for (size_t i = 0; i < curve_sets_count; i++) {
		BfCurveSet* pCurveSet = get_curve_set_by_index(i);


		pCurveSet->draw_indexed(command_buffer, index_offset);

		index_offset += pCurveSet->ready_elements_count;
	}
}
