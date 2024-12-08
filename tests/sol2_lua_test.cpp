#include <type_traits>
#define BF_CONFIG_MANAGER_TESTING

#include <bfConfigManager.h>
#include <bfConsole.h>

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <memory>
#include <sol/sol.hpp>

#include "bfBladeSection.h"
#include "bfGuiCreateWindowBladeSection.h"
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

      std::list<ptrContainer> v;
      for (int i = 0; i < 3; ++i)
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
         (*v.rbegin())
             ->add(std::make_shared<BfGuiCreateWindowContainer>(
                 std::weak_ptr<BfGuiCreateWindowContainer>()
             ));
      }

      auto containers_str = std::to_string(v, 0);
      containers_str = "localdata = " + containers_str + "\n";

      BfEventHandler::funcPtr = &BfConsole::print_single_single_event;
      BfEventHandler::funcPtrStr =
          &BfConsole::print_single_single_event_to_string;

      BfConfigManager::createInstance();
      REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::base));
      REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::package));
      REQUIRE_NOTHROW(BfConfigManager::loadScriptStr(containers_str));

      sol::table table = BfConfigManager::getLuaObj("localdata");
      // std::cout << BfConfigManager::getLuaTableStr(table);
      // REQUIRE_NOTHROW(sol::table(table.get<sol::table>(0)));
      // REQUIRE(
      //     sol::table(table[0]).get<std::string>("type") ==
      //     "BfGuiCreateWindowContainer"
      // );

      // for (auto& it : table)
      // {
      //    auto tmp = std::make_shared<BfGuiCreateWindowContainerObj>(
      //        std::weak_ptr<BfGuiCreateWindowContainer>()
      //    );
      //    BfConfigManager::loadBfGuiCreateWindowContainer(it.second, tmp);
      //
      //    // std::cout << BfConfigManager::getLuaTableStr(it.second);
      // }
      std::list<ptrContainer> new_c;
      // REQUIRE_NOTHROW(BfConfigManager::loadContainers(table, new_c));
   }
}

TEST_CASE("CREATE-INFOS")
{
   SECTION("BLADE-SECTION")
   {
      BfBladeSectionCreateInfo info;
      REQUIRE_NOTHROW(bfFillBladeSectionStandart(&info));
      // std::cout << std::to_string(info) << "\n";
   }
   SECTION("BLADE-BASE")
   {
      BfBladeBaseCreateInfo info;
      // REQUIRE_NOTHROW(bfFillBladeBaseStandart(&info));
      for (int i = 0; i < 3; ++i)
      {
         BfBladeSectionCreateInfo __;
         REQUIRE_NOTHROW(bfFillBladeSectionStandart(&__));
         info.section_infos.push_back(std::move(__));
      }
      // std::cout << std::to_string(info) << "\n";
   }
}

void checkBaseContainer(
    std::shared_ptr<BfGuiCreateWindowContainerObj> l,
    std::shared_ptr<BfGuiCreateWindowContainerObj> r
);
void checkBaseContainer(
    std::shared_ptr<BfGuiCreateWindowBladeSection> l,
    std::shared_ptr<BfGuiCreateWindowBladeSection> r
);
void checkBaseContainer(
    std::shared_ptr<BfGuiCreateWindowBladeBase> l,
    std::shared_ptr<BfGuiCreateWindowBladeBase> r
);

void
checkBaseContainer(ptrContainer l, ptrContainer r)
{
   REQUIRE(l->__id == r->__id);
   REQUIRE(l->__str_id == r->__str_id);
   REQUIRE(l->__is_button == r->__is_button);
   REQUIRE(l->__is_force_render == r->__is_force_render);
   REQUIRE(l->__is_render_header == r->__is_render_header);
   REQUIRE(l->__is_collapsed == r->__is_collapsed);
   REQUIRE(l->__resize_button_size.x == r->__resize_button_size.x);
   REQUIRE(l->__resize_button_size.y == r->__resize_button_size.y);
   REQUIRE(l->__bot_resize_button_size.x == r->__bot_resize_button_size.x);
   REQUIRE(l->__bot_resize_button_size.y == r->__bot_resize_button_size.y);
   REQUIRE(l->__window_pos.x == r->__window_pos.x);
   REQUIRE(l->__window_pos.y == r->__window_pos.y);
   REQUIRE(l->__window_size.x == r->__window_size.x);
   REQUIRE(l->__window_size.y == r->__window_size.y);
   REQUIRE(l->__containers.size() == r->__containers.size());

   auto l_it = l->__containers.begin();
   auto r_it = r->__containers.begin();

#define BFCONFIG_TEST_DYNAMIC_CAST(T, l, r)     \
   do                                           \
   {                                            \
      auto a = std::dynamic_pointer_cast<T>(l); \
      auto b = std::dynamic_pointer_cast<T>(r); \
      if (a && b) checkBaseContainer(a, b);     \
   } while (0)

   for (int i = 0; i < l->__containers.size(); ++i, l_it++, r_it++)
   {
      BFCONFIG_TEST_DYNAMIC_CAST(BfGuiCreateWindowContainer, *l_it, *l_it);
      BFCONFIG_TEST_DYNAMIC_CAST(BfGuiCreateWindowContainerObj, *l_it, *r_it);
      BFCONFIG_TEST_DYNAMIC_CAST(BfGuiCreateWindowBladeSection, *l_it, *r_it);
      BFCONFIG_TEST_DYNAMIC_CAST(BfGuiCreateWindowBladeBase, *l_it, *r_it);
   }
}

void
checkBaseContainer(
    std::shared_ptr<BfGuiCreateWindowContainerObj> l,
    std::shared_ptr<BfGuiCreateWindowContainerObj> r
)
{
   REQUIRE(l->__selected_layer == r->__selected_layer);
   REQUIRE(l->__name == r->__name);
   REQUIRE(l->__old_size.x == r->__old_size.x);
   REQUIRE(l->__old_size.y == r->__old_size.y);
   REQUIRE(l->__header_button_size.x == r->__header_button_size.x);
   REQUIRE(l->__header_button_size.y == r->__header_button_size.y);

   checkBaseContainer(
       std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(l),
       std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(r)
   );
}

void
checkBaseContainer(
    std::shared_ptr<BfGuiCreateWindowBladeSection> l,
    std::shared_ptr<BfGuiCreateWindowBladeSection> r
)
{
   // clang-format off
   REQUIRE(l->__mode == r->__mode);
   REQUIRE(l->__create_info.width == r->__create_info.width);
   REQUIRE(l->__create_info.install_angle == r->__create_info.install_angle);
   REQUIRE(l->__create_info.inlet_angle == r->__create_info.inlet_angle);
   REQUIRE(l->__create_info.outlet_angle == r->__create_info.outlet_angle);
   REQUIRE(l->__create_info.inlet_surface_angle == r->__create_info.inlet_surface_angle);
   REQUIRE(l->__create_info.outlet_surface_angle == r->__create_info.outlet_surface_angle);
   REQUIRE(l->__create_info.inlet_radius == r->__create_info.inlet_radius);
   REQUIRE(l->__create_info.outlet_radius == r->__create_info.outlet_radius);
   REQUIRE(l->__create_info.border_length == r->__create_info.border_length);
   REQUIRE(l->__create_info.is_triangulate  == r->__create_info.is_triangulate);
   REQUIRE(l->__create_info.is_center == r->__create_info.is_center);
   REQUIRE(l->__create_info.start_vertex == r->__create_info.start_vertex);
   REQUIRE(l->__create_info.grow_direction == r->__create_info.grow_direction);
   REQUIRE(l->__create_info.up_direction == r->__create_info.up_direction);
   // clang-format on

   checkBaseContainer(
       std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(l),
       std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(r)
   );
}

void
checkBaseContainer(
    std::shared_ptr<BfGuiCreateWindowBladeBase> l,
    std::shared_ptr<BfGuiCreateWindowBladeBase> r
)
{
   checkBaseContainer(
       std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(l),
       std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(r)
   );
}

TEST_CASE("Blade-base-section-windows")
{
   SECTION("BLADE-SECTION")
   {
      auto ptr = std::make_shared<BfGuiCreateWindowBladeSection>(
          std::weak_ptr<BfGuiCreateWindowContainer>()
      );

      ptr->toggleHeader();
      std::string str_ptr = "Data = " + std::to_string(*ptr);
      std::cout << str_ptr << "\n";
      auto ptr_back = std::make_shared<BfGuiCreateWindowBladeSection>(
          std::weak_ptr<BfGuiCreateWindowContainer>()
          // *ptr
      );

      BfConfigManager::createInstance();
      REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::base));
      REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::package));
      REQUIRE_NOTHROW(BfConfigManager::loadScriptStr(str_ptr));
      sol::table table = BfConfigManager::getLuaObj("Data");
      //
      REQUIRE_NOTHROW(
          BfConfigManager::loadBfGuiCreateWindowContainer(table, ptr_back)
      );
      checkBaseContainer(
          std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(ptr),
          std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(ptr_back)
      );
      checkBaseContainer(
          std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(ptr),
          std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(ptr_back)
      );
      checkBaseContainer(ptr, ptr_back);
   }
   SECTION("BLADE-BASE")
   {
      SECTION("SINGLE")
      {
         auto ptr = std::make_shared<BfGuiCreateWindowBladeBase>(
             std::weak_ptr<BfGuiCreateWindowContainer>()
         );

         ptr->toggleHeader();
         std::string str_ptr = "Data = " + std::to_string(*ptr);
         std::cout << str_ptr << "\n";
         auto ptr_back = std::make_shared<BfGuiCreateWindowBladeBase>(
             std::weak_ptr<BfGuiCreateWindowContainer>()
             // *ptr
         );

         BfConfigManager::createInstance();
         REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::base));
         REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::package));
         REQUIRE_NOTHROW(BfConfigManager::loadScriptStr(str_ptr));
         sol::table table = BfConfigManager::getLuaObj("Data");
         //
         REQUIRE_NOTHROW(
             BfConfigManager::loadBfGuiCreateWindowContainer(table, ptr_back)
         );
         checkBaseContainer(
             std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(ptr),
             std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(ptr_back)
         );
         checkBaseContainer(
             std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(ptr),
             std::dynamic_pointer_cast<BfGuiCreateWindowContainerObj>(ptr_back)
         );
      }

      SECTION("MULTIPLE")
      {
         auto ptr = std::make_shared<BfGuiCreateWindowBladeBase>(
             std::weak_ptr<BfGuiCreateWindowContainer>()
         );

         for (int i = 0; i < 3; ++i)
         {
            ptr->add(std::make_shared<BfGuiCreateWindowBladeSection>(
                std::weak_ptr<BfGuiCreateWindowContainer>()
            ));
         }

         ptr->toggleHeader();
         std::string str_ptr = "Data = " + std::to_string(*ptr);
         std::cout << str_ptr << "\n";
         auto ptr_back = std::make_shared<BfGuiCreateWindowBladeBase>(
             std::weak_ptr<BfGuiCreateWindowContainer>()
         );

         BfConfigManager::createInstance();
         REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::base));
         REQUIRE_NOTHROW(BfConfigManager::loadStdLibrary(sol::lib::package));
         REQUIRE_NOTHROW(BfConfigManager::loadScriptStr(str_ptr));
         sol::table table = BfConfigManager::getLuaObj("Data");
         //
         REQUIRE_NOTHROW(
             BfConfigManager::loadBfGuiCreateWindowContainer(table, ptr_back)
         );
         // checkBaseContainer(
         //     std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(ptr),
         // std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(ptr_back)
         // );
         // checkBaseContainer(
         //     std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(ptr),
         //     std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(ptr_back)
         // );
      }

      // auto ptr = std::make_shared<BfGuiCreateWindowBladeBase>(
      //     std::weak_ptr<BfGuiCreateWindowContainer>()
      // );
      //
      // for (int i = 0; i < 3; ++i)
      // {
      //    ptr->add(std::make_shared<BfGuiCreateWindowBladeSection>(
      //        std::weak_ptr<BfGuiCreateWindowContainer>()
      //    ));
      //
      //    BfBladeSectionCreateInfo __;
      //    REQUIRE_NOTHROW(bfFillBladeSectionStandart(&__));
      //    // info.section_infos.push_back(std::move(__));
      // }
      // std::cout << std::to_string(*ptr) << "\n";
   }
}
