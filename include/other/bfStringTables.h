#ifndef BF_STRING_TABLES_H
#define BF_STRING_TABLES_H

#include <imgui.h>

#include <format>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>

#include "bfBladeSection.h"
#include "bfGuiCreateWindowBladeBase.h"
#include "bfGuiCreateWindowBladeSection.h"
#include "bfGuiCreateWindowContainer.h"
#include "bfVertex2.hpp"

// template <typename T>
// class has_to_string_with_int
// {
// private:
//    template <typename U>
//    static auto test(int
//    ) -> decltype(std::to_string(std::declval<U>(), 0), std::true_type{});
//
//    template <typename>
//    static std::false_type test(...);
//
// public:
//    static constexpr bool value = decltype(test<T>(0))::value;
// };
//
// template <typename T>
// constexpr bool has_to_string_with_int_v = has_to_string_with_int<T>::value;

// Шаблон для проверки, существует ли функция `to_string` с параметром типа T
template < typename T, typename = void >
struct has_to_string_with_int : std::false_type
{
};

template < typename T >
struct has_to_string_with_int<
    T,
    std::void_t< decltype(std::to_string(std::declval< T >(), 0)) > >
    : std::true_type
{
};

// === === === === === === === === === === === === === === === === === === ===
template < typename T, typename = void >
struct is_container : std::false_type
{
};

template < typename T >
struct is_container<
    T,
    std::void_t<
        typename T::iterator,
        typename T::const_iterator,
        decltype(std::declval< T >().begin()),
        decltype(std::declval< T >().end()) > > : std::true_type
{
};

template < typename T >
constexpr bool is_container_v = has_to_string_with_int< T >::value;
// === === === === === === === === === === === === === === === === === === ===

template < class T >
std::string bfStringTablesGenPair(
    std::string name,
    const T& v,
    bool last = false,
    bool next_row = true,
    int indent = 0
);

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

// clang-format off
inline string
to_string(const BfGuiCreateWindowContainer& o, int indent = 0)
{
   auto indent_str = std::string(indent, '\t');
   return std::format(
       "{{\n"
       "{}{}{}{}{}{}{}{}{}{}{}{}\n"
       "{}}}",
        // indent_str,
	bfStringTablesGenPair("type", "BfGuiCreateWindowContainer", false, true, indent + 1),
	bfStringTablesGenPair("__id", o.__id, false, true, indent + 1),
	bfStringTablesGenPair("__str_id", o.__str_id, false, true, indent + 1),
	bfStringTablesGenPair("__is_button", o.__is_button, false, true, indent + 1),
	bfStringTablesGenPair("__is_force_render", o.__is_force_render, false, true, indent + 1),
	bfStringTablesGenPair("__is_render_header", o.__is_render_header, false, true, indent + 1),
	bfStringTablesGenPair("__is_collapsed", o.__is_collapsed, false, true, indent + 1),
	bfStringTablesGenPair("__resize_button_size", o.__resize_button_size, false, true, indent + 1),
	bfStringTablesGenPair("__bot_resize_button_size", o.__bot_resize_button_size, false, true, indent + 1),
	bfStringTablesGenPair("__window_pos", o.__window_pos, false, true, indent + 1),
	bfStringTablesGenPair("__window_size", o.__window_size, false, true, indent + 1),
	bfStringTablesGenPair("__containers", o.__containers, true, false, indent + 1),
        indent_str
   );
}
// clang-format on

// clang-format off
inline string
to_string(const BfGuiCreateWindowContainerObj& o, int indent = 0)
{
   auto indent_str = std::string(indent, '\t');
   return std::format(
       "{{\n"
       "{}{}{}{}{}{}\n"
       "{}}}",
        // indent_str,
	bfStringTablesGenPair("type", "BfGuiCreateWindowContainerObj", false, true, indent + 1),
	bfStringTablesGenPair("__selected_layer", o.__selected_layer, false, true, indent + 1),
	bfStringTablesGenPair("__name", std::string(o.__name.c_str()), false, true, indent + 1),
	bfStringTablesGenPair("__old_size", o.__old_size, false, true, indent + 1),
	bfStringTablesGenPair("__header_button_size", o.__header_button_size, false, true, indent + 1),
	bfStringTablesGenPair("base", static_cast<const BfGuiCreateWindowContainer&>(o), false, true, indent + 1),
        indent_str
   );
}
// clang-format on

// clang-format off
inline string
to_string(const BfBladeSectionCreateInfo& o, int indent = 0)
{
   auto indent_str = std::string(indent, '\t');
   return std::format(
       "{{\n"
       "{}{}{}{}{}{}{}{}{}{}{}{}{}\n"
       "{}}}",
	bfStringTablesGenPair("type", "BfBladeSectionCreateInfo", false, true, indent + 1),
	bfStringTablesGenPair("width", o.width, false, true, indent + 1),
	bfStringTablesGenPair("install_angle", o.install_angle, false, true, indent + 1),

	bfStringTablesGenPair("inlet_angle", o.inlet_angle, false, true, indent + 1),
	bfStringTablesGenPair("outlet_angle", o.outlet_angle, false, true, indent + 1),
	bfStringTablesGenPair("inlet_surface_angle", o.inlet_surface_angle, false, true, indent + 1),

	bfStringTablesGenPair("outlet_surface_angle", o.outlet_surface_angle, false, true, indent + 1),
	bfStringTablesGenPair("inlet_radius", o.inlet_radius, false, true, indent + 1),
	bfStringTablesGenPair("outlet_radius", o.outlet_radius, false, true, indent + 1),

	bfStringTablesGenPair("border_length", o.border_length, false, true, indent + 1),
	bfStringTablesGenPair("start_vertex", o.start_vertex, false, true, indent + 1),
	bfStringTablesGenPair("grow_direction", o.grow_direction, false, true, indent + 1),

	bfStringTablesGenPair("up_direction", o.up_direction, true, false, indent + 1),
        indent_str
   );
}

inline string
to_string(const BfBladeBaseCreateInfo& o, int indent = 0)
{
   auto indent_str = std::string(indent, '\t');
   std::ostringstream ss;
   ss << std::format("{}{{\n", indent_str);
   // ss << "{\n";
   for (auto info = o.section_infos.begin(); info != o.section_infos.end(); ++info) { 
      ss << std::to_string(*info, indent + 1);
      if (info != o.section_infos.end()) { 
	 ss << std::format(", -- {}\n", (int)(info - o.section_infos.begin()));
      }
      else {
	 ss << "\n";
      }
   }
   ss << indent_str << "}";
   return ss.str();
}

inline string
to_string(const BfGuiCreateWindowBladeSection& o, int indent = 0)
{
   auto indent_str = std::string(indent, '\t');
   return std::format(
       "{}{{\n"
       "{}{}{}{}\n"
       "{}}}",
        indent_str,
	bfStringTablesGenPair("type", "BfGuiCreateWindowBladeSection", false, true, indent + 1),
	bfStringTablesGenPair("__create_info", o.__create_info, false, true, indent + 1),
	bfStringTablesGenPair("__mode", (int)o.__mode, false, true, indent + 1),
	bfStringTablesGenPair("base", static_cast<const BfGuiCreateWindowContainerObj&>(o), true, false, indent + 1),
        indent_str
   );
}

inline string
to_string(const BfGuiCreateWindowBladeBase& o, int indent = 0)
{
   auto indent_str = std::string(indent, '\t');
   return std::format(
       "{}{{\n"
       "{}{}\n"
       "{}}}",
        indent_str,
	bfStringTablesGenPair("type", "BfGuiCreateWindowBladeBase", false, true, indent + 1),
	bfStringTablesGenPair("base", static_cast<const BfGuiCreateWindowContainerObj&>(o), true, false, indent + 1),
        indent_str
   );
}

// clang-format on

// clang-format off
// template<class T>
// inline string
// to_string(const T& o, int indent = 0)
// {
//    // FIXME: Add check for inside vector types!
//    // if (!std::is_base_of_v<BfGuiCreateWindowContainer, std::decay_t<T>> &&
//    //     !std::is_same_v<BfGuiCreateWindowContainer, std::decay_t<U>>) { 
//    //    return ""; 
//    // }
//    
//    auto indent_str = std::string(indent, '\t');
//    if (!std::size(o)) 
//       return std::format("{}{{}}", indent_str);
//    auto format_str = std::format("{}{{\n", indent_str);
//
//    for (auto it = std::begin(o); it != std::end(o); ++it) { 
//       if (auto casted = std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(*it)) 
//       {
// 	 format_str += std::to_string(*casted, indent + 1);
//       }
//       else if (auto casted = std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(*it)) 
//       {
// 	 format_str += std::to_string(*casted, indent + 1);
//       }
//       if (it != std::end(o)) format_str += ",\n";
//       else format_str += "\n";
//    }
//    format_str += std::format("{}}}", indent_str);
//    return format_str;
// }
// clang-format on

inline string
to_string(const std::list< ptrContainer > o, int indent = 0)
{
   auto indent_str = std::string(indent, '\t');
   if (!std::size(o))
      return std::format("{}{{}}", indent_str);
   auto format_str = std::format("{}{{\n", indent_str);

   for (auto it = std::begin(o); it != std::end(o); ++it)
   {
      if (auto casted =
              std::dynamic_pointer_cast< BfGuiCreateWindowBladeBase >(*it))
      {
         format_str += std::to_string(*casted, indent + 1);
      }
      else if (auto casted =
                   std::dynamic_pointer_cast< BfGuiCreateWindowBladeSection >(
                       *it
                   ))
      {
         format_str += std::to_string(*casted, indent + 1);
      }
      else if (auto casted =
                   std::dynamic_pointer_cast< BfGuiCreateWindowContainerObj >(
                       *it
                   ))
      {
         format_str += std::to_string(*casted, indent + 1);
      }
      else if (auto casted =
                   std::dynamic_pointer_cast< BfGuiCreateWindowContainerPopup >(
                       *it
                   ))
      {
         continue;
      }
      else if (auto casted =
                   std::dynamic_pointer_cast< BfGuiCreateWindowContainer >(*it))
      {
         format_str += std::to_string(*casted, indent + 1);
      }

      if (it != std::end(o))
         format_str += ",\n";
      else
         format_str += "\n";
   }
   format_str += std::format("{}}}", indent_str);
   return format_str;
}
} // namespace std

template < class T >
std::string
bfStringTablesGenPair(
    std::string name, const T& v, bool last, bool next_row, int indent
)
{
   auto indent_str = std::string(indent, '\t');
   // auto indent_str = std::string(indent, '');
   std::ostringstream ss;

   if constexpr (std::is_same_v< std::decay_t< T >, std::string >)
   {
      ss << indent_str << name << " = \"" << std::string(v.c_str()) << "\"";
   }
   else if constexpr (std::is_same_v< std::decay_t< T >, const char* > ||
                      std::is_same_v< std::decay_t< T >, char* >)
   {
      ss << indent_str << name << " = \"" << std::string(v) << "\"";
   }
   else if constexpr (std::is_floating_point_v< std::decay_t< T > >)
   {
      ss << std::fixed << std::setprecision(4);
      ss << indent_str << name << " = " << v;
   }

   else if constexpr (std::is_integral_v< std::decay_t< T > >)
   {
      ss << indent_str << name << " = " << v;
   }
   else if constexpr (is_container< T >::value)
   {
      ss << indent_str << name << " = {\n";
      bool first = true;
      int current_index = 0;
      for (const auto& elem : v)
      {
         if (auto casted =
                 std::dynamic_pointer_cast< BfGuiCreateWindowBladeBase >(elem))
         {
            ss << std::to_string(*casted, indent + 1);
         }
         else if (auto casted = std::dynamic_pointer_cast<
                      BfGuiCreateWindowBladeSection >(elem))
         {
            ss << std::to_string(*casted, indent + 1);
         }
         else if (auto casted = std::dynamic_pointer_cast<
                      BfGuiCreateWindowContainerObj >(elem))
         {
            ss << std::to_string(*casted, indent + 1);
         }
         else if (auto casted = std::dynamic_pointer_cast<
                      BfGuiCreateWindowContainerPopup >(elem))
         {
            current_index++;
            continue;
         }
         else if (auto casted =
                      std::dynamic_pointer_cast< BfGuiCreateWindowContainer >(
                          elem
                      ))
         {
            ss << std::to_string(*casted, indent + 1);
         }

         else
         {
            ss << std::to_string(*elem, indent + 1);
         }
         if (current_index != std::size(v))
            ss << ",\n";
         else
            ss << "\n";

         current_index++;
      }
      ss << indent_str << "}";
      ss << (last ? "" : ",") << (next_row ? "\n" : "");
      // return ss.str();
   }
   else if constexpr (std::is_same_v<
                          BfBladeSectionCreateInfo,
                          std::decay_t< T > >)
   {
      ss << indent_str << name << " = " << std::to_string(v, indent);
   }
   else if constexpr (has_to_string_with_int< std::decay_t< T > >::value)
   {
      ss << indent_str << name << " = " << std::to_string(v, indent);
   }
   else
   {
      ss << indent_str << name << " = " << std::to_string(v);
   }
   ss << (last ? "" : ",") << (next_row ? "\n" : "");

   return ss.str();
}

inline std::string
bfCastAndGetUpperContainerStr(ptrContainer c)
{
   // clang-format off
   if (auto _c = std::dynamic_pointer_cast<BfGuiCreateWindowBladeBase>(c))
   {
      return std::to_string(*_c);
   }
   else if (auto _c = std::dynamic_pointer_cast<BfGuiCreateWindowBladeSection>(c))
   {
      return std::to_string(*_c);
   }
   else if (auto _c = std::dynamic_pointer_cast<BfGuiCreateWindowContainerPopup>(c))
   {
      return std::to_string(*_c);
   }
   else if (auto _c = std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(c))
   {
      return std::to_string(*_c);
   }
   else if (auto _c = std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(c))
   {
      return std::to_string(*_c);
   }
   else { 
      return "";
   }
   // clang-format on
}

#endif
