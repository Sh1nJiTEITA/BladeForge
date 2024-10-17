#include "BfGuiCreateWindow.h"

#include "imgui.h"
#include "imgui_internal.h"

void BfGuiCreateWindowContainer::__clampPosition()
{
   ImVec2 outter_pos  = ImGui::FindWindowByName("Create")->Pos;
   ImVec2 outter_size = ImGui::FindWindowByName("Create")->Size;

   ImVec2 delta_pos{outter_pos.x - __old_outter_pos.x,
                    outter_pos.y - __old_outter_pos.y};

   bool is_outter_changed =
       outter_pos.x != __old_outter_pos.x || outter_pos.y != __old_outter_pos.y;

   // if (is_outter_changed)
   // {
   //    delta_pos.x += ;
   //    delta_pos.y += ;
   // }
   //
   ImVec2 new_pos = {
       std::clamp(__window_pos.x + delta_pos.x,
                  outter_pos.x,
                  outter_pos.x + outter_size.x - __window_size.x),
       std::clamp(__window_pos.y + delta_pos.y,
                  outter_pos.y,
                  outter_pos.y + outter_size.y - __window_size.y)};

   // ImGui::SetWindowPos(__findCurrentWindowPointer(), current_pos);
   //
   __old_outter_pos = outter_pos;

   ImGui::SetNextWindowPos(new_pos);
   ImGui::SetNextWindowSize(__window_size);
   __window_pos = new_pos;
}

void BfGuiCreateWindowContainer::__updatePosition()
{
   if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(0))
   {
      __window_pos.x += ImGui::GetIO().MouseDelta.x;
      __window_pos.y += ImGui::GetIO().MouseDelta.y;
   }
}

ImGuiWindow* BfGuiCreateWindowContainer::__findCurrentWindowPointer()
{
   return ImGui::FindWindowByName(__str_id.c_str());
}

BfGuiCreateWindowContainer::BfGuiCreateWindowContainer()
{
   static uint32_t growing_id = 0;
   //
   __str_id = std::format("##CreateWindow_{}", std::to_string(growing_id++));
}

void BfGuiCreateWindowContainer::render()
{
   if (__is_render)
   {
      __clampPosition();
      if (ImGui::Begin(__str_id.c_str(), &__is_render))
      {
         __updatePosition();

         // __window_size = ImGui::GetWindowSize();
      }
      ImGui::End();
   }
}

BfGuiCreateWindow::BfGuiCreateWindow()
{
   int a = 0;
   int b = 0;
   int c = 0;
}

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
