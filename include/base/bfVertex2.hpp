#ifndef BF_VERTEX2_H
#define BF_VERTEX2_H

// Graphics Libs
#include <glm/fwd.hpp>
#include <type_traits>
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif __linux__
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#include "bfVariative.hpp"

// STL
#include <array>
#include <map>
#include <set>
#include <vector>
// External
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/glm.hpp>
#include "bfMath.hpp"

struct bfVertex
{
   glm::vec3 pos;
   glm::vec3 color;

   static inline VkVertexInputBindingDescription getBindingDescription()
   {
      VkVertexInputBindingDescription bindingDescription{};
      bindingDescription.binding = 0;
      bindingDescription.stride = sizeof(bfVertex);
      bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      return bindingDescription;
   }
   static inline std::array<VkVertexInputAttributeDescription, 2>
   getAttributeDescriptions()
   {
      std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

      attributeDescriptions[0].binding = 0;
      attributeDescriptions[0].location = 0;
      attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[0].offset = offsetof(bfVertex, pos);

      attributeDescriptions[1].binding = 0;
      attributeDescriptions[1].location = 1;
      attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[1].offset = offsetof(bfVertex, color);

      return attributeDescriptions;
   }
};

struct BfVertex3
{
   glm::vec3 pos;
   glm::vec3 color;
   glm::vec3 normals;

   BfVertex3()
       : pos{0.0f, 0.0f, 0.0f}
       , color{1.0f, 1.0f, 1.0f}
       , normals{0.0f, 0.0f, 0.0f}
   {
   }

   glm::vec3 operator=(const BfVertex3 &o) { return o.pos; }
   bool operator==(const BfVertex3 &o)
   {
      return o.pos == this->pos && o.normals == this->normals &&
             o.color == this->color;
   };

   float &x() { return pos.x; }
   float &y() { return pos.y; }
   float &z() { return pos.z; }

   float &r() { return color.r; }
   float &g() { return color.g; }
   float &b() { return color.b; }

   float &nx() { return normals.x; }
   float &ny() { return normals.y; }
   float &nz() { return normals.z; }

   BfVertex3(BfVertex3 &&m) noexcept
       : pos{std::move(m.pos)}
       , color{std::move(m.color)}
       , normals{std::move(m.normals)}
   {
   }

   BfVertex3(const BfVertex3 &m) noexcept
       : pos{m.pos}, color{m.color}, normals{m.normals}
   {
   }

   BfVertex3(float x, float y, float z)
       : pos{x, y, z}, color{1.0f, 1.0f, 1.0f}, normals{0.0f, 0.0f, 0.0f}
   {
   }

   BfVertex3(
       const glm::vec3 &ipos,
       const glm::vec3 &icol = {1.0f, 1.0f, 1.0f},
       const glm::vec3 &inor = {0.0f, 0.0f, 0.0f}
   )
       : pos{ipos}, color{icol}, normals{inor}
   {
   }

   BfVertex3(
       glm::vec3 &&ipos,
       glm::vec3 &&icol = {1.0f, 1.0f, 1.0f},
       glm::vec3 &&inor = {0.0f, 0.0f, 0.0f}
   )
       : pos{std::move(ipos)}, color{std::move(icol)}, normals{std::move(inor)}
   {
   }

   inline bool equal(const BfVertex3 &o) const
   {
      return CHECK_FLOAT_EQUALITY(o.pos.x, pos.x) &&
             CHECK_FLOAT_EQUALITY(o.pos.y, pos.y) &&
             CHECK_FLOAT_EQUALITY(o.pos.z, pos.z);
   }

   static inline VkVertexInputBindingDescription getBindingDescription()
   {
      VkVertexInputBindingDescription bindingDescription{};
      bindingDescription.binding = 0;
      bindingDescription.stride = sizeof(BfVertex3);
      bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      return bindingDescription;
   }
   static inline std::array<VkVertexInputAttributeDescription, 3>
   getAttributeDescriptions()
   {
      std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

      attributeDescriptions[0].binding = 0;
      attributeDescriptions[0].location = 0;
      attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[0].offset = offsetof(BfVertex3, pos);

      attributeDescriptions[1].binding = 0;
      attributeDescriptions[1].location = 1;
      attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[1].offset = offsetof(BfVertex3, color);

      attributeDescriptions[2].binding = 0;
      attributeDescriptions[2].location = 2;
      attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[2].offset = offsetof(BfVertex3, normals);

      return attributeDescriptions;
   }

   friend std::ostream &operator<<(std::ostream &os, const BfVertex3 &vert)
   {
      os << "BfVertex3( p(" << vert.pos.x << ", " << vert.pos.y << ", "
         << vert.pos.z << "), c(" << vert.color.r << ", " << vert.color.g
         << ", " << vert.color.b << "), n(" << vert.normals.x << ", "
         << vert.normals.y << ", " << vert.normals.z << ") )";

      return os;
   }
};

struct BfVertex4
{
   glm::vec3 pos;
   glm::vec3 color;
   glm::vec3 normal;
   glm::uint32_t obj_id;

   static inline VkVertexInputBindingDescription getBindingDescription()
   {
      VkVertexInputBindingDescription bindingDescription{};
      bindingDescription.binding = 0;
      bindingDescription.stride = sizeof(BfVertex4);
      bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      return bindingDescription;
   }
   static inline std::array<VkVertexInputAttributeDescription, 4>
   getAttributeDescriptions()
   {
      std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

      attributeDescriptions[0].binding = 0;
      attributeDescriptions[0].location = 0;
      attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[0].offset = offsetof(BfVertex4, pos);

      attributeDescriptions[1].binding = 0;
      attributeDescriptions[1].location = 1;
      attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[1].offset = offsetof(BfVertex4, color);

      attributeDescriptions[2].binding = 0;
      attributeDescriptions[2].location = 2;
      attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[2].offset = offsetof(BfVertex4, normal);

      attributeDescriptions[3].binding = 0;
      attributeDescriptions[3].location = 2;
      attributeDescriptions[3].format =
          VK_FORMAT_R32_UINT;  // VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[3].offset = offsetof(BfVertex4, obj_id);

      return attributeDescriptions;
   }

   friend std::ostream &operator<<(std::ostream &os, const BfVertex4 &vert)
   {
      os << "BfVertex3( p(" << vert.pos.x << ", " << vert.pos.y << ", "
         << vert.pos.z << "), c(" << vert.color.r << ", " << vert.color.g
         << ", " << vert.color.b << "), n(" << vert.normal.x << ", "
         << vert.normal.y << ", " << vert.normal.z << ") obj_id(" << vert.obj_id
         << ')';

      return os;
   }
};

enum BfePipelineType
{
   BF_GRAPHICS_PIPELINE_LINES = 0,
   BF_GRAPHICS_PIPELINE_TRIANGLE = 1
};

#endif  // !BF_VERTEX_H
