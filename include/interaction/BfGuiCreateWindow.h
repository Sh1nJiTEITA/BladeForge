#ifndef BF_GUI_CREATE_WINDOW_H
#define BF_GUI_CREATE_WINDOW_H

#include <imgui.h>
#include <imgui_internal.h>

#include <cstdint>
#include <format>
#include <iostream>
#include <list>

class BfGuiCreateWindowContainer
{
   std::string __str_id;
   bool        __is_render = true;
   ImVec2      __window_pos;
   ImVec2      __old_outter_pos;
   ImVec2      __window_size = {100, 200};

   void         __clampPosition();
   void         __updatePosition();
   ImGuiWindow* __findCurrentWindowPointer();

public:
   BfGuiCreateWindowContainer();

   void render();
};

class BfGuiCreateWindow
{
   bool __is_render = true;

   std::list<BfGuiCreateWindowContainer> __containers;

   void __renderManagePanel();
   void __renderContainers();

   void __addBlankContainer();

public:
   BfGuiCreateWindow();

   void render();
};

#endif
