#include "BfMain.h"



void BfMain::__process_keys()
{
    if (glfwGetKey(__base.window->pWindow, GLFW_KEY_C) == GLFW_PRESS) {
        __base.window->is_free_camera_active = true;
    }
    else {
        __base.window->is_free_camera_active = false;
        __base.window->firstMouse = true;
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



    BfMeshHandler mesh_handler(4);
    mesh_handler.allocate_mesh(__base.allocator, 0, BF_MESH_TYPE_RECTANGLE);
    mesh_handler.allocate_mesh(__base.allocator, 1, BF_MESH_TYPE_RECTANGLE);

    BfMesh rectangle_mesh{};
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
    mesh_handler.load_mesh_to_buffers(__base.allocator, 1);



    while (!glfwWindowShouldClose(__base.window->pWindow))
    {
        glfwPollEvents();
        __process_keys();

        double currentTime = glfwGetTime();

        // If a second has passed.




        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::ShowDemoWindow();

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
