#include "BfGuiCreateWindow.h"

#include <string>

#include "imgui.h"
#include "imgui_internal.h"

void BfGuiCreateWindowContainer::__pushStyle()
{
   ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 2.0f);
   ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 2.0f);
}
void BfGuiCreateWindowContainer::__popStyle() { ImGui::PopStyleVar(2); }

void BfGuiCreateWindowContainer::__clampPosition()
{
   ImVec2 outter_pos  = ImGui::FindWindowByName("Create")->Pos;
   ImVec2 outter_size = ImGui::FindWindowByName("Create")->Size;

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

void BfGuiCreateWindowContainer::__changeCursorStyle()
{
   if (__is_resizing_hovered_v)
   {
      ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
      return;
   }

   if (__is_resizing_hovered_h)
   {
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
   if (ImGui::InvisibleButton(__str_left_resize_button_id.c_str(),
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
   if (ImGui::InvisibleButton(__str_right_resize_button_id.c_str(),
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
   if (ImGui::InvisibleButton(__str_bot_resize_button_id.c_str(),
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
   if (ImGui::InvisibleButton(__str_top_resize_button_id.c_str(),
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
      __window_pos.y += ImGui::GetIO().MouseDelta.y;
      __window_size.y -= ImGui::GetIO().MouseDelta.y;

      ImGui::SetWindowSize(__window_size);
   }
   else
   {
      __is_resizing = false;
   }
}

void BfGuiCreateWindowContainer::__renderChildBorder()
{
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
      ImGui::SliderFloat("Border X", &__resize_button_size.x, 0.1f, 20.0f);
      ImGui::SliderFloat("Border Y", &__bot_resize_button_size.y, 0.1f, 20.0f);
      __updateResizeButtonSize();

      ImGui::Text("size.x = %s",
                  std::to_string(__window_size.x -
                                 ImGui::GetStyle().WindowPadding.x * 2.0f -
                                 __bot_resize_button_size.x * 2 -
                                 ImGui::GetStyle().ItemSpacing.x * 2)
                      .c_str());

      ImGui::Text("wsize = %s, %s",
                  std::to_string(ImGui::GetWindowSize().x).c_str(),
                  std::to_string(ImGui::GetWindowSize().y).c_str()),

          __updatePosition();
   }
   ImGui::EndChild();
   // __popStyle();
   __updateResizeButtonSize();
}

BfGuiCreateWindowContainer::BfGuiCreateWindowContainer()
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

void BfGuiCreateWindowContainer::render()
{
   if (__is_render)
   {
      __is_resizing_hovered_h = false;
      __is_resizing_hovered_v = false;

      __clampPosition();

      if (__is_first_render)
      {
         __is_first_render = false;
         ImGui::SetNextWindowSize(__window_size);
      }

      if (ImGui::Begin(__str_id.c_str(),
                       &__is_render,
                       ImGuiWindowFlags_NoTitleBar      //
                           | ImGuiWindowFlags_NoResize  //
                           | ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoScrollWithMouse |
                           ImGuiWindowFlags_NoScrollbar))
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
         __renderChildBorder();
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
         __changeCursorStyle();
      }
      ImGui::End();
   }
}

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

void BfGuiCreateWindow::__renderContainers()
{
   for (auto& container : __containers)
   {
      container.render();
   }
}

void BfGuiCreateWindow::__addBlankContainer()
{
   __containers.push_back(std::move(BfGuiCreateWindowContainer()));
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
