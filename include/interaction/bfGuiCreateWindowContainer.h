#ifndef BF_GUI_CREATE_WINDOW_CONTAINER_H
#define BF_GUI_CREATE_WINDOW_CONTAINER_H

#include <bfBladeSection.h>
#include <bfIconsFontAwesome6.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <format>
#include <functional>
#include <list>
#include <memory>
#include <queue>
#include <string>
#include <vector>

class BfGuiCreateWindow;

class BfGuiCreateWindowContainer
{
   std::string __str_left_resize_button_id;
   std::string __str_right_resize_button_id;
   std::string __str_bot_resize_button_id;
   std::string __str_top_resize_button_id;
   std::string __str_child_border_id;
   //
   ImVec2 __resize_button_size     = {10.0f, 10.0f};
   ImVec2 __bot_resize_button_size = {10.0f, 10.0f};
   ImVec2 __old_outter_pos;
   //
   bool        __is_invisiable_buttons = true;
   bool        __is_first_render       = true;
   bool        __is_render             = true;
   bool        __is_dragging           = false;
   bool        __is_resizing           = false;
   static bool __is_resizing_hovered_h;
   static bool __is_resizing_hovered_v;
   //
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
   using swapFuncType =
       std::function<void(const std::string&, const std::string&)>;
   using moveFuncType =
       std::function<void(const std::string&, const std::string&)>;

protected:
   std::string   __str_id;
   ImVec2        __window_pos;
   ImVec2        __window_size = {150, 150};
   wptrContainer __root_container;
   //
   static swapFuncType __swapFunc;
   static swapFuncType __moveFunc;
   //
   bool __is_collapsed = false;
   //
   //
   //
   std::list<ptrContainer> __containers;

   virtual void __renderHeader();
   virtual void __renderChildContent();
   void         __updatePosition();
   void         __updateResizeButtonSize();

public:
   BfGuiCreateWindowContainer(wptrContainer root);
   bool render();

   // GETTERS --------------------
   const char*    name() noexcept;
   ImVec2&        pos() noexcept;
   ImVec2&        size() noexcept;
   wptrContainer& root() noexcept;
   // ----------------------------
   bool isEmpty() noexcept;
   bool isCollapsed() noexcept;
   //

   static void bindSwapFunction(swapFuncType func) noexcept;
   static void bindMoveFunction(swapFuncType func) noexcept;

   static void resetResizeHover();
   static void changeCursorStyle();

   // ITER
   std::list<ptrContainer>::iterator begin();
   std::list<ptrContainer>::iterator end();
   //
   std::list<ptrContainer>::reverse_iterator rbegin();
   std::list<ptrContainer>::reverse_iterator rend();

   void clearEmptyContainersByName(std::string);
   void add(ptrContainer container);
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
protected:
   static bool __is_moving_container;
   bool        __is_current_moving;
   bool        __is_drop_target;
   ImVec2      __old_size;

   virtual void __pushButtonColorStyle();
   virtual void __popButtonColorStyle();
   virtual void __renderChildContent() override;
   virtual void __renderHeader() override;

   virtual void __renderHeaderName();
   virtual void __renderChangeName();
   virtual void __renderDragDropSource();
   virtual void __renderDragDropTarget();

   virtual void __processDragDropSource();
   virtual void __processDragDropTarget();

public:
   BfGuiCreateWindowContainerObj(BfGuiCreateWindowContainer::wptrContainer root,
                                 bool is_target = true);

   static bool isAnyMoving() noexcept;
};

//
//
//
//
//

class BfGuiCreateWindowBladeSection : public BfGuiCreateWindowContainerObj
{
   BfBladeSectionCreateInfo __create_info;

   std::string __section_name;
   bool        __is_settings = false;

   /*
      Добавить какой-то статический архив для сваппа!
      После определенных действий (конца ренедра) вызывать свап!
      Важно, правильно найти, где находятся контейнеры и свапнуть.
   */

protected:
   void __renderSettingsWindow();
   void __renderSettings();

   virtual void __renderHeaderName() override;
   virtual void __renderChildContent() override;
   virtual void __processDragDropTarget() override;

public:
   BfGuiCreateWindowBladeSection(BfGuiCreateWindowContainer::wptrContainer root,
                                 bool is_target = true);
};

//
//
//
//
//

class BfGuiCreateWindowBladeBase : public BfGuiCreateWindowContainerObj
{
   std::string __base_name;

   void __setContainersPos();

protected:
   virtual void __renderHeaderName() override;
   virtual void __renderChildContent() override;
   virtual void __processDragDropTarget() override;

public:
   BfGuiCreateWindowBladeBase(BfGuiCreateWindowContainer::wptrContainer root,
                              bool is_target = true);
};

#endif
