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

   auto innerMap = [](auto container, void* data, size_t* offset) {
      // size_t size = sizeof(BfVertex3) * container.size();
      // memcpy(reinterpret_cast<char*>(data) + *offset, container.data(),
      // size); *offset += size;
      size_t size =
          sizeof(typename std::decay_t<decltype(container)>::value_type) *
          container.size();
      std::cout << "Copying " << size << " bytes to "
                << " buffer at offset " << *offset << "\n";
      memcpy(reinterpret_cast<char*>(data) + *offset, container.data(), size);
      *offset += size;
   };

   if (isBuffer())
   {  // clang-format off
      std::cout << "Updating root buffer\n";
      void* p_vertex = g.m_buffer->vertex().map();
      void* p_index = g.m_buffer->index().map();
      
         size_t offset_v = 0;
         size_t offset_i = 0;

      std::cout << "mapping vertices\n";
         m_obj.debug().printVertices();
         
         size_t size_v = sizeof(BfVertex3) * g.vertices().size();
         memcpy(
             reinterpret_cast<char *>(p_vertex) + offset_v,
             g.vertices().data(),
             size_v 
         );
         offset_v += size_v;


      std::cout << "mapping indices\n";

         m_obj.debug().printIndices();
         size_t size_i = sizeof(uint32_t) * g.indices().size();
         memcpy(
             reinterpret_cast<char *>(p_index) + offset_i,
             g.indices().data(),
             size_i 
         );
         offset_v += size_i;

         // innerMap(g.vertices(), p_vertex, &offset_v);
         // innerMap(g.indices(), p_index, &offset_i);

         for (const auto& child : g.m_children)
         {
            child->control().updateBuffer(p_vertex, p_index, &offset_v, &offset_i);
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

   vkCmdBindPipeline(
       combuffer,
       VK_PIPELINE_BIND_POINT_GRAPHICS,
       m_obj.m_pipeline
   );

   vkCmdDrawIndexed(
       combuffer,
       g.indices().size(),
       1,
       // index_offsets[0] + index_offset,
       index_offset,
       // vertex_offsets[0] + vertex_offset,
       vertex_offset,
       offset
   );
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
    BfObj root,
    size_t max_vertex,
    size_t max_obj
)
    : BfObjectId{typeName}
    , m_pipeline{pl}
    , m_modelMatrix{1.0f}
    , m_root{root}
    , m_isBuffer{root == nullptr}
    , m_type{type}
{
   if (root == nullptr)
   {
      if (m_type == Type::LAYER)
      {
         m_buffer = std::make_unique<BfObjectBuffer>(
             sizeof(BfVertex3),
             max_vertex,
             max_obj
         );
      }
      else
      {
         throw std::runtime_error(
             "Trying to construct draw object with type "
             " <object> but not root layer provided"
         );
      }
   }
}

void
BfDrawObjectBase::make()
{
   throw std::runtime_error(
       "[BfDrawObjectBase] make() method must be implemented. Dont use base "
       "class. Use BfDrawObject / BfDrawLayer"
   );
   // for (auto child : m_children)
   // {
   //    child->make();
   // }
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

BfDrawObject::BfDrawObject(BfOTypeName typeName, VkPipeline pl, BfObj root)
    : BfDrawObjectBase{typeName, pl, OBJECT, root, 0, 0} {};

void
BfDrawObject::make()
{
   throw std::runtime_error("[BfDrawObject] make() method must be implemented");
}

BfDrawLayer::BfDrawLayer(
    BfOTypeName typeName, BfObj root, size_t max_vertex, size_t max_obj
)
    : BfDrawObjectBase(typeName, nullptr, LAYER, root, max_vertex, max_obj)
{
}

void
BfDrawLayer::make()
{
   throw std::runtime_error(
       "[BfDrawLayer] make() method must be implemented. No vertices creation!"
   );
}

BfDrawRootLayer::BfDrawRootLayer(size_t max_vertex, size_t max_obj)
    : BfDrawLayer("Root Layer", nullptr, max_vertex, max_obj)
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
