#ifndef BF_LAYER_HANDLER_H
#define BF_LAYER_HANDLER_H

#include "bfDrawObject.h"
#include "bfDescriptor.h"


class BfLayerHandler {
	std::vector<std::shared_ptr<BfDrawLayer>> __layers;
	BfDescriptor* __pDescriptor;

public:
	

	BfLayerHandler(size_t reserved_layer_count);
	BfLayerHandler();
	~BfLayerHandler();


	BfEvent bind_descriptor(BfDescriptor* desc);
	BfEvent add(std::shared_ptr<BfDrawLayer> pLayer);
	void del(size_t id);

	const size_t get_whole_obj_count() const noexcept;
	const size_t get_layer_count() const noexcept;
	
	

	void map_model_matrices(size_t frame_index);
	void draw(VkCommandBuffer command_buffer, VkPipeline);

	std::shared_ptr<BfDrawLayer> get_layer_by_index(size_t index);
	std::shared_ptr<BfDrawLayer> get_layer_by_id(size_t id);

private:

	bool __is_space_for_new_layer();
	bool __is_layer_exists(uint32_t id);
};

#endif