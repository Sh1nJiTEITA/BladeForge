#include "bfDrawObject2.h"

#include <iterator>
#include <memory>
#include <stdexcept>

#include "bfTypeManager.h"
#include "bfUniforms.h"

namespace obj
{

BfDrawControlProxy::BfDrawControlProxy(BfDrawObjectBase& obj)
    : m_obj{obj}
{
}

const glm::mat4&
BfDrawControlProxy::model() const
{
   if (m_obj.m_type == BfDrawObjectBase::LAYER ||
       m_obj.m_type == BfDrawObjectBase::BUFFER_LAYER)
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
   return m_obj.m_type == BfDrawObjectBase::BUFFER_LAYER;
}

void
BfDrawControlProxy::mapModel(
    size_t frame_index, size_t& offset, void* data
) const
{
   auto& g = m_obj;

   // If current DrawObject is OBJECT -> map model data
   // to descriptor and add offset
   if (g.m_type == BfDrawObjectBase::OBJECT)
   {
      std::vector< BfObjectData > obj_data = g._objectData();
      assert(
          obj_data.size() == g.totalInstanceCount() &&
          "Instance count and return of _objectData are not the same"
      );
      memcpy(
          reinterpret_cast< char* >(data) + offset,
          obj_data.data(),
          sizeof(BfObjectData) * obj_data.size()
      );
      offset += sizeof(BfObjectData) * obj_data.size();
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
BfDrawControlProxy::updateBuffer(
    bool make,
    std::queue< std::shared_ptr< BfDrawObjectBase > >* q,
    void* v,
    void* i,
    size_t* off_v,
    size_t* off_i
)
{
   auto& g = m_obj;

   std::queue< std::shared_ptr< BfDrawObjectBase > > inner_q;

   if (isBuffer() && (v || i || off_v || off_i) && q)
   {
      q->push(m_obj.shared_from_this());
      return;
   }

   if (isBuffer())
   {

      void* p_vertex = g.m_buffer->vertex().map();
      void* p_index = g.m_buffer->index().map();

      size_t offset_v = 0;
      size_t offset_i = 0;

      for (const auto& child : g.m_children)
      {
         // clang-format off
         child->control().updateBuffer(make, 
                                       q == nullptr ? &inner_q : q,
                                       p_vertex, 
                                       p_index, 
                                       &offset_v, 
                                       &offset_i);
         // clang-format on
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
         memcpy(
             reinterpret_cast< char* >(v) + *off_v,
             g.vertices().data(),
             size
         );
         *off_v += size;

         size = sizeof(uint32_t) * g.indices().size();
         memcpy(
             reinterpret_cast< char* >(i) + *off_i,
             g.indices().data(),
             size
         );
         *off_i += size;
      }
      // If current DrawObject is LAYER -> iterate over
      // children and search for OBJECTs to map data or
      // other layers to search other OBJECTs recursively
      else
      {
         for (const auto& child : g.m_children)
         {
            // if (make)
            // {
            //    child->make();
            // }
            child->control().updateBuffer(
                make,
                q == nullptr ? &inner_q : q,
                v,
                i,
                off_v,
                off_i
            );
         }
      }
   }

   while (!inner_q.empty())
   {
      auto v = inner_q.front();
      inner_q.pop();

      v->control().updateBuffer(make);
   }
}

void
BfDrawControlProxy::draw(
    VkCommandBuffer combuffer,
    uint32_t viewport_index,
    std::queue< std::shared_ptr< BfDrawObjectBase > >* q,
    size_t* offset,
    size_t* index_offset,
    size_t* vertex_offset,
    bool is_render,
    uint32_t hovered_id
) const
{
   auto& g = m_obj;
   g.prerender(viewport_index);

   if (g.m_type == BfDrawObjectBase::OBJECT)
   {
      if (g.m_isrender && is_render)
      {
         vkCmdBindPipeline(
             combuffer,
             VK_PIPELINE_BIND_POINT_GRAPHICS,
             g.m_pipeline
         );

         vkCmdPushConstants(
             combuffer,
             *BfPipelineHandler::instance()->getLayout(
                 BfPipelineLayoutType_Main
             ),
             VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
             sizeof(BfPushConstants),
             sizeof(BfPushConstantsPerObject),
             offset
         );

         vkCmdDrawIndexed(
             combuffer,
             g.indices().size(),
             g.instanceCount(),
             *index_offset,
             *vertex_offset,
             g.baseInstance()
         );

         g.updateHoveredStatus(g.id() == hovered_id);
         auto ptr = &m_obj;
         base::g::intrstack().push([ptr]() { ptr->processInteraction(); });
      }

      *offset += g.totalInstanceCount();
      *index_offset += g.indices().size();
      *vertex_offset += g.vertices().size();
   }
   else // LAYER
   {
      std::queue< std::shared_ptr< BfDrawObjectBase > > inner_q;
      const bool defer_render = isBuffer() && q != nullptr;

      if (defer_render)
      {
         q->push(m_obj.shared_from_this());
      }
      else
      {
         if (isBuffer())
         {
            std::vector< VkDeviceSize > vert_offset = {0};
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

         for (auto& child : g.m_children)
         {
            child->control().draw(
                combuffer,
                viewport_index,
                q == nullptr ? &inner_q : q,
                offset,
                index_offset,
                vertex_offset,
                g.isRender() && is_render,
                hovered_id
            );
         }
      }

      // Deferred render
      if (q == nullptr)
      {
         while (!inner_q.empty())
         {
            auto v = inner_q.front();
            inner_q.pop();

            size_t ioffset = 0;
            size_t voffset = 0;

            v->control().draw(
                combuffer,
                viewport_index,
                nullptr,
                offset,
                &ioffset,
                &voffset,
                g.isRender() && is_render,
                hovered_id
            );
         }
      }
   }

   g.postrender(viewport_index);
}

void
BfDrawControlProxy::toggleHover(int status)
{
   if (status == -1)
   {
      m_obj.m_isHovered = !m_obj.m_isHovered;
   }
   else
   {
      m_obj.m_isHovered = static_cast< bool >(status);
   }
}

BfDrawDebugProxy::BfDrawDebugProxy(BfDrawObjectBase& obj)
    : m_obj{obj}
{
}

void
BfDrawDebugProxy::printVertices()
{
   fmt::println("[DEBUG] PRINTING VERTICES BEGIN");
   for (auto v : m_obj.m_vertices)
   {
      fmt::println("{}", v);
   }
   fmt::println("[DEBUG] PRINTING VERTICES END");
}
void
BfDrawDebugProxy::printIndices()
{
   for (auto i : m_obj.m_indices)
   {
      fmt::print("{}", i);
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
    , m_isrender{true}
    , m_instanceCount{1}
    , m_baseInstance{0}
    , m_totalInstanceCount{1}
{
   if (type == BfDrawObjectBase::BUFFER_LAYER)
   {
      m_buffer = std::make_unique< BfObjectBuffer >(
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
   if (m_type == OBJECT)
   {
      std::runtime_error("Trying to add DrawObjectBase to OBJECT");
   }
   m_children.push_back(n);
   auto weak = this->weak_from_this();
   n->m_root = weak_from_this();
}

void
BfDrawObjectBase::make()
{
   throw std::runtime_error(
       "[BfDrawObjectBase] make() method must be implemented. Dont use base "
       "class. Use BfDrawObject / BfDrawLayer"
   );
}

BfObj
BfDrawObjectBase::root()
{
   // if (auto v = m_root.lock())
   // {
   //    if (v->m_root.expired())
   //    {
   //       if (v->control().isBuffer())
   //       {
   //          return v;
   //       }
   //       else
   //       {
   //          throw std::runtime_error(
   //              "Root does not exist but this layer does not have buffer "
   //          );
   //       }
   //    }
   //    return v->root();
   // }
   // return shared_from_this();
   //
   //

   // std::cout << "inside " <<
   // BfTypeManager::inst().getTypeNameByTypeId(type())
   //           << "\n";

   if (control().isBuffer())
   {
      return shared_from_this();
   }

   if (auto v = m_root.lock())
   {
      return v->root();
   }

   throw std::runtime_error("Reached top of tree but no buffer found.");
}

std::vector< BfObjectData >
BfDrawObjectBase::_objectData()
{
   return {
       {.model_matrix = m_modelMatrix,
        .select_color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f),
        .index = 0,
        .id = id(),
        .line_thickness = 0.00025f}
   };
}

void
BfDrawObjectBase::_assignRoots()
{
   for (auto child : m_children)
   {
      child->m_root = weak_from_this();
      child->_assignRoots();
   }
}

/* BfDrawObject */

BfDrawObject::BfDrawObject(BfOTypeName typeName, VkPipeline pl, uint32_t disc)
    : BfDrawObjectBase{typeName, pl, OBJECT, 0, 0}
    , m_discretization{disc}
    , m_color{1.0f, 1.0f, 1.0f}
{
}

void
BfDrawObject::make()
{
   throw std::runtime_error("[BfDrawObject] make() method must be implemented");
}

void
BfDrawObject::_genIndicesStandart()
{
   m_indices.clear();
   m_indices.reserve(m_vertices.size());

   for (size_t i = 0; i < m_vertices.size(); ++i)
   {
      m_indices.emplace_back(i);
   }
}

/* BfDrawLayer */
BfDrawLayer::BfDrawLayer(BfOTypeName typeName, Type type)
    : BfDrawObjectBase(
          typeName,
          nullptr,
          type,
          type == BUFFER_LAYER ? 3000 : 0,
          type == BUFFER_LAYER ? 30 : 0
      )
{
}

void
BfDrawLayer::make()
{
   _assignRoots();
   for (auto child : m_children)
   {
      child->make();
   }
}

std::vector< BfObj >&
BfDrawObjectBase::children()
{
   return m_children;
}

// FIXME: No idea how this should be implemented
// For now it tries to toggle each individual nested element
// recursivly. If some object has been already toggled ->
// last call will switch render for it, but not for other objects
// in single layout.
bool
BfDrawLayer::toggleRender(int sts)
{
   // BUG: segmentation fault (no idea why)
   // bool current = toggleRender(sts);
   for (auto& child : m_children)
   {
      child->toggleRender(sts);
   }
   return BfDrawObjectBase::toggleRender(sts);
}

/* BfDrawRootLayer */
BfDrawRootLayer::BfDrawRootLayer(size_t max_vertex, size_t max_obj)
    : BfDrawObjectBase("Root Layer", nullptr, BUFFER_LAYER, max_vertex, max_obj)
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
}; // namespace obj
//
