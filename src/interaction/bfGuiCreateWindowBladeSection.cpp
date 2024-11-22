#include "bfGuiCreateWindowBladeSection.h"

#include "bfBladeSection.h"
#include "bfGuiCreateWindowBladeBase.h"
#include "bfLayerHandler.h"
#include "imgui.h"

BfGuiCreateWindowBladeSection::BfGuiCreateWindowBladeSection(
    wptrContainer root, bool is_target
)
    : __section_name{"Section"}, BfGuiCreateWindowContainerObj(root, is_target)
{
   __window_size = {500, 80};
   __old_size = {500, 350};
   bfFillBladeSectionStandart(&__create_info);

   // FIX: Fix in future: doublign layer create infos here and inside
   // BfGuiCreateWindowContainerObj
   __create_info.layer_create_info = __layer_create_info;
   __is_button &= ~BfGuiCreateWindowContainer_ButtonType_Top;
   __is_button &= ~BfGuiCreateWindowContainer_ButtonType_Bot;
   // __layer_choser = std::make_shared<BfGuiCreateWindowContainerPopup>(
   //     shared_from_this(),
   //     [&]() { this->__renderAvailableLayers(); });
   // // __layer_choser->hide();
   //
   // // add(std::dynamic_pointer_cast<BfGuiCreateWindowContainer>());
   // this->add(std::make_shared<BfGuiCreateWindowContainerPopup>(
   //     shared_from_this(),
   //     [&]() { this->__renderAvailableLayers(); }));
   this->__is_collapsed = true;
}

void
BfGuiCreateWindowBladeSection::setView(viewMode mode) noexcept
{
   __mode = mode;
}

BfBladeSectionCreateInfo
BfGuiCreateWindowBladeSection::createInfo() const noexcept
{
   return __create_info;
}

void
BfGuiCreateWindowBladeSection::clearSinglePopup(uint32_t id)
{
   std::erase_if(__containers, [&id](auto c) { return c->id() == id; });
}

void
BfGuiCreateWindowBladeSection::clearPopups()
{
   // NOTE: Not effective! by readeable
   if (__layer_choser.get())
   {
      clearSinglePopup(__layer_choser->id());
   }
}

float&
BfGuiCreateWindowBladeSection::zCoordinate()
{
   return __create_info.start_vertex.z;
}

void
BfGuiCreateWindowBladeSection::__renderHeaderName()
{
   uint32_t* selected_id = nullptr;

   if (auto shared_root = __root_container.lock())
   {
      if (auto casted_root =
              std::dynamic_pointer_cast<BfGuiCreateWindowBladeBase>(shared_root
              ))
      {
         selected_id = casted_root->selectedId();
      }
   }

   if (__mode == viewMode::SHORT)
   {
      float x = ImGui::GetWindowWidth() -
                ImGui::GetStyle().WindowPadding.x * 2.0f -
                ImGui::CalcTextSize(ICON_FA_WINDOW_RESTORE).x -
                ImGui::CalcTextSize(ICON_FA_MINIMIZE).x -
                ImGui::CalcTextSize(ICON_FA_INFO).x - 50.0f - 30.0f;

      ImGui::PushID(__str_id.c_str());
      {
         if (ImGui::RadioButton(
                 (std::string("##") + __str_id).c_str(),
                 __id == (selected_id ? *selected_id : false)
             ))
         {
            *selected_id = __id;
         }
      }
      ImGui::PopID();

      ImGui::SameLine();
      ImGui::PushID(name());

      float tmp_cursor_x = ImGui::GetCursorPosX();
      ImGui::Dummy({x, 20});
      if (ImGui::IsItemHovered())
      {
         ImGui::SetTooltip("Change name...");
      }
      ImGui::SameLine();
      ImGui::SetCursorPosX(tmp_cursor_x);
      ImGui::SetNextItemWidth(x);

      ImGui::PushStyleColor(ImGuiCol_FrameBg, {1.0, 1.0f, 1.0f, 0.0f});
      ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, {1.0, 1.0f, 1.0f, 0.2f});
      ImGui::PushStyleColor(ImGuiCol_FrameBgActive, {1.0, 1.0f, 1.0f, 0.5f});
      if (ImGui::InputText(
              "##edit",
              __section_name.data(),
              ImGuiInputTextFlags_EnterReturnsTrue
          ))
      {
      }
      ImGui::PopStyleColor(3);

      ImGui::PopID();
   }
   else if (__mode == viewMode::STD)
   {
      ImGui::SameLine();
      ImGui::PushID(name());
      {
         ImGui::SetNextItemWidth(
             ImGui::GetContentRegionAvail().x - __totalHeaderButtonsSize().x
         );
         if (ImGui::InputText(
                 "##edit",
                 __name.data(),
                 ImGuiInputTextFlags_EnterReturnsTrue
             ))
         {
         }
      }
      ImGui::PopID();
   }
}
void
BfGuiCreateWindowBladeSection::__renderChildContent()
{
   if (__mode == viewMode::STD)
   {
      if (ImGui::Button("Settings"))
      {
         __is_settings = !__is_settings;
      }
      ImGui::SameLine();

      if (ImGui::Button("Add to layer"))
      {
         if (!__layer_choser)
         {
            __layer_choser = std::make_shared<BfGuiCreateWindowContainerPopup>(
                shared_from_this(),
                BfGuiCreateWindowContainerPopup_Side_Right,
                false,
                [&](wptrContainer root) {
                   BfGuiSmartLayerObserver::instance()->renderChoser([&]() {
                      size_t selected_id =
                          BfGuiSmartLayerObserver::instance()->selectedLayer();
                      std::shared_ptr<BfDrawLayer> selected_layer =
                          BfLayerHandler::instance()->get_layer_by_id(
                              selected_id
                          );
                      // __createObj();
                      __addToLayer(selected_layer);
                      __layer_choser->toggleRender(false);
                   });
                }
            );
            this->add(__layer_choser);
         }
         else
         {
            __layer_choser->toggleRender();
         }
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

void
BfGuiCreateWindowBladeSection::__renderSettingsWindow()
{
   ImGui::SetNextWindowPos(
       {pos().x + 50.0f, pos().y + 50.0f},
       ImGuiCond_Appearing
   );
   ImGui::Begin((std::string("Settings ") + __section_name + name()).c_str());
   {
      __renderSettings();
   }
   ImGui::End();
}

void
BfGuiCreateWindowBladeSection::__renderSettings()
{
   auto renderRow = [&](const char* var_name,
                        const char* des_name,
                        const char* units_name,
                        float* value) {
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

   ImGui::BeginTable(
       "Section Settings",
       4,
       // ImGuiTableFlags_Borders |
       // ImGuiTableFlags_RowBg |
       // ImGuiTableFlags_Borders |
       ImGuiTableFlags_SizingFixedFit
   );
   {
      ImGui::TableSetupColumn("Variable", 0, 200.0f);
      ImGui::TableSetupColumn("Designation", 0, 50.0f);
      ImGui::TableSetupColumn("Units", 0, 50.0f);
      ImGui::TableSetupColumn("Value", 0, 100.0f);

      ImGui::TableHeadersRow();

      ImGui::TableNextRow();
      ImGui::TableNextRow();
      renderRow("Width", "B", "[m]", &__create_info.width);
      renderRow(
          "Install angle",
          BF_GREEK_ALPHA "_y",
          "[deg]",
          &__create_info.install_angle
      );

      ImGui::TableNextRow();
      ImGui::TableNextRow();
      renderRow(
          "Inlet angle",
          BF_GREEK_BETA "_1",
          "[deg]",
          &__create_info.inlet_angle
      );
      renderRow(
          "Outlet angle",
          BF_GREEK_BETA "_2",
          "[deg]",
          &__create_info.outlet_angle
      );

      ImGui::TableNextRow();
      ImGui::TableNextRow();
      renderRow(
          "Inlet surface angle",
          BF_GREEK_OMEGA "_1",
          "[deg]",
          &__create_info.inlet_surface_angle
      );
      renderRow(
          "Outlet surface angle",
          BF_GREEK_OMEGA "_2",
          "[deg]",
          &__create_info.outlet_surface_angle
      );

      ImGui::TableNextRow();
      ImGui::TableNextRow();
      renderRow("Inlet radius", "r_1", "[m]", &__create_info.inlet_radius);
      renderRow("Outlet radius", "r_2", "[m]", &__create_info.outlet_radius);

      ImGui::TableNextRow();
      ImGui::TableNextRow();
      renderRow(
          "Border length",
          BF_GREEK_DELTA_,
          "[m]",
          &__create_info.border_length
      );

      ImGui::EndTable();
   }
}

void
BfGuiCreateWindowBladeSection::__renderZCoordinate()
{
}

void
BfGuiCreateWindowBladeSection::__createObj()
{
   __layer_obj = std::make_shared<BfBladeSection>(&__create_info);
}

void
BfGuiCreateWindowBladeSection::__processDragDropTarget()
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
                     dropped_container
                 ))
         {
            __swapFunc(other_section->name(), this->name());
            clearPopups();
         }
         else
         {
         }
      }
      ImGui::EndDragDropTarget();
   }
}

void
BfGuiCreateWindowBladeSection::__renderInfoTooltip()
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

   std::string root_layer_name = "";
   if (auto shared_root = __ptr_root.lock())
   {
      root_layer_name = BfGuiSmartLayerObserver::layerName(shared_root);
   }

   ImGui::SetTooltip(
       "Root container: %s\n"
       "Inner containers:\n%s\n"
       "Root DrawLayer: %s\n"
       "Is force render: %s",
       root_name.c_str(),
       total_containers.c_str(),
       root_layer_name.c_str(),
       std::to_string(__is_force_render).c_str()
   );
}

void
BfGuiCreateWindowBladeSection::__prerender()
{
   if (__prerender_external_func) __prerender_external_func();
}

void
BfGuiCreateWindowBladeSection::__postrender()
{
   if (__postrender_external_func) __postrender_external_func();
}
