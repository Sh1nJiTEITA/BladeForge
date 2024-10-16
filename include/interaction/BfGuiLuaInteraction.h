#ifndef BF_GUI_LUA_INTERACTION_H
#define BF_GUI_LUA_INTERACTION_H

#include <imgui.h>

#include <filesystem>
#include <format>
#include <fstream>
#include <list>
#include <memory>
#include <variant>

#include "bfConfigManager.h"
#include "bfDrawObject.h"
#include "bfEvent.h"
#include "bfGuiFileDialog.h"

namespace fs = std::filesystem;
using BfObj =
    std::variant<std::shared_ptr<BfDrawLayer>, std::shared_ptr<BfDrawObj>>;

class BfGuiLuaDragDropElement
{
   bool __is_border   = true;
   bool __is_name     = true;
   bool __is_type     = true;

   bool __is_dragging = false;

   std::string __name = "INVALID";

   BfObj  __obj;
   ImVec2 __pos;

public:
   BfGuiLuaDragDropElement(BfObj __obj);
   std::string type();
   std::string id();
   void        draw();
   ImVec2&     pos() noexcept;
   bool&       isDragging() noexcept;
};

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
   enum BfGuiLuaDragDropEvent_
   {
      BfGuiLuaDragDropEvent_Copy,
      BfGuiLuaDragDropEvent_Move,
      BfGuiLuaDragDropEvent_Swap
   };

   // ===========================================
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
   void __renderDrop();

   void __runSelectedScript();

public:
   void draw();
   void pollEvents();

   static BfEvent readFileText(fs::path path, std::string* text);
};

#endif
