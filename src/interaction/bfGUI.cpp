#include "bfGUI.h"

std::string bfGetMenueInfoStr(BfGUI gui)
{
	if (gui.is_info) return bfSetMenueStr.at(BF_MENUE_STATUS_INFO_ENABLED);
	else return bfSetMenueStr.at(BF_MENUE_STATUS_INFO_DISABLED);
};

std::string bfGetMenueEventLogStr(BfGUI gui) {
    if (gui.is_info) return bfSetMenueStr.at(BF_MENUE_STATUS_EVENT_LOG_ENABLED);
    else return bfSetMenueStr.at(BF_MENUE_STATUS_EVENT_LOG_DISABLED);
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
                    std::string obj_name = "Obj " + std::to_string(obj->id.get());
                    
                    ImGui::Selectable(obj_name.c_str(),
                        layer->get_object_by_index(j)->get_pSelection(),
                        ImGuiSelectableFlags_AllowDoubleClick);
                    /*if (ImGui::TreeNode(obj_name.c_str())) {
                        
                        
                        ImGui::TreePop();
                    }*/


                }

                
                
                
                ImGui::TreePop();
            }
        }
        
        
        /*if (ImGui::TreeNode(var1.name.c_str())) {
            if (ImGui::IsItemClicked()) {
                ImGui::OpenPopup("Variable 1 Popup");
            }
            if (ImGui::BeginPopup("Variable 1 Popup")) {
                ShowVariableContents(var1);
                ImGui::EndPopup();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode(var2.name.c_str())) {
            if (ImGui::IsItemClicked()) {
                ImGui::OpenPopup("Variable 2 Popup");
            }
            if (ImGui::BeginPopup("Variable 2 Popup")) {
                ShowVariableContents(var2);
                ImGui::EndPopup();
            }
            ImGui::TreePop();
        }*/
    }

    ImGui::End();

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
    float x_data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
    float y_data[] = { 1.0f, 4.0f, 9.0f, 16.0f, 25.0f };

    std::vector<BfVertex3> dp{
        BfVertex3({0.0f, 0.0, 0.0}),
        BfVertex3({0.5f, 0.5, 0.0}),
        BfVertex3({1.0f, 1.0, 0.0}),
        BfVertex3({1.5f, 0.5, 0.0}),
        BfVertex3({2.0f, 0.0, 0.0}),
        //BfVertex3({2.0, 1.0, 0.0f})
    };


    auto get_vertices = [](std::vector<BfVertex3> d, int type){
        std::vector<float> o;
        o.reserve(d.size());
        for (const auto& it : d) {
            if (type == 0)
                o.emplace_back(it.pos.x);
            else if (type == 1)
                o.emplace_back(it.pos.y);
        }
        return o;
    };

    size_t n = 50;
    auto bezier_3 = std::make_shared<BfBezierCurve>(dp.size()-1, n, dp);
    auto d_bezier_3 = std::make_shared<BfBezierCurve>(bezier_3->get_derivative());
    bezier_3->create_vertices();
    
    std::vector<float> pos_x;
    pos_x.reserve(n);
    
    std::vector<float> pos_y;
    pos_y.reserve(n);

    std::vector<float> pos_x_d;
    pos_x_d.reserve(n);

    std::vector<float> pos_y_d;
    pos_y_d.reserve(n);

    std::vector<float> pos_x_ds;
    pos_x_ds.reserve(n);

    std::vector<float> pos_y_ds;
    pos_y_ds.reserve(n);

    float t;
    for (int i = 0; i < 50; i++) {

        t = static_cast<float>(i) / static_cast<float>(n - 1);
        glm::vec3 _v = bezier_3->get_single_vertex_v3(t);
        pos_x.emplace_back(_v.x);
        pos_y.emplace_back(_v.y);
    
        glm::vec3 _vd = bezier_3->get_single_derivative_1_analyt_v3(t);
        pos_x_d.emplace_back(_vd.x);
        pos_y_d.emplace_back(_vd.y);

        glm::vec3 _vds = d_bezier_3->get_single_vertex_v3(t);
        pos_x_ds.emplace_back(_vds.x);
        pos_y_ds.emplace_back(_vds.y);
    }
   
    auto dev_len_3 = bfMathGetBezierCurveLengthDerivative(bezier_3.get());
    std::vector<BfVertex3> new_bezier_3_vertices{ bezier_3->get_rdVertices() };
    
    static float lam = 0.0f;

    for (size_t i = 0; i < new_bezier_3_vertices.size(); i++) {
        if ((i == 0) || (i == new_bezier_3_vertices.size() - 1) || (i == 2))
            continue;

        new_bezier_3_vertices[i].pos -= lam * dev_len_3[i];
    }

    auto bezier_3_min_len_1 = std::make_shared<BfBezierCurve>(bezier_3->get_n(), 50, new_bezier_3_vertices);
    
    bezier_3_min_len_1->create_vertices();
    std::vector<float> bezier_3_min_len_1_x = get_vertices(bezier_3_min_len_1->get_rVertices(), 0);
    std::vector<float> bezier_3_min_len_1_y = get_vertices(bezier_3_min_len_1->get_rVertices(), 1);
 
    ImGui::Begin("My Window");
    if (ImPlot::BeginPlot("My Plot")) {
        ImPlot::SetupAxes("x", "y", ImPlotAxisFlags_NoGridLines, ImPlotAxisFlags_NoGridLines);
        
        ImPlot::PlotLine("B(x)", pos_x.data(), pos_y.data(), pos_x.size());
        ImPlot::PlotLine("dB(x)", pos_x_d.data(), pos_y_d.data(), pos_x.size());
        ImPlot::PlotLine("dBs(x)", pos_x_ds.data(), pos_y_ds.data(), pos_x_ds.size());
        ImPlot::PlotLine("B(x)_len_1", bezier_3_min_len_1_x.data(), bezier_3_min_len_1_y.data(), bezier_3_min_len_1_x.size());
      
        ImPlot::EndPlot();
    }


    ImGui::InputFloat("Lamda", &lam);

    ImGui::End();
};

