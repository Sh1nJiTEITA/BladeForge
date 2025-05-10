#include "bfGuiBody.h"
#include <bfGuiFileDialog.h>
#include <fmt/base.h>
#include <fmt/format.h>
#include <imgui.h>

namespace gui
{
// clang-format off
//
bool
presentBladeSectionTable(
   const char* label, 
   inputTableField values[], 
   const char* names[], 
   int count
)
{
   bool should_change = false;
   const auto table_flags = 0 
      | ImGuiTableFlags_NoHostExtendX 
      | ImGuiTableFlags_BordersInnerV;

   if (ImGui::BeginTable(label, 2, table_flags))
   {
      for (int i = 0; i < count; ++i)
      {
         ImGui::TableNextRow();
         ImGui::PushStyleColor(
             ImGuiCol_FrameBg,
             ImVec4{0.0f, 0.0f, 0.0f, 0.0f}
         );
         ImGui::TableSetColumnIndex(0);
         ImGui::Text("%s", names[i]);
         ImGui::TableSetColumnIndex(1);
         ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
         std::string field_name = fmt::format(
            "##bf-section-main-parameters-{}", 
            names[i]
         );

         bool sts = std::visit(
             [&field_name](auto&& arg) {
                using T = std::remove_reference_t< decltype(arg) >;
                if constexpr (std::is_same_v< T, int* >)
                {
                   return ImGui::InputInt(field_name.c_str(), arg);
                }
                else if constexpr (std::is_same_v< T, float* >)
                {
                   return ImGui::InputFloat(field_name.c_str(), arg);
                }
                else
                {
                   return false;
                }
             },
             values[i]
         );
         if (sts) {
            should_change = true;
         }
         ImGui::PopStyleColor();
      }
      ImGui::EndTable();
   }
   return should_change;
}


CenterCirclePopupReturnStatus
centerCirclesPopup(obj::section::CenterCircle& new_circle) 
{
   if (ImGui::BeginPopup("New control circle"))
   {
      ImGui::InputFloat("t", &new_circle.relativePos);
      ImGui::InputFloat("R", &new_circle.radius);

      if (ImGui::Button("OK"))
      {
         ImGui::CloseCurrentPopup();
         ImGui::EndPopup();
         return CenterCirclePopupReturnStatus::OK;
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel"))
      {
         ImGui::CloseCurrentPopup();
         ImGui::EndPopup();
         return CenterCirclePopupReturnStatus::CANCEL;
      }
      ImGui::EndPopup();
   }
   return CenterCirclePopupReturnStatus::IDLE;
}


bool presentCenterCirclesEditor(std::list< obj::section::CenterCircle >& circles) { 
   bool should_remake = false;
   // clang-format on
   ImGui::SeparatorText("Center Circles");
   if (ImGui::Button("New", {ImGui::GetContentRegionAvail().x, 0}))
   {
      ImGui::OpenPopup("New control circle");
   }

   static obj::section::CenterCircle new_circle{0, 0};
   auto sts = centerCirclesPopup(new_circle);
   if (sts == CenterCirclePopupReturnStatus::OK)
   {
      circles.push_back(new_circle);
      should_remake = true;
   };

   const float child_y = 38.0f;
   const float child_x = ImGui::GetContentRegionAvail().x;
   // clang-format on
   int i = 0;
   for (auto it = circles.begin(); it != circles.end(); ++it, ++i)
   {
      std::string child_title =
          fmt::format("##center-circle-input-data-child-name-{}", i);

      ImGui::BeginChild(child_title.c_str(), ImVec2{child_x, child_y}, true);
      {
         std::string title = fmt::format(ICON_FA_LIST " {}", i);
         ImGui::Button(title.c_str());

         if (ImGui::BeginDragDropSource())
         {
            ImGui::SetDragDropPayload(
                "center-circle-drag-drop",
                &i,
                sizeof(int)
            );
            ImGui::EndDragDropSource();
         }

         if (ImGui::BeginDragDropTarget())
         {
            if (const auto payload =
                    ImGui::AcceptDragDropPayload("center-circle-drag-drop"))
            {
               IM_ASSERT(payload->DataSize == sizeof(int));
               int n = *static_cast< const int* >(payload->Data);

               // Swap elements via iterators
               auto it_n = circles.begin();
               std::advance(it_n, n);
               std::swap(*it, *it_n);

               should_remake = true;
            }
            ImGui::EndDragDropTarget();
         }

         if (ImGui::BeginPopupContextItem())
         {
            ImGui::SetNextItemWidth(300.0f);
            if (ImGui::MenuItem("\xef\x80\x8d  Delete"))
            {
               it->relativePos = std::numeric_limits< float >::quiet_NaN();
               it->radius = std::numeric_limits< float >::quiet_NaN();
               should_remake = true;
            }
            ImGui::EndPopup();
         }

         ImGui::SameLine();
         float region_x = (ImGui::GetContentRegionAvail().x -
                           ImGui::GetStyle().ItemSpacing.x) *
                          0.5f;

         ImGui::SetNextItemWidth(region_x);
         const auto input_title_t =
             fmt::format("##center-circle-input-data-t{}", i);
         if (ImGui::DragFloat(
                 input_title_t.c_str(),
                 &it->relativePos,
                 0.001f,
                 0,
                 1,
                 "%.3f"
             ))
         {
            should_remake = true;
         }
         ImGui::SameLine();
         ImGui::SetNextItemWidth(region_x);
         const auto input_title_r =
             fmt::format("##center-circle-input-data-r{}", i);
         if (ImGui::DragFloat(
                 input_title_r.c_str(),
                 &it->radius,
                 0.001f,
                 0,
                 +FLT_MAX,
                 "%.3f"
             ))
         {
            should_remake = true;
         }
      }
      ImGui::EndChild();
   }
   // clang-format on
   return should_remake;
}

auto
makeTitle(const char* initial, uint32_t index) -> std::string
{
   return fmt::format("{}-{}", initial, index);
};

bool
presentBladeSectionCreateWindow(pSection sec)
{

   auto index = sec->id();

   static ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_NoUndocking;
   ImGuiWindowFlags window_flags =
       ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar;

   std::string this_window_title = makeTitle("MainDockSpaceHost", index);

   ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_FirstUseEver);
   ImGui::Begin(this_window_title.c_str(), nullptr, window_flags);

   std::string dockspace_title = makeTitle("BladeSectionDockspace", index);
   ImGuiID dockspace_id = ImGui::GetID(dockspace_title.c_str());
   ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dock_flags);

   static bool built = 0;
   if (built)
   {
      ImGui::DockBuilderRemoveNode(dockspace_id);
      ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());

      ImGuiID left_id = ImGui::DockBuilderSplitNode(
          dockspace_id,
          ImGuiDir_Left,
          0.35f,
          nullptr,
          &dockspace_id
      );
      ImGuiID right_id = dockspace_id;

      // clang-format off
      ImGui::DockBuilderDockWindow(makeTitle("Parameters", index).c_str(), left_id);
      ImGui::DockBuilderDockWindow(makeTitle("Preview", index).c_str(), right_id);
      ImGui::DockBuilderDockWindow(makeTitle("Image", index).c_str(), right_id);
      ImGui::DockBuilderDockWindow(makeTitle("Type Manager", index).c_str(), right_id);
      ImGui::DockBuilderFinish(dockspace_id);
      // clang-format on
   }

   ImGui::End();

   auto info = sec->info().getp();

   bool no_remake = true;
   // Left panel
   if (ImGui::Begin(makeTitle("Parameters", index).c_str()))
   {
      // clang-format off
      inputTableField outer_values[] = { &info->chord, &info->installAngle };
      const char* outer_names[] = {"Chord", "Install Angle"};
      no_remake *= !presentBladeSectionTable("Outer", outer_values, outer_names, 2);

      ImGui::SeparatorText("Average curve");
      inputTableField ave_values[] = {&info->initialBezierCurveOrder};
      const char* ave_names[] = {"Average curve order" };
      no_remake *= !presentBladeSectionTable("Ave", ave_values, ave_names, 1);

      ImGui::SeparatorText("Inlet");
      inputTableField inlet_values[] = {&info->inletAngle, &info->inletRadius};
      const char* inlet_names[] = {"Inlet Angle", "Inlet Radius"};
      no_remake *= !presentBladeSectionTable("Inlet", inlet_values, inlet_names, 2);

      ImGui::SeparatorText("Outlet");
      inputTableField outlet_values[] = {&info->outletAngle, &info->outletRadius};
      const char* outlet_names[] = {"Outlet Angle", "Outlet Radius"};
      no_remake *= !presentBladeSectionTable("Outlet", outlet_values, outlet_names, 2);
      no_remake *= !presentCenterCirclesEditor(info->centerCircles);
      // clang-format on
   }
   ImGui::End();

   // Right panel
   if (ImGui::Begin(makeTitle("Preview", index).c_str()))
   {
      // clang-format off
      // no_remake *= renderBitCheckbox("Chord",             static_cast<uint32_t>(obj::section::BfBladeSectionEnum::Chord), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("_ChordLeftBorder",  static_cast<uint32_t>(obj::section::BfBladeSectionEnum::_ChordLeftBorder), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("_ChordRightBorder", static_cast<uint32_t>(obj::section::BfBladeSectionEnum::_ChordRightBorder), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("InletCircle",       static_cast<uint32_t>(obj::section::BfBladeSectionEnum::InletCircle), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("OutletCircle",      static_cast<uint32_t>(obj::section::BfBladeSectionEnum::OutletCircle), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("InletDirection",    static_cast<uint32_t>(obj::section::BfBladeSectionEnum::InletDirection), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("OutletDirection",   static_cast<uint32_t>(obj::section::BfBladeSectionEnum::OutletDirection), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("AverageInitialCurve", static_cast<uint32_t>(obj::section::BfBladeSectionEnum::AverageInitialCurve), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("CenterCircles",     static_cast<uint32_t>(obj::section::BfBladeSectionEnum::CenterCircles), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("InletPack",          static_cast<uint32_t>(obj::section::BfBladeSectionEnum::InletPack), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("OutletPack",         static_cast<uint32_t>(obj::section::BfBladeSectionEnum::OutletPack), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("FrontCurveChain",    static_cast<uint32_t>(obj::section::BfBladeSectionEnum::FrontCurveChain), &info->renderBitSet);
      // no_remake *= renderBitCheckbox("BackCurveChain",     static_cast<uint32_t>(obj::section::BfBladeSectionEnum::BackCurveChain), &info->renderBitSet);
      // clang-format on
   }
   ImGui::End();

   return !no_remake;
}

bool
presentImageControlWindow(pImage img)
{
   ImGui::Begin("Image");
   {
      static fs::path file_path;
      static bool is_searching = false;
      img->quad()->presentContextMenu();
      if (ImGui::Button("Open Image"))
      {
         BfGuiFileDialog::instance()->openFile(
             &file_path,
             /* {".*"} */ {".jpeg", ".png", ".jpg"}
         );
         is_searching = true;
      }
      ImGui::SameLine();
      ImGui::Text("Path: %s", file_path.string().c_str());

      if (!BfGuiFileDialog::instance()->isActive() && is_searching)
      {
         is_searching = false;
         fmt::println("Found path: {}", file_path.string());
         if (fs::exists(file_path))
         {
            auto& man = base::desc::own::BfDescriptorPipelineDefault::manager();
            auto& texture = man.get< base::desc::own::BfDescriptorTextureTest >(
                base::desc::own::SetType::Texture,
                0
            );
            texture.reload(file_path);
            man.updateSets();
         }
      }
   }
   ImGui::End();
   return false;
}

bool
presentBodyCreateWindow(pBody body)
{
   static bool popen = true;
   if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) |
        ImGui::IsKeyDown(ImGuiKey_RightCtrl)) &&
       ImGui::IsKeyReleased(ImGuiKey_E))
   {
      fmt::println("Toggle create window : {}", popen);
      popen = !popen;
   }

   if (!popen)
      return false;

   std::vector< uint64_t > enabled;

   bool should_remake = false;
   ImGui::Begin("Create window", &popen);
   {
      if (ImGui::Button("Add empty section"))
      {
         body->addSection();
         should_remake = true;
      }

      const float child_y = 38.0f;
      const float child_x = ImGui::GetContentRegionAvail().x;
      uint64_t i = 0;

      // clang-format off
      for (auto sec = body->beginSection(); sec != body->endSection(); ++sec, ++i)
      {
         
         
      }
      // clang-format on
   }
   ImGui::End();

   return should_remake;
}

}; // namespace gui
