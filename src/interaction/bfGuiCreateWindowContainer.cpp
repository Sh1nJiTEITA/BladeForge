#include "bfGuiCreateWindowContainer.h"

#include <memory>

#include "bfIconsFontAwesome6.h"
#include "imgui.h"

bool BfGuiCreateWindowContainer::__is_resizing_hovered_h = false;
bool BfGuiCreateWindowContainer::__is_resizing_hovered_v = false;

void BfGuiCreateWindowContainer::__pushStyle()
{
   ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 2.0f);
   ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 2.0f);
}
void BfGuiCreateWindowContainer::__popStyle() { ImGui::PopStyleVar(2); }

void BfGuiCreateWindowContainer::__clampPosition()
{
   ImVec2 outter_pos;
   ImVec2 outter_size;
   if (auto shared_root = __root_container.lock())
   {
      outter_pos  = ImGui::FindWindowByName(shared_root->name())->Pos;
      outter_size = ImGui::FindWindowByName(shared_root->name())->Size;
   }
   else
   {
      outter_pos  = ImGui::FindWindowByName("Create")->Pos;
      outter_size = ImGui::FindWindowByName("Create")->Size;
   }

   ImVec2 delta_pos{outter_pos.x - __old_outter_pos.x,
                    outter_pos.y - __old_outter_pos.y};

   bool is_outter_changed =
       outter_pos.x != __old_outter_pos.x || outter_pos.y != __old_outter_pos.y;

   ImVec2 new_pos = {
       std::clamp(__window_pos.x + delta_pos.x,
                  outter_pos.x,
                  outter_pos.x + outter_size.x - __window_size.x),
       std::clamp(__window_pos.y + delta_pos.y,
                  outter_pos.y,
                  outter_pos.y + outter_size.y - __window_size.y)};

   __old_outter_pos = outter_pos;

   ImGui::SetNextWindowPos(new_pos);
   __window_pos = new_pos;
}

void BfGuiCreateWindowContainer::__updatePosition()
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

void BfGuiCreateWindowContainer::__updateResizeButtonSize()
{
   __resize_button_size.y = __window_size.y -
                            ImGui::GetStyle().WindowPadding.y * 2.0f -
                            __bot_resize_button_size.y * 2.0f -
                            ImGui::GetStyle().ItemSpacing.y * 2.0f;

   __bot_resize_button_size.x =
       __window_size.x - ImGui::GetStyle().WindowPadding.x * 2.0f -
       __resize_button_size.x * 2.0f - ImGui::GetStyle().ItemSpacing.x * 2.0f;
}

void BfGuiCreateWindowContainer::changeCursorStyle()
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

void BfGuiCreateWindowContainer::__renderLeftResizeButton()
{
   if (__is_invisiable_buttons
           ? ImGui::InvisibleButton(__str_left_resize_button_id.c_str(),
                                    __resize_button_size)
           : ImGui::Button(__str_left_resize_button_id.c_str(),
                           __resize_button_size))
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

void BfGuiCreateWindowContainer::__renderRightResizeButton()
{
   if (__is_invisiable_buttons
           ? ImGui::InvisibleButton(__str_right_resize_button_id.c_str(),
                                    __resize_button_size)
           : ImGui::Button(__str_right_resize_button_id.c_str(),
                           __resize_button_size))
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

void BfGuiCreateWindowContainer::__renderBotResizeButton()
{
   if (__is_invisiable_buttons
           ? ImGui::InvisibleButton(__str_bot_resize_button_id.c_str(),
                                    __bot_resize_button_size)
           : ImGui::Button(__str_bot_resize_button_id.c_str(),
                           __bot_resize_button_size))
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

void BfGuiCreateWindowContainer::__renderTopResizeButton()
{
   if (__is_invisiable_buttons
           ? ImGui::InvisibleButton(__str_top_resize_button_id.c_str(),
                                    __bot_resize_button_size)
           : ImGui::Button(__str_top_resize_button_id.c_str(),
                           __bot_resize_button_size))
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

bool BfGuiCreateWindowContainer::__renderChildBorder()
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
       true);
   {
      __updateResizeButtonSize();
      __renderHeader();
      if (!__is_collapsed) __renderChildContent();
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

void BfGuiCreateWindowContainer::__renderHeader() {}
void BfGuiCreateWindowContainer::__renderChildContent() {}

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

bool BfGuiCreateWindowContainer::render()
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
            root_pos  = shared_root->pos();
         }
         else
         {
            ImGuiWindow* root = ImGui::FindWindowByName("Create");
            root_size         = root->Size;
            root_pos          = root->Pos;
         }

         root_pos.x += root_size.x * 0.5f;
         root_pos.y += root_size.y * 0.5f;
         ImGui::SetNextWindowPos(root_pos);
         __window_pos = root_pos;
      }
      int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                  ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove |
                  ImGuiWindowFlags_NoScrollWithMouse |
                  ImGuiWindowFlags_NoScrollbar;

      if (ImGui::Begin(__str_id.c_str(), &__is_render, flags))
      {
         // if (__is_collapsed)
         // {
         //    __pushStyle();
         //    {
         //       __renderLeftResizeButton();
         //       ImGui::SameLine();
         //       is_window_hovered = __renderChildBorder();
         //       ImGui::SameLine();
         //       __renderRightResizeButton();
         //    }
         //    __popStyle();
         // }
         // else
         {
            __pushStyle();
            {
               ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                                    __resize_button_size.x +
                                    ImGui::GetStyle().ItemSpacing.x);
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

               ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                                    __resize_button_size.x +
                                    ImGui::GetStyle().ItemSpacing.x);

               __renderBotResizeButton();
            }
            __popStyle();
         }
      }
      ImGui::End();
   }
   return is_window_hovered;
}

const char* BfGuiCreateWindowContainer::name() noexcept
{
   return __str_id.c_str();
}
ImVec2& BfGuiCreateWindowContainer::pos() noexcept { return __window_pos; }
ImVec2& BfGuiCreateWindowContainer::size() noexcept { return __window_size; }

BfGuiCreateWindowContainer::wptrContainer&
BfGuiCreateWindowContainer::root() noexcept
{
   return __root_container;
}

bool BfGuiCreateWindowContainer::isEmpty() noexcept
{
   return __containers.empty();
}

bool BfGuiCreateWindowContainer::isCollapsed() noexcept
{
   return __is_collapsed;
}

bool BfGuiCreateWindowContainerObj::isAnyMoving() noexcept
{
   return BfGuiCreateWindowContainerObj::__is_moving_container;
}

void BfGuiCreateWindowContainerObj::setRootDelete(
    std::function<void(std::string)> f)
{
   __f_root_delete = f;
}

void BfGuiCreateWindowContainer::resetResizeHover()
{
   __is_resizing_hovered_h = false;
   __is_resizing_hovered_v = false;

   ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
}

void BfGuiCreateWindowContainer::bindSwapFunction(swapFuncType func) noexcept
{
   __swapFunc = func;
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
void BfGuiCreateWindowContainer::clearEmptyContainersByName(std::string name)
{
   __containers.remove_if([&name](auto c) { return c->name() == name; });
}

void BfGuiCreateWindowContainer::swapByName(const std::string& a,
                                            const std::string& b,
                                            bool               change_pos)

{
   auto itA =
       std::find_if(__containers.begin(), __containers.end(), [&a](auto c) {
          return a == std::string(c->name());
       });

   auto itB =
       std::find_if(__containers.begin(), __containers.end(), [&b](auto c) {
          return b == std::string(c->name());
       });

   if (itA != __containers.end() && itB != __containers.end())
   {
      std::cout << "Swapped done\n";
      std::iter_swap(itA, itB);
      if (change_pos)
      {
         std::swap((*itA)->pos(), (*itB)->pos());
      }
   }
   else
   {
      std::cout << "Swap failed: one or both elements not found\n";
   }
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

void BfGuiCreateWindowContainerObj::__pushButtonColorStyle()
{
   ImGui::PushStyleColor(
       ImGuiCol_Button,
       BF_GUI_CREATE_WINDOW_CONTAINER_OBJ_MOVE_BUTTON_PASSIVE);

   ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                         BF_GUI_CREATE_WINDOW_CONTAINER_OBJ_MOVE_BUTTON_HOVER);
}
void BfGuiCreateWindowContainerObj::__popButtonColorStyle()
{
   ImGui::PopStyleColor(2);
}

void BfGuiCreateWindowContainerObj::__renderChildContent()
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

void BfGuiCreateWindowContainerObj::__renderHeader()
{
   __pushButtonColorStyle();
   {
      ImGui::SetCursorPosX(ImGui::GetWindowWidth() -
                           ImGui::GetStyle().WindowPadding.x * 2.0f -
                           ImGui::CalcTextSize(ICON_FA_WINDOW_RESTORE).x);

      __renderDragDropSource();
      __renderDragDropTarget();

      ImGui::SameLine();
      ImGui::SetCursorPosX(ImGui::GetWindowWidth() -
                           ImGui::GetStyle().WindowPadding.x * 2.0f -
                           ImGui::CalcTextSize(ICON_FA_WINDOW_RESTORE).x -
                           ImGui::CalcTextSize(ICON_FA_MINIMIZE).x - 15.0f);

      if (ImGui::Button(ICON_FA_MINIMIZE))
      {
         __is_collapsed = !__is_collapsed;
         if (__is_collapsed)
         {
            __old_size      = size();
            __window_size.y = 80.0f;
         }
         else
         {
            __window_size.y = __old_size.y;
         }

         ImGui::SetWindowSize(__str_id.c_str(), __window_size);
      }

      ImGui::SameLine();
      ImGui::SetCursorPosX(ImGui::GetWindowWidth() -
                           ImGui::GetStyle().WindowPadding.x * 2.0f -
                           ImGui::CalcTextSize(ICON_FA_WINDOW_RESTORE).x -
                           ImGui::CalcTextSize(ICON_FA_MINIMIZE).x - 15.0f -
                           -ImGui::CalcTextSize(ICON_FA_INFO).x - 35.0f);

      if (ImGui::Button(ICON_FA_INFO))
      {
      }

      if (ImGui::IsItemHovered())
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
         ImGui::SetTooltip("Root container: %s\nInner containers:\n%s",
                           root_name.c_str(),
                           total_containers.c_str());
      }
   }
   __popButtonColorStyle();
}

void BfGuiCreateWindowContainerObj::__renderHeaderName()
{
   ImGui::SameLine();
   ImGui::SetCursorPosX(ImGui::GetStyle().WindowPadding.x);
   ImGui::Text("%s", name());
}

void BfGuiCreateWindowContainerObj::__renderDragDropSource()
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
      __is_current_moving   = true;
   }
   if (ImGui::IsMouseReleased(0))
   {
      __is_moving_container = false;
   }

   if (ImGui::BeginDragDropSource())
   {
      /*
         Так как перемещается именно этот КОНКРЕТНЫЙ КОНТЕЙНЕР, который
         щас рендерится, то мы получаем указатель на себя самого, так как
         выше (в окошке 'Create') они хранятся в виде std::shared_ptr

         Далее данный указатель мы передаем через ImGui::SetDragDropPayload

      */
      std::shared_ptr<BfGuiCreateWindowContainerObj> self = shared_from_this();

      ImGui::SetDragDropPayload("Container", &self, sizeof(self));
      ImGui::Text("Dragging Container: %s", __str_id.c_str());

      ImGui::EndDragDropSource();
   }
}

void BfGuiCreateWindowContainerObj::__renderDragDropTarget()
{
   if (!__is_drop_target) return;
   // Where to drop
   if (__is_moving_container && !__is_current_moving)
   {
      ImGui::SameLine();
      ImGui::SetCursorPosX(ImGui::GetStyle().WindowPadding.x);
      ImGui::Text("<---> Add here <--->");
   }
   else
   {
      __renderHeaderName();
   }

   if (ImGui::BeginDragDropTarget())
   {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("Container"))
      {
         /*
            Получаем указатель на окошко которое было сюда перемещено
         */
         std::shared_ptr<BfGuiCreateWindowContainerObj> dropped_container =
             *(std::shared_ptr<BfGuiCreateWindowContainerObj>*)payload->Data;

         /*
            Добавляем его к другим окошками в ДАННОМ ОКНЕ (куда было
            перемещено)
         */
         __containers.push_back(dropped_container);

         /*
            Добавляем его к другим окошками в ДАННОМ ОКНЕ (куда было
            перемещено)
         */
         auto wptr_old_root = (*__containers.rbegin())->root();

         /*
            Получаем указатель на внешнее окошко которое хранило то, что
            было перемещено
         */

         std::string dropped_name = (*__containers.rbegin())->name();
         if (auto shared_obj = wptr_old_root.lock())
         {
            // Удаляем из прошлого root-окна контейнер, который
            // был перемещен, чтобы он не дублировался
            shared_obj->clearEmptyContainersByName(dropped_name);
         }
         else
         {
            // BfGuiCreateWindow::instance()->removeByName(dropped_name);
            __f_root_delete(dropped_name);
         }

         /*
            Меняем перемещенному окну 'root'-указатель и 'root'-имя
         */
         (*__containers.rbegin())->root() =
             shared_from_this()->weak_from_this();
      }
      ImGui::EndDragDropTarget();
   }
}

std::function<void(std::string)>
    BfGuiCreateWindowContainerObj::__f_root_delete = [](std::string) {};

BfGuiCreateWindowContainerObj::BfGuiCreateWindowContainerObj(
    BfGuiCreateWindowContainer::wptrContainer root, bool is_target)
    : BfGuiCreateWindowContainer{root}
    , __old_size{__window_size}
    , __is_drop_target{is_target}
{
}

//
//
//
//
//

BfGuiCreateWindowBladeSection::BfGuiCreateWindowBladeSection(
    BfGuiCreateWindowContainer::wptrContainer root, bool is_target)
    : BfGuiCreateWindowContainerObj(root, is_target)
{
}

void BfGuiCreateWindowBladeSection::__renderDragDropSourceUp()
{
   ImGui::SameLine();
   ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f -
                        ImGui::CalcTextSize(ICON_FA_LOCATION_DOT).x * 0.5);
   ImGui::Button(ICON_FA_LOCATION_DOT "##UP");

   if (ImGui::BeginDragDropSource())
   {
      std::shared_ptr<BfGuiCreateWindowContainer> self = shared_from_this();

      ImGui::SetDragDropPayload("BladeSection", &self, sizeof(self));
      ImGui::Text("Binding container from top node: %s", __str_id.c_str());

      ImGui::EndDragDropSource();
   }
}

void BfGuiCreateWindowBladeSection::__renderDragDropSourceDown()
{
   ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f -
                        ImGui::CalcTextSize(ICON_FA_LOCATION_DOT).x * 0.5);
   ImGui::Button(ICON_FA_LOCATION_DOT "##DOWN");

   if (ImGui::BeginDragDropSource())
   {
      std::shared_ptr<BfGuiCreateWindowContainer> self = shared_from_this();

      ImGui::SetDragDropPayload("BladeSection", &self, sizeof(self));
      ImGui::Text("Binding container from bottom node: %s", __str_id.c_str());

      ImGui::EndDragDropSource();
   }
}

void BfGuiCreateWindowBladeSection::__renderDragDropTargetUp()
{
   if (ImGui::BeginDragDropTarget())
   {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("BladeSection"))
      {
         /*
            Получаем указатель на окошко которое было сюда перемещено
         */
         std::shared_ptr<BfGuiCreateWindowContainerObj> dropped_container =
             *(std::shared_ptr<BfGuiCreateWindowContainerObj>*)payload->Data;

         std::cout << dropped_container->name() << "\n";
      }
      ImGui::EndDragDropTarget();
   }
}

void BfGuiCreateWindowBladeSection::__renderDragDropTargetDown()
{
   if (ImGui::BeginDragDropTarget())
   {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("BladeSection"))
      {
         /*
            Получаем указатель на окошко которое было сюда перемещено
         */
         std::shared_ptr<BfGuiCreateWindowContainerObj> dropped_container =
             *(std::shared_ptr<BfGuiCreateWindowContainerObj>*)payload->Data;

         std::cout << dropped_container->name() << "\n";
      }
      ImGui::EndDragDropTarget();
   }
}
//
// void BfGuiCreateWindowBladeSection::__renderChildContent()
// {
//    // __renderDragDropSourceUp();
//    // __renderDragDropTargetUp();
//    ImGui::InputFloat("Width", &__create_info.width);
//    ImGui::InputFloat("Install Angle", &__create_info.install_angle);
//    ImGui::InputFloat("Inlet Angle", &__create_info.inlet_angle);
//    ImGui::InputFloat("Outlet Angle", &__create_info.outlet_angle);
//    // __renderDragDropSourceDown();
//    // __renderDragDropTargetDown();
// }

void BfGuiCreateWindowBladeSection::__renderDragDropTarget()
{
   if (!__is_drop_target) return;
   // Where to drop
   if (__is_moving_container && !__is_current_moving)
   {
      ImGui::SameLine();
      ImGui::SetCursorPosX(ImGui::GetStyle().WindowPadding.x);
      ImGui::Text("<---> Swap <--->");
   }
   else
   {
      __renderHeaderName();
   }

   if (!__is_drop_target) return;
   // Where to drop
   if (__is_moving_container && !__is_current_moving)
   {
      ImGui::SameLine();
      ImGui::SetCursorPosX(ImGui::GetStyle().WindowPadding.x);
      ImGui::Text("<---> Add here <--->");
   }
   else
   {
      __renderHeaderName();
   }

   if (ImGui::BeginDragDropTarget())
   {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("Container"))
      {
         // {
         //    /*
         //       Получаем указатель на окошко которое было сюда перемещено
         //    */
         std::shared_ptr<BfGuiCreateWindowContainerObj> dropped_container =
             *(std::shared_ptr<BfGuiCreateWindowContainerObj>*)payload->Data;

         __swapFunc(this->name(), dropped_container->name());
      }

      //
      //    // if (auto next =
      //    //         std::dynamic_pointer_cast<BfGuiCreateWindowBladeSection>(
      //    //             dropped_container))
      //    if (auto next =
      //    std::dynamic_pointer_cast<BfGuiCreateWindowContainer>(
      //            dropped_container))
      //    {
      //       std::cout << "Swapping: ";
      //       BfGuiCreateWindow::instance()
      //       // if (auto root = __root_container.lock())
      //       // {
      //       //    std::cout << "ROOT" << this->name() << " " << next->name()
      //       //              << "\n";
      //       //
      //       //    // Находим итератор на `this` в контейнере root
      //       //    auto it = std::find_if(root->begin(),
      //       //                           root->end(),
      //       //                           [&](const ptrContainer& a) {
      //       //                              return a->name() == this->name();
      //       //                           });
      //       //
      //       //    // Если итератор найден, меняем значения по указателям
      //       //    if (it != root->end())
      //       //    {
      //       //       std::cout << "Totaly swapping\n";
      //       //       // (*it).swap(next);
      //       //       next.swap(*it);
      //       //    }
      //    }
      //    else
      //    {
      //       // Обработка случая, когда root недоступен
      //    }
      // }
      //
      // else if (auto next_ =
      //              std::dynamic_pointer_cast<BfGuiCreateWindowBladeBase>(
      //                  dropped_container))
      // {
      //    std::cout << "can be base\n";
      // }
      // else
      // {
      //    /*
      //       Добавляем его к другим окошками в ДАННОМ ОКНЕ (куда было
      //       перемещено)
      //    */
      //    __containers.push_back(dropped_container);
      //
      //    /*
      //       Добавляем его к другим окошками в ДАННОМ ОКНЕ (куда было
      //       перемещено)
      //    */
      //    auto wptr_old_root = (*__containers.rbegin())->root();
      //
      //    /*
      //       Получаем указатель на внешнее окошко которое хранило то, что
      //       было перемещено
      //    */
      //
      //    std::string dropped_name = (*__containers.rbegin())->name();
      //    if (auto shared_obj = wptr_old_root.lock())
      //    {
      //       // Удаляем из прошлого root-окна контейнер, который
      //       // был перемещен, чтобы он не дублировался
      //       shared_obj->clearEmptyContainersByName(dropped_name);
      //    }
      //    else
      //    {
      //       // BfGuiCreateWindow::instance()->removeByName(dropped_name);
      //       __f_root_delete(dropped_name);
      //    }
      //
      //    /*
      //       Меняем перемещенному окну 'root'-указатель и 'root'-имя
      //    */
      //    (*__containers.rbegin())->root() =
      //        shared_from_this()->weak_from_this();
      // }
      //
      ImGui::EndDragDropTarget();
   }
}

//
//
//
//
//

void BfGuiCreateWindowBladeBase::__setContainersPos()
{
   ImVec2 avail = size();
   avail.x -= ImGui::GetStyle().WindowPadding.x * 2;
   avail.y -= ImGui::GetStyle().WindowPadding.y * 2;

   float next_container_h = ImGui::GetStyle().WindowPadding.y * 5;
   for (auto& c : __containers)
   {
      c->pos().y = pos().y + next_container_h;
      next_container_h += c->size().y - 25.0f;
   }
}
//
void BfGuiCreateWindowBladeBase::__renderChildContent()
{
   __setContainersPos();
}

BfGuiCreateWindowBladeBase::BfGuiCreateWindowBladeBase(
    BfGuiCreateWindowContainer::wptrContainer root, bool is_target)
    : BfGuiCreateWindowContainerObj{root, is_target}

{
}
