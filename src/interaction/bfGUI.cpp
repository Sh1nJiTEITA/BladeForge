#include "bfGUI.h"
#include "bfCurves3.h"




std::string bfGetMenueInfoStr(BfGUI gui)
{
	if (gui.is_info) return bfSetMenueStr.at(BF_MENUE_STATUS_INFO_ENABLED);
	else return bfSetMenueStr.at(BF_MENUE_STATUS_INFO_DISABLED);
};

std::string bfGetMenueEventLogStr(BfGUI gui) {
    if (gui.is_event_log) return bfSetMenueStr.at(BF_MENUE_STATUS_EVENT_LOG_ENABLED);
    else return bfSetMenueStr.at(BF_MENUE_STATUS_EVENT_LOG_DISABLED);
}

std::string bfGetMenueCameraInfoStr(BfGUI gui) {
    if (gui.is_camera_info) return bfSetMenueStr.at(BF_MENUE_STATUS_CAMERA_INFO_ENABLED);
    else return bfSetMenueStr.at(BF_MENUE_STATUS_CAMERA_INFO_DISABLED);
}

void bfPresentLayerHandler(BfLayerHandler& layer_handler)
{

    // ѕример данных (переменные)
    Variable var1 = { "Variable 1", { 10, 20, 30 } };
    Variable var2 = { "Variable 2", { 100, 200, 300 } };

    // ќтображаем основное окно
    ImGui::Begin("Debug Window");

    // ќтображаем переменные и позвол€ем пользователю нажать на них
    if (ImGui::CollapsingHeader("Variables")) {
        for (size_t i = 0; i < layer_handler.get_layer_count(); i++) {
            
            auto layer = layer_handler.get_layer_by_index(i);
            std::string layer_name = "Layer " + std::to_string(layer->id.get());

            if (ImGui::TreeNode(layer_name.c_str())) {
                for (size_t j = 0; j < layer->get_obj_count(); ++j) {
                    
                    auto obj = layer->get_object_by_index(j);
                    std::string obj_name = "Obj " + std::to_string(obj->id.get()) + ", " + bfGetStrNameDrawObjType(obj->id.get_type());
                    
                    ImGui::Selectable(obj_name.c_str(),
                        layer->get_object_by_index(j)->get_pSelection(),
                        ImGuiSelectableFlags_AllowDoubleClick);
                   
                }

                
                
                
                ImGui::TreePop();
            }
        }
        
        
        
    }

    if (ImGui::Button("Delete")) {
               
        std::vector<uint32_t> ids;
        ids.reserve(layer_handler.get_whole_obj_count());

        for (size_t i = 0; i < layer_handler.get_layer_count(); i++) {
            ids.clear();
            auto layer = layer_handler.get_layer_by_index(i);
            for (size_t j = 0; j < layer->get_obj_count(); j++) {
                if (*layer->get_object_by_index(j)->get_pSelection())
   
                    ids.emplace_back(layer->get_object_by_index(j)->id.get());
     
            }

            layer->del(ids);
        }
     
    }


    ImGui::End();

}

void bfPresentBladeSectionInside(BfBladeBase* layer, BfBladeSectionCreateInfo* info, BfBladeSectionCreateInfo* old)
{
    static int inputFloatMode = 0;
    auto make_row = [](std::string n,
        std::string d,
        std::string dim,
        float* value,
        float left,
        float right)
        {
            static int count = 0;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text(n.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text(d.c_str());
            ImGui::TableSetColumnIndex(2);
            std::string dsadasd = dim + "##float" + (char)(count);

            switch (inputFloatMode) {
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
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        //ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
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

        int flags = ImGuiTableFlags_NoHostExtendX |
            ImGuiTableFlags_SizingFixedFit;

        ImGui::BeginTable("FloatTable", 3, flags);
        {
            ImGui::TableSetupColumn("Parameter name");
            ImGui::TableSetupColumn("Description");
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();

            make_row("Width", "B", "[m]##0", &info->width, 0.0f, 10.0f);
            make_row("Install angle", "alpha_y", "[deg]##1", &info->install_angle, -180.0f, 180.0f);
            make_row("Inlet angle", "beta_1", "[deg]##2", &info->inlet_angle, -180.0f, 180.0f);
            make_row("Outlet angle", "beta_2", "[deg]##3", &info->outlet_angle, -180.0f, 180.0f);
            make_row("Inlet surface angle", "omega_1", "[deg]##4", &info->inlet_surface_angle, -45.0f, 45.0f);
            make_row("Outlet surface angle", "omega_2", "[deg]##5", &info->outlet_surface_angle, -45.0f, 45.0f);
            make_row("Inlet radius", "r_1", "[m]##6", &info->inlet_radius, 0.00001, 0.05);
            make_row("Outlet radius", "r_2", "[m]##7", &info->outlet_radius, 0.00001, 0.05);
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

    

    
    //if (!bfCheckBladeSectionCreateInfoEquality(*info, *old)) {

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

void ShowVariableContents(const Variable& var)
{
	ImGui::Text("Variable: %s", var.name.c_str());
	ImGui::Separator();
	ImGui::Text("Values:");
	for (size_t i = 0; i < var.values.size(); ++i) {
		ImGui::Text("- %d", var.values[i]);
	}
}

void ShowTestPlot()
{

    std::vector<glm::vec3> v{
        {0.0, 0.0, 0.0f},
        {1.0, 1.0f, 0.0f},
        {2.0f, 0.0f, 0.0f},
        {4.0f, 2.0f, 0.0f},
        {6.0f, -3.0f, 0.0f}
    };

    std::vector<float> v_x;
    std::vector<float> v_y;

    for (size_t i = 0; i < v.size(); i++) {
        v_x.push_back(v[i].x);
        v_y.push_back(v[i].y);

    }

    std::vector<glm::vec2> spl = bfMathSplineFit(v_x, v_y);


    BfCubicSplineCurve spline(50, v);
    spline.create_vertices();

    auto compareByX = [](const glm::vec3& a, const glm::vec3& b) {
        return a.x < b.x;
        };

    //std::sort(v.begin(), v.end(), compareByX);

    std::vector<float> x_;
    std::vector<float> y_;

    

    for (size_t i = 0; i < spl.size();i++) {
        
        x_.push_back(spl[i].x);
        y_.push_back(spl[i].y);
        
        // x_.push_back(spl[i].x);
        // y_.push_back(spl[i].y);
    }


    ImGui::Begin("My Window");
    if (ImPlot::BeginPlot("My Plot")) {
        ImPlot::SetupAxes("x", "y", ImPlotAxisFlags_NoGridLines, ImPlotAxisFlags_NoGridLines);
        
        ImPlot::PlotLine("B(x)", x_.data(), y_.data(), spl.size());
        ImPlot::PlotScatter("fsdf", v_x.data(), v_y.data(), v.size());

        ImPlot::EndPlot();
    }

    ImGui::End();
};

