#include "bfGui.h"

#include "bfEvent.h"

BfGui::BfGui() {}

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

            // std::cout << "pers active" << "\n";
         }
         if (ImGui::RadioButton("Ortho", is_ort))
         {
            is_ort                        = true;
            is_per                        = false;

            __ptr_base->window->proj_mode = 1;

            // std::cout << "Ortho active" << "\n";
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

void bfShowNestedLayersRecursive(std::shared_ptr<BfDrawLayer> l)
{
#define BF_OBJ_NAME_LEN 30
#define BF_LAYER_COLOR 1.0f, 0.55f, 0.1f, 1.0f

   auto gen_vert_count = [](std::shared_ptr<BfDrawObj> o) {
      std::string vert;
      vert.reserve(BF_OBJ_NAME_LEN);
      vert.append("\tVertices ");
      vert.append(std::move(std::to_string(o->get_vertices_count())));
      return vert;
   };

   auto gen_ind_count = [](std::shared_ptr<BfDrawObj> o) {
      std::string vert;
      vert.reserve(BF_OBJ_NAME_LEN);
      vert.append("\tIndices ");
      vert.append(std::move(std::to_string(o->get_indices_count())));
      return vert;
   };

   auto gen_dvert_count = [](std::shared_ptr<BfDrawObj> o) {
      std::string vert;
      vert.reserve(BF_OBJ_NAME_LEN);
      vert.append("\tDVertices ");
      vert.append(std::move(std::to_string(o->get_dvertices_count())));
      return vert;
   };

   size_t obj_count = l->get_obj_count();
   size_t lay_count = l->get_layer_count();

   std::string lay_name;
   lay_name.reserve(BF_OBJ_NAME_LEN);
   lay_name.append("Layer (");
   lay_name.append(bfGetStrNameDrawObjType(l->id.get_type()));
   lay_name.append(") ");
   lay_name.append(std::to_string(l->id.get()));

   ImVec4 layer_color(BF_LAYER_COLOR);

   if (ImGui::TreeNode(lay_name.c_str()))
   {
      for (size_t i = 0; i < lay_count; ++i)
      {
         bfShowNestedLayersRecursive(l->get_layer_by_index(i));
      }
      for (size_t i = 0; i < obj_count; ++i)
      {
         std::shared_ptr<BfDrawObj> obj = l->get_object_by_index(i);

         std::string obj_name;
         obj_name.reserve(BF_OBJ_NAME_LEN);
         obj_name.append("Object (");
         obj_name.append(bfGetStrNameDrawObjType(obj->id.get_type()));
         obj_name.append(") ");
         obj_name.append(std::to_string(obj->id.get()));

         ImGui::PushStyleColor(ImGuiCol_Text, layer_color);
         if (ImGui::TreeNode(obj_name.c_str()))
         {
            ImGui::Text(gen_dvert_count(obj).c_str());
            ImGui::Text(gen_vert_count(obj).c_str());
            ImGui::Text(gen_ind_count(obj).c_str());
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
