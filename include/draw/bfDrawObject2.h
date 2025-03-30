#ifndef BF_DRAWOBJECT2_H
#define BF_DRAWOBJECT2_H

#include <vulkan/vulkan_core.h>

#include <memory>

#include "bfDrawObjectBuffer.h"
#include "bfObjectId.h"
#include "bfPipeline.h"
#include "bfTypeManager.h"
#include "bfUniforms.h"
#include "bfVertex2.hpp"

namespace obj
{

class BfDrawObject;

using BfIndex = uint32_t;
using BfObj = std::shared_ptr<BfDrawObject>;

/**
 * @class BfDrawControlProxy
 * @brief Proxy class to work with render/vulkan bindings
 *
 */
class BfDrawControlProxy
{
public:
   BfDrawControlProxy(BfDrawObject& obj);
   std::vector<int32_t> vertexOffset() const;
   std::vector<int32_t> indexOffset() const;

   const glm::mat4& model() const;
   bool isBuffer() const;
   void mapModel(size_t frame_index, size_t& offset, void* data) const;

   void updateBuffer(
       void* v = nullptr,
       void* i = nullptr,
       size_t* off_v = nullptr,
       size_t* off_i = nullptr
   );

   void draw(
       VkCommandBuffer combuffer,
       size_t& offset,
       size_t& index_offset,
       size_t& vertex_offset
   ) const;

private:
   BfDrawObject& m_obj;
};

class BfDrawObject : public std::enable_shared_from_this<BfDrawObject>,
                     public BfObjectId
{
public:
   BfDrawObject(
       BfOTypeName typeName,
       VkPipeline pl,
       BfObj root = nullptr,
       size_t max_vertex = 2000,
       size_t max_obj = 20
   );

   const std::vector<BfVertex3>& vertices() const { return m_vertices; }
   const std::vector<BfIndex>& indices() const { return m_indices; }

   virtual void make();

   friend BfDrawControlProxy;
   BfDrawControlProxy control() { return BfDrawControlProxy(*this); };

protected:
   virtual BfObjectData _objectData();

protected:
   bool m_isBuffer;

   VkPipeline m_pipeline;
   glm::mat4 m_modelMatrix;

   std::unique_ptr<BfObjectBuffer> m_buffer;

   std::vector<BfVertex3> m_vertices;
   std::vector<BfIndex> m_indices;

   BfObj m_root;
   std::vector<BfObj> m_children;
};

class TestObj : protected BfDrawObject
{
public:
   TestObj(BfObj root)
       : BfDrawObject(
             "TestObj",
             *BfPipelineHandler::instance()->getPipeline(
                 BfPipelineType_Triangles
             ),
             root,
             20,
             20
         )
   {
   }

   virtual void make()
   {
      m_vertices = {
          {-1.0f, -1.0f, 0.0f},
          {-1.0f, 1.0f, 0.0f},
          {1.0f, 1.0f, 0.0f},
          {1.0f, -1.0f, 0.0f},
      };
      m_indices = {0, 1, 3, 1, 2, 3};
   }
};

}  // namespace obj

#endif
