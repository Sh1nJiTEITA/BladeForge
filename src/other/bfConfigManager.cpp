#include <bfConfigManager.h>

#include <exception>
#include <filesystem>
#include <memory>

#include "bfEvent.h"

std::shared_ptr<BfConfigManager> BfConfigManager::__instance = nullptr;

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

sol::object BfConfigManager::getLuaObj(const std::string& key)
{
   return BfConfigManager::getInstance()->__lua[key];
}
