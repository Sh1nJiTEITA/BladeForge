#ifndef BF_GUI_CREATE_WINDOW_CONTAINER_H
#define BF_GUI_CREATE_WINDOW_CONTAINER_H

#include <bfDrawObjectDefineType.h>
#include <bfGreekFont.h>
#include <bfIconsFontAwesome6.h>
#include <bfLayerHandler.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <functional>
#include <list>
#include <memory>
#include <queue>
#include <string>
#include <vector>

class BfGuiCreateWindow;
class BfGuiCreateWindowContainer;
class BfGuiCreateWindowContainerPopup;

#define BfGuiCreateWindowContainer_ButtonType_All \
   (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3)
#define BfGuiCreateWindowContainer_ButtonType_Left (1 << 0)
#define BfGuiCreateWindowContainer_ButtonType_Right (1 << 1)
#define BfGuiCreateWindowContainer_ButtonType_Top (1 << 2)
#define BfGuiCreateWindowContainer_ButtonType_Bot (1 << 3)

using swapFuncType =
    std::function<void(const std::string&, const std::string&)>;
using moveFuncType =
    std::function<void(const std::string&, const std::string&)>;
using ptrContainer = std::shared_ptr<BfGuiCreateWindowContainer>;
using wptrContainer = std::weak_ptr<BfGuiCreateWindowContainer>;

class BfGuiCreateWindowContainer
{
   std::string __str_child_border_id;

   bool __is_invisiable_buttons = true;
   bool __is_first_render = true;
   bool __is_render = true;
   bool __is_dragging = false;
   bool __is_resizing = false;

   static bool __is_resizing_hovered_h;
   static bool __is_resizing_hovered_v;

   void __renderResizeButton(int side);
   bool __renderChildBorder();
   void __updatePosition();
   void __updateResizeButtonSize();

public:
protected:
   uint32_t __id;
   std::string __str_id;

   uint8_t __is_button = BfGuiCreateWindowContainer_ButtonType_All;

   bool __is_force_render = false;
   bool __is_render_header = true;
   bool __is_collapsed = false;

   ImVec2 __resize_button_size = {10.0f, 10.0f};
   ImVec2 __bot_resize_button_size = {10.0f, 10.0f};

   ImVec2 __window_pos;
   ImVec2 __window_size = {150, 150};
   ImVec2 __old_outter_pos;

   std::list<ptrContainer> __containers;
   wptrContainer __root_container;

   static swapFuncType __swapFunc;
   static moveFuncType __moveFunc;

   virtual void __clampPosition();
   virtual void __renderHeader();
   virtual void __renderChildContent();
   virtual void __prerender();
   virtual void __postrender();

public:
   BfGuiCreateWindowContainer(wptrContainer root);
   bool render();
   void renderBodyView();

   // GETTERS --------------------
   const char* name() noexcept;
   ImVec2& pos() noexcept;
   ImVec2& size() noexcept;
   wptrContainer& root() noexcept;
   uint32_t id() noexcept;

#define BfGuiCreateWindowContainer_ToggleMode_Toggle -1
#define BfGuiCreateWindowContainer_ToggleMode_On true
#define BfGuiCreateWindowContainer_ToggleMode_Off false

   void toggleRender(
       int mode = BfGuiCreateWindowContainer_ToggleMode_Toggle
   ) noexcept;
   void toggleButton(
       int button_id, int mode = BfGuiCreateWindowContainer_ToggleMode_Toggle
   ) noexcept;
   void toggleHeader(
       int mode = BfGuiCreateWindowContainer_ToggleMode_Toggle
   ) noexcept;

   // ----------------------------
   bool isEmpty() noexcept;
   bool isCollapsed() noexcept;
   bool isForceRender() noexcept;

   static void bindSwapFunction(swapFuncType func) noexcept;
   static void bindMoveFunction(swapFuncType func) noexcept;
   static void resetResizeHover();
   static void changeCursorStyle();

   // ITER
   std::list<ptrContainer>::iterator begin();
   std::list<ptrContainer>::iterator end();
   std::list<ptrContainer>::reverse_iterator rbegin();
   std::list<ptrContainer>::reverse_iterator rend();

   void add(ptrContainer container);
   void rem(ptrContainer container);
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
   bool __is_current_moving;
   bool __is_drop_target;
   int __selected_layer = -1;

protected:
   // For changing initial size (mb)
   ImVec2 __old_size;
   std::string __name;
   ImVec2 __header_button_size = {20.0f, 20.0f};
   //
   BfDrawLayerCreateInfo __layer_create_info;
   std::shared_ptr<BfDrawLayer> __layer_obj;
   std::weak_ptr<BfDrawLayer> __ptr_root;

   virtual void __pushButtonColorStyle();
   virtual void __popButtonColorStyle();

   virtual void __renderChildContent() override;

   virtual void __renderHeader() override;
   virtual void __renderHeaderName();
   virtual void __renderChangeName();

   virtual void __renderInfoTooltip();

   virtual void __renderDragDropSource();
   virtual void __renderDragDropTarget();

   virtual void __processDragDropSource();
   virtual void __processDragDropTarget();

   virtual void __createObj();
   void __addToLayer(std::shared_ptr<BfDrawLayer> add_to);
   ImVec2 __totalHeaderButtonsSize();

public:
   BfGuiCreateWindowContainerObj(wptrContainer root, bool is_target = true);

   static bool isAnyMoving() noexcept;
};

//
//
//
//
//

#define BfGuiCreateWindowContainerPopup_Side_Left 0
#define BfGuiCreateWindowContainerPopup_Side_Right 1
#define BfGuiCreateWindowContainerPopup_Side_Top 2
#define BfGuiCreateWindowContainerPopup_Side_Bot 3

class BfGuiCreateWindowContainerPopup
    : public BfGuiCreateWindowContainer,
      public std::enable_shared_from_this<BfGuiCreateWindowContainerPopup>
{
   void __assignButtons();

protected:
   uint8_t __side;

   virtual void __clampPosition() override;
   virtual void __renderChildContent() override;
   virtual void __renderHeader() override;

public:
   std::function<void(wptrContainer root)> __renderPopupContentFunc;

   BfGuiCreateWindowContainerPopup(
       wptrContainer root,
       uint8_t side,
       bool is_force_render,
       std::function<void(wptrContainer)> popup_func
   );

   uint8_t side() noexcept;
   void setSize(ImVec2);
};

//
//
//
//
//

//
//
//
//
//

#endif
