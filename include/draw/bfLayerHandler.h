#ifndef BF_LAYER_HANDLER_H
#define BF_LAYER_HANDLER_H

#include <vulkan/vulkan_core.h>

#include <functional>
#include <stdexcept>

#include "bfDescriptor.h"
#include "bfDrawObject.h"
#include "bfVariative.hpp"

class BfLayerHandler
{
   std::vector<std::shared_ptr<BfDrawLayer>> __layers;
   //
   BfDescriptor* __pDescriptor;
   static BfLayerHandler* __pInstance;
   VmaAllocator* __pAllocator;

   VkPipeline* __pTrianglePipeline;
   VkPipeline* __pLinePipeline;

   struct __varTransaction
   {
      std::optional<BfDrawLayer::itptrVar_t> what;
      std::optional<BfDrawLayer::ptrLayer_t> above;
      std::optional<BfDrawLayer::ptrLayer_t> root;
   };

public:
   BfLayerHandler(size_t reserved_layer_count);
   BfLayerHandler();
   ~BfLayerHandler();

   static BfLayerHandler* instance() noexcept;
   VmaAllocator* allocator() noexcept;
   VkPipeline* trinagle_pipeline() noexcept;
   VkPipeline* line_pipeline() noexcept;

   BfEvent bind_descriptor(BfDescriptor* desc);
   BfEvent bind_allocator(VmaAllocator* allocator);
   // TODO: Remake pipelines
   BfEvent bind_trianle_pipeline(VkPipeline* allocator);
   BfEvent bind_line_pipeline(VkPipeline* allocator);

   BfEvent add(std::shared_ptr<BfDrawLayer> pLayer);

   BfEvent move_inner(
       size_t what_id,
       size_t where_id,
       std::function<void(int)> err_msg = nullptr
   );
   BfEvent swap_inner(
       size_t f_id, size_t s_id, std::function<void()> err_msg = nullptr
   );

   void del(size_t id);
   void kill();

   const size_t get_whole_obj_count() const noexcept;
   const size_t get_layer_count() const noexcept;

   void map_model_matrices(size_t frame_index);
   void draw(VkCommandBuffer command_buffer, VkPipeline);

   std::shared_ptr<BfDrawLayer> get_layer_by_index(size_t index);
   std::shared_ptr<BfDrawLayer> get_layer_by_id(size_t id);

   std::shared_ptr<BfDrawLayer>& get_ref_find_layer(size_t id);
   std::shared_ptr<BfDrawObj>& get_ref_find_obj(size_t id);

   BfDrawLayer::layerPair get_it_layer(size_t id);
   BfDrawLayer::objPair get_it_obj(size_t id);
   BfDrawLayer::varPair get_it_var(size_t id);

private:
   bool __is_space_for_new_layer();
   bool __is_layer_exists(uint32_t id);

   __varTransaction __gen_transaction_part(size_t id);
};

#endif
