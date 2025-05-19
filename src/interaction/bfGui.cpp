#include "bfGui.h"

#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fmt/base.h>
#include <fmt/format.h>
#include <glm/trigonometric.hpp>
#include <imgui_internal.h>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <variant>

#include "bfBladeBody.h"
#include "bfBladeSection2.h"
#include "bfCamera.h"
#include "bfCurves4.h"
#include "bfDrawObjectManager.h"
#include "bfIconsFontAwesome6.h"
#include "bfStep.h"
#include "bfTypeManager.h"
#include "imgui.h"

BfGui::BfGui()
{
   BfGuiFileDialog::bindInstance(&__file_dialog);
   BfConfigManager::createConfigData();
   BfConfigManager::createSavedFilesDir();
}

void
BfGui::pollEvents()
{
   while (!__queue_after_render.empty())
   {
      __queue_after_render.front()();
      __queue_after_render.pop();
   }

   __lua_interaction.pollEvents();
   __smart_layer_observer.pollEvents();
}

BfEvent
BfGui::bindBase(BfBase* base)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (base)
   {
      __ptr_base = base;
      event.action = BF_ACTION_TYPE_BIND_BASE_TO_GUI_SUCCESS;
   }
   else
   {
      event.action = BF_ACTION_TYPE_BIND_BASE_TO_GUI_FAILURE;
   }
   return event;
}

BfEvent
BfGui::bindHolder(BfHolder* handler)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (handler)
   {
      __ptr_holder = handler;
      event.action = BF_ACTION_TYPE_BIND_HOLDER_TO_GUI_SUCCESS;
   }
   else
   {
      event.action = BF_ACTION_TYPE_BIND_HOLDER_TO_GUI_FAILURE;
   }
   return event;
}

BfEvent
BfGui::bindSettings(std::filesystem::path path)
{
   auto abs_exe_directory = []() -> fs::path {
#if defined(_MSC_VER)
      wchar_t path[FILENAME_MAX] = {0};
      GetModuleFileNameW(nullptr, path, FILENAME_MAX);
      return std::filesystem::path(path).parent_path().string();
#elif defined(_WIN32)
      return std::filesystem::path(".");
#else
      char path[FILENAME_MAX];
      ssize_t count = readlink("/proc/self/exe", path, FILENAME_MAX);
      return std::filesystem::path(std::string(path, (count > 0) ? count : 0))
          .parent_path()
          .string();
#endif
   };

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (path.extension() != ".lua")
   {
      event.action = BF_ACTION_TYPE_BIND_GUI_SETTINGS_FAILURE;
      event.info =
          " input file path= " + path.string() + " extension is not '.lua'";
      return event;
   }
   path = std::filesystem::absolute(path);
   BfConfigManager::loadStdLibrary(sol::lib::base);
   BfConfigManager::loadStdLibrary(sol::lib::package);
   BfConfigManager::addPackagePath(abs_exe_directory() / "./scripts");
   BfConfigManager::loadRequireScript(path);

   sol::table settings_table = BfConfigManager::getLuaObj("guiconfig");

   BfConfigManager::fillFormFontSettings(
       settings_table["fonts"],
       &__settings_form
   );

   event.action = BF_ACTION_TYPE_BIND_GUI_SETTINGS_SUCCESS;
   return event;
}

BfEvent
BfGui::bindDefaultFont()
{
   ImGuiIO& io = ImGui::GetIO();
   ImFontConfig config;

   config.GlyphOffset.y = __settings_form.standart_font.glypth_offset.second;
   config.SizePixels = __settings_form.standart_font.size;

   static const ImWchar ranges[] = {
       0x0020,
       0x007F, // ASCII range
       0x0391,
       0x03A1, // Greek Capital Letters
       0x03A3,
       0x03A9, // Greek Capital Letters (continued)
       0x03B1,
       0x03C1, // Greek Small Letters
       0x03C3,
       0x03C9, // Greek Small Letters (continued)
       0
   };

   __default_font = io.Fonts->AddFontFromFileTTF(
       __settings_form.standart_font.name[__settings_form.standart_font.current]
           .c_str(),
       __settings_form.standart_font.size,
       &config
   );

   return BfEvent();
}

BfEvent
BfGui::bindIconFont()
{
   ImGuiIO& io = ImGui::GetIO();
   ImFontConfig config;

   config.GlyphOffset.y = __settings_form.icon_font.glypth_offset.second;
   config.SizePixels = __settings_form.icon_font.size;

   config.MergeMode = true;
   config.GlyphMinAdvanceX = __settings_form.icon_font.glypth_min_advance_x;
   //
   static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
   __icon_font = io.Fonts->AddFontFromFileTTF(
       __settings_form.icon_font.name[__settings_form.icon_font.current].c_str(
       ),
       __settings_form.icon_font.size,
       &config,
       icon_ranges
   );

   return BfEvent();
}

BfEvent
BfGui::bindGreekFont()
{
   ImGuiIO& io = ImGui::GetIO();
   ImFontConfig config;

   config.GlyphOffset.y = __settings_form.greek_font.glypth_offset.second;
   config.SizePixels = __settings_form.greek_font.size;

   config.MergeMode = true;
   config.GlyphMinAdvanceX = __settings_form.greek_font.glypth_min_advance_x;
   //
   static const ImWchar greek_ranges[] = {0x0370, 0x03FF, 0};

   __greek_font = io.Fonts->AddFontFromFileTTF(
       __settings_form.greek_font.name[__settings_form.icon_font.current].c_str(
       ),
       __settings_form.greek_font.size,
       &config,
       greek_ranges
   );

   return BfEvent();
}

void
BfGui::updateFonts()
{
   // if (!__default_font or !__icon_font)
   // {
   ImGuiIO& io = ImGui::GetIO();
   io.Fonts->Clear();

   bindDefaultFont();
   bindIconFont();
   bindGreekFont();

   ImGui_ImplVulkan_CreateFontsTexture();
   // }
}

std::string
BfGui::getMenuInfoStr()
{
   if (__is_info)
      return bfSetMenuStr.at(BF_MENU_STATUS_INFO_ENABLED);
   else
      return bfSetMenuStr.at(BF_MENU_STATUS_INFO_DISABLED);
}

std::string
BfGui::getMenuEventLogStr()
{
   if (__is_event_log)
      return bfSetMenuStr.at(BF_MENU_STATUS_EVENT_LOG_ENABLED);
   else
      return bfSetMenuStr.at(BF_MENU_STATUS_EVENT_LOG_DISABLED);
}

std::string
BfGui::getMenuCameraInfoStr()
{
   if (__is_camera_info)
      return bfSetMenuStr.at(BF_MENU_STATUS_CAMERA_INFO_ENABLED);
   else
      return bfSetMenuStr.at(BF_MENU_STATUS_CAMERA_INFO_DISABLED);
}

std::string
BfGui::getMenuSettingsInfoStr()
{
   if (__is_camera_info)
      return bfSetMenuStr.at(BF_MENU_STATUS_SETTINGS_ENABLED);
   else
      return bfSetMenuStr.at(BF_MENU_STATUS_SETTINGS_DISABLED);
}

std::string
BfGui::getMenuIsLeftDockTitleInfoStr()
{
   if (__is_left_dock_space_name)
      return bfSetMenuStr.at(BF_MENU_STATUS_LEFT_DOCK_TITLE_ENABLED);
   else
      return bfSetMenuStr.at(BF_MENU_STATUS_LEFT_DOCK_TITLE_DISABLED);
}

std::string
BfGui::getMenuNewCameraStatusStr()
{
   if (__is_new_cam)
      return bfSetMenuStr.at(BF_MENU_STATUS_NEW_CAM_ENABLED);
   else
      return bfSetMenuStr.at(BF_MENU_STATUS_NEW_CAM_DISABLED);
}

void
BfGui::presentLayerHandler() // FIXME: deprecate
{
   // if (!ImGui::Begin("Layer observer", nullptr, ImGuiWindowFlags_None))
   // {
   //    ImGui::End();
   //    return;
   // }
   // {
   //    for (size_t i = 0; i < __ptr_base->layer_handler.get_layer_count();
   //    i++)
   //    {
   //       auto layer = __ptr_base->layer_handler.get_layer_by_index(i);
   //       // bfShowNestedLayersRecursive(layer);
   //    }
   // }
   // ImGui::End();
}

void
BfGui::presentMenuBar()
{
   if (ImGui::BeginMainMenuBar())
   {
      if (ImGui::BeginMenu("File"))
      {
         if (ImGui::MenuItem("Create"))
         {
         }
         if (ImGui::MenuItem("Open", "Ctrl+O"))
         {
         }
         if (ImGui::MenuItem("Save", "Ctrl+S"))
         {
         }
         if (ImGui::MenuItem("Save as.."))
         {
         }
         if (ImGui::MenuItem(getMenuSettingsInfoStr().c_str()))
         {
            __is_settings = !__is_settings;
         }
         ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View"))
      {
         if (ImGui::MenuItem(getMenuInfoStr().c_str()))
         {
            __is_info = !__is_info;
         }
         if (ImGui::MenuItem(getMenuEventLogStr().c_str()))
         {
            __is_event_log = !__is_event_log;
         }
         if (ImGui::MenuItem(getMenuCameraInfoStr().c_str()))
         {
            __is_camera_info = !__is_camera_info;
         }
         if (ImGui::MenuItem(getMenuIsLeftDockTitleInfoStr().c_str()))
         {
            __is_left_dock_space_name = !__is_left_dock_space_name;
         }

         ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
   }
}

void
BfGui::presentCamera()
{
}

void
BfGui::presentTooltip()
{
   auto hovered_id = obj::BfDrawManager::getHovered();
   if (hovered_id)
   {
      try
      {
         auto o = obj::BfDrawManager::findObjectById(hovered_id);

         if (auto tq =
                 std::dynamic_pointer_cast< obj::curves::BfTextureQuad >(o))
         {
            if (tq->isLocked())
               return;
         }

         ImGui::BeginTooltip();

         ImGui::Text(
             "type=%s",
             obj::BfTypeManager::inst().getTypeNameById(hovered_id)
         );
         ImGui::Text("id=%i", hovered_id);
         ImGui::Separator();

         o->presentTooltipInfo();

         ImGui::EndTooltip();
      }
      catch (...)
      {
         return;
      }
   }
}

void
BfGui::presentLeftDock()
{
   float frame_h = ImGui::GetFrameHeight();
   float view_port_h = ImGui::GetMainViewport()->Size.y;
   float top_dock_space_h = ImGui::FindWindowByName("Top dock space")->Size.y;
   ImGui::SetNextWindowPos({0, top_dock_space_h + frame_h});

   ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove;
   if (!__is_left_dock_space_name)
   {
      flags |= ImGuiWindowFlags_NoTitleBar;
   }

   ImGui::SetNextWindowSizeConstraints(
       ImVec2(0.0f, view_port_h),
       ImVec2(FLT_MAX, view_port_h)
   );

   ImGui::PushStyleColor(
       ImGuiCol_DockingEmptyBg,
       ImVec4(0.05f, 0.05f, 0.05f, 0.05f)
   );
   {
      ImGui::Begin("Left dock space", nullptr, flags);
      {
         ImGuiID left_dockspace_id = ImGui::GetID("LeftDockSpace");
         ImGui::DockSpace(
             left_dockspace_id,
             ImVec2(0.0f, 0.0f),
             ImGuiDockNodeFlags_None
         );
      }
      ImGui::End();
   }
   ImGui::PopStyleColor();
}

void
BfGui::presentTopDock()
{
   float frame_h = ImGui::GetFrameHeight();
   float view_port_w = ImGui::GetMainViewport()->Size.x;

   ImGui::SetNextWindowPos({0, frame_h});

   ImGuiWindowFlags flags =
       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
   // ImGuiWindowFlags_NoResize;

   ImGui::SetNextWindowSizeConstraints(
       ImVec2(view_port_w, 0.0f),
       ImVec2(view_port_w, FLT_MAX)
   );

   ImGui::PushStyleColor(
       ImGuiCol_DockingEmptyBg,
       ImVec4(0.05f, 0.05f, 0.05f, 0.05f)
   );
   {
      ImGui::Begin("Top dock space", nullptr, flags);
      {
         ImGuiID top_dock_space_id = ImGui::GetID("TopDockSpace");
         ImGui::DockSpace(
             top_dock_space_id,
             ImVec2(0.0f, 0.0f),
             ImGuiDockNodeFlags_None
         );
      }
      ImGui::End();
   }
   ImGui::PopStyleColor();
}

void
BfGui::presentSettings()
{
   if (__is_settings)
   {
      auto createVectorStr = [&](const std::vector< std::string >& d) {
         std::vector< const char* > c_str_items;
         for (const auto& item : d)
         {
            c_str_items.push_back(item.c_str());
         }
         return c_str_items;
      };

      ImGui::Begin("Settings");
      {
         ImGui::Text("Standart font settings");
         if (ImGui::BeginTable(
                 "Standart Font",
                 2,
                 ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg
             ))
         {
            { // Standart font
               ImGui::TableNextRow();
               {
                  ImGui::TableSetColumnIndex(0);
                  ImGui::Text("Name");
                  ImGui::TableSetColumnIndex(1);
                  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                  if (ImGui::Combo(
                          "##SETTINGS_FONTS_STANDART_COMBO",
                          &this->__settings_form.standart_font.current,
                          createVectorStr(
                              this->__settings_form.standart_font.name
                          )
                              .data(),
                          static_cast< int >(
                              this->__settings_form.standart_font.name.size()
                          )
                      ))
                  {
                     __queue_after_render.push([this]() { updateFonts(); });
                  }
               }

               ImGui::TableNextRow();
               {
                  ImGui::TableSetColumnIndex(0);
                  ImGui::Text("Size");
                  ImGui::TableSetColumnIndex(1);
                  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                  if (ImGui::InputInt(
                          "##SETTINGS_FONTS_STANDART_SIZE",
                          &__settings_form.standart_font.size
                      ))
                  {
                     __queue_after_render.push([this]() { updateFonts(); });
                  }
               }

               ImGui::TableNextRow();
               {
                  ImGui::TableSetColumnIndex(0);
                  ImGui::Text("Glypth offset 'x'");
                  ImGui::TableSetColumnIndex(1);
                  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                  if (ImGui::InputFloat(
                          "##SETTINGS_FONTS_STANDART_GLYPTH_OFFSET_X",
                          &__settings_form.standart_font.glypth_offset.first
                      ))
                  {
                     __queue_after_render.push([this]() { updateFonts(); });
                  }
               }
               ImGui::TableNextRow();
               {
                  ImGui::TableSetColumnIndex(0);
                  ImGui::Text("Glypth offset 'y'");
                  ImGui::TableSetColumnIndex(1);
                  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                  if (ImGui::InputFloat(
                          "##SETTINGS_FONTS_STANDART_GLYPTH_OFFSET_Y",
                          &__settings_form.standart_font.glypth_offset.second
                      ))
                  {
                     __queue_after_render.push([this]() { updateFonts(); });
                  }
               }
               ImGui::TableNextRow();
               {
                  ImGui::TableSetColumnIndex(0);
                  ImGui::Text("Glypth minimum advance 'x'");
                  ImGui::TableSetColumnIndex(1);
                  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                  if (ImGui::InputFloat(
                          "##SETTINGS_FONTS_STANDART_GLYPTH_MIN_ADVANCE_X",
                          &__settings_form.standart_font.glypth_min_advance_x
                      ))
                  {
                     __queue_after_render.push([this]() { updateFonts(); });
                  }
               }
            } // ===> Standart Font
            ImGui::EndTable();
            ImGui::Spacing();
         }

         ImGui::Text("Icon font settings");
         if (ImGui::BeginTable(
                 "Icon Font",
                 2,
                 ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg
             ))
         { // Icon Font
            ImGui::TableNextRow();
            {
               ImGui::TableSetColumnIndex(0);
               ImGui::Text("Name");
               ImGui::TableSetColumnIndex(1);
               ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
               if (ImGui::Combo(
                       "##SETTINGS_FONTS_ICON_COMBO",
                       &this->__settings_form.icon_font.current,
                       createVectorStr(this->__settings_form.icon_font.name)
                           .data(),
                       static_cast< int >(
                           this->__settings_form.icon_font.name.size()
                       )
                   ))
               {
                  __queue_after_render.push([this]() { updateFonts(); });
               }
            }

            ImGui::TableNextRow();
            {
               ImGui::TableSetColumnIndex(0);
               ImGui::Text("Size");
               ImGui::TableSetColumnIndex(1);
               ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
               if (ImGui::InputInt(
                       "##SETTINGS_FONTS_ICON_SIZE",
                       &__settings_form.icon_font.size
                   ))
               {
                  __queue_after_render.push([this]() { updateFonts(); });
               }
            }
            ImGui::TableNextRow();
            {
               ImGui::TableSetColumnIndex(0);
               ImGui::Text("Glypth offset 'x'");
               ImGui::TableSetColumnIndex(1);
               ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
               if (ImGui::InputFloat(
                       "##SETTINGS_FONTS_ICON_GLYPTH_OFFSET_X",
                       &__settings_form.icon_font.glypth_offset.first
                   ))
               {
                  __queue_after_render.push([this]() { updateFonts(); });
               }
            }
            ImGui::TableNextRow();
            {
               ImGui::TableSetColumnIndex(0);
               ImGui::Text("Glypth offset 'y'");
               ImGui::TableSetColumnIndex(1);
               ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
               if (ImGui::InputFloat(
                       "##SETTINGS_FONTS_ICON_GLYPTH_OFFSET_Y",
                       &__settings_form.icon_font.glypth_offset.second
                   ))
               {
                  __queue_after_render.push([this]() { updateFonts(); });
               }
            }
            ImGui::TableNextRow();
            {
               ImGui::TableSetColumnIndex(0);
               ImGui::Text("Glypth minimum advance 'x'");
               ImGui::TableSetColumnIndex(1);
               ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
               if (ImGui::InputFloat(
                       "##SETTINGS_FONTS_ICON_GLYPTH_MIN_ADVANCE_X",
                       &__settings_form.icon_font.glypth_min_advance_x
                   ))
               {
                  __queue_after_render.push([this]() { updateFonts(); });
               }
            }
            ImGui::EndTable();
            ImGui::Spacing();
         } // ===> Icon font
      }
      ImGui::End();
   }
}

void
BfGui::presentLuaInteraction()
{
   __lua_interaction.draw();
}

void
BfGui::presentFileDialog()
{
   __file_dialog.draw();
}

void
BfGui::__presentOrthoSettings()
{
   if (__is_ortho_settings)
   {
      if (ImGui::Begin("Ortho settings", &__is_ortho_settings))
      {
         ImGui::InputFloat("Ortho-left", &__ptr_base->window->ortho_left, 0.1f);
         ImGui::InputFloat(
             "Ortho-right",
             &__ptr_base->window->ortho_right,
             0.1f
         );
         ImGui::InputFloat("Ortho-top", &__ptr_base->window->ortho_top, 0.1f);
         ImGui::InputFloat(
             "Ortho-bottom",
             &__ptr_base->window->ortho_bottom,
             0.1f
         );
         ImGui::InputFloat("Ortho-near", &__ptr_base->window->ortho_near, 0.1f);
         ImGui::InputFloat("Ortho-far", &__ptr_base->window->ortho_far, 0.1f);
         ImGui::Checkbox("is_asp", &__ptr_base->window->is_asp);
      }
      ImGui::End();
   }
};

void
BfGui::presentCreateWindow()
{
   __create_window.render();
}

void
BfGui::presentSmartLayerObserver()
{
   __smart_layer_observer.render();
}

void
BfGui::presentInfo()
{
   double currentTime = glfwGetTime();
   static int counter = 0;
   static int print_counter = 0;
   static double previousTime = currentTime;
   if (__is_info)
   {
      std::string fps = "Current FPS: ";
      int screenWidth = ImGui::GetIO().DisplaySize.x;
      int screenHeight = ImGui::GetIO().DisplaySize.y;
      auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBackground |
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;
      ImGui::SetNextWindowPos(
          ImVec2(screenWidth - 100, 100),
          ImGuiCond_Always,
          ImVec2(1.0f, 0.0f)
      );
      ImGui::Begin("Info", nullptr, flags);
      counter += 1;
      float x = 1.0; // displays the frame rate every 1 second
      if ((currentTime - previousTime) >= x)
      {
         print_counter = counter;
         counter = 0;
         previousTime = currentTime;
      }
      ImGui::Text("Current FPS: %i", print_counter);
      ImGui::End();
   }
}

void
BfGui::presentCameraWindow()
{
   __camera_window.render();
}

void
BfGui::presentIds()
{
   ImGui::Begin("Type Manager");
   auto& inst = obj::BfTypeManager::inst();

   if (ImGui::BeginTable("types : typename", 2))
   {
      ImGui::TableSetupColumn("type");
      ImGui::TableSetupColumn("typename");
      ImGui::TableHeadersRow();
      for (auto& [type, typeName] : inst.types())
      {
         ImGui::TableNextRow();

         ImGui::TableSetColumnIndex(0);
         ImGui::Text("%i", type);

         ImGui::TableSetColumnIndex(1);
         ImGui::Text("%s", typeName);
      }
      ImGui::EndTable();
   }

   if (ImGui::BeginTable("id : type", 2))
   {
      ImGui::TableSetupColumn("id");
      ImGui::TableSetupColumn("typeName");
      ImGui::TableHeadersRow();
      for (auto& [id, type] : inst.ids())
      {
         ImGui::TableNextRow();

         ImGui::TableSetColumnIndex(0);
         ImGui::Text("%i", id);

         ImGui::TableSetColumnIndex(1);
         ImGui::Text("%s", inst.getTypeNameById(id));
      }
      ImGui::EndTable();
   }

   ImGui::End();
}

void
BfGui::presentViewportRatioButton(BfBase& base)
{
   // clang-format off
   const auto flags = 0
      | ImGuiWindowFlags_NoTitleBar
      | ImGuiWindowFlags_NoDecoration
      | ImGuiWindowFlags_NoBackground
      | ImGuiWindowFlags_NoMove
      | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_NoResize
      | ImGuiWindowFlags_NoScrollWithMouse
      | ImGuiWindowFlags_NoNavInputs
      | ImGuiWindowFlags_NoNavFocus
      | ImGuiWindowFlags_NoBringToFrontOnFocus
   ;

   const auto extent = base.swap_chain_extent;
   const float button_w = 8.f;
   const float center_x = extent.width * base.viewport_ratio;
   const ImVec2 button_pos = { center_x - button_w * 0.5f,
                               0 };
   const ImVec2 button_sz = { button_w,
                              static_cast<float>(extent.height) };
   
   ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
   ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});

   ImGui::SetNextWindowPos(button_pos, ImGuiCond_Always);
   ImGui::SetNextWindowSize(button_sz, ImGuiCond_Always);
   ImGui::Begin("##viewport-change-ratio", nullptr, flags);
   
   if (ImGui::Button("##viewport-change-ratio-button", {button_w, ImGui::GetContentRegionAvail().y}))
   {
      // Click logic (optional)
   }

   if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
   {
      float delta = ImGui::GetIO().MouseDelta.x;
      base.viewport_ratio += delta / static_cast<float>(extent.width);
      base.viewport_ratio = std::clamp(base.viewport_ratio, 0.0f, 1.0f);
   }


   ImGui::End();
   ImGui::PopStyleVar(2);
   // clang-format on
}

void
BfGui::toggleRenderCreateWindow()
{
   __create_window.toggleRender();
}

// bool
// renderSectionAsPartOfBody(
//     std::shared_ptr< obj::section::BfBladeSection > sec,
//     uint64_t index,
//     ImVec2 sz
// )
// {
//    std::string title = fmt::format("##section-part-{}", i);
//    ImGui::BeginChild(title.c_str(), sz, true);
//    {
//
//       std::string buttontitle = fmt::format("#{}##section-part-button", i);
//       ImGui::Button(buttontitle.c_str());
//       ImGui::SameLine();
//
//       bool is_render = sec->isRender();
//       std::string isdrawtitle = fmt::format("##section-part-is-ren-{}", i);
//       if (ImGui::Checkbox(isdrawtitle.c_str(), &is_render))
//       {
//          sec->toggleRender();
//       }
//
//       if (is_render && presentBladeSectionCreateWindow(
//                            1 << index,
//                            sec->info().getp(),
//                            nullptr
//                        ))
//       {
//          sec->make();
//          sec->control().updateBuffer();
//       }
//
//       ImGui::SameLine();
//
//       std::string ztitle = fmt::format("z##section-part-z-{}", i);
//       auto& info = sec->info().get();
//
//       const float speed = body->length() / 100.f;
//
//       if (ImGui::DragFloat(ztitle.c_str(), &info.z, 0.01))
//       {
//          sec->make();
//          sec->control().updateBuffer();
//       }
//
//       ImGui::SameLine();
//       if (ImGui::Button("Save?"))
//       {
//          auto shapes = saveas::createSection(sec);
//          saveas::exportToSTEP(shapes, "./test.step");
//       }
//    }
//    ImGui::EndChild();
// }

bool
renderBitCheckbox(const char* label, uint32_t bit, uint32_t* bitfield)
{
   bool checked = (*bitfield & bit) != 0;
   if (ImGui::Checkbox(label, &checked))
   {
      if (checked)
         *bitfield |= bit; // set the bit
      else
         *bitfield &= ~bit; // clear the bit
      return true;
   }
   return false;
}

void
BfGui::presentEventLog()
{
   if (__is_event_log)
   {
      ImVec2 size = {400, 600};
      ImVec2 pos = {
          __ptr_base->swap_chain_extent.width - size[0],
          (float)(__ptr_base->swap_chain_extent.height - size[1])
      };

      ImGui::SetNextWindowPos(pos);
      ImGui::SetNextWindowSize(size);

      ImGuiWindowFlags windowFlags =
          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
          ImGuiWindowFlags_NoTitleBar |
          ImGuiWindowFlags_AlwaysVerticalScrollbar;

      ImGui::Begin("Console", nullptr, windowFlags);
      {
         auto it_event = BfEventHandler::single_events.rbegin();
         auto it_time = BfEventHandler::single_event_time.rbegin();
         auto it_message = BfEventHandler::single_event_message.rbegin();

         while (it_event != BfEventHandler::single_events.rend() &&
                it_time != BfEventHandler::single_event_time.rend() &&
                it_message != BfEventHandler::single_event_message.rend())
         {
            ImGui::TextColored(
                ImVec4{1.0f, 0.5f, 0.0f, 1.0f},
                it_time->c_str()
            );
            ImGui::SameLine();
            ImGui::TextWrapped(it_message->c_str());

            ++it_event;
            ++it_time;
            ++it_message;
         }
      }
      ImGui::End();
   }
}

void // FIXME: deprecate
bfPresentLayerHandler(BfLayerHandler& layer_handler)
{
   // ImGui::Begin("Layer observer");
   // {
   //    for (size_t i = 0; i < layer_handler.get_layer_count(); i++)
   //    {
   //       auto layer = layer_handler.get_layer_by_index(i);
   //       bfShowNestedLayersRecursive(layer);
   //    }
   // }
   // ImGui::End();
}

void
bfPresentBladeSectionInside(
    BfBladeBase* layer,
    BfBladeSectionCreateInfo* info,
    BfBladeSectionCreateInfo* old
)
{
   static int inputFloatMode = 0;

   auto make_row = [](std::string n,
                      std::string d,
                      std::string dim,
                      float* value,
                      float left,
                      float right) {
      static int count = 0;

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text(n.c_str());
      ImGui::TableSetColumnIndex(1);
      ImGui::Text(d.c_str());
      ImGui::TableSetColumnIndex(2);

      switch (inputFloatMode)
      {
      case 0:
         ImGui::InputFloat(dim.c_str(), value);
         break;
      case 1:
         ImGui::SliderFloat(dim.c_str(), value, left, right);
         break;
      }

      count++;
   };

   ImGuiWindowFlags window_flags =
       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
       // ImGuiWindowFlags_NoMove |
       ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
       ImGuiWindowFlags_AlwaysAutoResize;
   {
      ImGui::BeginGroup();
      {
         ImGui::Text("Input parameters mode");

         ImGui::RadioButton("Input float", &inputFloatMode, 0);
         ImGui::SameLine();
         ImGui::RadioButton("Slider", &inputFloatMode, 1);
      }
      ImGui::EndGroup();

      int flags =
          ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingFixedFit;

      ImGui::BeginTable("FloatTable", 3, flags);
      {
         ImGui::TableSetupColumn("Parameter name");
         ImGui::TableSetupColumn("Description");
         ImGui::TableSetupColumn("Value");
         ImGui::TableHeadersRow();

         make_row("Width", "B", "[m]##0", &info->width, 0.0f, 10.0f);
         make_row(
             "Install angle",
             "alpha_y",
             "[deg]##1",
             &info->install_angle,
             -180.0f,
             180.0f
         );
         make_row(
             "Inlet angle",
             "beta_1",
             "[deg]##2",
             &info->inlet_angle,
             -180.0f,
             180.0f
         );
         make_row(
             "Outlet angle",
             "beta_2",
             "[deg]##3",
             &info->outlet_angle,
             -180.0f,
             180.0f
         );
         make_row(
             "Inlet surface angle",
             "omega_1",
             "[deg]##4",
             &info->inlet_surface_angle,
             -45.0f,
             45.0f
         );
         make_row(
             "Outlet surface angle",
             "omega_2",
             "[deg]##5",
             &info->outlet_surface_angle,
             -45.0f,
             45.0f
         );
         make_row(
             "Inlet radius",
             "r_1",
             "[m]##6",
             &info->inlet_radius,
             0.00001,
             0.05
         );
         make_row(
             "Outlet radius",
             "r_2",
             "[m]##7",
             &info->outlet_radius,
             0.00001,
             0.05
         );
      }
      ImGui::EndTable();

      ImGui::BeginGroup();
      {
         ImGui::Text("Start point");
         ImGui::SliderFloat("X", &info->start_vertex.x, -10.0f, 10.0f);
         ImGui::SliderFloat("Y", &info->start_vertex.y, -10.0f, 10.0f);
         ImGui::SliderFloat("Z", &info->start_vertex.z, -10.0f, 10.0f);
      }
      ImGui::EndGroup();

      /*ImGui::BeginGroup();
      {
          ImGui::Text("Options");

          if (ImGui::Checkbox("Calculate center", &info.is_center))
              is_center_changed = true;
          else
              is_center_changed = false;

          if (ImGui::Checkbox("Triangulate", &info.is_triangulate))
              is_triangulate = true;
          else
              is_triangulate = false;

      }
      ImGui::EndGroup();*/
   }

   // if (!bfCheckBladeSectionCreateInfoEquality(*info, *old)) {

   //    if (id > 0) layer->del(id);

   //    /*auto blade_section_1 = std::make_shared<BfBladeSection>(
   //        info
   //    );*/

   //    //id = layer->add_section(*info);

   //    //id = blade_section_1->id.get();

   //    //layer->add(blade_section_1);
   //    layer->update_buffer();

   //}
   //*old = *info;
};
void
ShowTestPlot()
{
   std::vector< glm::vec3 > v{
       {0.0, 0.0, 0.0f},
       {1.0, 1.0f, 0.0f},
       {2.0f, 0.0f, 0.0f},
       {4.0f, 2.0f, 0.0f},
       {6.0f, -3.0f, 0.0f}
   };

   std::vector< float > v_x;
   std::vector< float > v_y;

   for (size_t i = 0; i < v.size(); i++)
   {
      v_x.push_back(v[i].x);
      v_y.push_back(v[i].y);
   }

   std::vector< glm::vec2 > spl = bfMathSplineFit(v_x, v_y);

   BfCubicSplineCurve spline(50, v);
   spline.createVertices();

   auto compareByX = [](const glm::vec3& a, const glm::vec3& b) {
      return a.x < b.x;
   };

   // std::sort(v.begin(), v.end(), compareByX);

   std::vector< float > x_;
   std::vector< float > y_;

   for (size_t i = 0; i < spl.size(); i++)
   {
      x_.push_back(spl[i].x);
      y_.push_back(spl[i].y);

      // x_.push_back(spl[i].x);
      // y_.push_back(spl[i].y);
   }

   ImGui::Begin("My Window");
   if (ImPlot::BeginPlot("My Plot"))
   {
      ImPlot::SetupAxes(
          "x",
          "y",
          ImPlotAxisFlags_NoGridLines,
          ImPlotAxisFlags_NoGridLines
      );

      ImPlot::PlotLine("B(x)", x_.data(), y_.data(), spl.size());
      ImPlot::PlotScatter("fsdf", v_x.data(), v_y.data(), v.size());

      ImPlot::EndPlot();
   }

   ImGui::End();
};
