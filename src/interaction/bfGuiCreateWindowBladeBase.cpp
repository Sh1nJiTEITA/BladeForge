#include "bfGuiCreateWindowBladeBase.h"

void BfGuiCreateWindowBladeBase::__setContainersPos()
{
   ImVec2 avail = size();
   //

   float next_container_h = ImGui::GetStyle().WindowPadding.y * 5;
   for (auto& c : __containers)
   {
      c->pos().y =
          pos().y - (c->popupSize().y - c->size().y) + next_container_h;
      next_container_h += c->popupSize().y;
   }
}

void BfGuiCreateWindowBladeBase::__renderHeaderName()
{
   float x = ImGui::GetWindowWidth() -
             ImGui::GetStyle().WindowPadding.x * 2.0f -
             ImGui::CalcTextSize(ICON_FA_WINDOW_RESTORE).x -
             ImGui::CalcTextSize(ICON_FA_MINIMIZE).x -
             ImGui::CalcTextSize(ICON_FA_INFO).x - 50.0f;

   static bool isEditing = false;
   ImGui::PushID(name());
   ImGui::Dummy({x, 20});
   if (ImGui::IsItemHovered())
   {
      ImGui::SetTooltip("Change name...");
   }

   // if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(1))
   // {
   // isEditing = true;
   // }
   ImGui::SameLine();
   ImGui::SetCursorPos(ImGui::GetCursorStartPos());
   // if (isEditing)
   // {
   ImGui::SetNextItemWidth(x);
   if (ImGui::InputText("##edit",
                        __base_name.data(),
                        ImGuiInputTextFlags_EnterReturnsTrue))
   {
   }

   //    if (!ImGui::IsItemActive())
   //    {
   //       isEditing = false;
   //    }
   // }
   // else
   // {
   //    ImGui::Text("%s", __base_name.c_str());
   // }

   ImGui::PopID();
}

void BfGuiCreateWindowBladeBase::__renderChildContent()
{
   __setContainersPos();
}

BfGuiCreateWindowBladeBase::BfGuiCreateWindowBladeBase(
    BfGuiCreateWindowContainer::wptrContainer root, bool is_target)
    : __base_name{"Blade base"}, BfGuiCreateWindowContainerObj{root, is_target}
{
}

void BfGuiCreateWindowBladeBase::__processDragDropTarget()
{
   if (ImGui::BeginDragDropTarget())
   {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("Container"))
      {
         std::shared_ptr<BfGuiCreateWindowContainerObj> dropped_container =
             *(std::shared_ptr<BfGuiCreateWindowContainerObj>*)payload->Data;

         if (auto other_base =
                 std::dynamic_pointer_cast<BfGuiCreateWindowBladeBase>(
                     dropped_container))
         {
            __swapFunc(other_base->name(), this->name());
         }
         else if (auto other_section =
                      std::dynamic_pointer_cast<BfGuiCreateWindowBladeSection>(
                          dropped_container))
         {
            __moveFunc(other_section->name(), this->name());
         }
         else
         {
         }
      }
      ImGui::EndDragDropTarget();
   }
}
