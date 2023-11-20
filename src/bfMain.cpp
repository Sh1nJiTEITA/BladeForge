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
    BfBindHolder(&__holder);


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
    bfCreateStandartRenderPass(__base);
    bfCreateGUIRenderPass(__base);

    bfInitDescriptors(__base);
    // ToDo
    bfCreateGraphicsPipelines(__base, "shaders/vert.spv", "shaders/frag.spv");
    bfCreateStandartFrameBuffers(__base);
    bfCreateGUIFrameBuffers(__base);
    bfCreateCommandPool(__base);

    bfAllocateBuffersForDynamicMesh(__base);
    bfCreateGUIDescriptorPool(__base);
    bfCreateStandartCommandBuffers(__base);
    bfCreateGUICommandBuffers(__base);
    bfCreateSyncObjects(__base);
    bfInitImGUI(__base);
}

void BfMain::__start_loop()
{
    __base.current_frame = 0;
    __base.x_scale = 1.0f;
    __base.y_scale = 1.0f;
    __base.is_resized = true;

    float local_x = 0;
    float local_y = 0;
    // Main Render Loop.

    double previousTime = glfwGetTime();

    float x = 1.0; // displays the frame rate every 1 second
    int counter = 0;
    int print_counter = 0;
    std::string fps = "Current FPS: ";

    BfMesh dynamic_mesh;
    int dots_size = 3;

    double __PI = 3.141592653589793238462643383279502884;

    /*std::vector<bfVertex> plain_vertices{
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    };

    std::vector<uint16_t> plain_indices{
        0,1,2,2,3,0
    };*/

    std::vector<bfVertex> plain_vertices{
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // 0
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // 1
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},  // 2
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, // 3
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},  // 4
    {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},   // 5
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},    // 6
    {{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}}    // 7
    };

    std::vector<uint16_t> plain_indices{
        0, 1, 2, 2, 3, 0,   // Передняя грань
        4, 5, 6, 6, 7, 4,   // Задняя грань
        0, 4, 7, 7, 3, 0,   // Левая грань
        1, 5, 6, 6, 2, 1,   // Правая грань
        3, 2, 6, 6, 7, 3,   // Верхняя грань
        0, 1, 5, 5, 4, 0    // Нижняя грань
    };

    std::vector<glm::vec3> plain_coo{
        {-0.134311, 0.882089,   -0.467724},
        {-0.994543, -0.690751,   0.815559},
        { 0.402315, -0.562322,   0.281398},
        { 0.706179, 0.289632,   -0.958235},
        { 0.643826, -0.518611,  -0.296751},
        {-0.808789, 0.713588,    0.124529},
        {-0.234509, 0.431998,   -0.899312},
        { 0.258508, 0.826344,    0.454646},
        { 0.860676, -0.122193,   0.781628},
        { 0.121563, -0.198783,   0.875485},
        {2.456, 5.783, 9.124},
        {8.321, 3.245, 6.987},
        {4.123, 7.876, 2.345},
        {9.456, 1.234, 5.678},
        {3.789, 6.543, 8.912},
        {7.012, 2.345, 4.567},
        {1.234, 8.901, 3.456},
        {6.789, 4.567, 7.890},
        {2.345, 9.012, 1.234},
        {5.678, 3.456, 6.789},
    };

    int planes_count = 20;
    BfMeshHandler mesh_handler(planes_count);
    BfMeshHandler::bind_mesh_handler(&mesh_handler);

    for (int i = 0; i < planes_count; i++) {
        mesh_handler.allocate_mesh(__base.allocator, i, BF_MESH_TYPE_CUBE);
        BfMesh* pMesh = mesh_handler.get_pMesh(i);
        pMesh->model_matrix = glm::translate(glm::mat4(1.0f), plain_coo[i]);
        pMesh->vertices = plain_vertices;
        pMesh->indices = plain_indices;

        mesh_handler.load_mesh_to_buffers(__base.allocator, i);
    }


   /* BfMesh rectangle_mesh{};
    rectangle_mesh.vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    };
    rectangle_mesh.indices = {
         0,1,2,2,3,0
    };
    mesh_handler.upload_mesh(0, rectangle_mesh);
    mesh_handler.load_mesh_to_buffers(__base.allocator, 0);
    
    BfMesh rectangle_mesh2{};
    rectangle_mesh2.vertices = {
        {{-0.5f, 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    rectangle_mesh2.indices = {
         0,1,2,2,3,0
    };

    mesh_handler.upload_mesh(1, rectangle_mesh2);
    mesh_handler.load_mesh_to_buffers(__base.allocator, 1);*/



    while (!glfwWindowShouldClose(__base.window->pWindow))
    {
        glfwPollEvents();
        __process_keys();

        double currentTime = glfwGetTime();

        // If a second has passed.




        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

        ImGui::Begin("Camera");

        static bool is_per = true;
        static bool is_ort = false;
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



        ImGui::Begin("Scale Window");
        ImGui::SliderFloat("x", &__base.x_scale, 0.0f, 2.0f);
        ImGui::SliderFloat("y", &__base.y_scale, 0.0f, 2.0f);

        ImGui::SliderFloat("+x", &__base.px, -1.0, 1.0f);
        ImGui::SliderFloat("+y", &__base.py, -1.0f, 1.0f);
        ImGui::SliderFloat("+z", &__base.pz, -1.0f, 1.0f);


        ImGui::End();

        ImGui::Begin("coordinates");

        ImGui::InputFloat("x", &local_x, 0.1f);
        ImGui::InputFloat("y", &local_y, 0.1f);

        if (ImGui::Button("Add")) {
            if ((local_x != 0) and (local_y != 0)) {
                __base.storage.push_back({ glm::vec2(local_x, local_y) });
                __base.storage.push_back({ glm::vec2(local_x, local_y) });
            }

        }

        ImGui::Begin("Performance");

        // Display the frame count here any way you want.
        counter += 1;
        if ((currentTime - previousTime) >= x) {

            print_counter = counter;
            counter = 0;
            previousTime = currentTime;
        }
        ImGui::Text((fps + std::to_string(print_counter)).c_str());
        ImGui::End();

        //ImGui::Begin("Circle");
        //{

        //    // ImGui::InputInt("Dots:", &dots_size);

        //    bool cslider = ImGui::SliderInt("Dots:", &dots_size, 3, 400);

        //    if ((ImGui::Button("Calculate circle") or cslider) and (dots_size != 0)) {


        //        float cRad = 0.7f;

        //        dynamic_mesh.vertices.clear();
        //        dynamic_mesh.indices.clear();
        //        dynamic_mesh.vertices.push_back({ {0.0f,0.0f}, {1.0, 1.0f, 1.0f} });

        //        for (int i = 0; i < dots_size; i++) {
        //            float theta1 = 2 * __PI * i / dots_size;
        //            float theta2 = 2 * __PI * (i + 1) / dots_size;

        //            float x1 = cRad * std::cos(theta1);
        //            float y1 = cRad * std::sin(theta1);

        //            float x2 = cRad * std::cos(theta2);
        //            float y2 = cRad * std::sin(theta2);

        //            int ii = i + 1;

        //            dynamic_mesh.vertices.push_back({ {0.0f, 0.0f}, {1.0 * ii / dots_size * 0.2, 1.0 * ii / dots_size * 0.5, 1.0 * ii / dots_size * 0.8} });
        //            dynamic_mesh.vertices.push_back({ {x1, y1},     {1.0 * ii / dots_size * 0.3, 1.0 * ii / dots_size * 0.6, 1.0 * ii / dots_size * 0.9} });
        //            dynamic_mesh.vertices.push_back({ {x2, y2},     {1.0 * ii / dots_size * 0.4, 1.0 * ii / dots_size * 0.7, 1.0 * ii / dots_size * 0.99} });

        //            //dynamic_mesh.vertices.push_back({ {0.0f, 0.0f}, {i / dots_size * 0.2,i / dots_size * 0.5,i / dots_size * 0.8} });
        //            //dynamic_mesh.vertices.push_back({ {x1, y1},     {i / dots_size * 0.3,i / dots_size * 0.6,i / dots_size * 0.9} });
        //            //dynamic_mesh.vertices.push_back({ {x2, y2},     {i / dots_size * 0.4,i / dots_size * 0.7,i / dots_size * 0.10} });

        //            dynamic_mesh.indices.push_back(dynamic_mesh.vertices.size() - 3); // Индексы трех вершин треугольника
        //            dynamic_mesh.indices.push_back(dynamic_mesh.vertices.size() - 2);
        //            dynamic_mesh.indices.push_back(dynamic_mesh.vertices.size() - 1);
        //        }
        //        __base.is_resized = true;
        //    }
        //}
        //ImGui::End();



        ImGui::End();
        ImGui::Render();

        bfDrawFrame(__base, dynamic_mesh, mesh_handler);

    }

    std::cout << "uniform-ave:" << BfExecutionTime::GetAverageTimeCut("uniform") << " median: " << BfExecutionTime::GetMedianTimeCut("uniform") << "\n";
    std::cout << "dynamic-ave:" << BfExecutionTime::GetAverageTimeCut("dynamic-mesh") << " median: " << BfExecutionTime::GetMedianTimeCut("dynamic-mesh") << "\n";
    vkDeviceWaitIdle(__base.device);


}

void BfMain::__kill()
{
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
