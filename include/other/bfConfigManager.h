#ifndef BF_CONFIG_MANAGER_H
#define BF_CONFIG_MANAGER_H

#include <bfEvent.h>

#include <filesystem>
#include <memory>
#include <sol/sol.hpp>

class BfConfigManager
{
   static std::shared_ptr<BfConfigManager> __instance;

   sol::state __lua;

   using BFCM = BfConfigManager;

public:
   BfConfigManager();
   BfConfigManager(const BfConfigManager&)            = delete;
   BfConfigManager& operator=(const BfConfigManager&) = delete;

   // Instance manipulation
   static void             createInstance();
   static void             destroyInstance();
   static void             recreateInstance();
   static BfConfigManager* getInstance();

   // Lua manipulation

   static BfEvent loadStdLibrary(sol::lib lib);
   static BfEvent addPackagePath(std::filesystem::path path);
   static BfEvent loadScript(std::filesystem::path path);
   static BfEvent loadRequireScript(std::filesystem::path path,
                                    std::string           varname = "");
};

#endif
