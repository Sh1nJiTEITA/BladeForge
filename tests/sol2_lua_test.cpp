#include <bfConfigManager.h>
#include <bfConsole.h>

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <memory>
#include <sol/sol.hpp>

#include "bfGuiCreateWindowContainer.h"
#include "bfStringTables.h"

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
       "beep.boop = 1"
   );

   REQUIRE(lua.get<vars>("beep").boop == 1);
   // std::cout << "Lua-tests ended\n";
};

// TEST_CASE("sol2->executing simple lua script", "[single-file]")
// {
//    sol::state lua;
//    lua.script_file("./scripts/test.lua");
//    sol::table settings = lua["settings"];
//
//    REQUIRE(settings["option1"].get<std::string>() == "value1");
//    REQUIRE(settings["option2"].get<int>() == 42);
//
//    sol::table option3 = settings["option3"];
//    REQUIRE(option3[1].get<int>() == 1);
//    REQUIRE(option3[2].get<int>() == 2);
//    REQUIRE(option3[3].get<int>() == 3);
// }

TEST_CASE(
    "sol2->executing multiple lua scripts in different tables", "[multi-file]"
)
{
   sol::state lua;
   lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table);
   lua.script(
       R"( package.path = package.path ..
   ";/home/snj/Code/BladeForge/build/scripts/?.lua")"
   );
   lua.script("m = require(\"test0\")");
}

TEST_CASE("bfConfigManager -> lua init", "[single-file]")
{
   BfEventHandler::funcPtr = &BfConsole::print_single_single_event;
   BfEventHandler::funcPtrStr = &BfConsole::print_single_single_event_to_string;

   BfConfigManager::createInstance();
   REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::base));
   REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::package));
   REQUIRE_NOTHROW(BfConfigManager::addPackagePath("./scripts"));
   REQUIRE_NOTHROW(BfConfigManager::loadRequireScript("./scripts/test0.lua"));
   sol::table table = BfConfigManager::getLuaObj("test0");
   REQUIRE(table.get<std::string>("option1") == "value83939393");
   REQUIRE(table.get<int>("option2") == 39123123);
   REQUIRE(table.get<sol::table>("option3")[1] == 9);
   REQUIRE(table.get<sol::table>("option3")[2] == 10);
   REQUIRE(table.get<sol::table>("option3")[3] == 12);

   // std::cout << BfConfigManager::getLuaTableStr(table);
}

TEST_CASE("find files")
{
   // auto abs = std::filesystem::current_path();

   std::vector<std::filesystem::path> out;
   BfConfigManager::getInstance()
       ->__findFilesInDir("./resources/", ".png", out);
   // for (int i = 0; i < out.size(); ++i)
   // {
   //    std::cout << out[i].string() << "\n";
   // }
}

TEST_CASE("bfStringTablesGenPair")
{
   SECTION("int")
   {
      std::string n = "name";
      REQUIRE(bfStringTablesGenPair(n, 10) == "name = 10,\n");
      REQUIRE(bfStringTablesGenPair(n, 10, true) == "name = 10\n");
      REQUIRE(bfStringTablesGenPair(n, 10, true, true, 1) == "\tname = 10\n");
      REQUIRE(bfStringTablesGenPair(n, 10, true, true, 2) == "\t\tname = 10\n");

      REQUIRE(bfStringTablesGenPair(n, 10, false) == "name = 10,\n");
      REQUIRE(bfStringTablesGenPair(n, 10, false, true, 1) == "\tname = 10,\n");
      REQUIRE(
          bfStringTablesGenPair(n, 10, false, true, 2) == "\t\tname = 10,\n"
      );

      REQUIRE(bfStringTablesGenPair(n, 10, false, false) == "name = 10,");
      REQUIRE(bfStringTablesGenPair(n, 10, false, false, 1) == "\tname = 10,");
      REQUIRE(
          bfStringTablesGenPair(n, 10, false, false, 2) == "\t\tname = 10,"
      );
   }

   SECTION("float")
   {
      std::string n = "name";
      REQUIRE(bfStringTablesGenPair(n, 10.0005) == "name = 10.0005,\n");
      REQUIRE(bfStringTablesGenPair(n, 10.0005, true) == "name = 10.0005\n");
      REQUIRE(
          bfStringTablesGenPair(n, 10.0005, true, true, 1) ==
          "\tname = 10.0005\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, 10.0005, true, true, 2) ==
          "\t\tname = 10.0005\n"
      );

      REQUIRE(bfStringTablesGenPair(n, 10.0005, false) == "name = 10.0005,\n");
      REQUIRE(
          bfStringTablesGenPair(n, 10.0005, false, true, 1) ==
          "\tname = 10.0005,\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, 10.0005, false, true, 2) ==
          "\t\tname = 10.0005,\n"
      );

      REQUIRE(
          bfStringTablesGenPair(n, 10.0005, false, false) == "name = 10.0005,"
      );
      REQUIRE(
          bfStringTablesGenPair(n, 10.0005, false, false, 1) ==
          "\tname = 10.0005,"
      );
      REQUIRE(
          bfStringTablesGenPair(n, 10.0005, false, false, 2) ==
          "\t\tname = 10.0005,"
      );
   }

   // SECTION("std::string")
   // {
   //    std::string n = "name";
   //    REQUIRE(bfStringTablesGenPair(n, "apple") == "name = \"apple\"\n");
   //    REQUIRE(bfStringTablesGenPair(n, "apple", true) == "name =
   //    \"apple\",\n"); REQUIRE(
   //        bfStringTablesGenPair(n, "apple", true, true, 1) ==
   //        "\tname = \"apple\",\n"
   //    );
   //    REQUIRE(
   //        bfStringTablesGenPair(n, "apple", true, true, 2) ==
   //        "\t\tname = \"apple\",\n"
   //    );
   //
   //    REQUIRE(bfStringTablesGenPair(n, "apple", false) == "name =
   //    \"apple\"\n"); REQUIRE(
   //        bfStringTablesGenPair(n, "apple", false, true, 1) ==
   //        "\tname = \"apple\",\n"
   //    );
   //    REQUIRE(
   //        bfStringTablesGenPair(n, "apple", false, true, 2) ==
   //        "\t\tname = \"apple\",\n"
   //    );
   //
   //    REQUIRE(
   //        bfStringTablesGenPair(n, "apple", false, false) == "name =
   //        \"apple\","
   //    );
   //    REQUIRE(
   //        bfStringTablesGenPair(n, "apple", false, false, 1) ==
   //        "\tname = \"apple\","
   //    );
   //    REQUIRE(
   //        bfStringTablesGenPair(n, "apple", false, false, 2) ==
   //        "\t\tname = \"apple\","
   //    );
   // }

   SECTION("ImVec2-float")
   {
      std::string n = "name";
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1.0005, 1.0005)) ==
          "name = { 1.0005, 1.0005 },\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1.0005, 1.0005), true) ==
          "name = { 1.0005, 1.0005 }\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1.0005, 1.0005), true, true, 1) ==
          "\tname = { 1.0005, 1.0005 }\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1.0005, 1.0005), true, true, 2) ==
          "\t\tname = { 1.0005, 1.0005 }\n"
      );

      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1.0005, 1.0005), false) ==
          "name = { 1.0005, 1.0005 },\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1.0005, 1.0005), false, true, 1) ==
          "\tname = { 1.0005, 1.0005 },\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1.0005, 1.0005), false, true, 2) ==
          "\t\tname = { 1.0005, 1.0005 },\n"
      );

      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1.0005, 1.0005), false, false) ==
          "name = { 1.0005, 1.0005 },"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1.0005, 1.0005), false, false, 1) ==
          "\tname = { 1.0005, 1.0005 },"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1.0005, 1.0005), false, false, 2) ==
          "\t\tname = { 1.0005, 1.0005 },"
      );
   }

   SECTION("ImVec4-float")
   {
      std::string n = "name";
      REQUIRE(
          bfStringTablesGenPair(n, ImVec4(1.0005, 2.0005, 3.0005, 4.0005)) ==
          "name = { 1.0005, 2.0005, 3.0005, 4.0005 },\n"
      );
      REQUIRE(
          bfStringTablesGenPair(
              n,
              ImVec4(1.0005, 2.0005, 3.0005, 4.0005),
              true
          ) == "name = { 1.0005, 2.0005, 3.0005, 4.0005 }\n"
      );
      REQUIRE(
          bfStringTablesGenPair(
              n,
              ImVec4(1.0005, 2.0005, 3.0005, 4.0005),
              true,
              true,
              1
          ) == "\tname = { 1.0005, 2.0005, 3.0005, 4.0005 }\n"
      );
      REQUIRE(
          bfStringTablesGenPair(
              n,
              ImVec4(1.0005, 2.0005, 3.0005, 4.0005),
              true,
              true,
              2
          ) == "\t\tname = { 1.0005, 2.0005, 3.0005, 4.0005 }\n"
      );

      REQUIRE(
          bfStringTablesGenPair(
              n,
              ImVec4(1.0005, 2.0005, 3.0005, 4.0005),
              false
          ) == "name = { 1.0005, 2.0005, 3.0005, 4.0005 },\n"
      );
      REQUIRE(
          bfStringTablesGenPair(
              n,
              ImVec4(1.0005, 2.0005, 3.0005, 4.0005),
              false,
              true,
              1
          ) == "\tname = { 1.0005, 2.0005, 3.0005, 4.0005 },\n"
      );
      REQUIRE(
          bfStringTablesGenPair(
              n,
              ImVec4(1.0005, 2.0005, 3.0005, 4.0005),
              false,
              true,
              2
          ) == "\t\tname = { 1.0005, 2.0005, 3.0005, 4.0005 },\n"
      );

      REQUIRE(
          bfStringTablesGenPair(
              n,
              ImVec4(1.0005, 2.0005, 3.0005, 4.0005),
              false,
              false
          ) == "name = { 1.0005, 2.0005, 3.0005, 4.0005 },"
      );
      REQUIRE(
          bfStringTablesGenPair(
              n,
              ImVec4(1.0005, 2.0005, 3.0005, 4.0005),
              false,
              false,
              1
          ) == "\tname = { 1.0005, 2.0005, 3.0005, 4.0005 },"
      );
      REQUIRE(
          bfStringTablesGenPair(
              n,
              ImVec4(1.0005, 2.0005, 3.0005, 4.0005),
              false,
              false,
              2
          ) == "\t\tname = { 1.0005, 2.0005, 3.0005, 4.0005 },"
      );
   }

   SECTION("ImVec2-int")
   {
      std::string n = "name";
      REQUIRE(bfStringTablesGenPair(n, ImVec2(1, 2)) == "name = { 1, 2 },\n");
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1, 2), true) == "name = { 1, 2 }\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1, 2), true, true, 1) ==
          "\tname = { 1, 2 }\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1, 2), true, true, 2) ==
          "\t\tname = { 1, 2 }\n"
      );

      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1, 2), false) == "name = { 1, 2 },\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1, 2), false, true, 1) ==
          "\tname = { 1, 2 },\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1, 2), false, true, 2) ==
          "\t\tname = { 1, 2 },\n"
      );

      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1, 2), false, false) ==
          "name = { 1, 2 },"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1, 2), false, false, 1) ==
          "\tname = { 1, 2 },"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec2(1, 2), false, false, 2) ==
          "\t\tname = { 1, 2 },"
      );
   }

   SECTION("ImVec4-int")
   {
      std::string n = "name";
      REQUIRE(
          bfStringTablesGenPair(n, ImVec4(1, 2, 3, 4)) ==
          "name = { 1, 2, 3, 4 },\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec4(1, 2, 3, 4), true) ==
          "name = { 1, 2, 3, 4 }\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec4(1, 2, 3, 4), true, true, 1) ==
          "\tname = { 1, 2, 3, 4 }\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec4(1, 2, 3, 4), true, true, 2) ==
          "\t\tname = { 1, 2, 3, 4 }\n"
      );

      REQUIRE(
          bfStringTablesGenPair(n, ImVec4(1, 2, 3, 4), false) ==
          "name = { 1, 2, 3, 4 },\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec4(1, 2, 3, 4), false, true, 1) ==
          "\tname = { 1, 2, 3, 4 },\n"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec4(1, 2, 3, 4), false, true, 2) ==
          "\t\tname = { 1, 2, 3, 4 },\n"
      );

      REQUIRE(
          bfStringTablesGenPair(n, ImVec4(1, 2, 3, 4), false, false) ==
          "name = { 1, 2, 3, 4 },"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec4(1, 2, 3, 4), false, false, 1) ==
          "\tname = { 1, 2, 3, 4 },"
      );
      REQUIRE(
          bfStringTablesGenPair(n, ImVec4(1, 2, 3, 4), false, false, 2) ==
          "\t\tname = { 1, 2, 3, 4 },"
      );
   }

   SECTION("glm::vec2")
   {
      std::string n = "name";
      REQUIRE(
          bfStringTablesGenPair(n, glm::vec2(10, 20)) == "name = { 10, 20 },\n"
      );
   }
   SECTION("glm::vec3")
   {
      std::string n = "name";
      REQUIRE(
          bfStringTablesGenPair(n, glm::vec3(10, 20, 30)) ==
          "name = { 10, 20, 30 },\n"
      );
   }
   SECTION("glm::vec4")
   {
      std::string n = "name";
      REQUIRE(
          bfStringTablesGenPair(n, glm::vec4(10, 20, 30, 40)) ==
          "name = { 10, 20, 30, 40 },\n"
      );
   }
   SECTION("BfGuiCreateWindowContainer")
   {
      // std::string n = "name";
      //
      // auto w = std::make_shared<BfGuiCreateWindowContainer>(
      //     std::weak_ptr<BfGuiCreateWindowContainer>()
      // );

      // std::cout << std::to_string(*w);

      // auto obj_w = BfGuiCreateWindowContainerObj(

      // auto obj_w = std::make_shared<BfGuiCreateWindowContainerObj>(
      //     std::weak_ptr<BfGuiCreateWindowContainer>(w)
      // );
      auto ptr = std::make_shared<BfGuiCreateWindowContainerObj>(
          std::weak_ptr<BfGuiCreateWindowContainer>()
      );

      // std::cout << std::to_string(*ptr, 0);

      std::list<ptrContainer> v;
      for (int i = 0; i < 1; ++i)
      {
         v.push_back(std::make_shared<BfGuiCreateWindowContainerObj>(
             std::weak_ptr<BfGuiCreateWindowContainer>()
         ));
         (*v.rbegin())
             ->add(std::make_shared<BfGuiCreateWindowContainerObj>(
                 std::weak_ptr<BfGuiCreateWindowContainer>()
             ));
      }
      for (int i = 0; i < 1; ++i)
      {
         v.push_back(std::make_shared<BfGuiCreateWindowContainer>(
             std::weak_ptr<BfGuiCreateWindowContainer>()
         ));
      }

      // std::cout << bfStringTablesGenPair("name", v) << "\n";

      BfEventHandler::funcPtr = &BfConsole::print_single_single_event;
      BfEventHandler::funcPtrStr =
          &BfConsole::print_single_single_event_to_string;

      BfConfigManager::createInstance();
      REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::base));
      REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::package));
      REQUIRE_NOTHROW(BfConfigManager::addPackagePath("./scripts"));
      REQUIRE_NOTHROW(BfConfigManager::loadRequireScript("./scripts/test3.lua")
      );
      sol::table table = BfConfigManager::getLuaObj("test3");
      std::cout << BfConfigManager::getLuaTableStr(table) << "\n";
      // REQUIRE(table.get<std::string>("option1") == "value83939393");
      // REQUIRE(table.get<int>("option2") == 39123123);
      // REQUIRE(table.get<sol::table>("option3")[1] == 9);
      // REQUIRE(table.get<sol::table>("option3")[2] == 10);
      // REQUIRE(table.get<sol::table>("option3")[3] == 12);
   }
}
