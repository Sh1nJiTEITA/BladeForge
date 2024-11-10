#include "bfLayerHandler.h"

#include <algorithm>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>

#include "bfDrawObject.h"
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

void
BfLayerHandler::kill()
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

BfLayerHandler*
BfLayerHandler::instance() noexcept
{
   return __pInstance;
}

VmaAllocator*
BfLayerHandler::allocator() noexcept
{
   return __pAllocator;
}

VkPipeline*
BfLayerHandler::trinagle_pipeline() noexcept
{
   return __pTrianglePipeline;
}
VkPipeline*
BfLayerHandler::line_pipeline() noexcept
{
   return __pLinePipeline;
}

BfEvent
BfLayerHandler::bind_descriptor(BfDescriptor* desc)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

   if (desc == nullptr)
   {
      event.action = BF_ACTION_TYPE_BIND_BFDESCRIPTOR_TO_LAYER_HANDLER_FAILURE;
      event.success = false;
      __pDescriptor = nullptr;
   }
   else
   {
      event.action = BF_ACTION_TYPE_BIND_BFDESCRIPTOR_TO_LAYER_HANDLER_SUCCESS;
      event.success = true;
      __pDescriptor = desc;
   }
   return event;
}

BfEvent
BfLayerHandler::bind_allocator(VmaAllocator* allocator)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (allocator)
   {
      event.action = BF_ACTION_TYPE_BIND_ALLOCATOR_TO_LAYER_HANDLER_SUCCESS;
      event.success = true;
      __pAllocator = allocator;
   }
   else
   {
      event.action = BF_ACTION_TYPE_BIND_ALLOCATOR_TO_LAYER_HANDLER_FAILURE;
      event.success = false;
   }
   return event;
}

BfEvent
BfLayerHandler::bind_trianle_pipeline(VkPipeline* pipeline)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (pipeline)
   {
      event.action =
          BF_ACTION_TYPE_BIND_TRIANGLE_PIPELINE_TO_LAYER_HANDLER_SUCCESS;
      event.success = true;
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

BfEvent
BfLayerHandler::bind_line_pipeline(VkPipeline* pipeline)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (pipeline)
   {
      event.action = BF_ACTION_TYPE_BIND_LINE_PIPELINE_TO_LAYER_HANDLER_SUCCESS;
      event.success = true;
      __pLinePipeline = pipeline;
   }
   else
   {
      event.action = BF_ACTION_TYPE_BIND_LINE_PIPELINE_TO_LAYER_HANDLER_FAILURE;
      event.success = false;
   }
   return event;
}

BfEvent
BfLayerHandler::add(std::shared_ptr<BfDrawLayer> pLayer)
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

BfEvent
BfLayerHandler::move_inner(
    size_t what, size_t where, std::function<void(int)> err_msg
)
{
   BfSingleEvent event{.type = BF_SINGLE_EVENT_TYPE_USER_EVENT};

   auto what_transaction = __gen_transaction_part(what);
   auto where_transaction = __gen_transaction_part(where);

   if (!where_transaction.what.has_value() ||
       !what_transaction.what.has_value())
   {
      // Do no exist
      if (err_msg) err_msg(0);
      event.action =
          BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_LAYER_OR_OBJ_NOT_EXISTS;
      return event;
   }
   if (what_transaction.above == nullptr)
   {
      // Nested
      if (err_msg) err_msg(1);
      event.action = BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_LAYER_NOT_NESTED;
      return event;
   }
   return std::visit(
       [&](auto&& it_what, auto&& it_where) {
          using what_t = std::decay_t<decltype(it_what)>;
          using where_t = std::decay_t<decltype(it_where)>;

          if constexpr (std::is_same_v<where_t, BfDrawLayer::itptrObj_t>)
          {
             if (err_msg) err_msg(2);
             event.action =
                 BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_MOVING_INSIDE_OBJ;
             return event;
          }

          if constexpr (std::is_same_v<what_t, BfDrawLayer::itptrLayer_t> &&
                        std::is_same_v<where_t, BfDrawLayer::itptrLayer_t>)
          {
             BfDrawLayer::ptrLayer_t tmp = *it_what;
             what_transaction.above->get()->del(tmp->id.get(), false);
             it_where->get()->add(tmp);
             where_transaction.root->get()->update_buffer();

             event.action =
                 BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_MOVING_LAYER_TO_LAYER;
             return event;
          }

          if constexpr (std::is_same_v<what_t, BfDrawLayer::itptrObj_t> &&
                        std::is_same_v<where_t, BfDrawLayer::itptrLayer_t>)
          {
             BfDrawLayer::ptrObj_t tmp = *it_what;
             what_transaction.above->get()->del(tmp->id.get(), false);
             it_where->get()->add(tmp);
             where_transaction.root->get()->update_buffer();

             event.action =
                 BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_MOVING_OBJ_TO_LAYER;
             return event;
          }
       },
       what_transaction.what.value(),
       where_transaction.what.value()
   );
}

BfEvent
BfLayerHandler::swap_inner(size_t f, size_t s, std::function<void(int)> err_msg)
{
   BfSingleEvent event{.type = BF_SINGLE_EVENT_TYPE_USER_EVENT};

   auto f_transaction = __gen_transaction_part(f);
   auto s_transaction = __gen_transaction_part(s);

   if (!f_transaction.what.has_value() || !s_transaction.what.has_value())
   {
      // Do no exist
      if (err_msg) err_msg(0);
      event.action =
          BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_LAYER_OR_OBJ_NOT_EXISTS;
      return event;
   }
   return std::visit(
       [&](auto&& it_f, auto&& it_s) {
          using f_t = std::decay_t<decltype(it_f)>;
          using s_t = std::decay_t<decltype(it_s)>;

          if constexpr (std::is_same_v<f_t, BfDrawLayer::itptrObj_t> &&
                        std::is_same_v<s_t, BfDrawLayer::itptrObj_t>)
          {
             std::swap(*it_f, *it_s);
             f_transaction.root->get()->update_buffer();
             s_transaction.root->get()->update_buffer();
             event.action =
                 BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_SWAPPING_OBJ_WITH_OBJ;
             event.info =
                 "Swapping obj (" + std::to_string(it_f->get()->id.get()) +
                 ") with obj (" + std::to_string(it_s->get()->id.get()) + ")";
             return event;
          }
          else if constexpr (std::is_same_v<f_t, BfDrawLayer::itptrLayer_t> &&
                             std::is_same_v<s_t, BfDrawLayer::itptrObj_t>)
          {
             BfDrawLayer::ptrLayer_t ptr_f = *it_f;
             BfDrawLayer::ptrObj_t ptr_s = *it_s;
             s_transaction.above->get()->add(ptr_f);
             f_transaction.above->get()->add(ptr_s);
             s_transaction.above->get()->del(ptr_s->id.get());
             f_transaction.above->get()->del(ptr_f->id.get());
             f_transaction.root->get()->update_buffer();
             s_transaction.root->get()->update_buffer();

             event.action =
                 BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_SWAPPING_LAYER_WITH_OBJ;
             event.info = "Swapping layer (" +
                          std::to_string(it_f->get()->id.get()) +
                          ") with "
                          "obj (" +
                          std::to_string(it_s->get()->id.get()) + ")";
             return event;
          }
          else if constexpr (std::is_same_v<f_t, BfDrawLayer::itptrObj_t> &&
                             std::is_same_v<s_t, BfDrawLayer::itptrLayer_t>)
          {
             BfDrawLayer::ptrObj_t ptr_f = *it_f;
             BfDrawLayer::ptrLayer_t ptr_s = *it_s;
             s_transaction.above->get()->add(ptr_f);
             f_transaction.above->get()->add(ptr_s);
             s_transaction.above->get()->del(ptr_s->id.get());
             f_transaction.above->get()->del(ptr_f->id.get());
             f_transaction.root->get()->update_buffer();
             s_transaction.root->get()->update_buffer();

             event.action =
                 BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_SWAPPING_OBJ_WITH_LAYER;
             event.info = "Swapping obj (" +
                          std::to_string(it_f->get()->id.get()) +
                          ") with "
                          "layer (" +
                          std::to_string(it_s->get()->id.get()) + ")";
             return event;
          }
          else if constexpr (std::is_same_v<f_t, BfDrawLayer::itptrLayer_t> &&
                             std::is_same_v<s_t, BfDrawLayer::itptrLayer_t>)
          {
             std::swap(*it_f, *it_s);
             f_transaction.root->get()->update_buffer();
             s_transaction.root->get()->update_buffer();
             event.action =
                 BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_SWAPPING_LAYER_WITH_LAYER;
             event.info =
                 "Swapping layer (" + std::to_string(it_f->get()->id.get()) +
                 ") with layer (" + std::to_string(it_s->get()->id.get()) + ")";
             return event;
          }
          else
          {
             event.action =
                 BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_SWAPPING_UNDERFINED;
             return event;
          }
       },
       f_transaction.what.value(),
       s_transaction.what.value()
   );
}

void
BfLayerHandler::del(size_t id)
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

void
BfLayerHandler::map_model_matrices(size_t frame_index)
{
   size_t obj_count = this->get_whole_obj_count();

   std::vector<BfObjectData> obj_datas;
   obj_datas.reserve(obj_count);

   if (!__pDescriptor->is_usage(BfDescriptorModelMtxUsage))
   {
      throw std::runtime_error(
          "No buffer for object data is ready in BfDescriptor"
      );
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

const size_t
BfLayerHandler::get_whole_obj_count() const noexcept
{
   size_t size = 0;
   for (const auto& layer : __layers)
   {
      size += layer->get_obj_count_downside();
   }
   return size;
}

const size_t
BfLayerHandler::get_layer_count() const noexcept
{
   return __layers.size();
}

void
BfLayerHandler::draw(VkCommandBuffer command_buffer, VkPipeline)
{
   size_t obj_data_offset = 0;

   for (const auto& layer : __layers)
   {
      size_t vertex_offset = 0;
      size_t index_offset = 0;

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

std::shared_ptr<BfDrawLayer>
BfLayerHandler::get_layer_by_index(size_t index)
{
   if (index > this->get_layer_count())
      throw std::runtime_error("Index > number of layers inside handler");
   return __layers.at(index);
}

std::shared_ptr<BfDrawLayer>
BfLayerHandler::get_layer_by_id(size_t id)
{
   for (auto& l : __layers)
   {
      if (l->id.get() == id)
      {
         return l;
      }
      if (l->get_layer_count())
      {
         auto found = l->get_layer_by_id(id);
         if (found) return found;
      }
   }
   return nullptr;
}

bool
BfLayerHandler::__is_space_for_new_layer()
{
   return __layers.size() < __layers.capacity();
}

bool
BfLayerHandler::__is_layer_exists(uint32_t id)
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

BfLayerHandler::__varTransaction
BfLayerHandler::__gen_transaction_part(size_t id)
{
   __varTransaction t{};
   for (auto l = __layers.begin(); l != __layers.end(); l++)
   {
      if (l->get()->id.get() == id)
      {
         return {.what = l, .above = nullptr, .root = *l};
      }
      else
      {
         auto found = l->get()->__it_find_var_by_id(id);
         if (found.second.has_value())
         {
            return {
                .what = found.second.value(),
                .above = found.first,
                .root = *l
            };
         }
      }
   }
   return {.what = std::nullopt, .above = nullptr, .root = nullptr};
}

std::shared_ptr<BfDrawLayer>&
BfLayerHandler::get_ref_find_layer(size_t id)
{
   for (auto& l : __layers)
   {
      if (l->id.get() == id)
      {
         return l;
      }
      std::shared_ptr<BfDrawLayer>& found = l->__ref_find_layer_by_id(id);
      if (found)
      {
         return found;
      }
   }
   static std::shared_ptr<BfDrawLayer> null_obj = nullptr;
   return null_obj;
}

std::shared_ptr<BfDrawObj>&
BfLayerHandler::get_ref_find_obj(size_t id)
{
   for (auto& l : __layers)
   {
      std::shared_ptr<BfDrawObj>& found = l->__ref_find_obj_by_id(id);
      if (found)
      {
         return found;
      }
   }
   static std::shared_ptr<BfDrawObj> null_obj = nullptr;
   return null_obj;
}

BfDrawLayer::layerPair
BfLayerHandler::get_it_layer(size_t id)
{
   for (auto l = __layers.begin(); l != __layers.end(); l++)
   {
      if (l->get()->id.get() == id)
      {
         return BfDrawLayer::layerPair(nullptr, l);
      }
      else
      {
         auto found = l->get()->__it_find_layer_by_id(id);
         if (found.second.has_value())
         {
            return found;
         }
      }
   }
   return BfDrawLayer::layerPair(nullptr, std::nullopt);
}

BfDrawLayer::objPair
BfLayerHandler::get_it_obj(size_t id)
{
   for (auto l = __layers.begin(); l != __layers.end(); ++l)
   {
      auto found = l->get()->__it_find_obj_by_id(id);
      if (found.second.has_value())
      {
         return found;
      }
   }
   return BfDrawLayer::objPair(nullptr, std::nullopt);
}

BfDrawLayer::varPair
BfLayerHandler::get_it_var(size_t id)
{
   for (auto l = __layers.begin(); l != __layers.end(); ++l)
   {
      if (l->get()->id.get() == id)
      {
         return BfDrawLayer::varPair(nullptr, l);
      }
      auto found = l->get()->__it_find_var_by_id(id);
      if (found.second.has_value())
      {
         return found;
      }
   }
   return BfDrawLayer::varPair(nullptr, std::nullopt);
}
