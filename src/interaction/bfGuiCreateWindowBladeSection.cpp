#include "bfGuiCreateWindowBladeSection.h"

#include <memory>

#include "bfBladeSection.h"
#include "bfLayerHandler.h"
#include "imgui.h"

BfGuiCreateWindowBladeSection::BfGuiCreateWindowBladeSection(
    BfGuiCreateWindowContainer::wptrContainer root, bool is_target)
    : __section_name{"Section"}, BfGuiCreateWindowContainerObj(root, is_target)
{
   __window_size  = {300, 100};
   __is_collapsed = false;
   bfFillBladeSectionStandart(&__create_info);

   // FIX: Fix in future: doublign layer create infos here and inside
   // BfGuiCreateWindowContainerObj
   __create_info.layer_create_info = __layer_create_info;

   // __layer_choser = std::make_shared<BfGuiCreateWindowContainerPopup>(
   //     shared_from_this(),
   //     [&]() { this->__renderAvailableLayers(); });
   // // __layer_choser->hide();
   //
   // // add(std::dynamic_pointer_cast<BfGuiCreateWindowContainer>());
   // this->add(std::make_shared<BfGuiCreateWindowContainerPopup>(
   //     shared_from_this(),
   //     [&]() { this->__renderAvailableLayers(); }));
}

void BfGuiCreateWindowBladeSection::__renderHeaderName()
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

   // if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
   // // if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
   // {
   //    isEditing = true;
   // }
   ImGui::SameLine();
   ImGui::SetCursorPos(ImGui::GetCursorStartPos());
   // if (isEditing)
   // {
   ImGui::SetNextItemWidth(x);
   if (ImGui::InputText("##edit",
                        __section_name.data(),
                        ImGuiInputTextFlags_EnterReturnsTrue))
   {
   }
   //    {
   //    }
   //    if (!ImGui::IsItemActive())
   //    {
   //       isEditing = false;
   //    }
   // }
   // else
   // {
   //    ImGui::Text("%s", __section_name.c_str());
   // }

   ImGui::PopID();
}
void BfGuiCreateWindowBladeSection::__renderChildContent()
{
   if (ImGui::Button("Settings"))
   {
      __is_settings = !__is_settings;
   }
   ImGui::SameLine();

   if (ImGui::Button("Add to layer"))
   {
      // __layer_choser->toggleRender();
      if (!__layer_choser)
      {
         __layer_choser = std::make_shared<BfGuiCreateWindowContainerPopup>(
             shared_from_this(),
             [&]() { this->__renderAvailableLayers(); });
         this->add(__layer_choser);
      }
      else
      {
         __layer_choser->toggleRender();
      }
   }

   if (__is_settings)
   {
      __renderSettingsWindow();
   }
   else
   {
      __renderSettings();
   }
}

void BfGuiCreateWindowBladeSection::__renderSettingsWindow()
{
   ImGui::SetNextWindowPos({pos().x + 50.0f, pos().y + 50.0f},
                           ImGuiCond_Appearing);
   ImGui::Begin((std::string("Settings ") + __section_name + name()).c_str());
   {
      __renderSettings();
   }
   ImGui::End();
}

void BfGuiCreateWindowBladeSection::__renderSettings()
{
   auto renderRow = [&](const char* var_name,
                        const char* des_name,
                        const char* units_name,
                        float*      value) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%s", var_name);
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s", des_name);
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%s", units_name);
      ImGui::TableSetColumnIndex(3);

      ImGui::PushID(var_name);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
      {
         ImGui::PushItemWidth(-FLT_MIN);
         ImGui::InputFloat("##value", value);
      }
      ImGui::PopStyleColor();
      ImGui::PopID();
   };

   ImGui::BeginTable("Section Settings",
                     4,
                     ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
   {
      ImGui::TableNextRow();
      {  // HEADER BEGIN

         ImGui::TableSetColumnIndex(0);
         ImGui::TableHeader("Variable");

         ImGui::TableSetColumnIndex(1);
         ImGui::TableHeader("Designation");

         ImGui::TableSetColumnIndex(2);
         ImGui::TableHeader("Units");

         ImGui::TableSetColumnIndex(3);
         ImGui::TableHeader("Value");
      }  // HEADER

      renderRow("Width", "B", "[m]", &__create_info.width);
      renderRow("Install angle",
                BF_GREEK_ALPHA "_y",
                "[deg]",
                &__create_info.install_angle);
      renderRow("Inlet angle",
                BF_GREEK_BETA "_1",
                "[deg]",
                &__create_info.inlet_angle);
      renderRow("Outlet angle",
                BF_GREEK_BETA "_2",
                "[deg]",
                &__create_info.outlet_angle);

      renderRow("Inlet surface angle",
                BF_GREEK_OMEGA "_1",
                "[deg]",
                &__create_info.inlet_surface_angle);
      renderRow("Outlet surface angle",
                BF_GREEK_OMEGA "_2",
                "[deg]",
                &__create_info.outlet_surface_angle);
      renderRow("Inlet radius", "r_1", "[m]", &__create_info.inlet_radius);
      renderRow("Outlet radius", "r_2", "[m]", &__create_info.outlet_radius);
      renderRow("Border length",
                BF_GREEK_DELTA_,
                "[m]",
                &__create_info.border_length);

      ImGui::EndTable();
   }
}

// void BfGuiCreateWindowBladeSection::__renderAvailableLayers() {}

void BfGuiCreateWindowBladeSection::__createObj()
{
   __layer_obj = std::make_shared<BfBladeSection>(&__create_info);
}

void BfGuiCreateWindowBladeSection::__addToLayer(
    std::shared_ptr<BfDrawLayer> add_to)
{
   auto lh = BfLayerHandler::instance();
   add_to->add(__layer_obj);
   add_to->update_buffer();
}

void BfGuiCreateWindowBladeSection::__processDragDropTarget()
{
   if (ImGui::BeginDragDropTarget())
   {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("Container"))
      {
         std::shared_ptr<BfGuiCreateWindowContainerObj> dropped_container =
             *(std::shared_ptr<BfGuiCreateWindowContainerObj>*)payload->Data;

         if (auto other_section =
                 std::dynamic_pointer_cast<BfGuiCreateWindowBladeSection>(
                     dropped_container))
         {
            __swapFunc(other_section->name(), this->name());
         }
         else
         {
         }
      }
      ImGui::EndDragDropTarget();
   }
}
