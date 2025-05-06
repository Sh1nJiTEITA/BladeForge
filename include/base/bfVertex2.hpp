#ifndef BF_VERTEX2_H
#define BF_VERTEX2_H

// Graphics Libs
#include "bfVariative.hpp"
#include <array>
#include <cmath>
#include <glm/fwd.hpp>
#include <glm/vector_relational.hpp>
#include <type_traits>
#include <utility>
#include <variant>

#if defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#endif

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

   // glm::vec3 operator=(const BfVertex3& o) { return o.pos; }
   //
   BfVertex3& operator=(const BfVertex3& o)
   {
      pos = o.pos;
      color = o.color;
      normals = o.normals;
      return *this;
   }

   operator glm::vec3() const { return pos; }

   bool operator==(const BfVertex3& o)
   {
      return o.pos == this->pos && o.normals == this->normals &&
             o.color == this->color;
   };

   float& x() { return pos.x; }
   float& y() { return pos.y; }
   float& z() { return pos.z; }

   float& r() { return color.r; }
   float& g() { return color.g; }
   float& b() { return color.b; }

   float& nx() { return normals.x; }
   float& ny() { return normals.y; }
   float& nz() { return normals.z; }

   BfVertex3(BfVertex3&& m) noexcept
       : pos{std::move(m.pos)}
       , color{std::move(m.color)}
       , normals{std::move(m.normals)}
   {
   }

   BfVertex3(const BfVertex3& m) noexcept
       : pos{m.pos}, color{m.color}, normals{m.normals}
   {
   }

   BfVertex3(float x, float y, float z)
       : pos{x, y, z}, color{1.0f, 1.0f, 1.0f}, normals{0.0f, 0.0f, 0.0f}
   {
   }

   BfVertex3(
       const glm::vec3& ipos,
       const glm::vec3& icol = {1.0f, 1.0f, 1.0f},
       const glm::vec3& inor = {0.0f, 0.0f, 0.0f}
   )
       : pos{ipos}, color{icol}, normals{inor}
   {
   }
   //
   BfVertex3(
       glm::vec3&& ipos,
       glm::vec3&& icol = {1.0f, 1.0f, 1.0f},
       glm::vec3&& inor = {0.0f, 0.0f, 0.0f}
   )
       : pos{std::move(ipos)}, color{std::move(icol)}, normals{std::move(inor)}
   {
   }
   // template <
   //     typename P = glm::vec3,
   //     typename C = glm::vec3,
   //     typename N = glm::vec3 >
   // BfVertex3(
   //     P&& ipos,
   //     C&& icol = glm::vec3{1.0f, 1.0f, 1.0f},
   //     N&& inor = glm::vec3{0.0f, 0.0f, 0.0f}
   // )
   //     : pos(std::forward< P >(ipos))
   //     , color(std::forward< C >(icol))
   //     , normals(std::forward< N >(inor))
   // {
   // }

   template < typename POS >
   BfVertex3 otherPos(POS&& pos) const
   {
      return BfVertex3{std::forward< POS >(pos), this->color, this->normals};
   }

   static BfVertex3 nan()
   {
      return BfVertex3{
          glm::vec3(std::nanf("")),
          glm::vec3(std::nanf("")),
          glm::vec3(std::nanf(""))
      };
   }

   inline bool equal(const BfVertex3& o) const
   {
      return glm::all(glm::equal(this->pos, o.pos));
   }

   static inline VkVertexInputBindingDescription getBindingDescription()
   {
      VkVertexInputBindingDescription bindingDescription{};
      bindingDescription.binding = 0;
      bindingDescription.stride = sizeof(BfVertex3);
      bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      return bindingDescription;
   }
   static inline std::array< VkVertexInputAttributeDescription, 3 >
   getAttributeDescriptions()
   {
      std::array< VkVertexInputAttributeDescription, 3 >
          attributeDescriptions{};

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

   // friend std::ostream& operator<<(std::ostream& os, const BfVertex3& vert)
   // {
   //    os << "BfVertex3( p(" << vert.pos.x << ", " << vert.pos.y << ", "
   //       << vert.pos.z << "), c(" << vert.color.r << ", " << vert.color.g
   //       << ", " << vert.color.b << "), n(" << vert.normals.x << ", "
   //       << vert.normals.y << ", " << vert.normals.z << ") )";
   //
   //    return os;
   // }
};

template <>
struct fmt::formatter< glm::vec3 >
{
   constexpr auto parse(format_parse_context& ctx)
   {
      return ctx.begin(); // no format spec
   }

   template < typename FormatContext >
   auto format(const glm::vec3& v, FormatContext& ctx) const
   {
      return fmt::format_to(
          ctx.out(),
          "({:.2f}, {:.2f}, {:.2f})",
          v.x,
          v.y,
          v.z
      );
   }
};

// Formatter for BfVertex3
template <>
struct fmt::formatter< BfVertex3 >
{
   constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

   template < typename FormatContext >
   auto format(const BfVertex3& v, FormatContext& ctx) const
   {
      return fmt::format_to(
          ctx.out(),
          "BfVertex3(pos={}, color={}, normals={})",
          v.pos,
          v.color,
          v.normals
      );
   }
};

// template <>
// struct fmt::formatter< BfVertex3 >
// {
//    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
//
//    template < typename FormatContext >
//    auto format(const BfVertex3& v, FormatContext& ctx)
//    {
//       return fmt::format_to(
//           ctx.out(),
//           "BfVertex3( p({:.3f}, {:.3f}, {:.3f}), c({:.3f}, {:.3f}, {:.3f}), "
//           "n({:.3f}, {:.3f}, {:.3f}) )",
//           v.pos.x,
//           v.pos.y,
//           v.pos.z,
//           v.color.r,
//           v.color.g,
//           v.color.b,
//           v.normals.x,
//           v.normals.y,
//           v.normals.z
//       );
//    }
// };

// template < typename b >
// struct fmt::
//     formatter< T, std::enable_if_t< std::is_base_of_v< BfVertex3, T >, char >
//     > : fmt::formatter< std::string >
// {
//    auto format(const BfVertex3& v, format_context& ctx) const
//    {
//       // return formatter< std::string >::format(
//       //     "BfVertex3( p({:.3f}, {:.3f}, {:.3f}), c({:.3f}, {:.3f},
//       {:.3f}), "
//       //     "n({:.3f}, {:.3f}, {:.3f}) )",
//       //     v.pos.x,
//       //     v.pos.y,
//       //     v.pos.z,
//       //     v.color.r,
//       //     v.color.g,
//       //     v.color.b,
//       //     v.normals.x,
//       //     v.normals.y,
//       //     v.normals.z,
//       //     ctx
//       // );
//       return formatter< std::string >::format("123");
//    }
// };

template < typename T >
struct BfVar
{
   // clang-format off
   using var = std::variant< T, T* >;

   BfVar() : m_value { BfVertex3{} } {}
   explicit BfVar(var&& v) noexcept : m_value { std::move(v) } {}
   explicit BfVar(const var& v) : m_value { v } {}
   BfVar(const BfVar& o) : m_value { o.m_value } {}
   BfVar(BfVar&& o) noexcept: m_value { std::move(o.m_value) }{}
   constexpr BfVar& operator=(const BfVar& o) { m_value = o.m_value; return *this; }
   constexpr BfVar& operator=(BfVar&& o) noexcept { m_value = std::move(o.m_value); return *this; }

   T* getp() { return &get(); }
   const T* getp() const { return &get(); }

   T& get() {
      return *std::visit([](auto &&v) -> T* {
         using _T = std::decay_t<decltype(v)>;
            if constexpr (std::is_pointer_v<_T>) { return v; }
            else { return &v; } 
         },
         m_value
      );
   }

   const T &get() const { 
      return *std::visit([](auto &&v) -> const T* {
         using _T = std::decay_t<decltype(v)>;
            if constexpr (std::is_pointer_v<_T>) { return v; }
            else { return &v; } 
         },
         m_value
      );
   } 
   // clanf-format on

private:
   var m_value;
};

struct BfVertex3Uni : BfVar<BfVertex3>
{
   // clang-format off
   BfVertex3Uni() : BfVar<BfVertex3> { BfVertex3{} } {}
   BfVertex3Uni(const BfVertex3Uni& o) : BfVar<BfVertex3> { o } {}
   BfVertex3Uni(BfVertex3Uni&& o) noexcept : BfVar<BfVertex3> { std::move(o) }{}
   explicit BfVertex3Uni(var&& v) noexcept : BfVar<BfVertex3> { std::move(v) } {}
   explicit BfVertex3Uni(const var& v) : BfVar<BfVertex3> { v } {}
   BfVertex3Uni& operator=(const BfVertex3Uni& o) = default;
   BfVertex3Uni& operator=(BfVertex3Uni&& o) noexcept = default;

   operator BfVertex3&() { return get(); }

   glm::vec3 &pos() { return get().pos; }
   const glm::vec3 &pos() const { return get().pos; }

   glm::vec3 &color() { return get().color; }
   const glm::vec3 &color() const { return get().color; }

   glm::vec3 &normals() { return get().normals; }
   const glm::vec3 &normals() const { return get().normals; }
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

// struct VertexT {
//     glm::vec2 pos;
//     glm::vec3 color;
//     glm::vec2 texCoord;
//
//     static VkVertexInputBindingDescription getBindingDescription() {
//         VkVertexInputBindingDescription bindingDescription{};
//         bindingDescription.binding = 0;
//         bindingDescription.stride = sizeof(VertexT);
//         bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//         return bindingDescription;
//     }
//
//     static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
//         std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
//
//         attributeDescriptions[0].binding = 0;
//         attributeDescriptions[0].location = 0;
//         attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
//         attributeDescriptions[0].offset = offsetof(VertexT, pos);
//
//         attributeDescriptions[1].binding = 0;
//         attributeDescriptions[1].location = 1;
//         attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
//         attributeDescriptions[1].offset = offsetof(VertexT, color);
//
//         attributeDescriptions[2].binding = 0;
//         attributeDescriptions[2].location = 2;
//         attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
//         attributeDescriptions[2].offset = offsetof(VertexT, texCoord);
//
//         return attributeDescriptions;
//     }
// };

#endif  // !BF_VERTEX_H
