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
   std::string __str_left_resize_button_id;
   std::string __str_right_resize_button_id;
   std::string __str_bot_resize_button_id;
   std::string __str_top_resize_button_id;

   std::string __str_child_border_id;

   ImVec2 __resize_button_size     = {10.0f, 10.0f};
   ImVec2 __bot_resize_button_size = {10.0f, 10.0f};

   bool __is_first_render          = true;
   bool __is_render                = true;
   bool __is_dragging              = false;
   bool __is_resizing              = false;
   bool __is_resizing_hovered_h    = false;
   bool __is_resizing_hovered_v    = false;

   ImVec2 __window_pos;
   ImVec2 __old_outter_pos;
   ImVec2 __window_size = {300, 300};

   void         __pushStyle();
   void         __popStyle();
   void         __clampPosition();
   void         __updatePosition();
   void         __updateResizeButtonSize();
   void         __changeCursorStyle();
   ImGuiWindow* __findCurrentWindowPointer();

   void __renderLeftResizeButton();
   void __renderRightResizeButton();
   void __renderBotResizeButton();
   void __renderTopResizeButton();
   void __renderChildBorder();

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
