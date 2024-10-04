#include <bfConfigManager.h>
#include <bits/fs_dir.h>

#include <exception>
#include <filesystem>
#include <memory>
#include <string>

#include "bfEvent.h"

std::shared_ptr<BfConfigManager> BfConfigManager::__instance = nullptr;

void BfConfigManager::__findFilesInDir(std::filesystem::path               root,
                                       std::string                         ext,
                                       std::vector<std::filesystem::path>& out)
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

void BfConfigManager::createInstance()
{
   if (!__instance)
   {
      __instance = std::shared_ptr<BfConfigManager>(new BfConfigManager());
      // __instance->__lua.open_libraries(sol::lib::base,
      //                                  sol::lib::package,
      //                                  sol::lib::table);
   }
}

void BfConfigManager::destroyInstance()
{
   if (__instance)
   {
      __instance.reset();
   }
}

void BfConfigManager::recreateInstance()
{
   BFCM::destroyInstance();
   BFCM::createInstance();
}

BfConfigManager* BfConfigManager::getInstance() { return __instance.get(); }

BfEvent BfConfigManager::loadStdLibrary(sol::lib lib)
{
   static std::map<sol::lib, std::string> std_lib_names = {
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
      event.action  = BF_ACTION_TYPE_LOAD_STD_LUA_LIBRARY_FAILURE;
      event.success = false;
      event.info    = "underfined";
   }
   else
   {
      auto inst = getInstance();
      inst->__lua.open_libraries(lib);

      event.action  = BF_ACTION_TYPE_LOAD_STD_LUA_LIBRARY_SUCCESS;
      event.success = true;
      event.info    = std_lib_names[lib];
   }
   return event;
}

BfEvent BfConfigManager::addPackagePath(std::filesystem::path path)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   {
      std::filesystem::path tmp_path = std::filesystem::absolute(path);

      if (!std::filesystem::exists(tmp_path))
      {
         event.action  = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info    = " input path does not exist";
         return event;
      }

      if (!std::filesystem::is_directory(tmp_path))
      {
         event.action  = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info    = " input path is not directory path";
         return event;
      }

      // Adding "/" sign to the end of directory path, caz
      // lua needs such paths for directories
      if (!tmp_path.empty() && tmp_path.string().back() != '/')
      {
         tmp_path = tmp_path / "";
      }

      std::string command_msg =
          "package.path = package.path .. "
          "\";" +
          tmp_path.string() + "?.lua\"";

      BfConfigManager::getInstance()->__lua.script(command_msg);

      event.action = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_SUCCESS;
      event.info   = "path is " + tmp_path.string();
   }
   return event;
}

BfEvent BfConfigManager::loadScript(std::filesystem::path path)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
   try
   {
      std::filesystem::path tmp_path = std::filesystem::absolute(path);
      if (!std::filesystem::exists(tmp_path))
      {
         event.action  = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info    = " input path does not exist";
         return event;
      }
      if (!std::filesystem::is_regular_file(tmp_path))
      {
         event.action  = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info    = " input path is not file path";
         return event;
      }
      if (tmp_path.extension() != ".lua")
      {
         event.action  = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info    = " input file path extension is not .lua";
         return event;
      }

      BfConfigManager::getInstance()->__lua.script_file(path.string());

      event.action = BF_ACTION_TYPE_LOAD_LUA_SCRIPT_SUCCESS;
   }
   catch (std::exception& e)
   {
      event.action  = BF_ACTION_TYPE_LOAD_LUA_SCRIPT_FAILURE;
      event.success = false;
      event.info    = " external error -> " + std::string(e.what());
      return event;
   }
   return event;
}

BfEvent BfConfigManager::loadRequireScript(std::filesystem::path path,
                                           std::string           varname)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
   try
   {
      std::filesystem::path tmp_path = std::filesystem::absolute(path);
      if (!std::filesystem::exists(tmp_path))
      {
         event.action  = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info    = " input path does not exist";
         return event;
      }
      if (!std::filesystem::is_regular_file(tmp_path))
      {
         event.action  = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info    = " input path is not file path";
         return event;
      }
      if (tmp_path.extension() != ".lua")
      {
         event.action  = BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE;
         event.success = false;
         event.info    = " input file path extension is not .lua";
         return event;
      }

      if (varname.empty())
      {
         varname = tmp_path.stem().string();
      }

      BfConfigManager::getInstance()->__lua.script(
          varname + " = " + "require(\"" + tmp_path.stem().string() + "\")");

      event.action = BF_ACTION_TYPE_LOAD_LUA_SCRIPT_SUCCESS;
      event.info   = " lua file with path " + tmp_path.string() +
                   " was loaded as " + varname;
   }
   catch (std::exception& e)
   {
      event.action  = BF_ACTION_TYPE_LOAD_LUA_SCRIPT_FAILURE;
      event.success = false;
      event.info    = " external error -> " + std::string(e.what());
      return event;
   }
   return event;
}

std::string BfConfigManager::__indent_separator = "   ";

std::string BfConfigManager::getLuaTableStr(sol::table table, int level)
{
   std::string str = "{\n";
   for (const auto& pair : table)
   {
      sol::object key   = pair.first;
      sol::object value = pair.second;

      std::string indent(level * __indent_separator.size(), ' ');

      if (value.is<sol::table>())
      {
         str += indent;
         str += (key.is<int>() ? "" : key.as<std::string>() + " = ");
         str += getLuaTableStr(value, level + 1);
      }
      else
      {
         str += indent + (key.is<int>() ? "" : key.as<std::string>() + " = ");
         if (value.is<std::string>())
         {
            str += value.as<std::string>();
         }
         else if (value.is<int>())
         {
            str += std::to_string(value.as<int>());
         }
         else if (value.is<float>())
         {
            str += std::to_string(value.as<float>());
         }
         else if (value.is<double>())
         {
            str += std::to_string(value.as<double>());
         }
         else if (value.is<bool>())
         {
            str += value.as<bool>() ? "true" : "false";
         }

         str += ",\n";
      }
   }
   str += std::string(level * __indent_separator.size(), ' ') + "}\n";
   return str;
}

sol::object BfConfigManager::getLuaObj(const std::string& key)
{
   return BfConfigManager::getInstance()->__lua[key];
}

BfEvent BfConfigManager::fillFormFont(sol::table obj, BfFormFont* form)
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
         sol::table font_names = obj.get<sol::table>("name");
         for (const auto& font_name : font_names)
         {
            form->name.push_back(font_name.second.as<std::string>());
         }
      }
      else if (obj["name"].get_type() == sol::type::string)
      {
         form->name.push_back(obj.get<std::string>("name"));
      }
      else if (std::filesystem::exists("./resources/fonts/Cousine-Regular.ttf"))
      {
         form->name.push_back("Cousine-Regular.ttf");
      }
      else
      {
         event.action = BF_ACTION_TYPE_FILL_FORM_FONT_NAME_INVALID_TYPE_FAILURE;
         return event;
      }
      form->current              = obj.get_or("current", 0);
      form->size                 = obj.get<int>("size");
      form->glypth_offset        = {obj.get<sol::table>("glypth_offset")[1],
                                    obj.get<sol::table>("glypth_offset")[2]};
      form->glypth_min_advance_x = obj.get_or("glypth_offset", 0.0f);
   }
   event.action = BF_ACTION_TYPE_FILL_FORM_SUCCESS;
   return event;
}

BfEvent BfConfigManager::fillFormFontSettings(sol::table          obj,
                                              BfFormFontSettings* form)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
   {
      sol::table font_paths = obj["fonts_paths"];

      for (const auto& font_path : font_paths)
      {
         form->font_directory_paths.push_back(
             fs::path(font_path.second.as<std::string>()));
      }
      BfConfigManager::fillFormFont(obj["standart_font"], &form->standart_font);
      BfConfigManager::fillFormFont(obj["icon_font"], &form->icon_font);

      for (const auto& font_root_path : form->font_directory_paths)
      {
         std::vector<std::filesystem::path> file_paths;
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
         }
      }
   }
   event.action = BF_ACTION_TYPE_FILL_FORM_SUCCESS;
   return event;
}
