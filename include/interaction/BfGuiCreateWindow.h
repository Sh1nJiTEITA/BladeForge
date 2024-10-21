#ifndef BF_GUI_CREATE_WINDOW_H
#define BF_GUI_CREATE_WINDOW_H

#include <imgui.h>
#include <imgui_internal.h>

#include <cstdint>
#include <format>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <stack>
#include <vector>

class BfGuiCreateWindowContainer
{
   std::string __str_left_resize_button_id;
   std::string __str_right_resize_button_id;
   std::string __str_bot_resize_button_id;
   std::string __str_top_resize_button_id;
   std::string __str_child_border_id;

   ImVec2 __resize_button_size     = {10.0f, 10.0f};
   ImVec2 __bot_resize_button_size = {10.0f, 10.0f};
   //
   bool __is_invisiable_buttons = true;
   //
   bool        __is_first_render = true;
   bool        __is_render       = true;
   bool        __is_dragging     = false;
   bool        __is_resizing     = false;
   static bool __is_resizing_hovered_h;
   static bool __is_resizing_hovered_v;

   ImVec2 __old_outter_pos;

   void __pushStyle();
   void __popStyle();
   void __clampPosition();
   void __updatePosition();
   void __updateResizeButtonSize();
   //
   ImGuiWindow* __findCurrentWindowPointer();

   void __renderLeftResizeButton();
   void __renderRightResizeButton();
   void __renderBotResizeButton();
   void __renderTopResizeButton();
   bool __renderChildBorder();

public:
   using ptrContainer = std::shared_ptr<BfGuiCreateWindowContainer>;

protected:
   std::string __str_root_name;
   std::string __str_id;
   //
   std::list<ptrContainer> __containers;

   virtual void __renderClildContent();

public:
   ImVec2 __window_pos;
   ImVec2 __window_size = {150, 150};

   BfGuiCreateWindowContainer(std::string root_name);
   bool        render();
   const char* name();

   bool isEmpty();
   //
   static void resetResizeHover();
   static void changeCursorStyle();

   std::list<ptrContainer>::iterator begin();
   std::list<ptrContainer>::iterator end();
   //
   std::list<ptrContainer>::reverse_iterator rbegin();
   std::list<ptrContainer>::reverse_iterator rend();
};

void BfGetWindowsUnderMouse(std::vector<ImGuiWindow*>&);

//
//
//
//
//

class BfGuiCreateWindowContainerObj : public BfGuiCreateWindowContainer
{
protected:
   virtual void __renderClildContent() override;

public:
   BfGuiCreateWindowContainerObj(std::string root_name);
};

//
//
//
//
//

class BfGuiCreateWindow
{
   bool __is_render = true;

   std::list<std::shared_ptr<BfGuiCreateWindowContainer>> __containers;

   void __renderManagePanel();
   void __renderContainers();

   void __addBlankContainer();

public:
   BfGuiCreateWindow();

   void render();
};

#endif
