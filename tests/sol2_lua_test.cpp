#include <bfConfigManager.h>
#include <bfConsole.h>

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <sol/sol.hpp>

TEST_CASE("sol2->standart test", "[single-file]")
{
   struct vars
   {
      int boop = 0;
   };
   sol::state lua;
   lua.new_usertype<vars>("vars", "boop", &vars::boop);
   lua.script(
       "beep = vars.new()\n"
       "beep.boop = 1");

   REQUIRE(lua.get<vars>("beep").boop == 1);
   std::cout << "Lua-tests ended\n";
};

TEST_CASE("sol2->executing simple lua script", "[single-file]")
{
   sol::state lua;
   lua.script_file("./scripts/test.lua");
   sol::table settings = lua["settings"];

   REQUIRE(settings["option1"].get<std::string>() == "value1");
   REQUIRE(settings["option2"].get<int>() == 42);

   sol::table option3 = settings["option3"];
   REQUIRE(option3[1].get<int>() == 1);
   REQUIRE(option3[2].get<int>() == 2);
   REQUIRE(option3[3].get<int>() == 3);
}

TEST_CASE("sol2->executing multiple lua scripts in different tables",
          "[multi-file]")
{
   sol::state lua;
   lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table);
   lua.script(
       R"( package.path = package.path ..
   ";/home/snj/Code/BladeForge/build/scripts/?.lua")");
   lua.script("m = require(\"test0\")");
}

TEST_CASE("bfConfigManager -> lua init", "[single-file]")
{
   BfEventHandler::funcPtr    = &BfConsole::print_single_single_event;
   BfEventHandler::funcPtrStr = &BfConsole::print_single_single_event_to_string;

   BfConfigManager::createInstance();
   REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::base));
   REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::package));
   REQUIRE_NOTHROW(BfConfigManager::addPackagePath("./scripts"));
   REQUIRE_NOTHROW(BfConfigManager::loadRequireScript("./scripts/test0.lua"));
}
