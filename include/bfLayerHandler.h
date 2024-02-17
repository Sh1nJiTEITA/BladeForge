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

	BfEvent bind_descriptor(BfDescriptor* desc);
	BfEvent add(std::shared_ptr<BfDrawLayer> pLayer);
	
	void map_model_matrices(size_t frame_index);
	const size_t get_whole_obj_count() const noexcept;
	void draw(VkCommandBuffer command_buffer, VkPipeline);

private:

	bool __is_space_for_new_layer();
};



#endif