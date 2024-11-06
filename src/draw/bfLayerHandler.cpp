#include "bfLayerHandler.h"

#include "bfEvent.h"

BfLayerHandler::BfLayerHandler(size_t reserved_layer_count)
    : __pDescriptor{nullptr}
{
   if (__pInstance)
   {
      throw std::runtime_error("Layer Handler already exists");
   }
   __pInstance = this;
   __layers.reserve(reserved_layer_count);
}

BfLayerHandler::BfLayerHandler()
    : BfLayerHandler(40)
{
}

void BfLayerHandler::kill()
{
   auto killer = BfLayerKiller::get_root();
   for (auto& l : __layers)
   {
      killer->add(l);
   }
   __layers.clear();
}

BfLayerHandler::~BfLayerHandler() { kill(); }

BfLayerHandler* BfLayerHandler::__pInstance = nullptr;

BfLayerHandler* BfLayerHandler::instance() noexcept { return __pInstance; }

VmaAllocator* BfLayerHandler::allocator() noexcept { return __pAllocator; }

VkPipeline* BfLayerHandler::trinagle_pipeline() noexcept
{
   return __pTrianglePipeline;
}
VkPipeline* BfLayerHandler::line_pipeline() noexcept { return __pLinePipeline; }

BfEvent BfLayerHandler::bind_descriptor(BfDescriptor* desc)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

   if (desc == nullptr)
   {
      event.action  = BF_ACTION_TYPE_BIND_BFDESCRIPTOR_TO_LAYER_HANDLER_FAILURE;
      event.success = false;
      __pDescriptor = nullptr;
   }
   else
   {
      event.action  = BF_ACTION_TYPE_BIND_BFDESCRIPTOR_TO_LAYER_HANDLER_SUCCESS;
      event.success = true;
      __pDescriptor = desc;
   }
   return event;
}

BfEvent BfLayerHandler::bind_allocator(VmaAllocator* allocator)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (allocator)
   {
      event.action  = BF_ACTION_TYPE_BIND_ALLOCATOR_TO_LAYER_HANDLER_SUCCESS;
      event.success = true;
      __pAllocator  = allocator;
   }
   else
   {
      event.action  = BF_ACTION_TYPE_BIND_ALLOCATOR_TO_LAYER_HANDLER_FAILURE;
      event.success = false;
   }
   return event;
}

BfEvent BfLayerHandler::bind_trianle_pipeline(VkPipeline* pipeline)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (pipeline)
   {
      event.action =
          BF_ACTION_TYPE_BIND_TRIANGLE_PIPELINE_TO_LAYER_HANDLER_SUCCESS;
      event.success       = true;
      __pTrianglePipeline = pipeline;
   }
   else
   {
      event.action =
          BF_ACTION_TYPE_BIND_TRIANGLE_PIPELINE_TO_LAYER_HANDLER_FAILURE;
      event.success = false;
   }
   return event;
}

BfEvent BfLayerHandler::bind_line_pipeline(VkPipeline* pipeline)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (pipeline)
   {
      event.action = BF_ACTION_TYPE_BIND_LINE_PIPELINE_TO_LAYER_HANDLER_SUCCESS;
      event.success   = true;
      __pLinePipeline = pipeline;
   }
   else
   {
      event.action = BF_ACTION_TYPE_BIND_LINE_PIPELINE_TO_LAYER_HANDLER_FAILURE;
      event.success = false;
   }
   return event;
}

BfEvent BfLayerHandler::add(std::shared_ptr<BfDrawLayer> pLayer)
{
   if (pLayer->is_nested())
   {
      throw std::runtime_error("Layer is nested to add it to handler");
   }

   BfSingleEvent event{};
   event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_LAYER_EVENT;
   if (!__is_space_for_new_layer())
   {
      event.action =
          BfEnActionType::BF_ACTION_TYPE_ADD_LAYER_TO_LAYER_HANDLER_FAILURE;
      event.success = false;
      return event;
   }
   else
   {
      event.action =
          BfEnActionType::BF_ACTION_TYPE_ADD_LAYER_TO_LAYER_HANDLER_SUCCESS;
      __layers.push_back(pLayer);
      return event;
   }
}

void BfLayerHandler::del(size_t id)
{
   for (auto l = __layers.begin(); l != __layers.end(); l++)
   {
      if (l->get()->id.get() == id)
      {
         __layers.erase(l);
      }
   }
}

// BfEvent BfLayerHandler::del(uint32_t id) {
//	BfSingleEvent event{};
//	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_LAYER_EVENT;
//	if (!__is_layer_exists(id)) {
//		event.action =
// BfEnActionType::BF_ACTION_TYPE_DEL_LAYER_FROM_LAYER_HANDLER_FAILURE;
//		event.success = false;
//		return event;
//	}
//	else {
//		event.action =
// BfEnActionType::BF_ACTION_TYPE_DEL_LAYER_FROM_LAYER_HANDLER_SUCCESS;
//
//		for (auto it = __layers.begin(); it != __layers.end(); it++) {
//			if (id == it->get()->id.get()) {
//				__layers_to_destroy.push_back(std::move(*it));
//				__layers.erase(it);
//				return event;
//			}
//		}
//	}
// }

void BfLayerHandler::map_model_matrices(size_t frame_index)
{
   size_t obj_count = this->get_whole_obj_count();

   std::vector<BfObjectData> obj_datas;
   obj_datas.reserve(obj_count);

   if (!__pDescriptor->is_usage(BfDescriptorModelMtxUsage))
   {
      throw std::runtime_error(
          "No buffer for object data is ready in BfDescriptor");
   }

   void* data;

   __pDescriptor->map_descriptor(BfDescriptorModelMtxUsage, frame_index, &data);
   {
      size_t offset = 0;
      for (const auto& layer : __layers)
      {
         /*for (const auto& obj : layer->__objects) {
                 BfObjectData obj_data = obj->get_obj_data();
                 memcpy(reinterpret_cast<char*>(data) + offset, &obj_data,
         sizeof(BfObjectData)); offset += sizeof(BfObjectData);
         }*/

         layer->map_model_matrices(frame_index, offset, data);

         /*std::vector<BfObjectData> obj_data = layer->get_obj_model_matrices();
         if (obj_data.empty()) {
                 throw std::runtime_error("Input layer object data is empty");
         }

         std::copy(obj_data.begin(), obj_data.end(), obj_datas.begin() +
         offset); offset += obj_data.size();*/
      }
   }

   __pDescriptor->unmap_descriptor(BfDescriptorModelMtxUsage, frame_index);
}

const size_t BfLayerHandler::get_whole_obj_count() const noexcept
{
   size_t size = 0;
   for (const auto& layer : __layers)
   {
      size += layer->get_obj_count_downside();
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

   for (const auto& layer : __layers)
   {
      size_t vertex_offset = 0;
      size_t index_offset  = 0;

      layer->draw(command_buffer, obj_data_offset, index_offset, vertex_offset);
   }

   /*vkCmdBindVertexBuffers(combuffer, 0, 1, __buffer.get_p_vertex_buffer(),
   nullptr); vkCmdBindIndexBuffer(combuffer, *__buffer.get_p_index_buffer(), 0,
   VK_INDEX_TYPE_UINT16);

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

std::shared_ptr<BfDrawLayer> BfLayerHandler::get_layer_by_id(size_t id)

{
   for (auto& it : __layers)
   {
      if (it->id.get() == id)
      {
         return it;
      }
   }
   std::cout << "NO LAYER FOUND\n";
   abort();
}

bool BfLayerHandler::__is_space_for_new_layer()
{
   return __layers.size() < __layers.capacity();
}

bool BfLayerHandler::__is_layer_exists(uint32_t id)
{
   for (const auto& it : __layers)
   {
      if (it->id.get() == id)
      {
         return true;
      }
   }
   return false;
}
