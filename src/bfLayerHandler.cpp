#include "bfLayerHandler.h"

BfLayerHandler::BfLayerHandler(size_t reserved_layer_count)
	: __pDescriptor{ nullptr }
{
	__layers.reserve(reserved_layer_count);
}

BfLayerHandler::BfLayerHandler()
	: BfLayerHandler(40)
{
	
}

BfEvent BfLayerHandler::bind_descriptor(BfDescriptor* desc)
{
	BfSingleEvent event{};
	event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

	if (desc == nullptr) {
		event.action = BF_ACTION_TYPE_BIND_BFDESCRIPTOR_TO_LAYER_HANDLER_FAILURE;
		event.success = false;
		__pDescriptor = nullptr;
	}
	else {
		event.action = BF_ACTION_TYPE_BIND_BFDESCRIPTOR_TO_LAYER_HANDLER_SUCCESS;
		event.success = true;
		__pDescriptor = desc;
	}
	return event;
}

BfEvent BfLayerHandler::add(std::shared_ptr<BfDrawLayer> pLayer)
{
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_LAYER_EVENT;
	if (!__is_space_for_new_layer()) {
		event.action = BfEnActionType::BF_ACTION_TYPE_ADD_LAYER_TO_LAYER_HANDLER_FAILURE;
		event.success = false;
		return event;
	}
	else {
		event.action = BfEnActionType::BF_ACTION_TYPE_ADD_LAYER_TO_LAYER_HANDLER_SUCCESS;
		__layers.push_back(pLayer);
		return event;
	}
}

void BfLayerHandler::map_model_matrices(size_t frame_index)
{
	size_t obj_count = this->get_whole_obj_count();
	
	std::vector<BfObjectData> obj_datas;
	obj_datas.reserve(obj_count);

	if (!__pDescriptor->is_usage(BfDescriptorModelMtxUsage)) {
		throw std::runtime_error("No buffer for object data is ready in BfDescriptor");
	}

	void* data;
	__pDescriptor->map_descriptor(BfDescriptorModelMtxUsage, frame_index, &data);
	{
		size_t offset = 0;
		for (const auto& layer : __layers) {
			for (const auto& obj : layer->__objects) {
				BfObjectData obj_data = obj->get_obj_data();
				memcpy(reinterpret_cast<char*>(data) + offset, &obj_data, sizeof(BfObjectData));
				offset += sizeof(BfObjectData);
			}
			
			/*std::vector<BfObjectData> obj_data = layer->get_obj_model_matrices();
			if (obj_data.empty()) {
				throw std::runtime_error("Input layer object data is empty");
			}

			std::copy(obj_data.begin(), obj_data.end(), obj_datas.begin() + offset);
			offset += obj_data.size();*/
		}


	}
	__pDescriptor->unmap_descriptor(BfDescriptorModelMtxUsage, frame_index);

}

const size_t BfLayerHandler::get_whole_obj_count() const noexcept
{
	size_t size = 0;
	for (const auto& layer : __layers) {
		size += layer->get_obj_count();
	}
	return size;
}

const size_t BfLayerHandler::get_layer_count() const noexcept
{
	return __layers.size();
}

void BfLayerHandler::draw(VkCommandBuffer command_buffer, VkPipeline)
{
	size_t obj_data_offset = 0;
	for (const auto& layer : __layers) {
		std::vector<VkDeviceSize> vert_offset = { 0 };
		vkCmdBindVertexBuffers(command_buffer, 
							   0, 
							   1, 
							   layer->__buffer.get_p_vertex_buffer(), 
							   vert_offset.data());
		vkCmdBindIndexBuffer(command_buffer, 
							 *layer->__buffer.get_p_index_buffer(), 
							 0, 
							 VK_INDEX_TYPE_UINT16);
		for (size_t i = 0; i < layer->__objects.size(); i++) {
			if (layer->__objects[i]->get_bound_pPipeline() == nullptr) {
				throw std::runtime_error("Object pipeline pointer is nullptr");
			}
			vkCmdBindPipeline(command_buffer, 
							  VK_PIPELINE_BIND_POINT_GRAPHICS, 
							  *layer->__objects[i]->get_bound_pPipeline());
			layer->update_vertex_offset();
			layer->update_index_offset();
			vkCmdDrawIndexed(
				command_buffer,
				10,//layer->__objects[i]->get_indices_count(),
				1,
				layer->__index_offsets[i],
				layer->__vertex_offsets[i],
				i + obj_data_offset
			);
		}
		obj_data_offset += layer->__objects.size();



		/*for (const auto& obj : layer->__objects) {

		}*/

		/*std::vector<BfObjectData> obj_data = layer->get_obj_model_matrices();
		if (obj_data.empty()) {
			throw std::runtime_error("Input layer object data is empty");
		}

		std::copy(obj_data.begin(), obj_data.end(), obj_datas.begin() + offset);
		offset += obj_data.size();*/
	}
	
	
	/*vkCmdBindVertexBuffers(combuffer, 0, 1, __buffer.get_p_vertex_buffer(), nullptr);
	vkCmdBindIndexBuffer(combuffer, *__buffer.get_p_index_buffer(), 0, VK_INDEX_TYPE_UINT16);

	for (size_t i = 0; i < __objects.size(); i++) {
		vkCmdDrawIndexed(
			combuffer,
			__objects[i]->get_indices_count(),
			1,
			__index_offsets[i],
			__vertex_offsets[i],
			i
		);
	}*/

}

std::shared_ptr<BfDrawLayer> BfLayerHandler::get_layer_by_index(size_t index)
{
	if (index > this->get_layer_count())
		throw std::runtime_error("Index > number of layers inside handler");
	return __layers.at(index);
}

bool BfLayerHandler::__is_space_for_new_layer()
{
	return __layers.size() < __layers.capacity();
}
