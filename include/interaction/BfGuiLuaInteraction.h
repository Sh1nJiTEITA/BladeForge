#ifndef BF_GUI_LUA_INTERACTION_H
#define BF_GUI_LUA_INTERACTION_H

#include <imgui.h>

#include <filesystem>
#include <fstream>
#include <list>
#include <memory>

#include "bfConfigManager.h"
#include "bfEvent.h"
#include "bfGuiFileDialog.h"

namespace fs = std::filesystem;

struct BfGuiLuaScript
{
   fs::path    path;
   std::string text;
   sol::table  table;
   //
   std::shared_ptr<BfLuaTable> read_table;
};

class BfGuiLuaInteraction
{
   bool            __is_render         = true;
   bool            __is_file_exploring = false;
   BfGuiLuaScript* __selected_script   = nullptr;
   //
   std::list<BfGuiLuaScript> __scripts;

   fs::path __new_script;

   void __render();
   void __renderImportScriptPannel();
   void __renderList();
   void __renderFileText();
   void __renderLuaTable(std::shared_ptr<BfLuaTable>);

   void __runSelectedScript();

public:
   void draw();
   void pollEvents();

   static BfEvent readFileText(fs::path path, std::string* text);
};

#endif
