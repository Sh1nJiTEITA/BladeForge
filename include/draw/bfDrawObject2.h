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

class BfDrawObjectBase;

using BfIndex = uint32_t;
using BfObj = std::shared_ptr<BfDrawObjectBase>;

/**
 * @class BfDrawControlProxy
 * @brief Proxy class to work with render/vulkan bindings
 *
 */
class BfDrawControlProxy
{
public:
   BfDrawControlProxy(BfDrawObjectBase& obj);
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
   BfDrawObjectBase& m_obj;
};

class BfDrawDebugProxy
{
public:
   BfDrawDebugProxy(BfDrawObjectBase& obj);

   void printVertices();
   void printIndices();

private:
   BfDrawObjectBase& m_obj;
};

class BfDrawObjectBase : public std::enable_shared_from_this<BfDrawObjectBase>,
                         public BfObjectId
{
public:
   enum Type
   {
      OBJECT,
      LAYER
   };

   BfDrawObjectBase(
       BfOTypeName typeName,
       VkPipeline pl,
       Type type,
       BfObj root = nullptr,
       size_t max_vertex = 2000,
       size_t max_obj = 20
   );

   const std::vector<BfVertex3>& vertices() const { return m_vertices; }
   const std::vector<BfIndex>& indices() const { return m_indices; }
   const Type drawtype() const { return m_type; }

   virtual void make();

   friend BfDrawControlProxy;
   BfDrawControlProxy control() { return BfDrawControlProxy(*this); };

   friend BfDrawDebugProxy;
   BfDrawDebugProxy debug() { return BfDrawDebugProxy(*this); };

protected:
   virtual BfObjectData _objectData();

protected:
   VkPipeline m_pipeline;
   glm::mat4 m_modelMatrix;

   std::vector<BfVertex3> m_vertices;
   std::vector<BfIndex> m_indices;

   BfObj m_root;
   std::vector<BfObj> m_children;

private:
   std::unique_ptr<BfObjectBuffer> m_buffer;
   Type m_type;
   bool m_isBuffer;
};

class BfDrawObject : protected BfDrawObjectBase
{
public:
   BfDrawObject(BfOTypeName typeName, VkPipeline pl, BfObj root);
   virtual void make() override;
};

class BfDrawLayer : protected BfDrawObjectBase
{
public:
   BfDrawLayer(
       BfOTypeName typeName,
       BfObj root = nullptr,
       size_t max_vertex = 2000,
       size_t max_obj = 20
   );
   BfDrawControlProxy control() = delete;
   virtual void make() override;
};

class BfDrawRootLayer : protected BfDrawLayer
{
public:
   BfDrawRootLayer(size_t max_vertex = 2000, size_t max_obj = 20);
   virtual void make() override;
};

class TestObj : public BfDrawObject
{
public:
   TestObj(BfObj root)
       : BfDrawObject(
             "TestObj",
             *BfPipelineHandler::instance()->getPipeline(
                 BfPipelineType_Triangles
             ),
             root
         )
   {
   }

   virtual void make() override
   {
      m_vertices = {
          {-1.0f, -1.0f, 0.0f},
          {-1.0f, 1.0f, 0.0f},
          {1.0f, 1.0f, 0.0f},
          {1.0f, -1.0f, 0.0f},
      };
      m_indices = {0, 1, 3, 1, 2, 3, 3, 0, 1};
   }
};

}  // namespace obj

#endif
