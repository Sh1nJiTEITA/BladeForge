#include "bfDrawObject2.h"

namespace obj
{

BfDrawControlProxy::BfDrawControlProxy(BfDrawObject& obj)
    : m_obj{obj}
{
}

std::vector<int32_t>
BfDrawControlProxy::vertexOffset() const
{
   std::vector<int32_t> offset(1 + m_obj.m_children.size());
   size_t growing_offset = m_obj.m_vertices.size();
   offset[0] = growing_offset;

   for (size_t i = 0; i < m_obj.m_children.size(); i++)
   {
      offset[i + 1] = growing_offset;
      growing_offset += m_obj.m_children[i]->vertices().size();
   }
   return offset;
}

std::vector<int32_t>
BfDrawControlProxy::indexOffset() const
{
   std::vector<int32_t> offset(1 + m_obj.m_children.size());
   size_t growing_offset = m_obj.m_indices.size();
   offset[0] = growing_offset;

   for (size_t i = 0; i < m_obj.m_children.size(); i++)
   {
      offset[i + 1] = growing_offset;
      growing_offset += m_obj.m_children[i]->indices().size();
   }
   return offset;
}

const glm::mat4&
BfDrawControlProxy::model() const
{
   return m_obj.m_modelMatrix;
}
bool
BfDrawControlProxy::isBuffer() const
{
   return m_obj.m_isBuffer;
}

void
BfDrawControlProxy::mapModel(size_t frame_index, size_t& offset, void* data)
    const
{
   BfObjectData obj_data = m_obj._objectData();
   memcpy(
       reinterpret_cast<char*>(data) + offset,
       &obj_data,
       sizeof(BfObjectData)
   );
   offset += sizeof(BfObjectData);

   for (size_t i = 0; i < m_obj.m_children.size(); i++)
   {
      m_obj.m_children[i]->control().mapModel(frame_index, offset, data);
   }
}

void
BfDrawControlProxy::updateBuffer(void* v, void* i, size_t* off_v, size_t* off_i)
{
   auto& g = m_obj;

   auto innerMap = [](const auto& container, void* data, size_t* offset) {
      size_t size = sizeof(BfVertex3) * container.size();
      memcpy(reinterpret_cast<char*>(data) + *offset, container.data(), size);
      *offset += size;
   };

   if (isBuffer())
   {  // clang-format off
      void* p_vertex = g.m_buffer->vertex().map();
      void* p_index = g.m_buffer->index().map();
      {
         size_t offset_v = 0;
         size_t offset_i = 0;

         innerMap(g.vertices(), p_vertex, &offset_v);
         innerMap(g.indices(), p_index, &offset_i);

         for (const auto& child : g.m_children)
         {
            child->control().updateBuffer(p_vertex, p_index, &offset_v, &offset_i);
         }
      }
      g.m_buffer->vertex().unmap();
      g.m_buffer->index().unmap();
   }
   else
   {  
      if (v == nullptr) throw std::runtime_error("vertex* = nullptr");
      if (i == nullptr) throw std::runtime_error("index* = nullptr");
      if (off_v == nullptr) throw std::runtime_error("offset_vertex* = nullptr");
      if (off_i == nullptr) throw std::runtime_error("offset_index* = nullptr");
      
      innerMap(g.vertices(), v, off_v);
      innerMap(g.indices(), i, off_i);

      for (const auto& child : g.m_children)
      {
         child->control().updateBuffer(v, i, off_v, off_i); 
      }

   }  // clang-format on
}

void
BfDrawControlProxy::draw(
    VkCommandBuffer combuffer,
    size_t& offset,
    size_t& index_offset,
    size_t& vertex_offset
) const
{
   auto& g = m_obj;

   auto index_offsets = indexOffset();
   auto vertex_offsets = vertexOffset();

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
   vkCmdDrawIndexed(
       combuffer,
       g.indices().size(),
       1,
       index_offsets[0] + index_offset,
       index_offsets[0] + vertex_offset,
       offset
   );
}

/*  BfDrawObject */

BfDrawObject::BfDrawObject(
    BfOTypeName typeName,
    VkPipeline pl,
    BfObj root,
    size_t max_vertex,
    size_t max_obj
)
    : BfObjectId{typeName}
    , m_pipeline{pl}
    , m_modelMatrix{1.0f}
    , m_root{root}
    , m_isBuffer{root == nullptr}
{
   if (root == nullptr)
   {
      m_buffer = std::make_unique<BfObjectBuffer>(
          sizeof(BfVertex3),
          max_vertex,
          max_obj
      );
   }
}

void
BfDrawObject::make()
{
   for (auto child : m_children)
   {
      child->make();
   }
}

BfObjectData
BfDrawObject::_objectData()
{
   return {
       .model_matrix = m_modelMatrix,
       .select_color = glm::vec3(1.0f, 0.5f, 0.0f),
       .index = 0,
       .id = id(),
       .line_thickness = 0.00025f
   };
}

};  // namespace obj
