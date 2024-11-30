#ifndef BF_CONFIG_MANAGER_H
#define BF_CONFIG_MANAGER_H

#include <bfEvent.h>
#include <bfGuiCreateWindowContainer.h>
#include <config_forms/bfFormGui.h>

#include <cassert>
#include <filesystem>
#include <memory>
#include <sol/sol.hpp>
#include <type_traits>
#include <typeindex>

#include "bfGuiCreateWindowBladeBase.h"
#include "bfGuiCreateWindowBladeSection.h"

// class BfLuaTable
// {
// public:
//    using _type = std::map<BfLuaValue, BfLuaValue>;
//
//    _type v;
//
//    BfLuaValue& operator[](BfLuaValue);
//    std::string get(BfLuaValue);
//    //
//    static std::string convert(const BfLuaValue&);
// };

class BfLuaTable;

using BfLuaValue =
    std::variant<int, double, std::string, bool, std::shared_ptr<BfLuaTable>>;

class BfLuaTable : public std::map<BfLuaValue, BfLuaValue>
{
public:
   static std::string convert(const BfLuaValue&);
};

class BfConfigManager
{
   using BFCM = BfConfigManager;

   static std::shared_ptr<BfConfigManager> __instance;
   static std::string __indent_separator;
   sol::state __lua;

public:
   static void __findFilesInDir(
       std::filesystem::path root,
       std::string ext,
       std::vector<std::filesystem::path>& out
   );

   BfConfigManager();

   BfConfigManager(const BfConfigManager&) = delete;
   BfConfigManager& operator=(const BfConfigManager&) = delete;

   static std::filesystem::path exePath();

   // Instance manipulation
   static void createInstance();
   static void destroyInstance();
   static void recreateInstance();
   static BfConfigManager* getInstance();

   // Lua manipulation
   static BfEvent loadStdLibrary(sol::lib lib);
   static BfEvent addPackagePath(std::filesystem::path path);
   static BfEvent loadScriptStr(const std::string& script);
   static BfEvent loadScript(std::filesystem::path path);
   static BfEvent loadRequireScript(
       std::filesystem::path path, std::string varname = ""
   );

   static std::string getLuaTableStr(sol::table table, int indent_level = 0);
   static sol::object getLuaObj(const std::string& key);

   static BfLuaTable convertLuaTable(sol::table* obj);

   static BfEvent fillFormFont(sol::table obj, BfFormFont* form);
   static BfEvent fillFormFontSettings(
       sol::table obs, BfFormFontSettings* form
   );
   //
   // static BfEvent loadBfGuiCreateWindowContainer(
   //     sol::table obj, std::shared_ptr<BfGuiCreateWindowContainer> c
   // );
   // static BfEvent loadBfGuiCreateWindowContainer(
   //     sol::table obj, std::shared_ptr<BfGuiCreateWindowContainerObj> c
   // );
   //
   template <class T>
   static BfEvent assertLoadingContainerParent(
       sol::table obj, std::shared_ptr<T> c
   )
   {
      static std::map<std::type_index, std::string> s{
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
          //
          {std::type_index(typeid(BfGuiCreateWindowContainerPopup)),
           "BfGuiCreateWindowContainer"},
          //
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
          //
          {std::type_index(typeid(BfGuiCreateWindowContainerObj)),
           "BfGuiCreateWindowContainer"},
          //
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
          //
          {std::type_index(typeid(BfGuiCreateWindowContainer)),
           "BfGuiCreateWindowContainer"},
          //
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
          //
          {std::type_index(typeid(BfGuiCreateWindowBladeBase)),
           "BfGuiCreateWindowContainerObj"},
          //
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
          //
          {std::type_index(typeid(BfGuiCreateWindowBladeSection)),
           "BfGuiCreateWindowContainerObj"}
          //
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
      };
      auto found = s.find(std::type_index(typeid(T)));
      if (found != s.end())
      {
         assert(obj.get<std::string>("type") == found->second);
      }
      else
      {
         std::cout << obj.get<std::string>("type") << "\n";
         throw std::runtime_error(
             "Input data type is invalid (input type is not supposed to be "
             "here)"
         );
      }
      return BfEvent();
   }

   template <class T>
   static BfEvent assertLoadingContainerType(
       sol::table obj, std::shared_ptr<T> c
   )
   {
      static std::map<std::type_index, std::string> s{
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
          //
          {std::type_index(typeid(BfGuiCreateWindowContainerPopup)),
           "BfGuiCreateWindowContainerPopup"},
          //
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
          //
          {std::type_index(typeid(BfGuiCreateWindowContainerObj)),
           "BfGuiCreateWindowContainerObj"},
          //
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
          //
          {std::type_index(typeid(BfGuiCreateWindowContainer)),
           "BfGuiCreateWindowContainer"},
          //
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
          //
          {std::type_index(typeid(BfGuiCreateWindowBladeBase)),
           "BfGuiCreateWindowBladeBase"},
          //
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
          //
          {std::type_index(typeid(BfGuiCreateWindowBladeSection)),
           "BfGuiCreateWindowBladeSection"}
          //
          // --- --- --- --- --- --- --- --- --- --- --- --- --- ---
      };

      auto found = s.find(std::type_index(typeid(T)));
      if (found != s.end())
      {
         std::cout << obj.get<std::string>("type") << " : " << found->second
                   << "\n";
         assert(obj.get<std::string>("type") == found->second);
      }
      else
      {
         std::cout << obj.get<std::string>("type") << "\n";
         throw std::runtime_error(
             "Input data type is invalid (input type is not supposed to be "
             "here)"
         );
      }

      return BfEvent();
   }

   template <class T>
   static std::shared_ptr<T> makeNeededWindowByLuaTable(sol::table obj)
   {
      static auto make = [&obj](auto ptr) {
         loadBfGuiCreateWindowContainer(obj, ptr);
         return std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(ptr);
      };

      using wptr = std::weak_ptr<BfGuiCreateWindowContainer>;
      // clang-format off
      static std::map<std::string, std::function<std::shared_ptr<BfGuiCreateWindowContainer>()>> s
      {
	    {"BfGuiCreateWindowContainerPopup", [&obj]() { return make(std::make_shared<BfGuiCreateWindowContainerPopup>(wptr())); }},
            {"BfGuiCreateWindowContainerObj",   [&obj]() { return make(std::make_shared<BfGuiCreateWindowContainerObj>  (wptr())); }},
            {"BfGuiCreateWindowContainer",      [&obj]() { return make(std::make_shared<BfGuiCreateWindowContainer>     (wptr())); }},
            {"BfGuiCreateWindowBladeBase",      [&obj]() { return make(std::make_shared<BfGuiCreateWindowBladeBase>     (wptr())); }},
            {"BfGuiCreateWindowBladeSection",   [&obj]() { return make(std::make_shared<BfGuiCreateWindowBladeSection>  (wptr())); }}
      };
      // clang-format on
      auto key = obj.get<std::string>("type");
      auto found = s.find(key);

      if (found != s.end())
      {
         return std::dynamic_pointer_cast<T>(found->second());
      }
      else
      {
         std::cout << "Invalid type: " << key << "\n";
         throw std::runtime_error("Input data type is invalid");
      }
   }

   template <class T>
   static BfEvent loadBfGuiCreateWindowContainer(
       sol::table obj, std::shared_ptr<T> c
   )
   {
      using dT = std::decay_t<T>;

      if constexpr (std::is_same_v<dT, BfGuiCreateWindowContainerPopup>)
      {
         BfConfigManager::assertLoadingContainerType(obj, c);
         return BfEvent();
      }
      else if constexpr (std::is_same_v<dT, BfGuiCreateWindowBladeBase>)
      {
         BfConfigManager::assertLoadingContainerType(obj, c);
         sol::table base = sol::table(obj["base"]);
         BfConfigManager::assertLoadingContainerParent(base, c);
         BfConfigManager::loadBfGuiCreateWindowContainer(
             base,
             std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(c)
         );
         //
         // return BfEvent();
      }
      else if constexpr (std::is_same_v<dT, BfGuiCreateWindowBladeSection>)
      {
         // clang-format off
         BfConfigManager::assertLoadingContainerType(obj, c);
         sol::table base = sol::table(obj["base"]);
         BfConfigManager::assertLoadingContainerParent(base, c);

         c->__mode = obj["__mode"].get_or(BfGuiCreateWindowBladeSection::viewMode::SHORT);

         assert(obj["__create_info"].valid());
         sol::table create_info_table = obj["__create_info"];
         assert(create_info_table["type"].valid() && (create_info_table["type"].get<std::string>() == "BfBladeSectionCreateInfo"));

         c->__create_info.width = create_info_table["width"].get_or<float>(1.0000);
         c->__create_info.install_angle = create_info_table["install_angle"].get_or<float>(102.0000);
         c->__create_info.inlet_angle = create_info_table["inlet_angle"].get_or<float>(25.0000);
         c->__create_info.outlet_angle = create_info_table["outlet_angle"].get_or<float>(42.0000);
         c->__create_info.inlet_surface_angle = create_info_table["inlet_surface_angle"].get_or<float>(15.0000);
         c->__create_info.outlet_surface_angle = create_info_table["outlet_surface_angle"].get_or<float>(15.0000);
         c->__create_info.inlet_radius = create_info_table["inlet_radius"].get_or<float>(0.0250);
         c->__create_info.outlet_radius = create_info_table["outlet_radius"].get_or<float>(0.0050);
         c->__create_info.border_length = create_info_table["border_length"].get_or<float>(20.0000);

         if (create_info_table["start_vertex"].valid()) {
             sol::table start_vertex = create_info_table["start_vertex"];
             c->__create_info.start_vertex.x = start_vertex[1].get_or<float>(0.0);
             c->__create_info.start_vertex.y = start_vertex[2].get_or<float>(0.0);
             c->__create_info.start_vertex.z = start_vertex[3].get_or<float>(0.0);
         }

         if (create_info_table["grow_direction"].valid()) {
             sol::table grow_direction = create_info_table["grow_direction"];
             c->__create_info.grow_direction.x = grow_direction[1].get_or<float>(0.0);
             c->__create_info.grow_direction.y = grow_direction[2].get_or<float>(0.0);
             c->__create_info.grow_direction.z = grow_direction[3].get_or<float>(0.0);
         }

         if (create_info_table["up_direction"].valid()) {
             sol::table up_direction = create_info_table["up_direction"];
             c->__create_info.up_direction.x = up_direction[1].get_or<float>(0.0);
             c->__create_info.up_direction.y = up_direction[2].get_or<float>(0.0);
             c->__create_info.up_direction.z = up_direction[3].get_or<float>(0.0);
         }
         // clang-format on

         BfConfigManager::loadBfGuiCreateWindowContainer(
             base,
             std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(c)
         );

         // return BfEvent();
      }
      else if constexpr (std::is_same_v<dT, BfGuiCreateWindowContainerObj>)
      {
         BfConfigManager::assertLoadingContainerType(obj, c);
         sol::table base = sol::table(obj["base"]);
         BfConfigManager::assertLoadingContainerParent(base, c);

         // clang-format off
         c->__selected_layer = obj["__selected_layer"].get_or<float>(0);
         c->__name = obj["__name"].get_or<std::string>("");
         c->__old_size.x = sol::table(obj["__old_size"])[1].get_or<float>(0.0);
         c->__old_size.y = sol::table(obj["__old_size"])[2].get_or<float>(0.0);
         c->__header_button_size.x = sol::table(obj["__header_button_size"])[1].get_or<float>(0.0);
         c->__header_button_size.y = sol::table(obj["__header_button_size"])[2].get_or<float>(0.0);
         // clang-format on
         BfConfigManager::loadBfGuiCreateWindowContainer(
             base,
             std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(c)
         );
         // return BfEvent();
      }
      else if constexpr (std::is_same_v<dT, BfGuiCreateWindowContainer>)
      {
         BfConfigManager::assertLoadingContainerType(obj, c);

         // clang-format off
         c->__id = obj["__id"].get_or(-1);
         c->__str_id = obj["__str_id"].get_or<std::string>("");
         c->__is_button = obj["__is_button"].get_or((int)BfGuiCreateWindowContainer_ButtonType_All);
         c->__is_force_render = obj["__is_force_render"].get_or<int>(false);
         c->__is_render_header = obj["__is_render_header"].get_or<int>(true);
         c->__is_collapsed = obj["__is_collapsed"].get_or<int>(false);
         c->__resize_button_size.x = sol::table(obj["__resize_button_size"])[1].get_or<float>(0.0);
         c->__resize_button_size.y = sol::table(obj["__resize_button_size"])[2].get_or<float>(0.0);
         c->__bot_resize_button_size.x = sol::table(obj["__bot_resize_button_size"])[1].get_or<float>(0.0);
         c->__bot_resize_button_size.y = sol::table(obj["__bot_resize_button_size"])[2].get_or<float>(0.0);
         c->__window_pos.x = sol::table(obj["__window_pos"])[1].get_or<float>(0.0);
         c->__window_pos.y = sol::table(obj["__window_pos"])[2].get_or<float>(0.0);
         c->__window_size.x = sol::table(obj["__window_size"])[1].get_or<float>(0.0);
         c->__window_size.y = sol::table(obj["__window_size"])[2].get_or<float>(0.0);
         // clang-format on

         sol::table inner = obj.get<sol::table>("__containers");
         assert(inner.valid());
         for (auto& it : inner)
         {
            sol::object v = it.second;
            if (v.is<sol::table>())
            {
               sol::table table = v.as<sol::table>();
               c->add(
                   makeNeededWindowByLuaTable<BfGuiCreateWindowContainer>(table)
               );
            }
         }
      }
      else
      {
         throw std::runtime_error("Underfined type to parse");
      }
      return BfEvent();
   }
   static BfEvent loadContainers(sol::table obj, std::list<ptrContainer>& c);
};

#endif
