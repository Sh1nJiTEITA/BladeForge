#include <catch2/catch_test_macros.hpp>

#include "bfBase.h"
#include "bfEvent.h"
#include "bfWindow.h"
#include "bfHolder.h"
#include "bfConsole.h"

BfEvent return_event() {
    BfSingleEvent se{};
    se.action = BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_SUCCESS;
    se.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

    return BfEvent(se);
}

TEST_CASE("BF_EVENT_H", "[single-file]") {
    
    //BfBase base;
    
    int event_number = 6;

    std::vector<BfSingleEvent> events(event_number);
    std::vector<BfEnActionType> event_actions(event_number, BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_SUCCESS);
    std::vector<BfEnSingleEventType> event_types(event_number, BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT);

    for (int i = 0; i < event_number; i++) {
        events[i].action = event_actions[i];
        events[i].type = event_types[i];

        BfSingleEvent local_event{ events[i] };

        BfEvent l(local_event);
    }

    return_event();

    //BfEvent(event);
    
    int i_start = 0;
    for (const auto& it : BfEventHandler::single_events) {
        //std::cout << it.code.value() << " " << it.info << " " << it.time << " " << (int)it.type << std::endl;
        
        int i_ = event_number - i_start;

        if (i_ == event_number) {
            REQUIRE(it.action == BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_SUCCESS);
            REQUIRE(it.type == BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT);
          
        }
        else {
            REQUIRE(it.action == event_actions[i_]);
            REQUIRE(it.type == event_types[i_]);
        }
        

        i_start++;
    }

}

TEST_CASE("BfHandler-test") {
    
    BfEventHandler::funcPtr = &BfConsole::print_single_single_event;
    BfHolder mHolder{};
    BfBindHolder(&mHolder);
    
    BfBase mBase{};
    
    bfHoldWindow(mBase.window);
    bfHoldPhysicalDevice(mBase.physical_device);
    
    bfSetWindowSize(mBase.window, 800, 600);
    bfSetWindowName(mBase.window, "BladeForge1001");
    bfCreateWindow(mBase.window);

    bfCreateInstance(mBase);
    bfCreateDebugMessenger(mBase);
    bfCreateSurface(mBase);
    bfCreatePhysicalDevice(mBase);
    bfCreateLogicalDevice(mBase);
    bfCreateAllocator(mBase);
    bfCreateSwapchain(mBase);
    bfCreateImageViews(mBase);
    bfCreateStandartRenderPass(mBase);
    bfCreateGUIRenderPass(mBase);
    //bfCreateDescriptorSetLayout(mBase);
    bfInitDescriptors(mBase); //
    bfCreateGraphicsPipelines(mBase, "shaders/vert.spv", "shaders/frag.spv");
    bfCreateStandartFrameBuffers(mBase);
    bfCreateGUIFrameBuffers(mBase);
    bfCreateCommandPool(mBase);

    BfMesh mesh{};
    mesh.vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    /*mesh.vertices = {
        {{10.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{10.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{10.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}
    };*/
    mesh.indices = {
        0,1,2,2,3,0
    };

    
    //bfUploadMesh(mBase, mesh);
    bfAllocateBuffersForDynamicMesh(mBase);
    //bfCreateUniformBuffers(mBase);
    //bfCreateStandartDescriptorPool(mBase);
    bfCreateGUIDescriptorPool(mBase);
    //bfCreateDescriptorSets(mBase);
    bfCreateStandartCommandBuffers(mBase);
    bfCreateGUICommandBuffers(mBase);
    bfCreateSyncObjects(mBase);
    bfInitImGUI(mBase);

    mBase.current_frame = 0;
    mBase.x_scale = 1.0f;
    mBase.y_scale = 1.0f;
    mBase.is_resized = true;

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


    while (!glfwWindowShouldClose(mBase.window->pWindow))
    {
        glfwPollEvents();

        double currentTime = glfwGetTime();

        // If a second has passed.
        



        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::ShowDemoWindow();

        ImGui::Begin("Scale Window");
        ImGui::SliderFloat("x", &mBase.x_scale, 0.0f, 2.0f);
        ImGui::SliderFloat("y", &mBase.y_scale, 0.0f, 2.0f);
        
        ImGui::SliderFloat("+x", &mBase.px, -1.0, 1.0f);
        ImGui::SliderFloat("+y", &mBase.py, -1.0f, 1.0f);
        ImGui::SliderFloat("+z", &mBase.pz, -1.0f, 1.0f);
        
        
        ImGui::End();

        ImGui::Begin("coordinates");

        ImGui::InputFloat("x", &local_x, 0.1f);
        ImGui::InputFloat("y", &local_y, 0.1f);

        if (ImGui::Button("Add")) {
            if ((local_x != 0) and (local_y != 0)) {
                mBase.storage.push_back({ glm::vec2(local_x, local_y) });
                mBase.storage.push_back({ glm::vec2(local_x, local_y) });
            }

        }

        ImGui::Begin("Performance");

        


        // Display the frame count here any way you want.
        counter += 1;
       

        if ((currentTime - previousTime) >= x){
           
            print_counter = counter;
            counter = 0;
            previousTime = currentTime;
        }
        

        
     
        ImGui::Text((fps + std::to_string(print_counter)).c_str());
        ImGui::End();

        ImGui::Begin("Circle");
        {
           
           // ImGui::InputInt("Dots:", &dots_size);
           
            bool cslider = ImGui::SliderInt("Dots:", &dots_size, 3, 400);

            if ((ImGui::Button("Calculate circle") or cslider) and (dots_size != 0)) {
                
                
                float cRad = 0.7f;

                dynamic_mesh.vertices.clear();
                dynamic_mesh.indices.clear();
                dynamic_mesh.vertices.push_back({ {0.0f,0.0f}, {1.0, 1.0f, 1.0f} });

                for (int i = 0; i < dots_size; i++) {
                    float theta1 = 2 * __PI * i / dots_size;
                    float theta2 = 2 * __PI * (i+1) / dots_size;

                    float x1 = cRad * std::cos(theta1);
                    float y1 = cRad * std::sin(theta1);

                    float x2 = cRad * std::cos(theta2);
                    float y2 = cRad * std::sin(theta2);

                    int ii = i + 1;

                    dynamic_mesh.vertices.push_back({ {0.0f, 0.0f}, {1.0 * ii / dots_size * 0.2, 1.0 * ii / dots_size * 0.5, 1.0 * ii / dots_size * 0.8} });
                    dynamic_mesh.vertices.push_back({ {x1, y1},     {1.0 * ii / dots_size * 0.3, 1.0 * ii / dots_size * 0.6, 1.0 * ii / dots_size * 0.9} });
                    dynamic_mesh.vertices.push_back({ {x2, y2},     {1.0 * ii / dots_size * 0.4, 1.0 * ii / dots_size * 0.7, 1.0 * ii / dots_size * 0.99} });

                    //dynamic_mesh.vertices.push_back({ {0.0f, 0.0f}, {i / dots_size * 0.2,i / dots_size * 0.5,i / dots_size * 0.8} });
                    //dynamic_mesh.vertices.push_back({ {x1, y1},     {i / dots_size * 0.3,i / dots_size * 0.6,i / dots_size * 0.9} });
                    //dynamic_mesh.vertices.push_back({ {x2, y2},     {i / dots_size * 0.4,i / dots_size * 0.7,i / dots_size * 0.10} });
                    
                    dynamic_mesh.indices.push_back(dynamic_mesh.vertices.size() - 3); // Индексы трех вершин треугольника
                    dynamic_mesh.indices.push_back(dynamic_mesh.vertices.size() - 2);
                    dynamic_mesh.indices.push_back(dynamic_mesh.vertices.size() - 1);
                }
                mBase.is_resized = true;
            }
        }
        ImGui::End();



        ImGui::End();
        ImGui::Render();
        
        bfDrawFrame(mBase, dynamic_mesh);
      
    }

    std::cout << "uniform-ave:" << BfExecutionTime::GetAverageTimeCut("uniform") << " median: " << BfExecutionTime::GetMedianTimeCut("uniform") << "\n";
    std::cout << "dynamic-ave:" << BfExecutionTime::GetAverageTimeCut("dynamic-mesh") << " median: " << BfExecutionTime::GetMedianTimeCut("dynamic-mesh") << "\n";
    vkDeviceWaitIdle(mBase.device);




    REQUIRE(BfEventHandler::is_all_ok() == true);
    BfConsole::print_all_single_events(
        (int)BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT |
        (int)BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_HOLDER_EVENT |
        (int)BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT |
        (int)BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_USER_EVENT
    );
}