#ifndef BF_CONFIG_MANAGER_H
#define BF_CONFIG_MANAGER_H

#include <bfEvent.h>
#include <config_forms/bfFormGui.h>

#include <filesystem>
#include <memory>
#include <sol/sol.hpp>

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
   static std::string                      __indent_separator;
   sol::state                              __lua;

public:
   static void __findFilesInDir(std::filesystem::path               root,
                                std::string                         ext,
                                std::vector<std::filesystem::path>& out);

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

   static std::string getLuaTableStr(sol::table table, int indent_level = 0);

   static sol::object getLuaObj(const std::string& key);

   static BfLuaTable convertLuaTable(sol::table* obj);

   static BfEvent fillFormFont(sol::table obj, BfFormFont* form);
   static BfEvent fillFormFontSettings(sol::table          obs,
                                       BfFormFontSettings* form);
};

#endif
