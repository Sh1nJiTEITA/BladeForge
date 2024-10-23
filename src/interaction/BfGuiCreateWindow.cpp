#include "BfGuiCreateWindow.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <tuple>

#include "bfIconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_internal.h"

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

ImGuiWindow* BfGuiCreateWindowContainer::__findCurrentWindowPointer()
{
   return ImGui::FindWindowByName(__str_id.c_str());
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
      __renderClildContent();
      __updatePosition();
      is_hovered = ImGui::IsWindowHovered();
   }
   ImGui::EndChild();
   __updateResizeButtonSize();
   return is_hovered;
}

void BfGuiCreateWindowContainer::__renderClildContent() {}

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

      if (ImGui::Begin(
              __str_id.c_str(),
              &__is_render,
              ImGuiWindowFlags_NoTitleBar      //
                  | ImGuiWindowFlags_NoResize  //
                  | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove |
                  ImGuiWindowFlags_NoScrollWithMouse |
                  ImGuiWindowFlags_NoScrollbar

              ))
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

bool BfGuiCreateWindowContainer::isEmpty() { return __containers.empty(); }

void BfGuiCreateWindowContainer::resetResizeHover()
{
   __is_resizing_hovered_h = false;
   __is_resizing_hovered_v = false;

   ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
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

//
//
//
//
//
//
//

bool BfGuiCreateWindowContainerObj::__is_moving_container = false;

void BfGuiCreateWindowContainerObj::__renderClildContent()
{
   ImGui::SetCursorPosX(ImGui::GetWindowWidth() -
                        ImGui::GetStyle().WindowPadding.x * 2.0f - 5.0f);

   __renderDragDropSource();

   if (__is_moving_container && !__is_current_moving)
   {
      ImGui::SameLine();
      ImGui::SetCursorPosX(ImGui::GetStyle().WindowPadding.x);
      ImGui::Text("<---> Add here <--->");
   }
   else
   {
      ImGui::SameLine();
      ImGui::SetCursorPosX(ImGui::GetStyle().WindowPadding.x);
      ImGui::Text("%s", name());
   }

   __renderDragDropTarget();

   if (ImGui::Button("Add container"))
   {
      auto new_container = std::make_shared<BfGuiCreateWindowContainerObj>(
          shared_from_this()->weak_from_this());
      new_container->__window_size.x -= 20.0f;
      new_container->__window_size.y -= 20.0f;
      __containers.push_back(std::move(new_container));
   }
   for (auto c : __containers)
   {
      ImGui::Text("%s", c->name());
   }
}

void BfGuiCreateWindowContainerObj::__renderDragDropSource()
{
   __is_current_moving = false;
   if (ImGui::Button(ICON_FA_ARROWS_SPIN))
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
            BfGuiCreateWindow::instance()->removeByName(dropped_name);
         }

         /*
            Меняем перемещенному окну 'root'-указатель и 'root'-имя
         */
         // (*__containers.rbegin())->__str_root_name = this->name();
         (*__containers.rbegin())->root() =
             shared_from_this()->weak_from_this();
      }
      ImGui::EndDragDropTarget();
   }
}

BfGuiCreateWindowContainerObj::BfGuiCreateWindowContainerObj(
    BfGuiCreateWindowContainer::wptrContainer root)
    : BfGuiCreateWindowContainer(root)
{
}
//
//
//
//
//
//
//

BfGuiCreateWindow* BfGuiCreateWindow::__instance = nullptr;

BfGuiCreateWindow::BfGuiCreateWindow()
{
   if (__instance)
   {
      throw std::runtime_error(
          "Create window was already created, "
          "could not be 2 at the same time");
   }

   __instance = this;
}

BfGuiCreateWindow* BfGuiCreateWindow::instance() noexcept { return __instance; }

void BfGuiCreateWindow::__renderManagePanel()
{
   ImGui::BeginChild("##CreateWindowManagePanel", {}, true);
   {
      if (ImGui::Button("Add container"))
      {
         __addBlankContainer();
      }
   }
   ImGui::EndChild();
}

void BfGetWindowsUnderMouse(std::vector<ImGuiWindow*>& www)
{
   ImVec2 mouse_pos = ImGui::GetMousePos();

   for (auto window = ImGui::GetCurrentContext()->Windows.begin();
        window != ImGui::GetCurrentContext()->Windows.end();
        ++window)
   {
      if ((*window)->Active && (*window)->Hidden == false &&
          (*window)->ParentWindow == nullptr)
      {
         ImVec2 min = (*window)->Pos;
         ImVec2 max = min;
         max.x += (*window)->Size.x;
         max.y += (*window)->Size.y;

         if (ImGui::IsMouseHoveringRect(min, max))
         {
            www.push_back(*window);
         }
      }
   }
}

void BfGuiCreateWindow::__renderContainers()
{
   std::stack<BfGuiCreateWindowContainer::ptrContainer> render_stack;
   std::list<std::pair<std::string, int>>               l;

   BfGuiCreateWindowContainer::resetResizeHover();
   int level = 0;
   for (auto container : __containers)
   {
      // container->clearEmptyContainers();
      render_stack.push(container);
      level = 0;
      while (!render_stack.empty())
      {
         bool is_level = true;
         //
         BfGuiCreateWindowContainer::ptrContainer current_window =
             render_stack.top();
         render_stack.pop();

         current_window->render();
         BfGuiCreateWindowContainer::changeCursorStyle();
         l.push_back({current_window->name(), level});

         for (auto child = current_window->begin();
              child != current_window->end();
              ++child)
         {
            render_stack.push(*child);
            if (is_level)
            {
               level++;
               is_level = false;
            }
         }
      }
   }

   l.sort([](std::pair<std::string, int> a, std::pair<std::string, int> b) {
      return a.second < b.second;
   });

   std::vector<ImGuiWindow*> windows_under_mouse;
   BfGetWindowsUnderMouse(windows_under_mouse);

   ImGuiWindow* last = nullptr;
   for (auto name = l.rbegin(); name != l.rend(); ++name)
   {
      bool is_found = false;
      for (auto& under : windows_under_mouse)
      {
         if (std::string(under->Name) == name->first)
         {
            ImGui::SetWindowFocus(under->Name);
            is_found = true;
            break;
         }
      }
      if (is_found) break;
   }
}

void BfGuiCreateWindow::__addBlankContainer()
{
   __containers.push_back(std::make_shared<BfGuiCreateWindowContainerObj>(
       std::weak_ptr<BfGuiCreateWindowContainer>()));
}

void BfGuiCreateWindow::removeByName(std::string name)
{
   std::erase_if(__containers,
                 [&name](auto c) { return std::string(c->name()) == name; });
}

void BfGuiCreateWindow::render()
{
   if (__is_render)
   {
      if (ImGui::Begin("Create",
                       &__is_render,
                       ImGuiWindowFlags_NoBringToFrontOnFocus |
                           ImGuiWindowFlags_NoDocking))
      {
         __renderManagePanel();
         __renderContainers();
      }
      ImGui::End();
   }
}

/*
      зарендерить все окна слоями! сначала на однмо уровне потом на другом и
   тд



*/
