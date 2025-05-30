#include "bfGuiCreateWindowContainer.h"

#include <sstream>

#include "bfIconsFontAwesome6.h"
#include "imgui.h"

bool BfGuiCreateWindowContainer::__is_resizing_hovered_h = false;
bool BfGuiCreateWindowContainer::__is_resizing_hovered_v = false;

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

   ImVec2 new_pos = {
       std::clamp(
           pos().x + delta_pos.x,
           outter_pos.x,
           outter_pos.x + outter_size.x - size().x
       ),
       std::clamp(
           pos().y + delta_pos.y,
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
   if (__is_resizing_hovered_v)
   {
      ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
      return;
   }
   else if (__is_resizing_hovered_h)
   {
      ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
      return;
   }
   else
   {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
   }
}

void
BfGuiCreateWindowContainer::__renderResizeButton(int side)
{
   if (!(__is_button & side))
   {
      if ((side == BfGuiCreateWindowContainer_ButtonType_Bot) ||
          (side == BfGuiCreateWindowContainer_ButtonType_Top))
      {
         ImGui::Dummy(__bot_resize_button_size);
      }
      else
      {
         ImGui::Dummy(__resize_button_size);
      }
      return;
   }
   else
   {
      ImGui::PushID(__id);
      {
         if ((side == BfGuiCreateWindowContainer_ButtonType_Bot) ||
             (side == BfGuiCreateWindowContainer_ButtonType_Top))
         {
            __is_invisiable_buttons ? ImGui::InvisibleButton(
                                          std::to_string(side).c_str(),
                                          __bot_resize_button_size
                                      )
                                    : ImGui::Button(
                                          std::to_string(side).c_str(),
                                          __bot_resize_button_size
                                      );
         }
         else
         {
            __is_invisiable_buttons ? ImGui::InvisibleButton(
                                          std::to_string(side).c_str(),
                                          __resize_button_size
                                      )
                                    : ImGui::Button(
                                          std::to_string(side).c_str(),
                                          __resize_button_size
                                      );
         }
      }
      ImGui::PopID();
   }

   if (ImGui::IsItemHovered() && (__is_button & side))
   {
      if ((side == BfGuiCreateWindowContainer_ButtonType_Bot) ||
          (side == BfGuiCreateWindowContainer_ButtonType_Top))
         __is_resizing_hovered_v = true;
      else
         __is_resizing_hovered_h = true;
   }

   if (ImGui::IsItemActive())
   {
      __is_resizing = true;
      switch (side)
      {
      case BfGuiCreateWindowContainer_ButtonType_Left:
         __window_size.x -= ImGui::GetIO().MouseDelta.x;
         __window_pos.x += ImGui::GetIO().MouseDelta.x;
         break;
      case BfGuiCreateWindowContainer_ButtonType_Right:
         __window_size.x += ImGui::GetIO().MouseDelta.x;

         break;
      case BfGuiCreateWindowContainer_ButtonType_Top:
         __window_pos.y += ImGui::GetIO().MouseDelta.y;
         __window_size.y -= ImGui::GetIO().MouseDelta.y;
         break;
      case BfGuiCreateWindowContainer_ButtonType_Bot:
         __window_size.y += ImGui::GetIO().MouseDelta.y;

         break;
      }

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
      if (__is_render_header)
         __renderHeader();
      if (!__is_collapsed)
      {
         if (__is_render_header)
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

std::function< void(const std::string&, const std::string&) >
    BfGuiCreateWindowContainer::__swapFunc =
        [](const auto& a, const auto& b) {};

std::function< void(const std::string&, const std::string&) >
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
   static uint32_t growing_id = 1;
   //
   __id = growing_id;
   __str_id = std::format("##CreateWindow_{}", std::to_string(growing_id));
   __str_child_border_id =
       std::format("##CreateWindowChildBorder_{}", std::to_string(growing_id));

   growing_id += 4;
}

bool
BfGuiCreateWindowContainer::render()
{
   bool is_window_hovered = false;
   if (__is_render)
   {
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
            ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 2.0f);
            ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 2.0f);
            {
               ImGui::SetCursorPosX(
                   ImGui::GetCursorPosX() + __resize_button_size.x +
                   ImGui::GetStyle().ItemSpacing.x
               );
               __renderResizeButton(BfGuiCreateWindowContainer_ButtonType_Top);
               __renderResizeButton(BfGuiCreateWindowContainer_ButtonType_Left);
               ImGui::SameLine();
            }
            ImGui::PopStyleVar(2);

            is_window_hovered = __renderChildBorder();

            ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 2.0f);
            ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 2.0f);
            {
               ImGui::SameLine();

               __renderResizeButton(
                   BfGuiCreateWindowContainer_ButtonType_Right
               );

               ImGui::SetCursorPosX(
                   ImGui::GetCursorPosX() + __resize_button_size.x +
                   ImGui::GetStyle().ItemSpacing.x
               );

               __renderResizeButton(BfGuiCreateWindowContainer_ButtonType_Bot);
            }
            ImGui::PopStyleVar(2);
         }
      }
      ImGui::End();
      __postrender();
   }
   return is_window_hovered;
}

void
BfGuiCreateWindowContainer::renderBodyView()
{
   __renderChildContent();
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

wptrContainer&
BfGuiCreateWindowContainer::root() noexcept
{
   return __root_container;
}

uint32_t
BfGuiCreateWindowContainer::id() noexcept
{
   return __id;
}

// clang-format off
std::string
BfGuiCreateWindowContainer::genTable()
{
   // std::stringstream ss;
   //
   // ss << "{\n"
   //       << "__is_force_render = "  << __is_force_render  << ","
   //       << "__is_render_header = " << __is_render_header << ","
   //       << "__is_collapsed = "     << __is_collapsed     << ","
   //
   //       << "__resize_button_size = {" 
   //          << __resize_button_size.x
   //          << ", "
   //          << __resize_button_size.y
   //       << "},"
   //
   //       << "__bot_resize_button_size" << __bot_resize_button_size
   //       << "__window_pos" << __window_pos << "__window_size" << __window_size
   //
   //       << "__id" << __id 
   //       << "__str_id" << __str_id
   //       << "__is_button" << __is_button
   //    << "}\n";
   return "";
}
// clang-format on

void
BfGuiCreateWindowContainer::toggleRender(int mode) noexcept
{
   if (mode == BfGuiCreateWindowContainer_ToggleMode_Toggle)
      __is_render = !__is_render;
   else
      __is_render = mode;
}

void
BfGuiCreateWindowContainer::toggleButton(int button_id, int mode) noexcept
{
   if (mode == BfGuiCreateWindowContainer_ToggleMode_Toggle)
      __is_button ^= button_id;
   else
      mode ? __is_button |= button_id : __is_button &= ~(button_id);
}

void
BfGuiCreateWindowContainer::toggleHeader(int mode) noexcept
{
   if (mode == BfGuiCreateWindowContainer_ToggleMode_Toggle)
      __is_render_header = !__is_render_header;
   else
      __is_render_header = mode;
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
BfGuiCreateWindowContainer::isForceRender() noexcept
{
   return __is_force_render;
}

bool
BfGuiCreateWindowContainerObj::isAnyMoving() noexcept
{
   return BfGuiCreateWindowContainerObj::__is_moving_container;
}
//
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
//
std::list< ptrContainer >::iterator
BfGuiCreateWindowContainer::begin()
{
   return __containers.begin();
}
std::list< ptrContainer >::iterator
BfGuiCreateWindowContainer::end()
{
   return __containers.end();
}

std::list< ptrContainer >::reverse_iterator
BfGuiCreateWindowContainer::rbegin()
{
   return __containers.rbegin();
}

std::list< ptrContainer >::reverse_iterator
BfGuiCreateWindowContainer::rend()
{
   return __containers.rend();
}

void
BfGuiCreateWindowContainer::add(ptrContainer container)
{
   __containers.push_back(container);
}

void
BfGuiCreateWindowContainer::rem(ptrContainer container)
{
   __containers.remove_if([&container](auto c) {
      return (c->name() == container->name()) && (c->id() == container->id());
   });
}

//
//
//
//
//
//
//

bool BfGuiCreateWindowContainerObj::__is_moving_container = false;

#define BF_GUI_CREATE_WINDOW_CONTAINER_OBJ_MOVE_BUTTON_PASSIVE                 \
   ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
#define BF_GUI_CREATE_WINDOW_CONTAINER_OBJ_MOVE_BUTTON_HOVER                   \
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
   }
}

void
BfGuiCreateWindowContainerObj::__renderHeader()
{
   __pushButtonColorStyle();
   {
      __renderDragDropTarget();
      ImGui::SameLine();
      if (ImGui::Button(
              __is_collapsed ? ICON_FA_EXPAND : ICON_FA_COMPRESS,
              __header_button_size
          ))
      {
         __is_collapsed = !__is_collapsed;
         __is_button ^= (BfGuiCreateWindowContainer_ButtonType_Top);
         __is_button ^= (BfGuiCreateWindowContainer_ButtonType_Bot);
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
      ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
      if (ImGui::Button("\uf256", __header_button_size))
      {
      }
      ImGui::PopItemFlag();
      ImGui::SameLine();
      __renderDragDropSource();
   }
   __popButtonColorStyle();
}

void
BfGuiCreateWindowContainerObj::__renderHeaderName()
{
   ImGui::SameLine();

   ImGui::PushID(name());
   {
      ImGui::SetNextItemWidth(
          ImGui::GetContentRegionAvail().x - __totalHeaderButtonsSize().x
      );
      ImGui::PushStyleColor(ImGuiCol_FrameBg, {1.0, 1.0f, 1.0f, 0.0f});
      ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, {1.0, 1.0f, 1.0f, 0.2f});
      ImGui::PushStyleColor(ImGuiCol_FrameBgActive, {1.0, 1.0f, 1.0f, 0.5f});
      if (ImGui::InputText(
              "##edit",
              __name.data(),
              ImGuiInputTextFlags_EnterReturnsTrue
          ))
      {
         __name = std::string(__name.c_str());
      }
      ImGui::PopStyleColor(3);
   }
   ImGui::PopID();
}

void
BfGuiCreateWindowContainerObj::__renderChangeName()
{
   ImGui::SameLine();
   ImGui::PushID(name());
   {
      if (ImGui::Button(
              "<==> Add \uf140 here <==>",
              {ImGui::GetContentRegionAvail().x - __totalHeaderButtonsSize().x,
               __header_button_size.y}
          ))
      {
      }
   }
   ImGui::PopID();
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
   if (!__is_drop_target)
      return;

   ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.0f);
   if (ImGui::Button(ICON_FA_INFO))
   {
   }
   if (ImGui::IsItemHovered())
   {
      __renderInfoTooltip();
   }

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
BfGuiCreateWindowContainerObj::__processDragDropSource()
{
   if (ImGui::BeginDragDropSource())
   {
      /*
         Так как перемещается именно этот КОНКРЕТНЫЙ КОНТЕЙНЕР, который
         щас рендерится, то мы получаем указатель на себя самого, так как
         выше (в окошке 'Create') они хранятся в виде std::shared_ptr

         Далее данный указатель мы передаем через ImGui::SetDragDropPayload

      */
      std::shared_ptr< BfGuiCreateWindowContainerObj > self =
          shared_from_this();

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
         std::shared_ptr< BfGuiCreateWindowContainerObj > dropped_container =
             *(std::shared_ptr< BfGuiCreateWindowContainerObj >*)payload->Data;
         __moveFunc(dropped_container->name(), this->name());
      }
      ImGui::EndDragDropTarget();
   }
}

void
BfGuiCreateWindowContainerObj::__createObj()
{
   __layer_obj = std::make_shared< BfDrawLayer >();
}

BfGuiCreateWindowContainerObj::BfGuiCreateWindowContainerObj(
    wptrContainer root, bool is_target
)
    : BfGuiCreateWindowContainer{root}
    , __old_size{__window_size}
    , __is_drop_target{is_target}
{
   __layer_create_info = {
       .allocator = BfLayerHandler::instance()
                        ? *BfLayerHandler::instance()->allocator()
                        : nullptr,
       .vertex_size = sizeof(BfVertex3),
       .max_vertex_count = 10000,
       .max_reserved_count = 1000,
       .is_nested = true
   };
}

void
BfGuiCreateWindowContainerObj::__addToLayer(
    std::shared_ptr< BfDrawLayer > add_to
)
{
   if (auto shared_root = __ptr_root.lock())
   {
      shared_root->del(__layer_obj->id.get(), true);
   }
   __createObj();
   __ptr_root = add_to;
   add_to->add(__layer_obj);
   add_to->update_buffer();
}

ImVec2
BfGuiCreateWindowContainerObj::__totalHeaderButtonsSize()
{
   return {
       (__header_button_size.x + ImGui::GetStyle().ItemSpacing.x) * 3,
       __header_button_size.y
   };
}

void
BfGuiCreateWindowContainerPopup::__assignButtons()
{
   switch (__side)
   {
   case BfGuiCreateWindowContainerPopup_Side_Left:
      this->toggleButton(BfGuiCreateWindowContainer_ButtonType_Right, false);
      break;
   case BfGuiCreateWindowContainerPopup_Side_Right:
      this->toggleButton(BfGuiCreateWindowContainer_ButtonType_Left, false);
      break;
   case BfGuiCreateWindowContainerPopup_Side_Top:
      this->toggleButton(BfGuiCreateWindowContainer_ButtonType_Bot, false);
      break;
   case BfGuiCreateWindowContainerPopup_Side_Bot:
      this->toggleButton(BfGuiCreateWindowContainer_ButtonType_Top, false);
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
      case BfGuiCreateWindowContainerPopup_Side_Left: {
         float top = shared_root->pos().y;
         float bot = shared_root->pos().y + shared_root->size().y - size().y;

         new_pos = {
             shared_root->pos().x - size().x + __resize_button_size.x * 3 +
                 padding_x,
             std::clamp(__window_pos.y + delta_pos.y, top, bot)
         };
      }
      break;
      case BfGuiCreateWindowContainerPopup_Side_Right: {
         float top = shared_root->pos().y;
         float bot = shared_root->pos().y + shared_root->size().y - size().y;

         new_pos = {
             shared_root->pos().x + shared_root->size().x -
                 __resize_button_size.x * 3 - padding_x,
             std::clamp(__window_pos.y + delta_pos.y, top, bot)
         };
      }
      break;
      case BfGuiCreateWindowContainerPopup_Side_Top: {
         float left = shared_root->pos().x;
         float right = shared_root->pos().x + shared_root->size().x - size().x;

         new_pos = {
             std::clamp(__window_pos.x + delta_pos.x, left, right),
             shared_root->pos().y - size().y + __bot_resize_button_size.y * 3
         };
      }
      break;
      case BfGuiCreateWindowContainerPopup_Side_Bot: {
         float left = shared_root->pos().x;
         float right = shared_root->pos().x + shared_root->size().x - size().x;

         new_pos = {
             std::clamp(__window_pos.x + delta_pos.x, left, right),
             shared_root->pos().y + shared_root->size().y -
                 __bot_resize_button_size.y * 3
         };
      }
      break;
      }
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
      __renderPopupContentFunc(__root_container);
   }
}

void
BfGuiCreateWindowContainerPopup::__renderHeader()
{
   ImGui::Text(ICON_FA_INFO);
   if (ImGui::IsItemHovered())
      ImGui::SetTooltip(
          "Render force status: %s",
          std::to_string(__is_force_render).c_str()
      );
   ImGui::SameLine();

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
    wptrContainer root,
    int side,
    bool is_force_render,
    std::function< void(wptrContainer) > popup_func
)
    : BfGuiCreateWindowContainer{root}
    , __side{side}
    , __renderPopupContentFunc{popup_func}
{
   __assignButtons();
   __is_force_render = is_force_render;
}

int
BfGuiCreateWindowContainerPopup::side() noexcept
{
   return __side;
}

void
BfGuiCreateWindowContainerPopup::setSize(ImVec2 size)
{
   __window_size = size;
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
