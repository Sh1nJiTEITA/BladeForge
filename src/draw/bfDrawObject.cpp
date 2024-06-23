#include "bfDrawObject.h"

// BfDrawObj::BfDrawObj() :
//	id()
//{
//
// }
//// 8(495) 377 10-86
// BfDrawObj::BfDrawObj(const BfDrawObj& obj)
//{
//	this->__vertices = obj.__vertices;
//	this->__dvertices = obj.__dvertices;
//	this->__indices = obj.__indices;
//	this->id = obj.id;
// }

BfDrawObj::BfDrawObj()
    : BfDrawObj(0)
{
}

BfDrawObj::BfDrawObj(uint32_t type)
    : id(type)
{
}

BfDrawObj::BfDrawObj(const std::vector<BfVertex3> &dvert, uint32_t type)
    : __dvertices{dvert}, id(type)
{
}

bool BfDrawObj::is_ok()
{
   bool decision =
       !__vertices.empty() * !__indices.empty() * BfObjID::is_id_exists(id);

   return decision;
}

const std::vector<BfVertex3> &BfDrawObj::get_rVertices() const
{
   return __vertices;
}

const std::vector<BfVertex3> &BfDrawObj::get_rdVertices() const
{
   return __dvertices;
}

const std::vector<uint32_t> &BfDrawObj::get_rIndices() const
{
   return __indices;
}

BfVertex3 *BfDrawObj::get_pVertices() { return __vertices.data(); }

BfVertex3 *BfDrawObj::get_pdVertices() { return __dvertices.data(); }

uint32_t *BfDrawObj::get_pIndices() { return __indices.data(); }

BfObjectData BfDrawObj::get_obj_data()
{
   BfObjectData data{};
   data.id             = this->id.get();
   data.model_matrix   = __model_matrix;
   data.line_thickness = __line_thickness;

   if (__is_selected)
      data.select_color = glm::vec3(1.0f, 0.5f, 0.0f);
   else
      data.select_color = glm::vec3(1.0f, 1.0f, 1.0f);
   return data;
}

const size_t BfDrawObj::get_vertices_count() const { return __vertices.size(); }

const size_t BfDrawObj::get_dvertices_count() const
{
   return __dvertices.size();
}

const size_t BfDrawObj::get_indices_count() const { return __indices.size(); }

size_t BfDrawObj::get_vertex_data_size()
{
   return sizeof(BfVertex3) * __vertices.size();
}

size_t BfDrawObj::get_index_data_size()
{
   return sizeof(uint32_t) * __indices.size();
}

VkPipeline *BfDrawObj::get_bound_pPipeline() { return __pPipeline; }

// void BfDrawObj::set_obj_data(BfObjectData obj_data)
//{
//	__obj_data = obj_data;
// }

glm::mat4 &BfDrawObj::get_model_matrix() { return __model_matrix; }

void BfDrawObj::bind_pipeline(VkPipeline *pPipeline)
{
   __pPipeline = pPipeline;
}

void BfDrawObj::set_color(glm::vec3 c) { __main_color = c; }

void BfDrawObj::create_indices()
{
   // TODO
   if (!__indices.empty()) __indices.clear();

   for (size_t i = 0; i < __vertices.size(); ++i)
   {
      __indices.emplace_back(i);
   }
}

void BfDrawObj::create_vertices() {}

void BfDrawObj::clear_vetices() { __vertices.clear(); }
void BfDrawObj::clear_indices() { __indices.clear(); }

std::unordered_set<unsigned int> BfObjID::__existing_values;
std::map<uint32_t, uint32_t>     BfObjID::__existing_pairs;

BfObjID::BfObjID()
    : BfObjID(0)
{
}

BfObjID::BfObjID(uint32_t type)
    : __type{type}
{
   static uint32_t value = 0;
   __value               = ++value;
   BfObjID::__existing_values.insert(__value);
   BfObjID::__existing_pairs.insert(std::make_pair(__value, __type));
}

BfObjID::~BfObjID()
{
   // std::cout << "\nID DECONSTRUCTOR\n";
   BfObjID::__existing_values.erase(__value);
   BfObjID::__existing_pairs.erase(__value);
}

const uint32_t BfObjID::get() const { return __value; }

const uint32_t BfObjID::get_type() const { return __type; }

bool BfObjID::is_id_exists(uint32_t id)
{
   if (__existing_values.contains(id))
      return true;
   else
      return false;
}

bool BfObjID::is_id_exists(BfObjID &id)
{
   return BfObjID::is_id_exists(id.get());
}

uint32_t BfObjID::find_type(uint32_t type)
{
   return BfObjID::__existing_pairs[type];
}

BfDrawLayer::BfDrawLayer(VmaAllocator allocator,
                         size_t       vertex_size,
                         size_t       max_vertex_count,
                         size_t       max_reserved_count,
                         bool         is_nested,
                         uint32_t     id_type)

    : __reserved_n{static_cast<uint32_t>(max_reserved_count)}
    , __buffer{allocator,
               vertex_size,
               max_vertex_count,
               max_reserved_count,
               is_nested}
    , id{id_type}

{
   __objects.reserve(__reserved_n);
   __vertex_offsets.resize(__reserved_n, 0);
   __index_offsets.resize(__reserved_n, 0);
}

BfDrawLayer::BfDrawLayer(const BfDrawLayerCreateInfo &info)
    : BfDrawLayer(info.allocator,
                  info.vertex_size,
                  info.max_vertex_count,
                  info.max_reserved_count,
                  info.is_nested,
                  info.id_type)
{
}

BfDrawLayer::~BfDrawLayer() {}

bool BfDrawLayer::is_nested() const noexcept { return __buffer.is_nested(); }

void BfDrawLayer::remake()
{
   for (auto &it : __layers)
   {
      it->remake();
   }

   this->del_all();
}

std::vector<std::shared_ptr<BfDrawObj>>::iterator BfDrawLayer::begin()
{
   return __objects.begin();
}
std::vector<std::shared_ptr<BfDrawObj>>::iterator BfDrawLayer::end()
{
   return __objects.end();
}

const size_t BfDrawLayer::get_whole_vertex_count() const noexcept
{
   size_t count = 0;
   for (auto &it : __objects)
   {
      count += it->get_vertices_count();
   }
   return count;
}

const size_t BfDrawLayer::get_whole_index_count() const noexcept
{
   size_t count = 0;
   for (auto &it : __objects)
   {
      count += it->get_indices_count();
   }
   return count;
}

const size_t BfDrawLayer::get_obj_count() const noexcept
{
   return __objects.size();
}

const size_t BfDrawLayer::get_layer_count() const noexcept
{
   return __layers.size();
}

const size_t BfDrawLayer::get_obj_count_downside() const noexcept
{
   if (__layers.empty()) return __objects.size();
   size_t size = 0;
   for (auto &l : __layers)
   {
      size += l->get_obj_count_downside();
   }
   return size + __objects.size();
}

const std::vector<BfObjectData> BfDrawLayer::get_obj_model_matrices()
    const noexcept
{
   std::vector<BfObjectData> obj_data;
   obj_data.reserve(this->get_obj_count());

   for (const auto &obj : __objects)
   {
      obj_data.push_back(obj->get_obj_data());
   }

   return obj_data;
}

void BfDrawLayer::add(std::shared_ptr<BfDrawObj> obj)
{
   if (!obj->is_ok()) throw std::runtime_error("object is incorrect");
   __objects.emplace_back(obj);
}

void BfDrawLayer::add(std::shared_ptr<BfDrawLayer> layer)
{
   if (!layer->is_nested())
      throw std::runtime_error(
          "Import layer is not nested, can't add to over layer");
   __layers.emplace_back(layer);
}

void BfDrawLayer::add_l(std::shared_ptr<BfDrawObj> obj)
{
   __objects.emplace_back(obj);
}

void generate_draw_data() {}

void BfDrawLayer::del(uint32_t id)
{
   std::shared_ptr<BfDrawLayer> out_layer;

   __objects.erase(
       std::remove_if(__objects.begin(),
                      __objects.end(),
                      [id](const auto &obj) { return obj->id.get() == id; }),
       __objects.end());

   for (auto l = __layers.begin(); l != __layers.end(); l++)
   {
      if (l->get()->id.get() == id)
      {
         out_layer = std::move(*l);
         __layers.erase(l);
         break;
      }
   }

   // TODO: BE CAREFULL ABORT WAS NOT COMMENTED
   if (out_layer.get() == nullptr)
   {
      // abort();
   }
   else
   {
      auto root = BfLayerKiller::get_root();
      root->add(out_layer);
   }

   //__layers.erase(
   //	std::remove_if(__layers.begin(), __layers.end(),
   //		[id](const auto& layer) { return layer->id.get() == id; }),
   //	__layers.end()
   //);

   this->update_buffer();
}

void BfDrawLayer::del(const std::vector<uint32_t> &id)
{
   if (id.size() == 0) return;

   auto con_o = [&id](std::shared_ptr<BfDrawObj> obj) {
      auto it = std::find(id.begin(), id.end(), obj->id.get());

      if (it != id.end())
      {
         return true;
      }
      else
         return false;
   };

   std::vector<std::shared_ptr<BfDrawLayer>> out_layers;

   auto con_l = [&id, &out_layers](std::shared_ptr<BfDrawLayer> obj) {
      auto it = std::find(id.begin(), id.end(), obj->id.get());

      if (it != id.end())
      {
         out_layers.push_back(std::move(obj));
         return true;
      }
      else
         return false;
   };

   auto rem_o = std::remove_if(__objects.begin(), __objects.end(), con_o);
   __objects.erase(rem_o, __objects.end());

   auto rem_l = std::remove_if(__layers.begin(), __layers.end(), con_l);

   for (auto &l : out_layers)
   {
      auto root = BfLayerKiller::get_root();
      root->add(l);
   }

   __layers.erase(rem_l, __layers.end());

   this->update_buffer();
}

void BfDrawLayer::del_all()
{
   std::vector<uint32_t> ids;
   ids.reserve(__objects.size() + __layers.size());
   for (auto &o : __objects)
   {
      ids.push_back(o->id.get());
   }
   /*for (auto& l : __layers) {
           ids.push_back(l->id.get());
   }*/
   this->del(ids);
}

void BfDrawLayer::generate_draw_data()
{
   for (size_t i = 0; i < this->get_obj_count(); ++i)
   {
      auto obj = this->get_object_by_index(i);
      // obj->set_color({ 1.0f, 1.0f, 1.0f });
      obj->create_vertices();
      obj->create_indices();
      // obj->bind_pipeline(&__info.pipeline);
   }
   this->update_buffer();
}

std::vector<int32_t> &BfDrawLayer::update_vertex_offset()
{
   __vertex_offsets.clear();
   __vertex_offsets.resize(__objects.size());

   size_t growing_offset = 0;
   for (size_t i = 0; i < __objects.size(); i++)
   {
      __vertex_offsets[i] = growing_offset;
      growing_offset += __objects[i]->get_vertices_count();
   }
   return __vertex_offsets;
}

std::vector<int32_t> &BfDrawLayer::update_index_offset()
{
   __index_offsets.clear();
   __index_offsets.resize(__objects.size());

   size_t growing_offset = 0;
   for (size_t i = 0; i < __objects.size(); i++)
   {
      __index_offsets[i] = growing_offset;
      growing_offset += __objects[i]->get_indices_count();
   }
   return __index_offsets;
}

void BfDrawLayer::update_nested(void *v, void *i, size_t &off_v, size_t &off_i)
{
   if (!is_nested()) throw std::runtime_error("layer is not nested");
   if (v == nullptr) throw std::runtime_error("vertex* = nullptr");
   if (i == nullptr) throw std::runtime_error("index* = nullptr");

   for (const auto &obj : __objects)
   {
      size_t size_v = sizeof(BfVertex3) * obj->get_vertices_count();

      memcpy(reinterpret_cast<char *>(v) + off_v, obj->get_pVertices(), size_v);
      off_v += size_v;

      size_t size_i = sizeof(uint32_t) * obj->get_indices_count();
      memcpy(reinterpret_cast<char *>(i) + off_i, obj->get_pIndices(), size_i);
      off_i += size_i;
   }

   for (const auto &layer : __layers)
   {
      layer->update_nested(v, i, off_v, off_i);
   }

   this->update_vertex_offset();
   this->update_index_offset();
}

void BfDrawLayer::update_buffer()
{
   if (!is_nested())
   {
      void *vertex_data = __buffer.map_vertex_memory();
      void *index_data  = __buffer.map_index_memory();

      size_t offset_v   = 0;
      for (const auto &obj : __objects)
      {
         size_t size = sizeof(BfVertex3) * obj->get_vertices_count();

         memcpy(reinterpret_cast<char *>(vertex_data) + offset_v,
                obj->get_pVertices(),
                size);

         offset_v += size;
      }

      size_t offset_i = 0;
      for (const auto &obj : __objects)
      {
         size_t size = sizeof(uint32_t) * obj->get_indices_count();

         memcpy(reinterpret_cast<char *>(index_data) + offset_i,
                obj->get_pIndices(),
                size);

         offset_i += size;
      }

      for (const auto &layer : __layers)
      {
         layer->update_nested(vertex_data, index_data, offset_v, offset_i);
      }

      __buffer.unmap_vertex_memory();
      __buffer.unmap_index_memory();

      this->update_vertex_offset();
      this->update_index_offset();
   }
}

void BfDrawLayer::clear_buffer()
{
   if (!is_nested())
   {
      __buffer.clear_index_buffer();
      __buffer.clear_vertex_buffer();
   }
}

void BfDrawLayer::set_color(glm::vec3 c)
{
   for (auto &l : __layers)
   {
      l->set_color(c);
   }
   for (auto &o : __objects)
   {
      o->set_color(c);
   }
}

void BfDrawLayer::draw(VkCommandBuffer combuffer,
                       size_t         &offset,
                       size_t         &index_offset,
                       size_t         &vertex_offset)
{
   if (!is_nested())
   {
      std::vector<VkDeviceSize> vert_offset = {0};
      // Local elements
      vkCmdBindVertexBuffers(combuffer,
                             0,
                             1,
                             __buffer.get_p_vertex_buffer(),
                             vert_offset.data());
      vkCmdBindIndexBuffer(combuffer,
                           *__buffer.get_p_index_buffer(),
                           0,
                           VK_INDEX_TYPE_UINT32);
   }

   // this->update_vertex_offset();
   // this->update_index_offset();

   VkPipeline *current_pipeline = nullptr;

   for (size_t i = 0; i < __objects.size(); i++)
   {
      if (__objects[i] == nullptr) continue;
      if (__objects[i]->get_bound_pPipeline() == nullptr)
      {
         throw std::runtime_error("Object pipeline pointer is nullptr");
      }
      if (!__objects[i]->is_draw) continue;

      if (!current_pipeline ||
          current_pipeline != __objects[i]->get_bound_pPipeline())
      {
         current_pipeline = __objects[i]->get_bound_pPipeline();
         vkCmdBindPipeline(combuffer,
                           VK_PIPELINE_BIND_POINT_GRAPHICS,
                           *current_pipeline);
      }

      vkCmdDrawIndexed(combuffer,
                       __objects[i]->get_indices_count(),
                       1,
                       __index_offsets[i] + index_offset,
                       __vertex_offsets[i] + vertex_offset,
                       i + offset);
   }
   if (!__objects.empty())
   {
      vertex_offset += this->get_whole_vertex_count();
      index_offset += this->get_whole_index_count();
      offset += __objects.size()-1;
   }

   for (const auto &layer : __layers)
   {
      layer->draw(combuffer, offset, index_offset, vertex_offset);
   }
}

void BfDrawLayer::map_model_matrices(size_t  frame_index,
                                     size_t &offset,
                                     void   *data)
{
   for (size_t i = 0; i < __objects.size(); i++)
   {
      if (!__objects[i]->is_draw) continue;
      BfObjectData obj_data = __objects[i]->get_obj_data();
      memcpy(reinterpret_cast<char *>(data) + offset,
             &obj_data,
             sizeof(BfObjectData));
      offset += sizeof(BfObjectData);
   }
   for (size_t i = 0; i < __layers.size(); i++)
   {
      __layers[i]->map_model_matrices(frame_index, offset, data);
   }
}

std::shared_ptr<BfDrawObj> BfDrawLayer::get_object_by_index(size_t index)
{
   if (index >= __objects.size())
      return nullptr;
   else
      // throw std::runtime_error("input object index > objects in layer");
      return __objects.at(index);
}

std::shared_ptr<BfDrawLayer> BfDrawLayer::get_layer_by_index(size_t index)
{
   if (index >= __layers.size())
      return nullptr;
   else
      // throw std::runtime_error("input object index > objects in layer");
      return __layers.at(index);
}

bool *BfGuiIntegration::get_pSelection() { return &__is_selected; }

BfLayerKiller *BfLayerKiller::__p = nullptr;

BfLayerKiller::BfLayerKiller()
{
   __layers.reserve(10000);
   BfLayerKiller::set_root(this);
}

BfLayerKiller::~BfLayerKiller() { __p == nullptr; }

void BfLayerKiller::add(std::shared_ptr<BfDrawLayer> layer)
{
   __layers.push_back(std::move(layer));
}

void BfLayerKiller::kill() { __layers.clear(); }

BfLayerKiller *BfLayerKiller::get_root() { return __p; }
void           BfLayerKiller::set_root(BfLayerKiller *k) { __p = k; }
