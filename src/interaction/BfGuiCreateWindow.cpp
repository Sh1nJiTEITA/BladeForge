#include "BfGuiCreateWindow.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <queue>
#include <string>

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
   ImVec2 outter_pos  = ImGui::FindWindowByName(__str_root_name.c_str())->Pos;
   ImVec2 outter_size = ImGui::FindWindowByName(__str_root_name.c_str())->Size;

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
   static bool is = false;
   if (__is_invisiable_buttons
           ? ImGui::InvisibleButton(__str_top_resize_button_id.c_str(),
                                    __bot_resize_button_size)
           : ImGui::InvisibleButton(__str_top_resize_button_id.c_str(),
                                    __bot_resize_button_size))
   {
   }

   if (ImGui::IsItemHovered())
   {
      __is_resizing_hovered_v = true;
   }

   if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
   {
      is = true;
   }
   if (ImGui::IsMouseReleased(0))
   {
      is = false;
   }

   if (is)
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
   // std::cout << __str_id << " " << "ACT:" << ImGui::IsItemActive()
   //           << " DR: " << ImGui::IsMouseDragging(0)
   //           << " RES: " << __is_resizing << " HOV: " <<
   //           ImGui::IsItemHovered()
   //           << "\n";
}

bool BfGuiCreateWindowContainer::__renderChildBorder()
{
   bool is_hovered;
   ImGui::BeginChild(
       __str_child_border_id.c_str(),
       {
           __window_size.x - ImGui::GetStyle().WindowPadding.x * 2.0f -
               // __bot_resize_button_size.x * 2.0f -
               __resize_button_size.x * 2.0f -
               ImGui::GetStyle().ItemSpacing.x * 2.0f,

           __window_size.y - ImGui::GetStyle().WindowPadding.y * 2.0f -
               __bot_resize_button_size.y * 2.0f -
               //__resize_button_size.y -
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
   // __popStyle();
   __updateResizeButtonSize();
   return is_hovered;
}

void BfGuiCreateWindowContainer::__renderClildContent() {}

BfGuiCreateWindowContainer::BfGuiCreateWindowContainer(std::string root_name)
    : __str_root_name{root_name}
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
      // __is_resizing_hovered_h = false;
      // __is_resizing_hovered_v = false;

      __clampPosition();

      if (__is_first_render)
      {
         __is_first_render = false;
         ImGui::SetNextWindowSize(__window_size);
         ImGuiWindow* root = ImGui::FindWindowByName(__str_root_name.c_str());
         ImVec2       pos  = root->Pos;
         pos.x += root->Size.x * 0.5f;
         pos.y += root->Size.y * 0.5f;
         ImGui::SetNextWindowPos(pos);
         __window_pos = pos;
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
         // __changeCursorStyle();
      }
      ImGui::End();
   }
   return is_window_hovered;
}

const char* BfGuiCreateWindowContainer::name() { return __str_id.c_str(); }

bool BfGuiCreateWindowContainer::isEmpty() { return __containers.empty(); }

void BfGuiCreateWindowContainer::resetResizeHover()
{
   // std::cout << __is_resizing_hovered_h << " " << __is_resizing_hovered_v
   // << "\n";

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

//
//
//
//
//
//
//

void BfGuiCreateWindowContainerObj::__renderClildContent()
{
   ImGui::SeparatorText("Object");
   if (ImGui::Button("Add container"))
   {
      auto new_container =
          std::make_shared<BfGuiCreateWindowContainerObj>(__str_id);
      new_container->__window_size.x -= 20.0f;
      new_container->__window_size.y -= 20.0f;
      __containers.push_back(std::move(new_container));
   }
}

// void BfGuiCreateWindowContainerObj::__renderChildWindows()
// {
//    ImGuiWindow* current = ImGui::FindWindowByName(__str_id.c_str());
//
//    if (!__containers.empty())
//    {
//       if (__containers.size() >= 2)
//       {
//          // for (auto container : __containers)
//
//          auto next_container = __containers.begin()++;
//
//          for (auto container = __containers.begin();
//               container != __containers.end();
//               ++container)
//          {
//             // ImGui::SetNextWindowFocus();
//             (*container)->render();
//
//             /*
//              Рендерить уровнями!
//             */
//             ImGui::BringWindowToDisplayBehind(
//                 current,
//                 ImGui::FindWindowByName((*next_container)->name()));
//
//             ImGui::BringWindowToDisplayBehind(
//                 ImGui::FindWindowByName((*container)->name()),
//                 ImGui::FindWindowByName((*next_container)->name()));
//             ImGui::BringWindowToDisplayFront(
//                 ImGui::FindWindowByName((*container)->name()));
//             next_container++;
//          }
//       }
//       else
//       {
//          for (auto container = __containers.begin();
//               container != __containers.end();
//               ++container)
//          {
//             // ImGui::SetNextWindowFocus();
//             (*container)->render();
//
//             ImGui::BringWindowToDisplayBehind(
//                 current,
//                 ImGui::FindWindowByName((*(container))->name()));
//          }
//       }
//    }
// }

BfGuiCreateWindowContainerObj::BfGuiCreateWindowContainerObj(
    std::string root_name)
    : BfGuiCreateWindowContainer(root_name)
{
}
//
//
//
//
//
//
//

BfGuiCreateWindow::BfGuiCreateWindow() {}

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

   for (auto name = l.rbegin(); name != l.rend(); ++name)
   {
      std::cout << name->first << " level:" << name->second << "\n";
   }

   ImGuiWindow* last = nullptr;
   for (auto name = l.rbegin(); name != l.rend(); ++name)
   {
      bool is_found = false;
      for (auto& under : windows_under_mouse)
      {
         std::cout << "hovered: " << under->Name << "\n";
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
   __containers.push_back(
       std::make_shared<BfGuiCreateWindowContainerObj>("Create"));
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
      зарендерить все окна слоями! сначала на однмо уровне потом на другом и тд



*/
