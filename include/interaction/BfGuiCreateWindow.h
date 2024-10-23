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

#include "bfIconsFontAwesome6.h"

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
   //
   bool        __is_invisiable_buttons = true;
   bool        __is_first_render       = true;
   bool        __is_render             = true;
   bool        __is_dragging           = false;
   bool        __is_resizing           = false;
   static bool __is_resizing_hovered_h;
   static bool __is_resizing_hovered_v;

   ImVec2 __old_outter_pos;

   void __pushStyle();
   void __popStyle();
   void __clampPosition();

   //
   void __renderLeftResizeButton();
   void __renderRightResizeButton();
   void __renderBotResizeButton();
   void __renderTopResizeButton();
   bool __renderChildBorder();

public:
   using ptrContainer  = std::shared_ptr<BfGuiCreateWindowContainer>;
   using wptrContainer = std::weak_ptr<BfGuiCreateWindowContainer>;

protected:
   std::string   __str_id;
   ImVec2        __window_pos;
   ImVec2        __window_size = {150, 150};
   wptrContainer __root_container;

   bool __is_collapsed = false;
   //
   std::list<ptrContainer> __containers;

   virtual void __renderHeader();
   virtual void __renderClildContent();
   virtual void __prerender();

   void __updatePosition();
   void __updateResizeButtonSize();

public:
   BfGuiCreateWindowContainer(wptrContainer root);
   bool render();

   const char*    name() noexcept;
   ImVec2&        pos() noexcept;
   ImVec2&        size() noexcept;
   wptrContainer& root() noexcept;

   bool isEmpty() noexcept;
   bool isCollapsed() noexcept;
   //
   static void resetResizeHover();
   static void changeCursorStyle();

   std::list<ptrContainer>::iterator begin();
   std::list<ptrContainer>::iterator end();
   //
   std::list<ptrContainer>::reverse_iterator rbegin();
   std::list<ptrContainer>::reverse_iterator rend();

   void clearEmptyContainersByName(std::string);
};

void BfGetWindowsUnderMouse(std::vector<ImGuiWindow*>&);

//
//
//
//
//

class BfGuiCreateWindowContainerObj
    : public BfGuiCreateWindowContainer,
      public std::enable_shared_from_this<BfGuiCreateWindowContainerObj>
{
   static bool __is_moving_container;
   bool        __is_current_moving;
   ImVec2      __old_size;

protected:
   virtual void __pushButtonColorStyle();
   virtual void __popButtonColorStyle();
   virtual void __renderClildContent() override;
   virtual void __prerender() override;
   virtual void __renderHeader() override;

   virtual void __renderDragDropSource();
   virtual void __renderDragDropTarget();

public:
   BfGuiCreateWindowContainerObj(
       BfGuiCreateWindowContainer::wptrContainer root);

   static bool isAnyMoving() noexcept;
};

//
//
//
//
//

class BfGuiCreateWindow
{
   static BfGuiCreateWindow* __instance;
   //
   bool __is_render = true;

   std::list<BfGuiCreateWindowContainer::ptrContainer> __containers;

   void __renderManagePanel();
   void __renderContainers();

   void __renderDragDropZone();
   void __addBlankContainer();

public:
   BfGuiCreateWindow();
   static BfGuiCreateWindow* instance() noexcept;

   void removeByName(std::string);
   void render();
};

#endif
