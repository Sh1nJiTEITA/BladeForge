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

#define BF_LOAD_VAR(OBJ, KEY, TYPE, DEFAULT) \
   (OBJ[KEY].valid() ? OBJ.get<TYPE>(KEY) : DEFAULT)

#define BF_LOAD_VEC2(obj, table_key, index, default_value)                \
   (obj[table_key].valid() ? sol::table(obj[table_key]).get<float>(index) \
                           : default_value)

   template <class T>
   static BfEvent loadBfGuiCreateWindowContainer(
       sol::table obj, std::shared_ptr<T> c
   )
   {
      using dT = std::decay_t<T>;

      if constexpr (std::is_same_v<
                        dT,
                        std::decay_t<BfGuiCreateWindowContainerPopup>()>)
      {
         BfConfigManager::assertLoadingContainerType(obj, c);
         return BfEvent();
      }
      else if constexpr (std::is_same_v<
                             dT,
                             std::decay_t<BfGuiCreateWindowBladeBase>()>)
      {
         BfConfigManager::assertLoadingContainerType(obj, c);

         return BfEvent();
      }
      else if constexpr (std::is_same_v<
                             dT,
                             std::decay_t<BfGuiCreateWindowBladeSection>()>)
      {
         BfConfigManager::assertLoadingContainerType(obj, c);
         return BfEvent();
      }
      else if constexpr (std::is_same_v<
                             dT,
                             std::decay_t<BfGuiCreateWindowContainerObj>()>)
      {
         BfConfigManager::assertLoadingContainerType(obj, c);
         sol::table base = sol::table(obj["base"]);
         BfConfigManager::assertLoadingContainerParent(base, c);

         c->__selected_layer = BF_LOAD_VAR(obj, "__selected_layer", int, 0);
         c->__name = BF_LOAD_VAR(obj, "__name", std::string, "");
         c->__old_size.x = BF_LOAD_VEC2(obj, "__old_size", 1, 0.0f);
         c->__old_size.y = BF_LOAD_VEC2(obj, "__old_size", 2, 0.0f);
         c->__header_button_size.x =
             BF_LOAD_VEC2(obj, "__header_button_size", 1, 0.0f);
         c->__header_button_size.y =
             BF_LOAD_VEC2(obj, "__header_button_size", 2, 0.0f);

         BfConfigManager::loadBfGuiCreateWindowContainer(
             base,
             std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(c)
         );
         return BfEvent();
      }
      else if constexpr (std::is_same_v<dT, BfGuiCreateWindowContainer>)
      {
         BfConfigManager::assertLoadingContainerType(obj, c);

         c->__id = BF_LOAD_VAR(obj, "__id", int, -1);
         c->__str_id = BF_LOAD_VAR(obj, "__str_id", std::string, "");
         c->__is_button = BF_LOAD_VAR(
             obj,
             "__is_button",
             int,
             BfGuiCreateWindowContainer_ButtonType_All
         );
         c->__is_force_render =
             BF_LOAD_VAR(obj, "__is_force_render", int, false);
         c->__is_render_header =
             BF_LOAD_VAR(obj, "__is_render_header", int, true);
         c->__is_collapsed = BF_LOAD_VAR(obj, "__is_collapsed", int, false);
         c->__resize_button_size.x =
             BF_LOAD_VEC2(obj, "__resize_button_size", 1, 0.0f);
         c->__resize_button_size.y =
             BF_LOAD_VEC2(obj, "__resize_button_size", 2, 0.0f);
         c->__bot_resize_button_size.x =
             BF_LOAD_VEC2(obj, "__bot_resize_button_size", 1, 0.0f);
         c->__bot_resize_button_size.y =
             BF_LOAD_VEC2(obj, "__bot_resize_button_size", 2, 0.0f);
         c->__window_pos.x = BF_LOAD_VEC2(obj, "__window_pos", 1, 0.0f);
         c->__window_pos.y = BF_LOAD_VEC2(obj, "__window_pos", 2, 0.0f);
         c->__window_size.x = BF_LOAD_VEC2(obj, "__window_size", 1, 0.0f);
         c->__window_size.y = BF_LOAD_VEC2(obj, "__window_size", 2, 0.0f);
         return BfEvent();
      }
      return BfEvent();
   }

   static BfEvent loadContainers(sol::table obj, std::list<ptrContainer>& c);
};

#endif
