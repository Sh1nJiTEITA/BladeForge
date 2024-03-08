#include "BfMain.h"




void BfMain::__process_keys()
{
    if (glfwGetKey(__base.window->pWindow, GLFW_KEY_C) == GLFW_PRESS) {
        bfToggleCamParts(__base.window, 0, true);
        return;
    }
    else {
        bfToggleCamParts(__base.window, 0, false);
    }
    
    if ((glfwGetMouseButton(__base.window->pWindow, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS) &&
        (glfwGetKey(__base.window->pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)) {
        bfToggleCamParts(__base.window, 1, true);
        return;
    }
    else {
        bfToggleCamParts(__base.window, 1, false);
    }

    if (glfwGetMouseButton(__base.window->pWindow, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS) {
        bfToggleCamParts(__base.window, 2, true);
        return;
    }
    else {
        bfToggleCamParts(__base.window, 2, false);
    }

 
}

void BfMain::__init()
{
    bfBindHolder(&__holder);
    bfBindGeometryHolder(&__geometry_holder);

    bfHoldWindow(__base.window);
    bfHoldPhysicalDevice(__base.physical_device);

    bfSetWindowSize(__base.window, BF_START_W, BF_START_H);
    bfSetWindowName(__base.window, BF_APP_NAME);
    bfCreateWindow(__base.window);

    bfCreateInstance(__base);
    bfCreateDebugMessenger(__base);
    bfCreateSurface(__base);
    bfCreatePhysicalDevice(__base);
    bfCreateLogicalDevice(__base);
    bfCreateAllocator(__base);

    bfCreateSwapchain(__base);

    bfCreateImageViews(__base);
    bfCreateDepthBuffer(__base);
    bfCreateStandartRenderPass(__base);
    bfCreateGUIRenderPass(__base);

    bfCreateIDMapImage(__base);
    bfInitOwnDescriptors(__base);
    
    bfCreateGraphicsPipelines(__base, "shaders/vert.spv", "shaders/frag.spv");
    bfCreateStandartFrameBuffers(__base);
    bfCreateGUIFrameBuffers(__base);
    bfCreateCommandPool(__base);

    bfCreateGUIDescriptorPool(__base);
    bfCreateStandartCommandBuffers(__base);
    bfCreateGUICommandBuffers(__base);
    bfCreateSyncObjects(__base);
    bfInitImGUI(__base);
    
    bfBindGeometryHolderOutsideAllocator(__base.allocator);
}

void BfMain::__kill()
{
    bfDestoryImGUI(__base);
    bfDestorySyncObjects(__base);
    bfDestroyGUICommandBuffers(__base);
    bfDestroyStandartCommandBuffers(__base);
    bfDestroyCommandPool(__base);
    bfDestroyGUIFrameBuffers(__base);
    bfDestroyStandartFrameBuffers(__base);
    bfDestroyDepthBuffer(__base);
    bfDestroyGraphicsPipelines(__base);
    bfDestroyIDMapImage(__base);
    bfDestroyGUIRenderPass(__base);
    bfDestroyStandartRenderPass(__base);
    bfDestroyImageViews(__base);
    bfDestroySwapchain(__base);
    //bfDestroyAllocator(__base);
    //bfDestroyLogicalDevice(__base);
    //bfDestroySurface(__base);
    //bfDestroyInstance(__base);
}

void BfMain::__start_loop()
{
    __base.current_frame = 0;
    __base.is_resized = true;


    double previousTime = glfwGetTime();

// BfLayerHandler
    auto layer_1 = std::make_shared<BfDrawLayer>(__base.allocator, 
                                                 sizeof(BfVertex3), 
                                                 1000, 
                                                 100);

    auto layer_2 = std::make_shared<BfDrawLayer>(__base.allocator,
        sizeof(BfVertex3),
        1000,
        100);

    std::vector<BfVertex3> plane_vertices = {
        {{0.0f, 10.0f, 0},  {0.91f,0.91f,0.91f},{0.81f,0.81f,0.81f}},
        {{-10.0f, -10.0f, 0}, {0.92f,0.92f,0.92f},{0.82f,0.82f,0.82f}},
        {{-10.0f, 0.0f, 0},{0.93f,0.93f,0.93f},{0.83f,0.83f,0.83f}},
    };

    auto obj_1 = std::make_shared<BfPlane>(plane_vertices);
    obj_1->create_vertices();
    obj_1->create_indices();
    obj_1->bind_pipeline(&__base.triangle_pipeline);
    

    auto obj_2 = std::make_shared<BfPlane>(plane_vertices);
    obj_2->get_model_matrix() = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    obj_2->create_vertices();
    obj_2->create_indices();
    obj_2->bind_pipeline(&__base.triangle_pipeline);

    auto o_line_x = std::make_shared<BfSingleLine>(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(1.0f,0.0f,0.0f));
    o_line_x->set_color({ 1.0f, 0.0f, 0.0f });
    o_line_x->create_vertices();
    o_line_x->create_indices();
    o_line_x->bind_pipeline(&__base.line_pipeline);

    auto o_line_y = std::make_shared<BfSingleLine>(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
    o_line_y->set_color({ 0.0f, 1.0f, 0.0f });
    o_line_y->create_vertices();
    o_line_y->create_indices();
    o_line_y->bind_pipeline(&__base.line_pipeline);

    auto o_line_z = std::make_shared<BfSingleLine>(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f));
    o_line_z->set_color({ 0.0f, 0.0f, 1.0f });
    o_line_z->create_vertices();
    o_line_z->create_indices();
    o_line_z->bind_pipeline(&__base.line_pipeline);

    layer_1->add(o_line_x);
    layer_1->add(o_line_y);
    layer_1->add(o_line_z);

    layer_2->add(obj_1);
    layer_2->add(obj_2);
    layer_1->update_buffer();
    layer_2->update_buffer();

    __base.layer_handler.add(layer_1);
    __base.layer_handler.add(layer_2);



    while (!glfwWindowShouldClose(__base.window->pWindow))
    {
        glfwPollEvents();
        __process_keys();
        double currentTime = glfwGetTime();


        uint32_t id_on_cursor;
        void* id_on_cursor_ = __base.id_image_buffer.allocation_info.pMappedData;
        memcpy(&id_on_cursor, id_on_cursor_, sizeof(uint32_t));
        __base.pos_id = id_on_cursor;


 

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

        __present_menu_bar();
        __present_camera();
        __present_info(currentTime, __base.pos_id);
        __present_event_log();
       // __present_id_map(__base, image_data_);
        bfPresentLayerHandler(__base.layer_handler);
        ShowTestPlot();
        
       





        ImGui::Render();

        bfDrawFrame(__base);

    }

    std::cout << BfExecutionTime::GetStr("uniform") << "\n";
    std::cout << BfExecutionTime::GetStr("draw_indexed") << "\n";
    std::cout << BfExecutionTime::GetStr("filter_mesh") << "\n";
    std::cout << BfExecutionTime::GetStr("draw_mesh") << "\n";
    std::cout << BfExecutionTime::GetStr("VkBind") << "\n";
    std::cout << BfExecutionTime::GetStr("VkDraw") << "\n";
    std::cout << BfExecutionTime::GetStr("GeometryPtr") << "\n";
    std::cout << BfExecutionTime::GetStr("FullDraw") << "\n";
    
    vkDeviceWaitIdle(__base.device);


}



void BfMain::__present_info(double currentTime, uint32_t id_map) {
    static int counter = 0;
    static int print_counter = 0;
    
    static double previousTime = currentTime;
    if (__gui.is_info) {

        std::string fps = "Current FPS: ";

        int screenWidth = ImGui::GetIO().DisplaySize.x;
        int screenHeight = ImGui::GetIO().DisplaySize.y;

        auto flags = ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoInputs;

        ImGui::SetNextWindowPos(ImVec2(screenWidth - 20, 20), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        ImGui::Begin("Info", nullptr, flags);

        // Display the frame count here any way you want.
        counter += 1;

        float x = 1.0; // displays the frame rate every 1 second
        if ((currentTime - previousTime) >= x) {

            print_counter = counter;
            counter = 0;
            previousTime = currentTime;
        }
        ImGui::Text((fps + std::to_string(print_counter)).c_str());

        std::string pos_string = "Position = (" + std::to_string(__base.window->pos.x) + ", "
            + std::to_string(__base.window->pos.y) + ", "
            + std::to_string(__base.window->pos.z) + ")";

        std::string up_string = "Up-vector = (" + std::to_string(__base.window->up.x) + ", "
            + std::to_string(__base.window->up.y) + ", "
            + std::to_string(__base.window->up.z) + ")";

        std::string front_string = "Front-vector = (" + std::to_string(__base.window->front.x) + ", "
            + std::to_string(__base.window->front.y) + ", "
            + std::to_string(__base.window->front.z) + ")";

        std::string center_string = "Center-vector = (" + std::to_string(__base.window->front.x + __base.window->pos.x) + ", "
            + std::to_string(__base.window->front.y + __base.window->pos.y) + ", "
            + std::to_string(__base.window->front.z + __base.window->pos.z) + ")";

        std::string yaw_string = "Current-yaw = " + std::to_string(__base.window->yaw);
        std::string pitch_string = "Current-pitch = " + std::to_string(__base.window->pitch);

        std::string mpos_string = "Mouse-pos = " + std::to_string(__base.window->xpos) + ", " + std::to_string(__base.window->ypos);
        std::string selected_id_string = "Selected_id = " + std::to_string(id_map);

        ImGui::Text(pos_string.c_str());
        ImGui::Text(up_string.c_str());
        ImGui::Text(front_string.c_str());
        ImGui::Text(center_string.c_str());
        ImGui::Text(yaw_string.c_str());
        ImGui::Text(pitch_string.c_str());
        ImGui::Text(mpos_string.c_str());
        ImGui::Text(selected_id_string.c_str());

        ImGui::End();
    }
}

void BfMain::__present_camera()
{
    ImGui::Begin("Camera");
    static bool is_per = true;
    static bool is_ort = false;

    ImGui::BeginTable("VIEWS", 3);

        ImGui::TableNextRow(); // 0
    
            ImGui::TableSetColumnIndex(0);
            ImGui::TableSetColumnIndex(1); if (ImGui::Button("Up", ImVec2(50,50))) bfSetOrthoUp(__base.window);
            ImGui::TableSetColumnIndex(2);

        ImGui::TableNextRow(); // 1

            ImGui::TableSetColumnIndex(0); if (ImGui::Button("Left", ImVec2(50, 50))) bfSetOrthoLeft(__base.window);
            ImGui::TableSetColumnIndex(1); if (ImGui::Button("Near", ImVec2(50, 50))) bfSetOrthoNear(__base.window);
            ImGui::TableSetColumnIndex(2); if (ImGui::Button("Right", ImVec2(50, 50))) bfSetOrthoRight(__base.window);

        ImGui::TableNextRow(); //2

            ImGui::TableSetColumnIndex(0); 
            ImGui::TableSetColumnIndex(1); if (ImGui::Button("Bottom", ImVec2(50, 50))) bfSetOrthoBottom(__base.window);
            ImGui::TableSetColumnIndex(2); if (ImGui::Button("Far", ImVec2(50, 50))) bfSetOrthoFar(__base.window);

    ImGui::EndTable();

    if (ImGui::RadioButton("Perspective", is_per)) {
        is_ort = false;
        is_per = true;

        __base.window->proj_mode = 0;

        std::cout << "pers active" << "\n";
    }
    if (ImGui::RadioButton("Ortho", is_ort)) {
        is_ort = true;
        is_per = false;

        __base.window->proj_mode = 1;

        std::cout << "Ortho active" << "\n";
    }

    ImGui::InputFloat("Ortho-left", &__base.window->ortho_left, 0.1f);
    ImGui::InputFloat("Ortho-right", &__base.window->ortho_right, 0.1f);
    ImGui::InputFloat("Ortho-top", &__base.window->ortho_top, 0.1f);
    ImGui::InputFloat("Ortho-bottom", &__base.window->ortho_bottom, 0.1f);
    ImGui::InputFloat("Ortho-near", &__base.window->ortho_near, 0.1f);
    ImGui::InputFloat("Ortho-far", &__base.window->ortho_far, 0.1f);
    ImGui::Checkbox("is_asp", &__base.window->is_asp);

    ImGui::End();
}

void BfMain::__present_menu_bar()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Create")) {
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
            }
            if (ImGui::MenuItem("Save as..")) {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
          
            if (ImGui::MenuItem(bfGetMenueInfoStr(__gui).c_str())) {
                __gui.is_info = !__gui.is_info;
            }
            if (ImGui::MenuItem(bfGetMenueEventLogStr(__gui).c_str())) {
                __gui.is_event_log = !__gui.is_event_log;
            }
            ImGui::EndMenu();
        }


        ImGui::EndMainMenuBar();
    }
}

void BfMain::__present_id_map(BfBase& base, std::vector<uint32_t> data)
{
    ImGui::Begin("IdMap");
    {
        
 
        if (ImGui::Button("Show Id map")) {
            std::ofstream outFile("id_map.txt");

            if (outFile.is_open()) {
                for (int i = 0; i < base.swap_chain_extent.height; i++) {
                    for (int j = 0; j < base.swap_chain_extent.width; j++) {
                        outFile << data[j + i * base.swap_chain_extent.width] << " ";
                    }
                    outFile << "\n";
                }

            }
            
            outFile << "Пример текста для записи в файл." << std::endl;
            outFile << "Это вторая строка." << std::endl;
            outFile << "И еще одна строка." << std::endl;

            // Закрываем файл
            outFile.close();
            
            std::random_device rd;
            std::mt19937 mt(rd());


            std::map<uint32_t, glm::vec4> id_color_map;
            for (int i = 0; i < base.swap_chain_extent.width * base.swap_chain_extent.height; ++i) {
                if (!id_color_map.contains((int)data[i])) {
                    

                    float red = (float)(mt() % 255);
                    float green = (float)(mt() % 255);
                    float blue = (float)(mt() % 255);
                    
                    id_color_map[(int)data[i]] = glm::vec4(red, green, blue, 1.0f);

                    std::cout << id_color_map[(int)data[i]].r << ", " << id_color_map[(int)data[i]].g << ", " << id_color_map[data[i]].b << "\n";
                }
            }
            


            std::vector<uint8_t> rgba_data(base.swap_chain_extent.width * base.swap_chain_extent.height * 3);
            size_t index = 0;
            for (int i = 0; i < base.swap_chain_extent.width * base.swap_chain_extent.height * 3; i+=3) {
                rgba_data[i + 0] = int(id_color_map[(int)data[index]].r);
                rgba_data[i + 1] = int(id_color_map[(int)data[index]].g);
                rgba_data[i + 2] = int(id_color_map[(int)data[index]].b);
                //rgba_data[i + 3] = id_color_map[data[i]].a;

                index++;

            }

            if (!stbi_write_png("output.png", base.swap_chain_extent.width, base.swap_chain_extent.height, 3, rgba_data.data(), base.swap_chain_extent.width * 3)) {
                // Обработайте ошибку сохранения изображения
                //std::cerr << "Ошибка сохранения изображения." << std::endl;
            }
        }

            


        //ImGui::TextWrapped(s.c_str());
    }
    ImGui::End();
}

void BfMain::__present_event_log()
{
    
    if (__gui.is_event_log) {
        ImVec2 size = { 400, 600 };
        ImVec2 pos = { __base.swap_chain_extent.width - size[0], (float)(__base.swap_chain_extent.height - size[1])};
    
        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | 
                                       ImGuiWindowFlags_NoResize | 
                                       ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoBackground |
                                       ImGuiWindowFlags_NoTitleBar | 
                                       ImGuiWindowFlags_AlwaysVerticalScrollbar;
        
       
     

        // Устанавливаем скролл внизу

        ImGui::Begin("Console", nullptr, windowFlags);
        {
            
                
            
            auto it_event = BfEventHandler::single_events.rbegin();
            auto it_time = BfEventHandler::single_event_time.rbegin();
            auto it_message = BfEventHandler::single_event_message.rbegin();

            // Перебираем оба списка одновременно в обратном порядке
            while (it_event != BfEventHandler::single_events.rend() &&
                it_time != BfEventHandler::single_event_time.rend() &&
                it_message != BfEventHandler::single_event_message.rend()) {
                ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.0f, 1.0f }, it_time->c_str());
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



BfMain::BfMain() :
    __base{},
    __holder{}
{
}

void BfMain::run()
{
    __init();
    __start_loop();

    __kill();
 }
