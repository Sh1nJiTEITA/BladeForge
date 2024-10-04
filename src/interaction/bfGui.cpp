#include "bfGui.h"

#include <csignal>
#include <filesystem>
#include <set>
#include <sstream>
#include <string>

#include "bfConfigManager.h"
#include "bfEvent.h"
#include "bfIconsFontAwesome6.h"
#include "config_forms/bfFormGui.h"
#include "imgui.h"

BfGui::BfGui() {}

void BfGui::pollEvents()
{
   while (!__queue_after_render.empty())
   {
      __queue_after_render.front()();
      __queue_after_render.pop();
   }
}

BfEvent BfGui::bindBase(BfBase *base)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (base)
   {
      __ptr_base   = base;
      event.action = BF_ACTION_TYPE_BIND_BASE_TO_GUI_SUCCESS;
   }
   else
   {
      event.action = BF_ACTION_TYPE_BIND_BASE_TO_GUI_FAILURE;
   }
   return event;
}

BfEvent BfGui::bindHolder(BfHolder *handler)
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

BfEvent BfGui::bindSettings(std::filesystem::path path)
{
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
   BfConfigManager::addPackagePath("./scripts");
   BfConfigManager::loadRequireScript(path);

   sol::table settings_table = BfConfigManager::getLuaObj("guiconfig");

   BfConfigManager::fillFormFontSettings(settings_table["fonts"],
                                         &__settings_form);

   event.action = BF_ACTION_TYPE_BIND_GUI_SETTINGS_SUCCESS;
   return event;
}

BfEvent BfGui::bindDefaultFont()
{
   ImGuiIO     &io = ImGui::GetIO();
   ImFontConfig config;

   config.GlyphOffset.y = __settings_form.standart_font.glypth_offset.second;
   config.SizePixels    = __settings_form.standart_font.size;

   __default_font       = io.Fonts->AddFontFromFileTTF(
       __settings_form.standart_font.name[__settings_form.standart_font.current]
           .c_str(),
       __settings_form.standart_font.size,
       &config);

   return BfEvent();
}

BfEvent BfGui::bindIconFont()
{
   ImGuiIO     &io = ImGui::GetIO();
   ImFontConfig config;

   config.GlyphOffset.y    = __settings_form.icon_font.glypth_offset.second;
   config.SizePixels       = __settings_form.icon_font.size;

   config.MergeMode        = true;
   config.GlyphMinAdvanceX = __settings_form.icon_font.glypth_min_advance_x;
   //
   static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
   __icon_font                        = io.Fonts->AddFontFromFileTTF(
       __settings_form.icon_font.name[__settings_form.icon_font.current]
           .c_str(),
       __settings_form.icon_font.size,
       &config,
       icon_ranges);

   return BfEvent();
}

void BfGui::updateFonts()
{
   // if (!__default_font or !__icon_font)
   // {
   ImGuiIO &io = ImGui::GetIO();
   io.Fonts->Clear();

   bindDefaultFont();
   bindIconFont();

   ImGui_ImplVulkan_CreateFontsTexture();
   // }
}

std::string BfGui::getMenueInfoStr()
{
   if (__is_info)
      return bfSetMenueStr.at(BF_MENUE_STATUS_INFO_ENABLED);
   else
      return bfSetMenueStr.at(BF_MENUE_STATUS_INFO_DISABLED);
}

std::string BfGui::getMenueEventLogStr()
{
   if (__is_event_log)
      return bfSetMenueStr.at(BF_MENUE_STATUS_EVENT_LOG_ENABLED);
   else
      return bfSetMenueStr.at(BF_MENUE_STATUS_EVENT_LOG_DISABLED);
}

std::string BfGui::getMenueCameraInfoStr()
{
   if (__is_camera_info)
      return bfSetMenueStr.at(BF_MENUE_STATUS_CAMERA_INFO_ENABLED);
   else
      return bfSetMenueStr.at(BF_MENUE_STATUS_CAMERA_INFO_DISABLED);
}

void BfGui::presentLayerHandler()
{
   ImGui::Begin("Layer observer");
   {
      for (size_t i = 0; i < __ptr_base->layer_handler.get_layer_count(); i++)
      {
         auto layer = __ptr_base->layer_handler.get_layer_by_index(i);
         bfShowNestedLayersRecursive(layer);
      }
   }
   ImGui::End();
}

void BfGui::presentMenueBar()
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
         ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View"))
      {
         if (ImGui::MenuItem(getMenueInfoStr().c_str()))
         {
            __is_info = !__is_info;
         }
         if (ImGui::MenuItem(getMenueEventLogStr().c_str()))
         {
            __is_event_log = !__is_event_log;
         }
         if (ImGui::MenuItem(getMenueCameraInfoStr().c_str()))
         {
            __is_camera_info = !__is_camera_info;
         }

         ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
   }
}

void BfGui::presentCamera()
{
   if (__is_camera_info)
   {
      static bool is_per = true;
      static bool is_ort = false;

      ImGui::Begin("Camera");
      {
         ImGui::BeginTable("VIEWS", 3);
         {
            ImGui::TableNextRow();  // 0
            {
               ImGui::TableSetColumnIndex(1);
               if (ImGui::Button("Up", ImVec2(50, 50)))
               {
                  bfSetOrthoUp(__ptr_base->window);
               }
            }

            ImGui::TableNextRow();  // 1
            {
               ImGui::TableSetColumnIndex(2);
               if (ImGui::Button("Left", ImVec2(50, 50)))
               {
                  bfSetOrthoLeft(__ptr_base->window);
               }

               ImGui::TableSetColumnIndex(1);
               if (ImGui::Button("Near", ImVec2(50, 50)))
               {
                  bfSetOrthoNear(__ptr_base->window);
               }

               ImGui::TableSetColumnIndex(2);
               if (ImGui::Button("Right", ImVec2(50, 50)))
               {
                  bfSetOrthoRight(__ptr_base->window);
               }
            }

            ImGui::TableNextRow();  // 2
            {
               ImGui::TableSetColumnIndex(1);
               if (ImGui::Button("Bottom", ImVec2(50, 50)))
               {
                  bfSetOrthoBottom(__ptr_base->window);
               }

               ImGui::TableSetColumnIndex(2);
               if (ImGui::Button("Far", ImVec2(50, 50)))
               {
                  bfSetOrthoFar(__ptr_base->window);
               }
            }
         }
         ImGui::EndTable();
         /**/
         if (ImGui::RadioButton("Perspective", is_per))
         {
            is_ort                        = false;
            is_per                        = true;

            __ptr_base->window->proj_mode = 0;
         }
         if (ImGui::RadioButton("Ortho", is_ort))
         {
            is_ort                        = true;
            is_per                        = false;

            __ptr_base->window->proj_mode = 1;
         }

         ImGui::InputFloat("Ortho-left", &__ptr_base->window->ortho_left, 0.1f);
         ImGui::InputFloat("Ortho-right",
                           &__ptr_base->window->ortho_right,
                           0.1f);
         ImGui::InputFloat("Ortho-top", &__ptr_base->window->ortho_top, 0.1f);
         ImGui::InputFloat("Ortho-bottom",
                           &__ptr_base->window->ortho_bottom,
                           0.1f);
         ImGui::InputFloat("Ortho-near", &__ptr_base->window->ortho_near, 0.1f);
         ImGui::InputFloat("Ortho-far", &__ptr_base->window->ortho_far, 0.1f);
         ImGui::Checkbox("is_asp", &__ptr_base->window->is_asp);
      }
      ImGui::End();
   }
}

void BfGui::presentToolType()
{
   if (__ptr_base->pos_id != 0)
   {
      ImGui::BeginTooltip();

      ImGui::Text((std::string("type=").append(bfGetStrNameDrawObjType(
                       BfObjID::find_type(__ptr_base->pos_id))))
                      .c_str());

      ImGui::Text((std::string("id=")
                       .append(std::to_string(__ptr_base->pos_id))
                       .c_str()));
      ImGui::EndTooltip();
   }
}

void BfGui::presentLeftDock()
{
   /*int flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;*/
   /*int flags = 0;*/

   ImGui::SetNextWindowPos(ImVec2(0, 20));
   /*ImGui::SetNextWindowSizeConstraints(ImVec2(200, 100), ImVec2(FLT_MAX,
    * 100));*/

   ImGuiWindowFlags flags =
       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;

   ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg,
                         ImVec4(0.05f, 0.05f, 0.05f, 0.05f));
   {
      ImGui::Begin("0x313312393812938103281", nullptr, flags);
      {
         ImGuiID left_dockspace_id = ImGui::GetID("LeftDockSpace");
         /*ImGui::SetNextWindowPos(*/
         /*    ImGui::GetCursorScreenPos());  // Устанавливаем начальную
          * позицию*/
         /*                                   //*/
         /*ImVec2 dockspace_size = ImVec2(ImGui::GetContentRegionAvail().x *
          * 0.3f,*/
         /*                               ImGui::GetContentRegionAvail().y);*/
         /*ImGui::SetNextWindowSize(dockspace_size);  // Размер док-зоны*/
         ImGui::DockSpace(left_dockspace_id,
                          ImVec2(0.0f, 0.0f),
                          ImGuiDockNodeFlags_None);
      }
      ImGui::End();
   }
   ImGui::PopStyleColor();
}

void BfGui::presentSettings()
{
   auto createVectorStr = [&](const std::vector<std::string> &d) {
      std::vector<const char *> c_str_items;
      for (const auto &item : d)
      {
         c_str_items.push_back(item.c_str());
      }
      return c_str_items;
   };

   ImGui::Begin("Settings");
   {
      ImGui::Text("Standart font settings");
      if (ImGui::BeginTable("Standart Font",
                            2,
                            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
      {
         {  // Standart font
            ImGui::TableNextRow();
            {
               ImGui::TableSetColumnIndex(0);
               ImGui::Text("Name");
               ImGui::TableSetColumnIndex(1);
               ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
               if (ImGui::Combo(
                       "##SETTINGS_FONTS_STANDART_COMBO",
                       &this->__settings_form.standart_font.current,
                       createVectorStr(this->__settings_form.standart_font.name)
                           .data(),
                       static_cast<int>(
                           this->__settings_form.standart_font.name.size())))
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
               if (ImGui::InputInt("##SETTINGS_FONTS_STANDART_SIZE",
                                   &__settings_form.standart_font.size))
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
                       &__settings_form.standart_font.glypth_offset.first))
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
                       &__settings_form.standart_font.glypth_offset.second))
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
                       &__settings_form.standart_font.glypth_min_advance_x))
               {
                  __queue_after_render.push([this]() { updateFonts(); });
               }
            }
         }  // ===> Standart Font
      }
      ImGui::EndTable();
      ImGui::Spacing();

      ImGui::Text("Icon font settings");
      if (ImGui::BeginTable("Icon Font",
                            2,
                            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
      {  // Icon Font
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
                    static_cast<int>(
                        this->__settings_form.icon_font.name.size())))
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
            if (ImGui::InputInt("##SETTINGS_FONTS_ICON_SIZE",
                                &__settings_form.icon_font.size))
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
                    &__settings_form.icon_font.glypth_offset.first))
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
                    &__settings_form.icon_font.glypth_offset.second))
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
                    &__settings_form.icon_font.glypth_min_advance_x))
            {
               __queue_after_render.push([this]() { updateFonts(); });
            }
         }
      }  // ===> Icon font
      ImGui::EndTable();
      ImGui::Spacing();
   }
   ImGui::End();
}

void BfGui::presentEventLog()
{
   if (__is_event_log)
   {
      ImVec2 size = {400, 600};
      ImVec2 pos  = {__ptr_base->swap_chain_extent.width - size[0],
                     (float)(__ptr_base->swap_chain_extent.height - size[1])};

      ImGui::SetNextWindowPos(pos);
      ImGui::SetNextWindowSize(size);

      ImGuiWindowFlags windowFlags =
          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
          ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
          ImGuiWindowFlags_NoTitleBar |
          ImGuiWindowFlags_AlwaysVerticalScrollbar;

      ImGui::Begin("Console", nullptr, windowFlags);
      {
         auto it_event   = BfEventHandler::single_events.rbegin();
         auto it_time    = BfEventHandler::single_event_time.rbegin();
         auto it_message = BfEventHandler::single_event_message.rbegin();

         while (it_event != BfEventHandler::single_events.rend() &&
                it_time != BfEventHandler::single_event_time.rend() &&
                it_message != BfEventHandler::single_event_message.rend())
         {
            ImGui::TextColored(ImVec4{1.0f, 0.5f, 0.0f, 1.0f},
                               it_time->c_str());
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

void bfShowNestedLayersRecursive(std::shared_ptr<BfDrawLayer> l)
{
#define BF_OBJ_NAME_LEN 30
#define BF_LAYER_COLOR 1.0f, 0.55f, 0.1f, 1.0f

   size_t obj_count     = l->get_obj_count();
   size_t lay_count     = l->get_layer_count();

   std::string lay_name = "L (" + bfGetStrNameDrawObjType(l->id.get_type()) +
                          ") " + std::to_string(l->id.get());

   if (ImGui::TreeNode(lay_name.c_str()))
   {
      for (size_t i = 0; i < lay_count; ++i)
      {
         bfShowNestedLayersRecursive(l->get_layer_by_index(i));
      }
      for (size_t i = 0; i < obj_count; ++i)
      {
         std::shared_ptr<BfDrawObj> obj = l->get_object_by_index(i);
         //
         std::string obj_name = ICON_FA_BOX_ARCHIVE "(" +
                                bfGetStrNameDrawObjType(obj->id.get_type()) +
                                ") " + std::to_string(obj->id.get());

         ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(BF_LAYER_COLOR));
         if (ImGui::TreeNode(obj_name.c_str()))
         {
            ImGui::Text(
                ("\tVertices " + std::to_string(obj->get_vertices_count()))
                    .c_str());

            ImGui::Text(
                ("\tIndices " + std::to_string(obj->get_indices_count()))
                    .c_str());

            ImGui::Text(
                ("\tDVertices " + std::to_string(obj->get_dvertices_count()))
                    .c_str());

            ImGui::TreePop();
         }
         ImGui::PopStyleColor();
      }
      ImGui::TreePop();
   }
}

void bfPresentLayerHandler(BfLayerHandler &layer_handler)
{
   ImGui::Begin("Layer observer");
   {
      for (size_t i = 0; i < layer_handler.get_layer_count(); i++)
      {
         auto layer = layer_handler.get_layer_by_index(i);
         bfShowNestedLayersRecursive(layer);
      }
   }
   ImGui::End();
}

void bfPresentBladeSectionInside(BfBladeBase              *layer,
                                 BfBladeSectionCreateInfo *info,
                                 BfBladeSectionCreateInfo *old)
{
   static int inputFloatMode = 0;

   auto make_row             = [](std::string n,
                      std::string d,
                      std::string dim,
                      float      *value,
                      float       left,
                      float       right) {
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
         make_row("Install angle",
                  "alpha_y",
                  "[deg]##1",
                  &info->install_angle,
                  -180.0f,
                  180.0f);
         make_row("Inlet angle",
                  "beta_1",
                  "[deg]##2",
                  &info->inlet_angle,
                  -180.0f,
                  180.0f);
         make_row("Outlet angle",
                  "beta_2",
                  "[deg]##3",
                  &info->outlet_angle,
                  -180.0f,
                  180.0f);
         make_row("Inlet surface angle",
                  "omega_1",
                  "[deg]##4",
                  &info->inlet_surface_angle,
                  -45.0f,
                  45.0f);
         make_row("Outlet surface angle",
                  "omega_2",
                  "[deg]##5",
                  &info->outlet_surface_angle,
                  -45.0f,
                  45.0f);
         make_row("Inlet radius",
                  "r_1",
                  "[m]##6",
                  &info->inlet_radius,
                  0.00001,
                  0.05);
         make_row("Outlet radius",
                  "r_2",
                  "[m]##7",
                  &info->outlet_radius,
                  0.00001,
                  0.05);
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
void ShowTestPlot()
{
   std::vector<glm::vec3> v{{0.0, 0.0, 0.0f},
                            {1.0, 1.0f, 0.0f},
                            {2.0f, 0.0f, 0.0f},
                            {4.0f, 2.0f, 0.0f},
                            {6.0f, -3.0f, 0.0f}};

   std::vector<float> v_x;
   std::vector<float> v_y;

   for (size_t i = 0; i < v.size(); i++)
   {
      v_x.push_back(v[i].x);
      v_y.push_back(v[i].y);
   }

   std::vector<glm::vec2> spl = bfMathSplineFit(v_x, v_y);

   BfCubicSplineCurve spline(50, v);
   spline.create_vertices();

   auto compareByX = [](const glm::vec3 &a, const glm::vec3 &b) {
      return a.x < b.x;
   };

   // std::sort(v.begin(), v.end(), compareByX);

   std::vector<float> x_;
   std::vector<float> y_;

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
      ImPlot::SetupAxes("x",
                        "y",
                        ImPlotAxisFlags_NoGridLines,
                        ImPlotAxisFlags_NoGridLines);

      ImPlot::PlotLine("B(x)", x_.data(), y_.data(), spl.size());
      ImPlot::PlotScatter("fsdf", v_x.data(), v_y.data(), v.size());

      ImPlot::EndPlot();
   }

   ImGui::End();
};
