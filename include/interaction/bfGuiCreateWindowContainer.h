#ifndef BF_GUI_CREATE_WINDOW_CONTAINER_H
#define BF_GUI_CREATE_WINDOW_CONTAINER_H

#include <bfDrawObjectDefineType.h>
#include <bfGreekFont.h>
#include <bfIconsFontAwesome6.h>
#include <bfLayerHandler.h>
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
class BfGuiCreateWindowContainerPopup;

class BfGuiCreateWindowContainer
{
   std::string __str_left_resize_button_id;
   std::string __str_right_resize_button_id;
   std::string __str_bot_resize_button_id;
   std::string __str_top_resize_button_id;
   std::string __str_child_border_id;

   //
   bool __is_invisiable_buttons = true;
   bool __is_first_render = true;
   bool __is_render = true;
   bool __is_dragging = false;
   bool __is_resizing = false;

   static bool __is_resizing_hovered_h;
   static bool __is_resizing_hovered_v;
   //
   void __pushStyle();
   void __popStyle();
   //
   void __renderLeftResizeButton();
   void __renderRightResizeButton();
   void __renderBotResizeButton();
   void __renderTopResizeButton();
   bool __renderChildBorder();

public:
   using ptrContainer = std::shared_ptr<BfGuiCreateWindowContainer>;
   using wptrContainer = std::weak_ptr<BfGuiCreateWindowContainer>;
   using swapFuncType =
       std::function<void(const std::string&, const std::string&)>;
   using moveFuncType =
       std::function<void(const std::string&, const std::string&)>;

protected:
   uint32_t __id;

   bool __is_left_button = true;
   bool __is_right_button = true;
   bool __is_top_button = true;
   bool __is_bot_button = true;
   bool __is_force_render = false;
   bool __is_render_header = true;

   ImVec2 __resize_button_size = {10.0f, 10.0f};
   ImVec2 __bot_resize_button_size = {10.0f, 10.0f};

   std::string __str_id;
   ImVec2 __window_pos;
   ImVec2 __window_size = {150, 150};
   ImVec2 __old_outter_pos;
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

   virtual void __clampPosition();
   virtual void __renderHeader();
   virtual void __renderChildContent();
   virtual void __prerender();
   virtual void __postrender();
   void __updatePosition();
   void __updateResizeButtonSize();

public:
   enum BUTTON_TYPE
   {
      TOP,
      BOT,
      RIGHT,
      LEFT
   };

   BfGuiCreateWindowContainer(wptrContainer root);
   bool render();
   void renderBodyView();

   // GETTERS --------------------
   const char* name() noexcept;
   ImVec2& pos() noexcept;
   ImVec2& size() noexcept;
   wptrContainer& root() noexcept;

   ImVec2 popupSize();
   ImVec2 popupPos();
   uint32_t id() noexcept;

   void show();
   void hide();
   void toggleRender();

   void enableForceRender() noexcept;
   void disableForceRender() noexcept;
   void toggleForceRender() noexcept;

   void enableButton(BUTTON_TYPE button_id);
   void disableButton(BUTTON_TYPE button_id);
   void toggleButton(BUTTON_TYPE button_id);

   void hideHeader() noexcept;
   void showHeader() noexcept;
   void toggleHeader() noexcept;

   // ----------------------------
   bool isEmpty() noexcept;
   bool isCollapsed() noexcept;
   bool isForceRender() noexcept;
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
   //
   bool __is_current_moving;
   bool __is_drop_target;
   int __selected_layer = -1;
   ImVec2 __old_size;
   //
   BfDrawLayerCreateInfo __layer_create_info;
   std::shared_ptr<BfDrawLayer> __layer_obj;

   virtual void __pushButtonColorStyle();
   virtual void __popButtonColorStyle();
   virtual void __renderChildContent() override;
   virtual void __renderHeader() override;

   virtual void __renderHeaderName();
   virtual void __renderChangeName();
   virtual void __renderDragDropSource();
   virtual void __renderDragDropTarget();
   virtual void __renderInfoTooltip();

   virtual void __renderAvailableLayers();

   virtual void __processDragDropSource();
   virtual void __processDragDropTarget();

   virtual void __createObj();
   virtual void __addToLayer(std::shared_ptr<BfDrawLayer> add_to);

public:
   BfGuiCreateWindowContainerObj(
       BfGuiCreateWindowContainer::wptrContainer root, bool is_target = true
   );

   static bool isAnyMoving() noexcept;
};

void bfShowNestedLayersRecursive(std::shared_ptr<BfDrawLayer> l);
void bfShowNestedLayersRecursiveWithSelectables(
    std::shared_ptr<BfDrawLayer> l, int& selected_id
);

void bfShowNestedLayersRecursiveWithRadioButtons(
    std::shared_ptr<BfDrawLayer> l, int& selectedId
);

class BfGuiCreateWindowContainerPopup
    : public BfGuiCreateWindowContainer,
      public std::enable_shared_from_this<BfGuiCreateWindowContainerPopup>
{
public:
   /*
     TODO: Fix logic for top/bot window popups
   */
   enum SIDE
   {
      LEFT,
      RIGHT,
      TOP,
      BOT
   };

private:
   void __assignButtons();

protected:
   SIDE __side;

   virtual void __clampPosition() override;
   virtual void __renderChildContent() override;
   virtual void __renderHeader() override;

public:
   std::function<void()> __renderPopupContentFunc;

   BfGuiCreateWindowContainerPopup(
       BfGuiCreateWindowContainer::wptrContainer root,
       BfGuiCreateWindowContainerPopup::SIDE side,
       bool is_force_render,
       std::function<void()> popup_func
   );

   SIDE side() noexcept;
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
