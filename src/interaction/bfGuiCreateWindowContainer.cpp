#include "bfGuiCreateWindowContainer.h"

#include <memory>

#include "bfCurves2.h"
#include "bfLayerHandler.h"
#include "bfVariative.hpp"
#include "imgui.h"

bool BfGuiCreateWindowContainer::__is_resizing_hovered_h = false;
bool BfGuiCreateWindowContainer::__is_resizing_hovered_v = false;

void
BfGuiCreateWindowContainer::__pushStyle()
{
   ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 2.0f);
   ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 2.0f);
}
void
BfGuiCreateWindowContainer::__popStyle()
{
   ImGui::PopStyleVar(2);
}

void
BfGuiCreateWindowContainer::__clampPosition()
{
   ImVec2 outter_pos;
   ImVec2 outter_size;
   if (auto shared_root = __root_container.lock())
   {
      outter_pos = ImGui::FindWindowByName(shared_root->name())->Pos;
      outter_size = ImGui::FindWindowByName(shared_root->name())->Size;
   }
   else
   {
      outter_pos = ImGui::FindWindowByName("Create")->Pos;
      outter_size = ImGui::FindWindowByName("Create")->Size;
   }

   ImVec2 delta_pos{
       outter_pos.x - __old_outter_pos.x,
       outter_pos.y - __old_outter_pos.y
   };

   // ImVec2 new_pos   = {std::clamp(popupPos().x + delta_pos.x,
   //                              outter_pos.x,
   //                              outter_pos.x + outter_size.x -
   //                              popupSize().x),
   //                     std::clamp(popupPos().y + delta_pos.y,
   //                              outter_pos.y,
   //                              outter_pos.y + outter_size.y -
   //                              popupSize().y)};
   //
   //
   ImVec2 new_pos = {
       std::clamp(
           pos().x + delta_pos.x,
           outter_pos.x,
           outter_pos.x + outter_size.x - size().x
       ),
       std::clamp(
           popupPos().y + delta_pos.y,
           outter_pos.y,
           outter_pos.y + outter_size.y - size().y
       )
   };
   __old_outter_pos = outter_pos;

   ImGui::SetNextWindowPos(new_pos);
   __window_pos = new_pos;
}

void
BfGuiCreateWindowContainer::__updatePosition()
{
   if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
   {
      __is_dragging = true;
   }
   if (ImGui::IsMouseReleased(0))
   {
      __is_dragging = false;
   }
   if (__is_dragging && !__is_resizing)
   {
      __window_pos.x += ImGui::GetIO().MouseDelta.x;
      __window_pos.y += ImGui::GetIO().MouseDelta.y;
   }
}

void
BfGuiCreateWindowContainer::__updateResizeButtonSize()
{
   __resize_button_size.y = __window_size.y -
                            ImGui::GetStyle().WindowPadding.y * 2.0f -
                            __bot_resize_button_size.y * 2.0f -
                            ImGui::GetStyle().ItemSpacing.y * 2.0f;

   __bot_resize_button_size.x =
       __window_size.x - ImGui::GetStyle().WindowPadding.x * 2.0f -
       __resize_button_size.x * 2.0f - ImGui::GetStyle().ItemSpacing.x * 2.0f;
}

void
BfGuiCreateWindowContainer::changeCursorStyle()
{
   if (__is_resizing_hovered_v || __is_resizing_hovered_h)
   {
      if (__is_resizing_hovered_v)
         ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);

      if (__is_resizing_hovered_h)
         ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

      return;
   }

   ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
}

void
BfGuiCreateWindowContainer::__renderLeftResizeButton()
{
   bool buttonClicked = false;

   if (!__is_left_button)
   {
      ImGui::Dummy(__resize_button_size);
   }
   else
   {
      if (__is_invisiable_buttons)
      {
         buttonClicked = ImGui::InvisibleButton(
             __str_left_resize_button_id.c_str(),
             __resize_button_size
         );
      }
      else
      {
         buttonClicked = ImGui::Button(
             __str_left_resize_button_id.c_str(),
             __resize_button_size
         );
      }
   }
   if (buttonClicked)
   {
   }

   if (ImGui::IsItemHovered())
   {
      __is_resizing_hovered_h = true;
   }

   if (ImGui::IsItemActive())
   {
      __is_resizing = true;
      __window_size.x -= ImGui::GetIO().MouseDelta.x;
      __window_pos.x += ImGui::GetIO().MouseDelta.x;

      ImGui::SetWindowSize(__window_size);
   }
   else
   {
      __is_resizing = false;
   }
}

void
BfGuiCreateWindowContainer::__renderRightResizeButton()
{
   bool buttonClicked = false;

   if (!__is_right_button)
   {
      ImGui::Dummy(__resize_button_size);
   }
   else
   {
      if (__is_invisiable_buttons)
      {
         buttonClicked = ImGui::InvisibleButton(
             __str_right_resize_button_id.c_str(),
             __resize_button_size
         );
      }
      else
      {
         buttonClicked = ImGui::Button(
             __str_right_resize_button_id.c_str(),
             __resize_button_size
         );
      }
   }
   if (buttonClicked)
   {
   }

   if (ImGui::IsItemHovered())
   {
      __is_resizing_hovered_h = true;
   }

   if (ImGui::IsItemActive())
   {
      __is_resizing = true;
      __window_size.x += ImGui::GetIO().MouseDelta.x;

      ImGui::SetWindowSize(__window_size);
   }
   else
   {
      __is_resizing = false;
   }
}

void
BfGuiCreateWindowContainer::__renderBotResizeButton()
{
   bool buttonClicked = false;

   if (!__is_bot_button)
   {
      ImGui::Dummy(__resize_button_size);
   }
   else
   {
      if (__is_invisiable_buttons)
      {
         buttonClicked = ImGui::InvisibleButton(
             __str_bot_resize_button_id.c_str(),
             __bot_resize_button_size
         );
      }
      else
      {
         buttonClicked = ImGui::Button(
             __str_bot_resize_button_id.c_str(),
             __bot_resize_button_size
         );
      }
   }
   if (buttonClicked)
   {
   }

   if (__is_collapsed) return;

   if (ImGui::IsItemHovered())
   {
      __is_resizing_hovered_v = true;
   }

   if (ImGui::IsItemActive())
   {
      __is_resizing = true;
      __window_size.y += ImGui::GetIO().MouseDelta.y;

      ImGui::SetWindowSize(__window_size);
   }
   else
   {
      __is_resizing = false;
   }
}

void
BfGuiCreateWindowContainer::__renderTopResizeButton()
{
   bool buttonClicked = false;

   if (!__is_top_button)
   {
      ImGui::Dummy(__bot_resize_button_size);
   }
   else
   {
      if (__is_invisiable_buttons)
      {
         buttonClicked = ImGui::InvisibleButton(
             __str_top_resize_button_id.c_str(),
             __bot_resize_button_size
         );
      }
      else
      {
         buttonClicked = ImGui::Button(
             __str_top_resize_button_id.c_str(),
             __bot_resize_button_size
         );
      }
   }

   if (buttonClicked)
   {
   }

   if (__is_collapsed) return;

   if (ImGui::IsItemHovered())
   {
      __is_resizing_hovered_v = true;
   }

   if (ImGui::IsItemHovered())
   {
      __is_resizing_hovered_v = true;
   }
   if (ImGui::IsItemActive())
   {
      __is_resizing = true;
      __window_pos.y += ImGui::GetIO().MouseDelta.y;
      __window_size.y -= ImGui::GetIO().MouseDelta.y;

      ImGui::SetWindowSize(__window_size);
   }
   else
   {
      __is_resizing = false;
   }
}

bool
BfGuiCreateWindowContainer::__renderChildBorder()
{
   bool is_hovered;
   ImGui::BeginChild(
       __str_child_border_id.c_str(),
       {
           __window_size.x - ImGui::GetStyle().WindowPadding.x * 2.0f -
               __resize_button_size.x * 2.0f -
               ImGui::GetStyle().ItemSpacing.x * 2.0f,

           __window_size.y - ImGui::GetStyle().WindowPadding.y * 2.0f -
               __bot_resize_button_size.y * 2.0f -
               ImGui::GetStyle().ItemSpacing.y * 2.0f,
       },
       true
   );
   {
      __updateResizeButtonSize();
      __renderHeader();
      if (!__is_collapsed)
      {
         ImGui::Separator();
         __renderChildContent();
      }
      __updatePosition();
      is_hovered = ImGui::IsWindowHovered();
   }
   ImGui::EndChild();
   __updateResizeButtonSize();
   return is_hovered;
}

std::function<void(const std::string&, const std::string&)>
    BfGuiCreateWindowContainer::__swapFunc =
        [](const auto& a, const auto& b) {};

std::function<void(const std::string&, const std::string&)>
    BfGuiCreateWindowContainer::__moveFunc =
        [](const auto& a, const auto& b) {};

void
BfGuiCreateWindowContainer::__renderHeader()
{
}
void
BfGuiCreateWindowContainer::__renderChildContent()
{
}
void
BfGuiCreateWindowContainer::__prerender()
{
}
void
BfGuiCreateWindowContainer::__postrender()
{
}

BfGuiCreateWindowContainer::BfGuiCreateWindowContainer(wptrContainer root)
    : __root_container{root}
{
   static uint32_t growing_id = 0;
   //
   __str_id = std::format("##CreateWindow_{}", std::to_string(growing_id));
   __str_left_resize_button_id =
       std::format("##CreateWindowLeftResize_{}", std::to_string(growing_id));
   __str_right_resize_button_id =
       std::format("##CreateWindowRightResize_{}", std::to_string(growing_id));
   __str_bot_resize_button_id =
       std::format("##CreateWindowBotResize_{}", std::to_string(growing_id));
   __str_top_resize_button_id =
       std::format("##CreateWindowTopResize_{}", std::to_string(growing_id));
   __str_child_border_id =
       std::format("##CreateWindowChildBorder_{}", std::to_string(growing_id));

   growing_id++;
}

bool
BfGuiCreateWindowContainer::render()
{
   bool is_window_hovered = false;
   if (__is_render)
   {
      // std::cout << __window_size.x << " " << __window_size.y << "\n";
      __clampPosition();

      if (__is_first_render)
      {
         __is_first_render = false;
         ImGui::SetNextWindowSize(__window_size);

         ImVec2 root_size;
         ImVec2 root_pos;
         if (auto shared_root = __root_container.lock())
         {
            root_size = shared_root->size();
            root_pos = shared_root->pos();
         }
         else
         {
            ImGuiWindow* root = ImGui::FindWindowByName("Create");
            root_size = root->Size;
            root_pos = root->Pos;
         }

         root_pos.x += root_size.x * 0.5f;
         root_pos.y += root_size.y * 0.5f;
         ImGui::SetNextWindowPos(root_pos);
         __window_pos = root_pos;
      }
      // int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
      int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground |
                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse |
                  ImGuiWindowFlags_NoScrollbar |
                  ImGuiWindowFlags_AlwaysAutoResize;
      __prerender();
      if (ImGui::Begin(__str_id.c_str(), &__is_render, flags))
      {
         {
            __pushStyle();
            {
               ImGui::SetCursorPosX(
                   ImGui::GetCursorPosX() + __resize_button_size.x +
                   ImGui::GetStyle().ItemSpacing.x
               );
               __renderTopResizeButton();
               __renderLeftResizeButton();
               ImGui::SameLine();
            }
            __popStyle();
            is_window_hovered = __renderChildBorder();
            __pushStyle();
            {
               ImGui::SameLine();
               __renderRightResizeButton();

               ImGui::SetCursorPosX(
                   ImGui::GetCursorPosX() + __resize_button_size.x +
                   ImGui::GetStyle().ItemSpacing.x
               );

               __renderBotResizeButton();
            }
            __popStyle();
         }
      }
      ImGui::End();
      __postrender();
   }
   return is_window_hovered;
}

const char*
BfGuiCreateWindowContainer::name() noexcept
{
   return __str_id.c_str();
}
ImVec2&
BfGuiCreateWindowContainer::pos() noexcept
{
   return __window_pos;
}
ImVec2&
BfGuiCreateWindowContainer::size() noexcept
{
   return __window_size;
}

BfGuiCreateWindowContainer::wptrContainer&
BfGuiCreateWindowContainer::root() noexcept
{
   return __root_container;
}

ImVec2
BfGuiCreateWindowContainer::popupSize()
{
   ImVec2 outsize = this->size();
   float max_x = 0;
   float max_y = 0;
   for (auto c : __containers)
   {
      if (auto potential_popup =
              std::dynamic_pointer_cast<BfGuiCreateWindowContainerPopup>(c))
      {
         switch (potential_popup->side())
         {
            case BfGuiCreateWindowContainerPopup::LEFT:
               max_x = std::max(max_x, potential_popup->size().x);
               break;
            case BfGuiCreateWindowContainerPopup::RIGHT:
               max_x = std::max(max_x, potential_popup->size().x);
               break;
            case BfGuiCreateWindowContainerPopup::TOP:
               max_y = std::max(max_y, potential_popup->size().y);
               break;
            case BfGuiCreateWindowContainerPopup::BOT:
               max_y = std::max(max_y, potential_popup->size().y);
               break;
         }
      }
   }
   outsize.x += max_x;
   outsize.y += max_y;
   return outsize;
}

ImVec2
BfGuiCreateWindowContainer::popupPos()
{
   ImVec2 outpos = pos();
   for (auto c : __containers)
   {
      if (auto potential_popup =
              std::dynamic_pointer_cast<BfGuiCreateWindowContainerPopup>(c))
      {
         if (potential_popup->side() == BfGuiCreateWindowContainerPopup::TOP)
         {
            outpos.y += c->size().y;
         }
      }
   }
   return outpos;
}

void
BfGuiCreateWindowContainer::show()
{
   __is_render = true;
}
void
BfGuiCreateWindowContainer::hide()
{
   __is_render = false;
}
void
BfGuiCreateWindowContainer::toggleRender()
{
   __is_render = !__is_render;
}

bool
BfGuiCreateWindowContainer::isEmpty() noexcept
{
   return __containers.empty();
}

bool
BfGuiCreateWindowContainer::isCollapsed() noexcept
{
   return __is_collapsed;
}

bool
BfGuiCreateWindowContainerObj::isAnyMoving() noexcept
{
   return BfGuiCreateWindowContainerObj::__is_moving_container;
}

void
BfGuiCreateWindowContainer::resetResizeHover()
{
   __is_resizing_hovered_h = false;
   __is_resizing_hovered_v = false;

   ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
}

void
BfGuiCreateWindowContainer::bindSwapFunction(swapFuncType func) noexcept
{
   __swapFunc = func;
}

void
BfGuiCreateWindowContainer::bindMoveFunction(swapFuncType func) noexcept
{
   __moveFunc = func;
}

std::list<BfGuiCreateWindowContainer::ptrContainer>::iterator
BfGuiCreateWindowContainer::begin()
{
   return __containers.begin();
}
std::list<BfGuiCreateWindowContainer::ptrContainer>::iterator
BfGuiCreateWindowContainer::end()
{
   return __containers.end();
}
//
std::list<BfGuiCreateWindowContainer::ptrContainer>::reverse_iterator
BfGuiCreateWindowContainer::rbegin()
{
   return __containers.rbegin();
}
std::list<BfGuiCreateWindowContainer::ptrContainer>::reverse_iterator
BfGuiCreateWindowContainer::rend()
{
   return __containers.rend();
}
void
BfGuiCreateWindowContainer::clearEmptyContainersByName(std::string name)
{
   __containers.remove_if([&name](auto c) { return c->name() == name; });
}
void
BfGuiCreateWindowContainer::add(ptrContainer container)
{
   __containers.push_back(container);
}

//
//
//
//
//
//
//

bool BfGuiCreateWindowContainerObj::__is_moving_container = false;

#define BF_GUI_CREATE_WINDOW_CONTAINER_OBJ_MOVE_BUTTON_PASSIVE \
   ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
#define BF_GUI_CREATE_WINDOW_CONTAINER_OBJ_MOVE_BUTTON_HOVER \
   ImVec4(1.0f, 1.0f, 1.0f, 1.0f)

void
BfGuiCreateWindowContainerObj::__pushButtonColorStyle()
{
   ImGui::PushStyleColor(
       ImGuiCol_Button,
       BF_GUI_CREATE_WINDOW_CONTAINER_OBJ_MOVE_BUTTON_PASSIVE
   );

   ImGui::PushStyleColor(
       ImGuiCol_ButtonHovered,
       BF_GUI_CREATE_WINDOW_CONTAINER_OBJ_MOVE_BUTTON_HOVER
   );
}
void
BfGuiCreateWindowContainerObj::__popButtonColorStyle()
{
   ImGui::PopStyleColor(2);
}

void
BfGuiCreateWindowContainerObj::__renderChildContent()
{
   if (!__is_collapsed)
   {
      // if (ImGui::Button("Add container"))
      // {
      //    auto new_container =
      //    std::make_shared<BfGuiCreateWindowContainerObj>(
      //        shared_from_this()->weak_from_this());
      //    new_container->__window_size.x -= 20.0f;
      //    new_container->__window_size.y -= 20.0f;
      //    __containers.push_back(std::move(new_container));
      // }
   }
}

void
BfGuiCreateWindowContainerObj::__renderHeader()
{
   __pushButtonColorStyle();
   {
      ImGui::SetCursorPosX(
          ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x * 2.0f -
          ImGui::CalcTextSize(ICON_FA_WINDOW_RESTORE).x
      );

      __renderDragDropSource();
      __renderDragDropTarget();

      ImGui::SameLine();
      ImGui::SetCursorPosX(
          ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x * 2.0f -
          ImGui::CalcTextSize(ICON_FA_WINDOW_RESTORE).x -
          ImGui::CalcTextSize(ICON_FA_MINIMIZE).x - 15.0f
      );

      if (ImGui::Button(ICON_FA_MINIMIZE))
      {
         __is_collapsed = !__is_collapsed;
         if (__is_collapsed)
         {
            __old_size = size();
            __window_size.y = 80.0f;
         }
         else
         {
            __window_size.y = __old_size.y;
         }

         ImGui::SetWindowSize(__str_id.c_str(), __window_size);
      }

      ImGui::SameLine();
      ImGui::SetCursorPosX(
          ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x * 2.0f -
          ImGui::CalcTextSize(ICON_FA_WINDOW_RESTORE).x -
          ImGui::CalcTextSize(ICON_FA_MINIMIZE).x - 15.0f -
          -ImGui::CalcTextSize(ICON_FA_INFO).x - 35.0f
      );

      if (ImGui::Button(ICON_FA_INFO))
      {
      }

      if (ImGui::IsItemHovered())
      {
         __renderInfoTooltip();
      }
   }
   __popButtonColorStyle();
}

void
BfGuiCreateWindowContainerObj::__renderHeaderName()
{
   ImGui::Text("%s", name());
}

void
BfGuiCreateWindowContainerObj::__renderChangeName()
{
   ImGui::Text("<---> Add here <--->");
}

void
BfGuiCreateWindowContainerObj::__renderDragDropSource()
{
   __is_current_moving = false;
   if (ImGui::Button(ICON_FA_WINDOW_RESTORE))
   {
   }
   /*
      Суть в том, что когда окошко начинает двигаться, то есть
      Кнопка задержана и активна одновременно, то работают 2
      флага -> первый отвечает за то что какой-то контейнер будет
      помещен куда-то, а значит другие контейнеры кроме этого
      должны открыть зону, куда можно будет поместить этот контейнер,
      это осуществляется за счет 'static' переменной внутри класса
      Второй флаг отвечает за не отображение зоны перемещения в окне,
      которое пермещается, это необходимо, так как при пермещении
      ОКНА само окно не исчезает.
   */
   if (ImGui::IsItemActive() && ImGui::IsMouseDown(0))
   {
      __is_moving_container = true;
      __is_current_moving = true;
   }
   if (ImGui::IsMouseReleased(0))
   {
      __is_moving_container = false;
   }
   __processDragDropSource();
}

void
BfGuiCreateWindowContainerObj::__renderDragDropTarget()
{
   if (!__is_drop_target) return;
   // Where to drop
   ImGui::SameLine();
   ImGui::SetCursorPosX(ImGui::GetStyle().WindowPadding.x);
   if (__is_moving_container && !__is_current_moving)
   {
      __renderChangeName();
   }
   else
   {
      __renderHeaderName();
   }
   __processDragDropTarget();
}

void
BfGuiCreateWindowContainerObj::__renderInfoTooltip()
{
   std::string root_name;
   if (auto shared_root = __root_container.lock())
   {
      root_name = shared_root->name();
   }
   else
   {
      root_name = "Create";
   }

   std::string total_containers = "";
   for (auto& c : __containers)
   {
      total_containers += std::string(c->name()) + "\n";
   }
   ImGui::SetTooltip(
       "Root container: %s\nInner containers:\n%s",
       root_name.c_str(),
       total_containers.c_str()
   );
}

void
BfGuiCreateWindowContainerObj::__renderAvailableLayers()
{
   auto lh = BfLayerHandler::instance();
   for (size_t i = 0; i < lh->get_layer_count(); i++)
   {
      auto layer = lh->get_layer_by_index(i);
      // bfShowNestedLayersRecursiveWithSelectables(layer, __selected_layer);
      bfShowNestedLayersRecursiveWithRadioButtons(layer, __selected_layer);
   }

   if (ImGui::Button("Add this to layer") && __selected_layer != -1)
   {
      std::cout << "Creating begin\n";
      __createObj();
      std::cout << "Creating end\n";
      __addToLayer(BfLayerHandler::instance()->get_layer_by_id(__selected_layer)
      );
   }
}

void
BfGuiCreateWindowContainerObj::__processDragDropSource()
{
   if (ImGui::BeginDragDropSource())
   {
      // pos().x += ImGui::GetMouseDragDelta().x;
      // pos().y += ImGui::GetMouseDragDelta().y;
      // ImGui::ResetMouseDragDelta();
      /*
         Так как перемещается именно этот КОНКРЕТНЫЙ КОНТЕЙНЕР, который
         щас рендерится, то мы получаем указатель на себя самого, так как
         выше (в окошке 'Create') они хранятся в виде std::shared_ptr

         Далее данный указатель мы передаем через ImGui::SetDragDropPayload

      */
      std::shared_ptr<BfGuiCreateWindowContainerObj> self = shared_from_this();

      ImGui::SetDragDropPayload("Container", &self, sizeof(self));
      ImGui::Text("Moving: %s", __str_id.c_str());

      ImGui::EndDragDropSource();
   }
}

void
BfGuiCreateWindowContainerObj::__processDragDropTarget()
{
   if (ImGui::BeginDragDropTarget())
   {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("Container"))
      {
         std::shared_ptr<BfGuiCreateWindowContainerObj> dropped_container =
             *(std::shared_ptr<BfGuiCreateWindowContainerObj>*)payload->Data;
         __moveFunc(dropped_container->name(), this->name());
      }
      ImGui::EndDragDropTarget();
   }
}

void
BfGuiCreateWindowContainerObj::__createObj()
{
   __layer_obj = std::make_shared<BfDrawLayer>();
}

BfGuiCreateWindowContainerObj::BfGuiCreateWindowContainerObj(
    BfGuiCreateWindowContainer::wptrContainer root, bool is_target
)
    : BfGuiCreateWindowContainer{root}
    , __old_size{__window_size}
    , __is_drop_target{is_target}
{
   __layer_create_info = {
       .allocator = *BfLayerHandler::instance()->allocator(),
       .vertex_size = sizeof(BfVertex3),
       .max_vertex_count = 10000,
       .max_reserved_count = 1000,
       .is_nested = true
   };
}

void
BfGuiCreateWindowContainerObj::__addToLayer(std::shared_ptr<BfDrawLayer> add_to)
{
   add_to->add(__layer_obj);
   add_to->update_buffer();
}

void
bfShowNestedLayersRecursive(std::shared_ptr<BfDrawLayer> l)
{
#define BF_OBJ_NAME_LEN 30
#define BF_LAYER_COLOR 1.0f, 0.55f, 0.1f, 1.0f

   size_t obj_count = l->get_obj_count();
   size_t lay_count = l->get_layer_count();

   std::string lay_name = "L (" + bfGetStrNameDrawObjType(l->id.get_type()) +
                          ") " + std::to_string(l->id.get());
   if (ImGui::TreeNode(lay_name.c_str()))
   {
      for (size_t i = 0; i < lay_count; ++i)
      {
         bfShowNestedLayersRecursive(l->get_layer_by_index(i));
      }
      for (size_t i = 0; i < obj_count; ++i)
      {
         std::shared_ptr<BfDrawObj> obj = l->get_object_by_index(i);
         //
         std::string obj_name = ICON_FA_BOX_ARCHIVE "(" +
                                bfGetStrNameDrawObjType(obj->id.get_type()) +
                                ") " + std::to_string(obj->id.get());

         ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(BF_LAYER_COLOR));
         if (ImGui::TreeNode(obj_name.c_str()))
         {
            ImGui::Text(("\tVertices " +
                         std::to_string(obj->get_vertices_count()))
                            .c_str());

            ImGui::Text(("\tIndices " + std::to_string(obj->get_indices_count())
            )
                            .c_str());

            ImGui::Text(("\tDVertices " +
                         std::to_string(obj->get_dvertices_count()))
                            .c_str());

            ImGui::TreePop();
         }
         ImGui::PopStyleColor();
      }
      ImGui::TreePop();
   }
}

void
bfShowNestedLayersRecursiveWithSelectables(
    std::shared_ptr<BfDrawLayer> l, int& selectedId
)
{
#define BF_OBJ_NAME_LEN 30
#define BF_LAYER_COLOR 1.0f, 0.55f, 0.1f, 1.0f

   size_t obj_count = l->get_obj_count();
   size_t lay_count = l->get_layer_count();

   std::string lay_name = "L (" + bfGetStrNameDrawObjType(l->id.get_type()) +
                          ") " + std::to_string(l->id.get());

   ImGuiTreeNodeFlags flags =
       (selectedId == l->id.get() ? ImGuiTreeNodeFlags_Selected : 0);
   if (ImGui::TreeNodeEx(lay_name.c_str(), flags))
   {
      if (ImGui::IsItemClicked())
      {
         selectedId = l->id.get();
      }

      for (size_t i = 0; i < lay_count; ++i)
      {
         bfShowNestedLayersRecursiveWithSelectables(
             l->get_layer_by_index(i),
             selectedId
         );
      }

      for (size_t i = 0; i < obj_count; ++i)
      {
         std::shared_ptr<BfDrawObj> obj = l->get_object_by_index(i);
         std::string obj_name = ICON_FA_BOX_ARCHIVE "(" +
                                bfGetStrNameDrawObjType(obj->id.get_type()) +
                                ") " + std::to_string(obj->id.get());

         ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(BF_LAYER_COLOR));
         ImGuiTreeNodeFlags obj_flags =
             (selectedId == obj->id.get() ? ImGuiTreeNodeFlags_Selected : 0);

         if (ImGui::TreeNode(obj_name.c_str()))
         {
            ImGui::Text(
                "%s",
                ("\tVertices " + std::to_string(obj->get_vertices_count()))
                    .c_str()
            );
            ImGui::Text(
                "%s",
                ("\tIndices " + std::to_string(obj->get_indices_count()))
                    .c_str()
            );
            ImGui::Text(
                "%s",
                ("\tDVertices " + std::to_string(obj->get_dvertices_count()))
                    .c_str()
            );

            ImGui::TreePop();
         }
         ImGui::PopStyleColor();
      }
      ImGui::TreePop();
   }
}

void
bfShowNestedLayersRecursiveWithRadioButtons(
    std::shared_ptr<BfDrawLayer> l, int& selectedId
)
{
#define BF_OBJ_NAME_LEN 30
#define BF_LAYER_COLOR 1.0f, 0.55f, 0.1f, 1.0f

   size_t obj_count = l->get_obj_count();
   size_t lay_count = l->get_layer_count();

   std::string lay_name = "L (" + bfGetStrNameDrawObjType(l->id.get_type()) +
                          ") " + std::to_string(l->id.get());

   ImGuiTreeNodeFlags flags =
       ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

   // Радиокнопка перед нодой слоя
   if (ImGui::RadioButton(
           ("##radio" + lay_name).c_str(),
           selectedId == l->id.get()
       ))
   {
      selectedId = l->id.get();
   }
   ImGui::SameLine();  // Перемещаемся на ту же строку для отображения названия
                       // слоя

   if (ImGui::TreeNodeEx(lay_name.c_str(), flags))
   {
      for (size_t i = 0; i < lay_count; ++i)
      {
         bfShowNestedLayersRecursiveWithRadioButtons(
             l->get_layer_by_index(i),
             selectedId
         );
      }

      for (size_t i = 0; i < obj_count; ++i)
      {
         std::shared_ptr<BfDrawObj> obj = l->get_object_by_index(i);
         //
         std::string obj_name = ICON_FA_BOX_ARCHIVE "(" +
                                bfGetStrNameDrawObjType(obj->id.get_type()) +
                                ") " + std::to_string(obj->id.get());

         ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(BF_LAYER_COLOR));
         if (ImGui::TreeNode(obj_name.c_str()))
         {
            ImGui::Text(("\tVertices " +
                         std::to_string(obj->get_vertices_count()))
                            .c_str());

            ImGui::Text(("\tIndices " + std::to_string(obj->get_indices_count())
            )
                            .c_str());

            ImGui::Text(("\tDVertices " +
                         std::to_string(obj->get_dvertices_count()))
                            .c_str());

            ImGui::TreePop();
         }
         ImGui::PopStyleColor();
      }
      ImGui::TreePop();
   }
}

void
BfGuiCreateWindowContainerPopup::__assignButtons()
{
   switch (__side)
   {
      case LEFT:
         __is_right_button = false;
         break;
      case RIGHT:
         __is_left_button = false;
         break;
      case TOP:
         __is_bot_button = false;
         break;
      case BOT:
         __is_top_button = false;
         break;
   }
}

void
BfGuiCreateWindowContainerPopup::__clampPosition()
{
   ImVec2 outter_pos;
   ImVec2 outter_size;
   ImVec2 new_pos;

   if (auto shared_root = __root_container.lock())
   {
      outter_pos = ImGui::FindWindowByName(shared_root->name())->Pos;
      // outter_size = ImGui::FindWindowByName(shared_root->name())->Size;
      outter_size = shared_root->size();

      ImVec2 delta_pos{
          outter_pos.x - __old_outter_pos.x,
          outter_pos.y - __old_outter_pos.y
      };

      float x_clamp = __window_pos.x + delta_pos.x;

      ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImDrawList* draw_list = ImGui::GetForegroundDrawList(viewport);

      float padding_x = ImGui::GetStyle().WindowPadding.x;
      float padding_y = ImGui::GetStyle().WindowPadding.y;

      switch (__side)
      {
         case LEFT: {
            float top = shared_root->pos().y;
            float bot = shared_root->pos().y + shared_root->size().y - size().y;

            new_pos = {
                shared_root->pos().x - size().x + __resize_button_size.x * 3 +
                    padding_x,
                std::clamp(__window_pos.y + delta_pos.y, top, bot)
            };
         }
         break;
         case RIGHT: {
            float top = shared_root->pos().y;
            float bot = shared_root->pos().y + shared_root->size().y - size().y;

            new_pos = {
                shared_root->pos().x + shared_root->size().x -
                    __resize_button_size.x * 3 - padding_x,
                std::clamp(__window_pos.y + delta_pos.y, top, bot)
            };
         }
         break;
         case TOP: {
            float left = shared_root->pos().x;
            float right =
                shared_root->pos().x + shared_root->size().x - size().x;

            new_pos = {
                std::clamp(__window_pos.x + delta_pos.x, left, right),
                shared_root->pos().y - size().y + __bot_resize_button_size.y * 3
            };
         }
         break;
         case BOT: {
            float left = shared_root->pos().x;
            float right =
                shared_root->pos().x + shared_root->size().x - size().x;

            new_pos = {
                std::clamp(__window_pos.x + delta_pos.x, left, right),
                shared_root->pos().y + shared_root->size().y -
                    __bot_resize_button_size.y * 3
            };
         }
         break;
      }

      // draw_list->AddLine({left_x, pos().y + 50.0f},
      //                    {right_x, pos().y + 50.0f},
      //                    IM_COL32(255, 0, 0, 255),
      //                    2.0f);

      // if (outter_pos.x - __window_size.x <= x_clamp && x_clamp <=
      // outter_pos.x)
      // {
      //    x_clamp =
      //        std::clamp(x_clamp, outter_pos.x - __window_size.x,
      //        outter_pos.x);
      // }
      // else if (outter_pos.x + outter_size.x <= x_clamp &&
      //          x_clamp <= outter_pos.x + outter_size.x +
      //          __window_size.x)
      // {
      //    float left_x  = outter_pos.x + outter_size.x;
      //    float right_x = outter_pos.x + outter_size.x +
      //    __window_size.x;
      //
      //    x_clamp       = std::clamp(x_clamp, left_x, right_x);
      //
      //    draw_list->AddLine({left_x, pos().y + 50.0f},
      //                       {right_x, pos().y + 50.0f},
      //                       IM_COL32(255, 0, 0, 255),
      //                       2.0f);
      // }
      // else
      // {
      //    std::cout << outter_pos.x + outter_size.x << "<=" << x_clamp
      //    << "<="
      //              << outter_pos.x + outter_size.x + __window_size.x -
      //                     ImGui::GetStyle().WindowPadding.x * 2
      //              << "\n";
      // }
   }
   else
   {
      outter_pos = ImGui::FindWindowByName("Create")->Pos;
      outter_size = ImGui::FindWindowByName("Create")->Size;

      ImVec2 delta_pos{
          outter_pos.x - __old_outter_pos.x,
          outter_pos.y - __old_outter_pos.y
      };

      new_pos = {
          std::clamp(
              __window_pos.x + delta_pos.x,
              outter_pos.x,
              outter_pos.x + outter_size.x - __window_size.x
          ),
          std::clamp(
              __window_pos.y + delta_pos.y,
              outter_pos.y,
              outter_pos.y + outter_size.y - __window_size.y
          )
      };
   }

   __old_outter_pos = outter_pos;
   __window_pos = new_pos;
   ImGui::SetNextWindowPos(new_pos);
}

void
BfGuiCreateWindowContainerPopup::__renderChildContent()
{
   if (__renderPopupContentFunc)
   {
      __renderPopupContentFunc();
   }
}

void
BfGuiCreateWindowContainerPopup::__renderHeader()
{
   ImGui::PushStyleColor(ImGuiCol_Button, {1.0f, 1.0f, 1.0f, 0.0f});
   ImGui::SetCursorPosX(
       ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("\uf00d").x
   );
   if (ImGui::Button("\uf00d"))
   {
      toggleRender();
   }
   ImGui::PopStyleColor();
}

BfGuiCreateWindowContainerPopup::BfGuiCreateWindowContainerPopup(
    BfGuiCreateWindowContainer::wptrContainer root,
    std::function<void()> popup_func = nullptr
)
    : BfGuiCreateWindowContainer{root}
    , __side{BfGuiCreateWindowContainerPopup::RIGHT}
    , __renderPopupContentFunc{popup_func}
{
   // __assignButtons();
}

BfGuiCreateWindowContainerPopup::SIDE
BfGuiCreateWindowContainerPopup::side() noexcept
{
   return __side;
}

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
//
