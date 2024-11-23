#ifndef BF_STRING_TABLES_H
#define BF_STRING_TABLES_H

#include <imgui.h>

#include <format>
#include <iomanip>
#include <sstream>
#include <string>
#include <type_traits>

#include "bfVertex2.hpp"

namespace std
{

inline string
to_string(const ImVec4& o)
{
   return std::format("{{ {}, {}, {}, {} }}", o.x, o.y, o.z, o.w);
}

inline string
to_string(const ImVec2& o)
{
   return std::format("{{ {}, {} }}", o.x, o.y);
}

inline string
to_string(const glm::vec2& o)
{
   return std::format("{{ {}, {} }}", o.x, o.y);
}

inline string
to_string(const glm::vec3& o)
{
   return std::format("{{ {}, {}, {} }}", o.x, o.y, o.z);
}

inline string
to_string(const glm::vec4& o)
{
   return std::format("{{ {}, {}, {}, {} }}", o.x, o.y, o.z, o.w);
}

inline string
to_string(const BfVertex3& o)
{
   return std::format(
       "{{ {}, {}, {} }}",
       std::to_string(o.pos),
       std::to_string(o.color),
       std::to_string(o.normals)
   );
}

}  // namespace std

template <class T>
std::string
bfStringTablesGenPair(
    std::string name,
    const T& v,
    bool last = false,
    bool next_row = true,
    int indent = 0
)
{
   auto indent_str = std::string(indent, '\t');
   using T_t = std::decay_t<T>;
   // if constexpr (std::is_same_v<T_t, ImVec2>)
   // {
   //    std::ostringstream ss;
   //    ss << indent_str << name
   //       << " = { "
   //       // << v.x << ", " << v.y
   //       << std::to_string(v)
   //
   //       << " }" << (last ? "," : "") << (next_row ? "\n" : "");
   //
   //    return ss.str();
   // }
   // else if constexpr (std::is_same_v<T_t, ImVec4>)
   // {
   //    std::ostringstream ss;
   //    ss << indent_str << name
   //       << " = { "
   //       // << v.x << ", " << v.y << ", " << v.z << ", " << v.w
   //       << std::to_string(v) << " }"
   //
   //       << (last ? "," : "") << (next_row ? "\n" : "");
   //
   //    return ss.str();
   // }

   if constexpr (std::is_same_v<T_t, std::string> ||
                 std::is_same_v<T_t, const char*> || std::is_same_v<T_t, char*>)
   {
      std::ostringstream ss;
      ss << indent_str << name << " = \"" << std::string(v) << "\""
         << (last ? "," : "") << (next_row ? "\n" : "");

      return ss.str();
   }
   else if constexpr (std::is_same_v<T_t, float> || std::is_same_v<T_t, double>)
   {
      std::ostringstream ss;
      ss << std::fixed << std::setprecision(4);
      ss << indent_str << name << " = " << v << (last ? "," : "")
         << (next_row ? "\n" : "");

      return ss.str();
   }

   else
   {
      std::ostringstream ss;
      ss << indent_str << name << " = " << std::to_string(v)
         << (last ? "," : "") << (next_row ? "\n" : "");

      return ss.str();
   }
}

#endif
