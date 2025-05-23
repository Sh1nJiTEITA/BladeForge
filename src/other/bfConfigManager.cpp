#include <bfConfigManager.h>
#include <bits/fs_dir.h>

#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sol/forward.hpp>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "bfEvent.h"
#include "bfGuiCreateWindowContainer.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

std::shared_ptr< BfConfigManager > BfConfigManager::__instance = nullptr;
//
// BfLuaValue& BfLuaTable::operator[](BfLuaValue t) { return v[t]; }
//
// std::string BfLuaTable::get(BfLuaValue key_str)
// {
//    return BfLuaTable::convert(v[key_str]);
// }

std::string
BfLuaTable::convert(const BfLuaValue& v)
{
   std::string o;
   std::visit(
       [&o](auto&& arg) {
          using T = std::decay_t< decltype(arg) >;
          if constexpr (std::is_same_v< T, std::string >)
          {
             o = arg;
          }
          else if constexpr (std::is_same_v< T, int >)
          {
             o = std::to_string(arg);
          }
          else if constexpr (std::is_same_v< T, float >)
          {
             o = std::to_string(arg);
          }
          else if constexpr (std::is_same_v< T, double >)
          {
             o = std::to_string(arg);
          }
          else if constexpr (std::is_same_v< T, bool >)
          {
             o = std::to_string(arg);
          }
       },
       v
   );
   return o;
}

void
BfConfigManager::__findFilesInDir(
    std::filesystem::path root,
    std::string ext,
    std::vector< std::filesystem::path >& out
)
{
   for (const auto& entry : std::filesystem::directory_iterator(root))
   {
      if (entry.is_directory())
      {
         __findFilesInDir(std::filesystem::absolute(entry), ext, out);
      }
      else if (entry.is_regular_file())
      {
         if (ext == "")
         {
            out.push_back(entry.path());
         }
         else if (entry.path().extension() == ext)
         {
            out.push_back(entry.path());
         }
      }
   }
}

BfConfigManager::BfConfigManager() {}

std::filesystem::path
BfConfigManager::exePath()
{
#ifdef _WIN32
   char buffer[MAX_PATH];
   DWORD size = GetModuleFileNameA(nullptr, buffer, MAX_PATH);

   if (size == 0)
   {
      throw std::runtime_error("Failed to retrieve the executable path");
   }

   return std::filesystem::path(buffer).remove_filename();
#elif __linux__
   char buffer[2048];
   ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
   if (count != -1)
   {
      buffer[count] = '\0';
      return std::filesystem::path(buffer).remove_filename();
   }
   else
   {
      throw std::runtime_error("Failed to resolve executable path");
   }
#elif __APPLE__
   char buffer[2048];
   uint32_t size = sizeof(buffer);
   if (_NSGetExecutablePath(buffer, &size) == 0)
   {
      return std::filesystem::path(buffer).remove_filename();
   }
   else
   {
      throw std::runtime_error("Buffer too small; increase its size");
   }
#else
   throw std::runtime_error("Unsupported platform");
#endif
}

void
BfConfigManager::createInstance()
{
   if (!__instance)
   {
      __instance = std::shared_ptr< BfConfigManager >(new BfConfigManager());
      // __instance->__lua.open_libraries(sol::lib::base,
      //                                  sol::lib::package,
      //                                  sol::lib::table);
   }
}

void
BfConfigManager::destroyInstance()
{
   if (__instance)
   {
      __instance.reset();
   }
}

void
BfConfigManager::recreateInstance()
{
   BFCM::destroyInstance();
   BFCM::createInstance();
}

BfConfigManager*
BfConfigManager::getInstance()
{
   return __instance.get();
}

BfEvent
BfConfigManager::loadStdLibrary(sol::lib lib)
{
   static std::map< sol::lib, std::string > std_lib_names = {
       {sol::lib::base, "base"},
       {sol::lib::package, "package"},
       {sol::lib::coroutine, "coroutine"},
       {sol::lib::string, "string"},
       {sol::lib::os, "os"},
       {sol::lib::math, "math"},
       {sol::lib::table, "table"},
       {sol::lib::debug, "debug"},
       {sol::lib::bit32, "bit32"},
       {sol::lib::io, "io"},
       {sol::lib::ffi, "ffi"},
       {sol::lib::jit, "jit"},
       {sol::lib::utf8, "utf8"},
       {sol::lib::count, "count"},
   };

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (std_lib_names.find(lib) == std_lib_names.end())
   {
      event.action = BF_ACTION_TYPE_LOAD_STD_LUA_LIBRARY_FAILURE;
      event.success = false;
      event.info = "underfined";
   }
   else
   {
      auto inst = getInstance();
      inst->__lua.open_libraries(lib);

      event.action = BF_ACTION_TYPE_LOAD_STD_LUA_LIBRARY_SUCCESS;
      event.success = true;
      event.info = std_lib_names[lib];
   }
   return event;
}

BfEvent
BfConfigManager::addPackagePath(std::filesystem::path path)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   {
      std::filesystem::path tmp_path = std::filesystem::absolute(path);

      if (!std::filesystem::exists(tmp_path))
      {
         event.action = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info = " input path does not exist";
         return event;
      }

      if (!std::filesystem::is_directory(tmp_path))
      {
         event.action = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info = " input path is not directory path";
         return event;
      }

      // Adding "/" sign to the end of directory path, caz
      // lua needs such paths for directories
      if (!tmp_path.empty() && tmp_path.string().back() != '/')
      {
         tmp_path = tmp_path / "";
      }

      std::string lua_path = tmp_path.string();
      std::replace(lua_path.begin(), lua_path.end(), '\\', '/');

      std::string command_msg = "package.path = package.path .. "
                                "\";" +
                                lua_path + "?.lua\"";

      BfConfigManager::getInstance()->__lua.script(command_msg);

      event.action = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_SUCCESS;
      event.info = "path is " + tmp_path.string();
   }
   return event;
}

BfEvent
BfConfigManager::loadScriptStr(const std::string& script)
{
   BfConfigManager::getInstance()->__lua.script(script);
   return BfEvent();
}

BfEvent
BfConfigManager::loadScript(std::filesystem::path path)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
   try
   {
      std::filesystem::path tmp_path = std::filesystem::absolute(path);
      if (!std::filesystem::exists(tmp_path))
      {
         event.action = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info = " input path does not exist";
         return event;
      }
      if (!std::filesystem::is_regular_file(tmp_path))
      {
         event.action = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info = " input path is not file path";
         return event;
      }
      if (tmp_path.extension() != ".lua")
      {
         event.action = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info = " input file path extension is not .lua";
         return event;
      }

      BfConfigManager::getInstance()->__lua.script_file(path.string());

      event.action = BF_ACTION_TYPE_LOAD_LUA_SCRIPT_SUCCESS;
   }
   catch (std::exception& e)
   {
      event.action = BF_ACTION_TYPE_LOAD_LUA_SCRIPT_FAILURE;
      event.success = false;
      event.info = " external error -> " + std::string(e.what());
      return event;
   }
   return event;
}

BfEvent
BfConfigManager::loadRequireScript(
    std::filesystem::path path, std::string varname
)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
   try
   {
      std::filesystem::path tmp_path = std::filesystem::absolute(path);
      if (!std::filesystem::exists(tmp_path))
      {
         event.action = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info = " input path does not exist";
         return event;
      }
      if (!std::filesystem::is_regular_file(tmp_path))
      {
         event.action = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info = " input path is not file path";
         return event;
      }
      if (tmp_path.extension() != ".lua")
      {
         event.action = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info = " input file path extension is not .lua";
         return event;
      }

      if (varname.empty())
      {
         varname = tmp_path.stem().string();
      }

      BfConfigManager::getInstance()->__lua.script(
          varname + " = " + "require(\"" + tmp_path.stem().string() + "\")"
      );

      event.action = BF_ACTION_TYPE_LOAD_LUA_SCRIPT_SUCCESS;
      event.info = " lua file with path " + tmp_path.string() +
                   " was loaded as " + varname;
   }
   catch (std::exception& e)
   {
      event.action = BF_ACTION_TYPE_LOAD_LUA_SCRIPT_FAILURE;
      event.success = false;
      event.info = " external error -> " + std::string(e.what());
      return event;
   }
   return event;
}

std::string BfConfigManager::__indent_separator = "   ";

std::string
BfConfigManager::getLuaTableStr(sol::table table, int level)
{
   std::string str = "{\n";
   for (const auto& pair : table)
   {
      sol::object key = pair.first;
      sol::object value = pair.second;

      std::string indent(level * __indent_separator.size(), ' ');

      if (value.is< sol::table >())
      {
         str += indent;
         str += (key.is< int >() ? "" : key.as< std::string >() + " = ");
         str += getLuaTableStr(value, level + 1);
      }
      else
      {
         str +=
             indent + (key.is< int >() ? "" : key.as< std::string >() + " = ");
         if (value.is< std::string >())
         {
            str += value.as< std::string >();
         }
         else if (value.is< int >())
         {
            str += std::to_string(value.as< int >());
         }
         else if (value.is< float >())
         {
            str += std::to_string(value.as< float >());
         }
         else if (value.is< double >())
         {
            str += std::to_string(value.as< double >());
         }
         else if (value.is< bool >())
         {
            str += value.as< bool >() ? "true" : "false";
         }

         str += ",\n";
      }
   }
   str += std::string(level * __indent_separator.size(), ' ') + "}\n";
   return str;
}

sol::object
BfConfigManager::getLuaObj(const std::string& key)
{
   return BfConfigManager::getInstance()->__lua[key];
}

BfLuaTable
BfConfigManager::convertLuaTable(sol::table* obj)
{
   BfLuaTable table;
   for (const auto& pair : *obj)
   {
      sol::object key = pair.first;
      sol::object value = pair.second;
      std::string key_str;

      if (key.is< std::string >())
      {
         key_str = key.as< std::string >();
      }
      else if (key.is< int >()) // для массивов
      {
         key_str = std::to_string(key.as< int >());
      }

      if (value.is< sol::table >())
      {
         sol::table t = value;
         table[key_str] = std::make_shared< BfLuaTable >(convertLuaTable(&t));
      }
      else
      {
         if (value.is< std::string >())
         {
            table[key_str] = value.as< std::string >();
         }
         else if (value.is< int >())
         {
            table[key_str] = std::to_string(value.as< int >());
         }
         else if (value.is< float >())
         {
            table[key_str] = std::to_string(value.as< float >());
         }
         else if (value.is< double >())
         {
            table[key_str] = std::to_string(value.as< double >());
         }
         else if (value.is< bool >())
         {
            table[key_str] = value.as< bool >() ? "true" : "false";
         }
      }
   }
   return table;
};

BfEvent
BfConfigManager::fillFormFont(sol::table obj, BfFormFont* form)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
   {
      if (!obj["name"].valid())
      {
         event.action = BF_ACTION_TYPE_FILL_FORM_FONT_NAME_INVALID_NAME_FAILURE;
         return event;
      }
      if (obj["name"].get_type() == sol::type::table)
      {
         sol::table font_names = obj.get< sol::table >("name");
         for (const auto& font_name : font_names)
         {
            form->name.push_back(font_name.second.as< std::string >());
         }
      }
      else if (obj["name"].get_type() == sol::type::string)
      {
         form->name.push_back(obj.get< std::string >("name"));
      }
      else if (std::filesystem::exists(
                   exePath() / "./resources/fonts/Cousine-Regular.ttf"
               ))
      {
         form->name.push_back("Cousine-Regular.ttf");
      }
      else
      {
         event.action = BF_ACTION_TYPE_FILL_FORM_FONT_NAME_INVALID_TYPE_FAILURE;
         return event;
      }
      form->current = obj.get_or("current", 0);
      form->size = obj.get< int >("size");
      form->glypth_offset = {
          obj.get< sol::table >("glypth_offset")[1],
          obj.get< sol::table >("glypth_offset")[2]
      };
      form->glypth_min_advance_x = obj.get_or("glypth_offset", 0.0f);
   }
   event.action = BF_ACTION_TYPE_FILL_FORM_SUCCESS;
   return event;
}

BfEvent
BfConfigManager::fillFormFontSettings(sol::table obj, BfFormFontSettings* form)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
   {
      sol::table font_paths = obj["fonts_paths"];

      for (const auto& font_path : font_paths)
      {
         form->font_directory_paths.push_back(
             exePath() / fs::path(font_path.second.as< std::string >())
         );
      }
      BfConfigManager::fillFormFont(obj["standart_font"], &form->standart_font);
      BfConfigManager::fillFormFont(obj["icon_font"], &form->icon_font);
      BfConfigManager::fillFormFont(obj["greek_font"], &form->greek_font);

      for (const auto& font_root_path : form->font_directory_paths)
      {
         std::vector< std::filesystem::path > file_paths;
         BfConfigManager::__findFilesInDir(font_root_path, "", file_paths);

         for (const auto& found_font_root_path : file_paths)
         {
            for (int i = 0; i < form->standart_font.name.size(); ++i)
            {
               if (found_font_root_path.filename() ==
                   form->standart_font.name[i])
               {
                  form->standart_font.name[i] = found_font_root_path.string();
               }
            }

            for (int i = 0; i < form->icon_font.name.size(); ++i)
            {
               if (found_font_root_path.filename() == form->icon_font.name[i])
               {
                  form->icon_font.name[i] = found_font_root_path.string();
               }
            }

            for (int i = 0; i < form->greek_font.name.size(); ++i)
            {
               if (found_font_root_path.filename() == form->greek_font.name[i])
               {
                  form->greek_font.name[i] = found_font_root_path.string();
               }
            }
         }
      }
   }
   event.action = BF_ACTION_TYPE_FILL_FORM_SUCCESS;
   return event;
}

std::filesystem::path
BfConfigManager::getConfigPath()
{
   std::filesystem::path configPath;

#ifdef _WIN32
   const char* appdata = std::getenv("APPDATA");
   if (appdata)
   {
      configPath = std::filesystem::path(appdata) / "bladeforge";
   }
   else
   {
      throw std::runtime_error("Failed to get APPDATA environment variable");
   }
#elif __APPLE__
   const char* home = std::getenv("HOME");
   if (home)
   {
      configPath = std::filesystem::path(home) / "Library" /
                   "Application Support" / "bladeforge";
   }
   else
   {
      throw std::runtime_error("Failed to get HOME environment variable");
   }
#else // Linux/Unix
   const char* home = std::getenv("HOME");
   if (home)
   {
      configPath = std::filesystem::path(home) / ".config" / "bladeforge";
   }
   else
   {
      throw std::runtime_error("Failed to get HOME environment variable");
   }
#endif

   return configPath;
}

std::filesystem::path
BfConfigManager::getSavePath()
{
   return BfConfigManager::getConfigPath() / "saves/";
}

BfEvent
BfConfigManager::createConfigData()
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   auto needed_path = BfConfigManager::getConfigPath();
   if (!std::filesystem::exists(needed_path))
   {
      std::filesystem::create_directory(needed_path);
      event.action = BF_ACTION_TYPE_CREATE_CONFIG_DIR_SUCCESS;
   }
   else
   {
      event.action = BF_ACTION_TYPE_CREATE_CONFIG_DIR_FAILURE;
   }
   event.info = " " + needed_path.string();

   return event;
}

BfEvent
BfConfigManager::createSavedFilesDir()
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   auto saveDir = BfConfigManager::getSavePath();
   if (!std::filesystem::exists(saveDir))
   {
      std::filesystem::create_directory(saveDir);
      event.action = BF_ACTION_TYPE_CREATE_CONFIG_SAVES_DIR_SUCCESS;
   }
   else
   {
      event.action = BF_ACTION_TYPE_CREATE_CONFIG_SAVES_DIR_FAILURE;
   }
   event.info = " " + saveDir.string();

   return event;
}
/*
      NOTE: Логичнее тут использовать виртуальный метод для каждого из
      дочерних классов, но раз уже было решено преобразовывать через
      std::to_string, то ладно
*/
std::string
BfConfigManager::createContainersSaveFileString(
    const std::list< ptrContainer >& cs
)
{
   std::stringstream ss;
   ss << "Containers = {\n";
   for (auto& it : cs)
   {
      // clang-format off
         if (auto casted = std::dynamic_pointer_cast<BfGuiCreateWindowBladeBase>(it))
         {
               ss << std::to_string(*casted) << ",\n";   
         }
         else if (auto casted = std::dynamic_pointer_cast<BfGuiCreateWindowBladeSection>(it)) 
         {
               ss << std::to_string(*casted) << ",\n";   
         }
         else if (auto casted = std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(it)) 
         {
               ss << std::to_string(*casted) << ",\n";   
         }
         else if (auto casted = std::dynamic_pointer_cast<BfGuiCreateWindowContainerPopup>(it)) 
         {
               // ss << std::to_string(*casted) << ",\n";   
               continue;
         }
         else if (auto casted = std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(it)) 
         {
               ss << std::to_string(*casted) << ",\n";   
         }
      // clang-format on
   }
   ss << "}";
   return ss.str();
}

BfEvent
BfConfigManager::saveContainers(
    const std::list< ptrContainer >& cs, fs::path path
)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   event.info = std::string(" file path '") + fs::absolute(path).string() + "'";

   std::ofstream s;
   s.open(path);
   if (!s.is_open())
   {
      event.action = BF_ACTION_TYPE_SAVE_CONTAINERS_FAILURE;
      event.success = false;
      return event;
   }
   else
   {
      event.action = BF_ACTION_TYPE_SAVE_CONTAINERS_SUCCESS;
      event.success = true;
   }

   // Stream the output directly to the file
   s << BfConfigManager::createContainersSaveFileString(cs);

   // Check for writing errors (optional)
   if (!s.good())
   {
      // std::cerr << "NOT GOOD\n";
   }

   s.close(); // Explicitly close the file (optional)
   return event;
}

BfEvent
BfConfigManager::loadContainers(std::list< ptrContainer >& cs, fs::path path)
{
   BfConfigManager::recreateInstance();
   BfConfigManager::loadStdLibrary(sol::lib::base);
   BfConfigManager::loadStdLibrary(sol::lib::package);
   BfConfigManager::addPackagePath(BfConfigManager::getSavePath());
   BfConfigManager::loadRequireScript(path);
   auto table = BfConfigManager::getLuaObj("Containers");

   for (auto elem : sol::table(table))
   {
      // std::cout << "Loading "
      //           << sol::table(elem.second).get<std::string>("type") << "\n";
      auto c = BfConfigManager::loadBfGuiCreateWindowContainerSmart(
          sol::table(elem.second)
      );
      cs.emplace_back(std::move(c));
   }
   return BfEvent();
}

// BfEve fs::path path
// {
//    BfConfigManag::crent BfConfigManager::loadContainers(
//        sol::table obj,
//        std::list<ptrContainer> & c
//    )
//    {
//       for (auto& it : obj)
//       {
//          if (!obj.is<sol::table>())
//          {
//             throw std::runtime_error("Input table cell is not lua-table");
//          }
//          auto tmp = std::make_shared<BfGuiCreateWindowContainerObj>(
//              std::weak_ptr<BfGuiCreateWindowContainer>()
//          );
//
//          BfConfigManager::loadBfGuiCreateWindowContainer(it.second, tmp);
//          c.push_back(tmp);
//
//          // //
//          // std::string type_str =
//          // sol::table(it.second).get<std::string>("type");
//          // //
//          // if (type_str == "BfGuiCreateWindowContainer")
//          // {
//          //    auto tmp = std::make_shared<BfGuiCreateWindowContainer>(
//          //        std::weak_ptr<BfGuiCreateWindowContainer>()
//          //    );
//          //    BfConfigManager::loadBfGuiCreateWindowContainer(it.second,
//          tmp);
//          //    c.push_back(tmp);
//          // }
//          // else if (type_str == "BfGuiCreateWindowContainerObj")
//          // {
//          //    auto tmp = std::make_shared<BfGuiCreateWindowContainerObj>(
//          //        std::weak_ptr<BfGuiCreateWindowContainer>()
//          //    );
//          //
//          //    BfConfigManager::loadBfGuiCreateWindowContainer(it.second,
//          tmp);
//          //    c.push_back(tmp);
//          // }
//          // else
//          // {
//          // }
//       }
//       return BfEvent();
//    }
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
