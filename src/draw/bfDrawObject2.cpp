#include "bfDrawObject2.h"

#include <stdexcept>

namespace obj
{

BfDrawControlProxy::BfDrawControlProxy(BfDrawObjectBase& obj)
    : m_obj{obj}
{
}

std::vector<int32_t>
BfDrawControlProxy::vertexOffset() const
{
   // Check not to calc offsets for OBJECT
   if (m_obj.m_type == BfDrawObjectBase::OBJECT)
   {
      throw std::runtime_error("OBJECT cant have vertex layer offsets");
   }

   std::vector<int32_t> offset;
   offset.reserve(m_obj.m_children.size());
   size_t growing_offset = 0;

   for (size_t i = 0; i < m_obj.m_children.size(); i++)
   {
      // auto size = m_obj.m_children[i]->vertices().size();
      //
      // // Check if layer has vertices inside?
      // if ((m_obj.m_type == BfDrawObjectBase::LAYER) && (size != 0))
      // {
      //    throw std::runtime_error(
      //        "Trying to calc vertexOffset() but found "
      //        "vertices inside LAYER object"
      //    );
      // }

      // offset[i] = growing_offset;
      // growing_offset += size;
      // if (m_obj.m_children[i]->m_type == BfDrawObjectBase::LAYER) continue;

      auto size = m_obj.m_children[i]->vertices().size();
      offset.push_back(growing_offset);
      growing_offset += size;
   }
   return offset;
}

std::vector<int32_t>
BfDrawControlProxy::indexOffset() const
{
   if (m_obj.m_type == BfDrawObjectBase::OBJECT)
   {
      throw std::runtime_error("OBJECT cant have index layer offsets");
   }

   std::vector<int32_t> offset;
   offset.reserve(m_obj.m_children.size());
   size_t growing_offset = 0;

   for (size_t i = 0; i < m_obj.m_children.size(); i++)
   {
      // if ((m_obj.m_type == BfDrawObjectBase::LAYER))
      // {
      //    throw std::runtime_error(
      //        "Trying to calc vertexOffset() but found "
      //        "vertices inside LAYER object"
      //    );
      // }
      // if (m_obj.m_children[i]->m_type == BfDrawObjectBase::LAYER) continue;

      auto size = m_obj.m_children[i]->indices().size();
      offset.push_back(growing_offset);
      growing_offset += size;
   }
   return offset;
}

const glm::mat4&
BfDrawControlProxy::model() const
{
   if (m_obj.m_type == BfDrawObjectBase::LAYER)
   {
      throw std::runtime_error(
          "[BfDrawControlProxy] cant use model() method inside LAYER"
      );
   }
   return m_obj.m_modelMatrix;
}
bool
BfDrawControlProxy::isBuffer() const
{
   return m_obj.m_type == BfDrawObjectBase::ROOT_LAYER;
}

void
BfDrawControlProxy::mapModel(size_t frame_index, size_t& offset, void* data)
    const
{
   auto& g = m_obj;

   // If current DrawObject is OBJECT -> map model data
   // to descriptor and add offset
   if (g.m_type == BfDrawObjectBase::OBJECT)
   {
      BfObjectData obj_data = g._objectData();
      memcpy(
          reinterpret_cast<char*>(data) + offset,
          &obj_data,
          sizeof(BfObjectData)
      );
      offset += sizeof(BfObjectData);
   }
   else
   {
      for (size_t i = 0; i < g.m_children.size(); i++)
      {
         g.m_children[i]->control().mapModel(frame_index, offset, data);
      }
   }
}

void
BfDrawControlProxy::updateBuffer(void* v, void* i, size_t* off_v, size_t* off_i)
{
   auto& g = m_obj;

   // Flexible lambda to map both index/vertex data
   auto innerMap = [](auto container,
                      size_t single_size,
                      void* data,
                      size_t* offset) {
      // Size of container to copy to
      // sizeof(typename std::decay_t<decltype(container)>::value_type) *
      // container.size();
      // Copy transoformed byte codes to buffer pointer
      std::cout << "copying single_size=" << single_size << "\n";
      size_t size = container.size() * single_size;
      memcpy(reinterpret_cast<char*>(data) + *offset, container.data(), size);
      *offset += size;
   };

   if (isBuffer() && (v || i || off_v || off_i))
   {
      throw std::runtime_error(
          "Trying to update buffer but updateBuffer() method seems to work "
          "incorrect."
          "input layer has buffer, but void pointers or offset pointers "
          "provided"
      );
   }

   if (isBuffer())
   {
      void* p_vertex = g.m_buffer->vertex().map();
      void* p_index = g.m_buffer->index().map();

      size_t offset_v = 0;
      size_t offset_i = 0;

      for (const auto& child : g.m_children)
      {
         child->control().updateBuffer(p_vertex, p_index, &offset_v, &offset_i);
      }

      g.m_buffer->vertex().unmap();
      g.m_buffer->index().unmap();
   }
   else
   {
      // clang-format off
      if (v == nullptr) throw std::runtime_error("vertex* = nullptr");
      if (i == nullptr) throw std::runtime_error("index* = nullptr");
      if (off_v == nullptr) throw std::runtime_error("offset_vertex* = nullptr");
      if (off_i == nullptr) throw std::runtime_error("offset_index* = nullptr");
      // clang-format on

      // If current DrawObject is OBJECT -> copy data
      // to buffer. Each OBJECT object can't have children
      // so where is no need to iterator over children.
      if (g.m_type == BfDrawObjectBase::OBJECT)
      {
         size_t size = sizeof(BfVertex3) * g.vertices().size();
         memcpy(reinterpret_cast<char*>(v) + *off_v, g.vertices().data(), size);
         *off_v += size;

         size = sizeof(uint32_t) * g.indices().size();
         memcpy(reinterpret_cast<char*>(i) + *off_i, g.indices().data(), size);
         *off_i += size;

         // innerMap(g.vertices(), sizeof(BfVertex3), v, off_v);
         // innerMap(g.indices(), sizeof(uint32_t), i, off_i);
      }
      // If current DrawObject is LAYER -> iterate over
      // children and search for OBJECTs to map data or
      // other layers to search other OBJECTs recursively
      else
      {
         for (const auto& child : g.m_children)
         {
            child->control().updateBuffer(v, i, off_v, off_i);
         }
      }
   }
}

void
BfDrawControlProxy::draw(
    VkCommandBuffer combuffer,
    size_t offset,
    size_t index_offset,
    size_t vertex_offset
) const
{
   auto& g = m_obj;

   if (g.m_type == BfDrawObjectBase::OBJECT)
   {
      // DRAW SINGLE OBJECT
      vkCmdBindPipeline(
          combuffer,
          VK_PIPELINE_BIND_POINT_GRAPHICS,
          g.m_pipeline
      );
      vkCmdDrawIndexed(
          combuffer,
          g.indices().size(),
          1,
          index_offset,
          vertex_offset,
          offset
      );
   }
   else  // IF LAYER
   {
      if (isBuffer())
      {
         std::vector<VkDeviceSize> vert_offset = {0};
         // Local elements
         vkCmdBindVertexBuffers(
             combuffer,
             0,
             1,
             &g.m_buffer.get()->vertex().raw(),
             vert_offset.data()
         );

         vkCmdBindIndexBuffer(
             combuffer,
             g.m_buffer.get()->index().raw(),
             0,
             VK_INDEX_TYPE_UINT32
         );
      }

      auto calc_index_offsets = indexOffset();
      auto calc_vertex_offsets = vertexOffset();

      for (size_t i = 0; i < g.m_children.size(); ++i)
      {
         g.m_children.at(i)->control().draw(
             combuffer,
             offset + i,
             index_offset + calc_index_offsets[i],
             vertex_offset + calc_vertex_offsets[i]
         );
      }
   }
}

BfDrawDebugProxy::BfDrawDebugProxy(BfDrawObjectBase& obj)
    : m_obj{obj}
{
}

void
BfDrawDebugProxy::printVertices()
{
   for (auto v : m_obj.m_vertices)
   {
      std::cout << v << "\n";
   }
}
void
BfDrawDebugProxy::printIndices()
{
   for (auto i : m_obj.m_indices)
   {
      std::cout << i << "\n";
   }
}

/*  BfDrawObjectBase */

BfDrawObjectBase::BfDrawObjectBase(
    BfOTypeName typeName,
    VkPipeline pl,
    Type type,
    size_t max_vertex,
    size_t max_obj
)
    : BfObjectId{typeName}
    , m_pipeline{pl}
    , m_modelMatrix{1.0f}
    , m_type{type}
    , m_root{}
{
   if (type == BfDrawObjectBase::ROOT_LAYER)
   {
      m_buffer = std::make_unique<BfObjectBuffer>(
          sizeof(BfVertex3),
          max_vertex,
          max_obj
      );
   }
   // else
   // {
   //    throw std::runtime_error(
   //        "Trying to construct draw object with type "
   //        " <object> but not root layer provided"
   //    );
   // }
}

void
BfDrawObjectBase::add(BfObj n)
{
   m_children.push_back(n);
   m_root = shared_from_this();
}

void
BfDrawObjectBase::make()
{
   throw std::runtime_error(
       "[BfDrawObjectBase] make() method must be implemented. Dont use base "
       "class. Use BfDrawObject / BfDrawLayer"
   );
}

BfObjectData
BfDrawObjectBase::_objectData()
{
   return {
       .model_matrix = m_modelMatrix,
       .select_color = glm::vec3(1.0f, 0.5f, 0.0f),
       .index = 0,
       .id = id(),
       .line_thickness = 0.00025f
   };
}

/* BfDrawObject */

BfDrawObject::BfDrawObject(BfOTypeName typeName, VkPipeline pl)
    : BfDrawObjectBase{typeName, pl, OBJECT, 0, 0} {};

void
BfDrawObject::make()
{
   throw std::runtime_error("[BfDrawObject] make() method must be implemented");
}

/* BfDrawLayer */
BfDrawLayer::BfDrawLayer(BfOTypeName typeName)
    : BfDrawObjectBase(typeName, nullptr, LAYER, 0, 0)
{
}

void
BfDrawLayer::make()
{
   throw std::runtime_error(
       "[BfDrawLayer] make() method must be implemented. No vertices creation!"
   );
}

/* BfDrawRootLayer */
BfDrawRootLayer::BfDrawRootLayer(size_t max_vertex, size_t max_obj)
    : BfDrawObjectBase("Root Layer", nullptr, ROOT_LAYER, max_vertex, max_obj)
{
}

void
BfDrawRootLayer::make()
{
   for (auto child : m_children)
   {
      child->make();
   }
}
};  // namespace obj
//
