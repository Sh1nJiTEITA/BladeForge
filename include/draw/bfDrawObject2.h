#ifndef BF_DRAWOBJECT2_H
#define BF_DRAWOBJECT2_H

#include <vulkan/vulkan_core.h>

#include <memory>
#include <random>

#include "bfDrawObject.h"
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
using BfObjWeak = std::weak_ptr<BfDrawObjectBase>;

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
       size_t offset,
       size_t index_offset,
       size_t vertex_offset
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
      LAYER,
      ROOT_LAYER,
   };

   BfDrawObjectBase(
       BfOTypeName typeName,
       VkPipeline pl,
       Type type,
       size_t max_vertex = 2000,
       size_t max_obj = 20
   );

   const std::vector<BfVertex3>& vertices() const { return m_vertices; }
   const std::vector<BfIndex>& indices() const { return m_indices; }
   const Type drawtype() const { return m_type; }

   void add(BfObj n);

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

   BfObjWeak m_root;
   std::vector<BfObj> m_children;

private:
   std::unique_ptr<BfObjectBuffer> m_buffer;
   Type m_type;
};

/**
 * @class BfDrawObject
 * @brief
 *
 */
class BfDrawObject : public BfDrawObjectBase
{
public:
   BfDrawObject(BfOTypeName typeName, VkPipeline pl);
   virtual void make() override;

   void add() = delete;
};

/**
 * @class BfDrawLayer
 * @brief
 *
 */
class BfDrawLayer : public BfDrawObjectBase
{
public:
   BfDrawLayer(BfOTypeName typeName);

   virtual void make() override;
   const std::vector<BfVertex3>& vertices() const = delete;
   const std::vector<BfIndex>& indices() const = delete;
};

/**
 * @class BfDrawRootLayer
 * @brief
 *
 */
class BfDrawRootLayer : public BfDrawObjectBase
{
public:
   BfDrawRootLayer(size_t max_vertex = 2000, size_t max_obj = 20);

   virtual void make() override;
   const std::vector<BfVertex3>& vertices() const = delete;
   const std::vector<BfIndex>& indices() const = delete;
};

/**
 * @class TestObj
 * @brief
 *
 */
class TestObj : public BfDrawObject
{
public:
   TestObj()
       : BfDrawObject(
             "TestObj",
             *BfPipelineHandler::instance()->getPipeline(
                 BfPipelineType_Triangles
             )
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
      m_indices = {0, 1, 3, 1, 2, 3};
   }
};

/**
 * @class TestObj2
 * @brief
 *
 */
class TestObj2 : public BfDrawObject
{
public:
   TestObj2()
       : BfDrawObject(
             "TestObj2",
             *BfPipelineHandler::instance()->getPipeline(
                 BfPipelineType_Triangles
             )
         )
   {
   }

   virtual void make() override
   {
      m_vertices = {
          {-1.0f, 0.0f, -1.0f},
          {-1.0f, 0.0f, 1.0f},
          {1.0f, 0.0f, 1.0f},
          {1.0f, 0.0f, -1.0f},
      };
      m_indices = {0, 1, 3, 1, 2, 3};
   }
};

/**
 * @class TestLayer
 * @brief
 *
 */
class TestLayer : public BfDrawLayer
{
public:
   TestLayer()
       : BfDrawLayer("Test layer")
   {
   }

   virtual void make() override
   {
      auto test1 = std::make_shared<TestObj>();
      test1->make();
      add(test1);
      auto test2 = std::make_shared<TestObj2>();
      test2->make();
      add(test2);
   }
};

class RandomPlane : public BfDrawObject
{
public:
   RandomPlane()
       : BfDrawObject(
             "RandomPlane",
             *BfPipelineHandler::instance()->getPipeline(
                 BfPipelineType_Triangles
             )
         )
   {
   }

   virtual void make() override
   {
      static std::random_device rd;
      static std::mt19937 gen(rd());
      std::uniform_real_distribution<float> dist(-5.0f, 5.0f);
      std::uniform_real_distribution<float> angleDist(
          0.0f,
          glm::radians(360.0f)
      );

      glm::vec3 translation = {dist(gen), 0.0f, dist(gen)};
      float rotationAngle = angleDist(gen);
      glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation);
      transform =
          glm::rotate(transform, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

      std::vector<glm::vec3> baseVertices = {
          {-0.5f, 0.0f, -0.5f},
          {-0.5f, 0.0f, 0.5f},
          {0.5f, 0.0f, 0.5f},
          {0.5f, 0.0f, -0.5f},
      };

      m_vertices.clear();
      for (const auto& v : baseVertices)
      {
         glm::vec4 transformed = transform * glm::vec4(v, 1.0f);
         m_vertices.push_back({transformed.x, transformed.y, transformed.z});
      }

      m_indices = {0, 1, 3, 1, 2, 3};
   }
};

}  // namespace obj

#endif
