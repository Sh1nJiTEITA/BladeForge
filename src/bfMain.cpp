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

    //bfInitDescriptors(__base);
    bfCreateIDMapImage(__base);
    bfInitOwnDescriptors(__base);
    
    // ToDo
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





    BfMesh dynamic_mesh;
    int dots_size = 3;

    double __PI = 3.141592653589793238462643383279502884;

    std::vector<bfVertex> plain_vertices0{
        {{0.0, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{0.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    };

    std::vector<uint16_t> plain_indices0{
        0,1,2,2,3,0
    };


    std::vector<bfVertex> cube_vertices{
        {{-0.5f, -0.5f, -0.5f,}, {  0.0f, 0.0f,  1.0f}},
        {{ 0.5f, -0.5f, -0.5f,}, {  1.0f, 0.0f,  1.0f}},
        {{ 0.5f,  0.5f, -0.5f,}, {  1.0f, 1.0f,  1.0f}},
        {{ 0.5f,  0.5f, -0.5f,}, {  1.0f, 1.0f,  1.0f}},
        {{-0.5f,  0.5f, -0.5f,}, {  0.0f, 1.0f,  1.0f}},
        {{-0.5f, -0.5f, -0.5f,}, {  0.0f, 0.0f,  1.0f}},

        {{-0.5f, -0.5f,  0.5f,}, {  0.0f, 0.0f,  1.0f}},
        {{ 0.5f, -0.5f,  0.5f,}, {  1.0f, 0.0f,  1.0f}},
        {{ 0.5f,  0.5f,  0.5f,}, {  1.0f, 1.0f,  1.0f}},
        {{ 0.5f,  0.5f,  0.5f,}, {  1.0f, 1.0f,  1.0f}},
        {{-0.5f,  0.5f,  0.5f,}, {  0.0f, 1.0f,  1.0f}},
        {{-0.5f, -0.5f,  0.5f,}, {  0.0f, 0.0f,  1.0f}},

        {{-0.5f,  0.5f,  0.5f,}, {  1.0f, 0.0f,  1.0f}},
        {{-0.5f,  0.5f, -0.5f,}, {  1.0f, 1.0f,  1.0f}},
        {{-0.5f, -0.5f, -0.5f,}, {  0.0f, 1.0f,  1.0f}},
        {{-0.5f, -0.5f, -0.5f,}, {  0.0f, 1.0f,  1.0f}},
        {{-0.5f, -0.5f,  0.5f,}, {  0.0f, 0.0f,  1.0f}},
        {{-0.5f,  0.5f,  0.5f,}, {  1.0f, 0.0f,  1.0f}},

        {{ 0.5f,  0.5f,  0.5f,}, {  1.0f, 0.0f,  1.0f}},
        {{ 0.5f,  0.5f, -0.5f,}, {  1.0f, 1.0f,  1.0f}},
        {{ 0.5f, -0.5f, -0.5f,}, {  0.0f, 1.0f,  1.0f}},
        {{ 0.5f, -0.5f, -0.5f,}, {  0.0f, 1.0f,  1.0f}},
        {{ 0.5f, -0.5f,  0.5f,}, {  0.0f, 0.0f,  1.0f}},
        {{ 0.5f,  0.5f,  0.5f,}, {  1.0f, 0.0f,  1.0f}},

        {{-0.5f, -0.5f, -0.5f,}, {  0.0f, 1.0f,  1.0f}},
        {{ 0.5f, -0.5f, -0.5f,}, {  1.0f, 1.0f,  1.0f}},
        {{ 0.5f, -0.5f,  0.5f,}, {  1.0f, 0.0f,  1.0f}},
        {{ 0.5f, -0.5f,  0.5f,}, {  1.0f, 0.0f,  1.0f}},
        {{-0.5f, -0.5f,  0.5f,}, {  0.0f, 0.0f,  1.0f}},
        {{-0.5f, -0.5f, -0.5f,}, {  0.0f, 1.0f,  1.0f}},

        {{-0.5f,  0.5f, -0.5f,}, {  0.0f, 1.0f,  1.0f}},
        {{ 0.5f,  0.5f, -0.5f,}, {  1.0f, 1.0f,  1.0f}},
        {{ 0.5f,  0.5f,  0.5f,}, {  1.0f, 0.0f,  1.0f}},
        {{ 0.5f,  0.5f,  0.5f,}, {  1.0f, 0.0f,  1.0f}},
        {{-0.5f,  0.5f,  0.5f,}, {  0.0f, 0.0f,  1.0f}},
        {{-0.5f,  0.5f, -0.5f,}, {  0.0f, 1.0f,  1.0f}}
    };

    std::vector<uint16_t> cube_indices{
        0,1,2,3,4,5,6,
        7,8,9,10,11,12,
        13,14,15,16,17,
        18,19,20,21,22,
        23,24,25,26,27,
        28,29,30,31,32,33,
        34,35
    };

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

    std::vector<bfVertex> plain_vertices2{
     {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
     {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
     {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
     {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},

     {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
     {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
     {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f} },
     {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> plain_indices2 = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };

    std::vector<bfVertex> cube_vertices2{
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // 0
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // 1
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},   // 5
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},   // 5
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},  // 4
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // 0

        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, // 3
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},  // 2
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},    // 6
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},    // 6
        {{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}},    // 7
        { {-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, // 3

        {{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}},    // 7
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},  // 4
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},   // 5
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},   // 5
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},    // 6
        {{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}},    // 7

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // 0
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // 1
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},  // 2
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},  // 2
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, // 3
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // 0

        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},    // 6
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},   // 5
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // 1
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // 1
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},  // 2
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},    // 6

        {{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}},    // 7
        {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},  // 4
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // 0
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // 0
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, // 3
        {{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}},    // 7
    };

    std::vector<uint16_t> plain_indices{
        0, 1, 2, 2, 3, 0,   // Передняя грань
        0, 4, 7, 7, 3, 0,   // Левая грань
        0, 1, 5, 5, 4, 0,    // Нижняя грань
        1, 5, 6, 6, 2, 1,   // Правая грань
        3, 2, 6, 6, 7, 3,   // Верхняя грань
        4, 5, 6, 6, 7, 4   // Задняя грань
    };

    std::vector<glm::vec3> plain_coo{
        {0.5f,0.5f,0.5f},
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

    };

    std::vector<BfVertex3> basises_vertices{
        {{0.0f,0.0f,0.0f},{1.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}},
        {{10.0f,0.0f,0.0f},{1.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}},

        {{0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f}, {0.0f,0.0f,0.0f}},
        {{0.0f,10.0f,0.0f},{0.0f,1.0f,0.0f}, {0.0f,0.0f,0.0f}},

        {{0.0f,0.0f,0.0f},{0.0f,0.0f,1.0f}, {0.0f,0.0f,0.0f}},
        {{0.0f,0.0f,10.0f},{0.0f,0.0f,1.0f}, {0.0f,0.0f,0.0f}}
    };



    //BfBezier bezier2(2, { {0.0f, 0.0f, 0.0f}, {0.0f, 3.5f, -1.0f}, {2.5f, 1.5f,4.0f} });
    //BfBezier bezier2(2, { {0.0f, 0.0f, 0.0f}, {0.0f, 3.5f, 0.0f}, {2.5f, 1.5f,0.0f} });

    //BfBezier bezier2(3, { {0.2, 1.5, 0.0f}, {1.2, 0.2, 0.0f}, {2.2, 0.95,0.0f},{1.4, 2.4, 0.0f} });
    BfBezier bezier2(3, { {-0.5, 0.1, 0.0f}, {-0.1, 0.7, 0.0f}, {0.1, 0.7,0.0f},{0.5, 0.1, 0.0f} });
    bezier2.set_color({ 0.7,0.6,0.4 });
    bezier2.update_vertices(50);

    std::vector<glm::vec3> grad = bezier2.get_length_grad({ 0, 1, 1, 0 });
    
    //BfBezier bezier2a(3, { {0.5, 0.35, 0.0f}, {0.45, 2.35, 0.0f}, {2.2, 2.35,0.0f},{2.2, 1.35, 0.0f} });
    float lam = 0.1;
    size_t nuu = 2;

    std::vector<BfBezier> bez_less(1+ nuu);
    bez_less[0] = bezier2;


    for (int i = 1; i < nuu; i++) {
      
        
        std::vector<glm::vec3> dp = bez_less[i - 1].get_vertices();
        std::vector<glm::vec3> dp_less = bez_less[i - 1].get_length_grad({ 0,1,1,0 });

        for (int j = 0; j < dp.size(); j++) {
            dp[j] -= lam * dp_less[j];
        }

        bez_less[i] = BfBezier(3, dp);
        bez_less[i].update_vertices(50);
    }

    BfBezier bezier2a(3, 
        { {0.2 - lam*grad[0].x, 1.5 - lam * grad[0].y, 0.0f - lam * grad[0].z},
        {1.2 - lam * grad[1].x, 0.2 - lam * grad[1].y, 0.0f - lam * grad[1].z},
        {2.2 - lam * grad[2].x, 0.95 - lam * grad[2].y,0.0f - lam * grad[2].z},
        {1.4 - lam * grad[3].x, 2.4 - lam * grad[3].y, 0.0f - lam * grad[3].z}});
    

    bezier2a.set_color({ 0.3,0.7,0.9 });

    bezier2a.update_vertices(50);
    


// CURVE-SET
    // Bezier
    
    bfAllocateGeometrySet(BF_GEOMETRY_TYPE_CURVE_BEZIER, 10000);
    bfAllocateGeometrySet(BF_GEOMETRY_TYPE_HANDLE_CURVE_BEZIER, 10000);
    bfAllocateGeometrySet(BF_GEOMETRY_TYPE_CARCASS_CURVE_BEZIER, 10000);

    bfBindGraphicsPipeline(BF_GEOMETRY_TYPE_CURVE_BEZIER, &__base.line_pipeline, BF_GRAPHICS_PIPELINE_LINES);
    bfBindGraphicsPipeline(BF_GEOMETRY_TYPE_HANDLE_CURVE_BEZIER, &__base.triangle_pipeline, BF_GRAPHICS_PIPELINE_TRIANGLE);
    bfBindGraphicsPipeline(BF_GEOMETRY_TYPE_CARCASS_CURVE_BEZIER, &__base.line_pipeline, BF_GRAPHICS_PIPELINE_LINES);

    int bezier_count = 1+ nuu;

    std::vector<BfObjectData> set_bezier2_obj_data(bezier_count);
    for (int i = 0; i < bezier_count; i++) {
        set_bezier2_obj_data[i].id = i+1;
        set_bezier2_obj_data[i].model_matrix = glm::translate(glm::mat4(1.0f), //glm::vec3(1.0f * i, 1.0f * i, 1.0f * i));
            glm::vec3(1.0f, 1.0f, 1.0f));

        bfAddToHolder(bez_less[i], set_bezier2_obj_data[i]);
       //bfAddToHolder(bezier2, set_bezier2_obj_data[i]);
       //bfAddToHolder(bezier2a, set_bezier2_obj_data[i]);

 
    }


    __geometry_holder.get_geometry_set(BF_GEOMETRY_TYPE_CURVE_BEZIER)->write_to_buffers();
    __geometry_holder.get_geometry_set(BF_GEOMETRY_TYPE_HANDLE_CURVE_BEZIER)->write_to_buffers();
    __geometry_holder.get_geometry_set(BF_GEOMETRY_TYPE_CARCASS_CURVE_BEZIER)->write_to_buffers();

    // Linear

    //BfCurveSet linear_set(BF_CURVE_TYPE_LINEAR, 3, __base.allocator);

    bfAllocateGeometrySet(BF_GEOMETRY_TYPE_CURVE_LINEAR, 3);
    bfBindGraphicsPipeline(BF_GEOMETRY_TYPE_CURVE_LINEAR, &__base.line_pipeline, BF_GRAPHICS_PIPELINE_LINES);


    BfObjectData set_linear_besises1_obj_data{};
    set_linear_besises1_obj_data.id = 10001;
    set_linear_besises1_obj_data.model_matrix = glm::mat4(1.0f);

    BfObjectData set_linear_besises2_obj_data{};
    set_linear_besises2_obj_data.id = 10002;
    set_linear_besises2_obj_data.model_matrix = glm::mat4(1.0f);

    BfObjectData set_linear_besises3_obj_data{};
    set_linear_besises3_obj_data.id = 10003;
    set_linear_besises3_obj_data.model_matrix = glm::mat4(1.0f);

    BfLine ort_x({ {0.0f,0.0f,0.0f}, {1.0f,0.0f,0.0f} }, { {10.0f,0.0f,0.0f},{1.0f,0.0f,0.0f} });
    BfLine ort_y({ {0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f} }, { { 0.0f,10.0f,0.0f }, { 0.0f,1.0f,0.0f } });
    BfLine ort_z({ {0.0f,0.0f,0.0f},{0.0f,0.0f,1.0f} }, { {0.0f,0.0f,10.0f},{0.0f,0.0f,1.0f} });


    bfAddToHolder(ort_x, set_linear_besises1_obj_data);
    bfAddToHolder(ort_y, set_linear_besises2_obj_data);
    bfAddToHolder(ort_z, set_linear_besises3_obj_data);

    __geometry_holder.get_geometry_set(BF_GEOMETRY_TYPE_CURVE_LINEAR)->write_to_buffers();
// NEW LAYER SYSTEM

    BfLayer layer({ 0,0,0 }, { 1,0,0 }, { 1,1,0 });
    layer.bind_allocator(&__base.allocator);
    
    auto rectangle_1 = std::make_shared<BfRectangle>(
        BfVertex3({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }),
        BfVertex3({0.0f,1.0f,0.0f}, {1.0f,1.0f,1.0f})
    );

    auto rec_id = layer.add_obj(rectangle_1);
    rectangle_1->calculate_geometry(BF_DRAW_OBJECT_TRIANGLE_MODE_BIT);
    
    layer.allocate_buffers(100);
    layer.write_to_buffers();



// CURVE-SET
   
    std::string front_string;
    std::string up_string;
    std::string pos_string;
    std::string center_string;

    //BfMesh* pMesh1 = mesh_handler.get_pMesh(0);
    //BfMesh* pMesh2 = mesh_handler.get_pMesh(1);
    glm::vec3 pos_vec{ 0.0f,0.0f,0.0f };
    
    bool is_x_rotating = false;
    bool is_y_rotating = false;
    bool is_z_rotating = false;

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

   /* void* vert_d = layer_1->__buffer.get_vertex_allocation_info().pMappedData;
    std::array<BfVertex3, 10> vert_dd;
    memcpy(vert_dd.data(), vert_d, sizeof(BfVertex3) * 10);
    void* ind_d = layer_1->__buffer.get_index_allocation_info().pMappedData;
    std::array<uint16_t, 10> ind_dd;
    memcpy(ind_dd.data(), ind_d, sizeof(uint16_t) * 10);*/


    while (!glfwWindowShouldClose(__base.window->pWindow))
    {
        glfwPollEvents();
        __process_keys();
        double currentTime = glfwGetTime();

        // If a second has passed.

        std::array<uint32_t, 32> ddata;

        void* pdata = __base.descriptor.get_buffer(BfDescriptorPosPickUsage, __base.current_frame)->allocation_info.pMappedData;
        //void* pdata = __base.frame_pack[__base.current_frame].pos_pick_buffer->allocation_info.pMappedData;

        memcpy(ddata.data(), pdata, sizeof(uint32_t) * 32);

        size_t image_size = __base.swap_chain_extent.height * __base.swap_chain_extent.width * sizeof(uint32_t);
        
        std::vector<uint32_t> image_data_;
        image_data_.resize(__base.swap_chain_extent.height * __base.swap_chain_extent.width);
        
        void* image_data = __base.id_image_buffer.allocation_info.pMappedData;
        memcpy(image_data_.data(), image_data, image_size);

        uint32_t pos_id = image_data_[__base.window->xpos + __base.window->ypos * __base.swap_chain_extent.width];


 

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::ShowDemoWindow();

        __present_menu_bar();
        __present_camera();
        __present_info(currentTime, pos_id);
        __present_id_map(__base, image_data_);
        bfPresentLayerHandler(__base.layer_handler);
        ShowTestPlot();
        
        static ImVec2 button_pos{ 500, 500 };

        ImGui::SetNextWindowPos(button_pos, 0, ImVec2(0.5, 0.5f));     


        ImGui::SetNextWindowSize(ImVec2(20, 20));
        ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration);

        // Определяем размер кнопки
        float buttonWidth = 20;
     

        // Вычисляем позицию X для центрирования кнопки внутри окна
        float centerX = ImGui::GetWindowSize().x * 0.5f - buttonWidth * 0.5f;
        float centerY = ImGui::GetWindowSize().y * 0.5f - buttonWidth * 0.5f;

        // Устанавливаем курсор по X координате для центрирования кнопки
        ImGui::SetCursorPosX(centerX);
        ImGui::SetCursorPosY(centerY);

        // Создаем кнопку с заданным размером
        ImGui::Button(" ", ImVec2(buttonWidth, buttonWidth));
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
            button_pos = ImGui::GetMousePos();
        }
        ImGui::End();







        ImGui::Begin("Scale Window");
            
            

            ImGui::SliderFloat("x", &__base.x_scale, 0.0f, 2.0f);
            ImGui::SliderFloat("y", &__base.y_scale, 0.0f, 2.0f);
            ImGui::InputFloat("XYZ", &__base.xyz_scale, 0.1f);

            ImGui::SliderFloat("+x", &__base.px, -1.0, 1.0f);
            ImGui::SliderFloat("+y", &__base.py, -1.0f, 1.0f);
            ImGui::SliderFloat("+z", &__base.pz, -1.0f, 1.0f);

            /*ImGui::BeginTable("Move Objets", 2);
                
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                if (ImGui::Button("+x_pos")) {
                    pMesh1->model_matrix = glm::translate(pMesh1->model_matrix, glm::vec3(1.0f, 0.0f, 0.0f));
                    pMesh2->model_matrix = glm::translate(pMesh2->model_matrix, glm::vec3(1.0f, 0.0f, 0.0f));
                }
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Button("-x_pos")) {
                    pMesh1->model_matrix = glm::translate(pMesh1->model_matrix, glm::vec3(-1.0f, 0.0f, 0.0f));
                    pMesh2->model_matrix = glm::translate(pMesh2->model_matrix, glm::vec3(-1.0f, 0.0f, 0.0f));
                }
                
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                if (ImGui::Button("+y_pos")) {
                    pMesh1->model_matrix = glm::translate(pMesh1->model_matrix, glm::vec3(0.0f, 1.0f, 0.0f));
                    pMesh2->model_matrix = glm::translate(pMesh2->model_matrix, glm::vec3(0.0f, 1.0f, 0.0f));
                }
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Button("-y_pos")) {
                    pMesh1->model_matrix = glm::translate(pMesh1->model_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
                    pMesh2->model_matrix = glm::translate(pMesh2->model_matrix, glm::vec3(0.0f, -1.0f, 0.0f));
                }

                ImGui::TableNextRow();
                
                ImGui::TableSetColumnIndex(0);
                if (ImGui::Button("+z_pos")) {
                    pMesh1->model_matrix = glm::translate(pMesh1->model_matrix, glm::vec3(0.0f, 0.0f, 1.0f));
                    pMesh2->model_matrix = glm::translate(pMesh2->model_matrix, glm::vec3(0.0f, 0.0f, 1.0f));
                }
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Button("-z_pos")) {
                    pMesh1->model_matrix = glm::translate(pMesh1->model_matrix, glm::vec3(0.0f, 0.0f, -1.0f));
                    pMesh2->model_matrix = glm::translate(pMesh2->model_matrix, glm::vec3(0.0f, 0.0f, -1.0f));
                }

            ImGui::EndTable();*/

           /* ImGui::Checkbox("x_rot", &is_x_rotating);
            ImGui::Checkbox("y_rot", &is_y_rotating);
            ImGui::Checkbox("z_rot", &is_z_rotating);

            float xr = 0.0f;
            float yr = 0.0f;
            float zr = 0.0f;

            if (is_x_rotating == true) xr = 0.1;
            if (is_y_rotating == true) yr = 0.1;
            if (is_z_rotating == true) zr = 0.1;
            
            if (is_x_rotating || is_y_rotating || is_z_rotating)
            {
                pMesh1->model_matrix = glm::rotate(pMesh1->model_matrix, (float)currentTime / 5, glm::vec3(xr, yr, zr));
                pMesh2->model_matrix = glm::rotate(pMesh2->model_matrix, (float)currentTime / 5, glm::vec3(xr, yr, zr));
            }*/

            if (ImGui::Button("Toggle Front-vec")) {
                __base.window->front *= -1;
            }

        ImGui::End();

        ImGui::Begin("coordinates");

        ImGui::InputFloat("x", &local_x, 0.1f);
        ImGui::InputFloat("y", &local_y, 0.1f);

       /* if (ImGui::Button("Add")) {
            if ((local_x != 0) and (local_y != 0)) {
                __base.storage.push_back({ glm::vec2(local_x, local_y) });
                __base.storage.push_back({ glm::vec2(local_x, local_y) });
            }

        }*/

        

       
        
        /*BfExecutionTime::BeginTimeCut("Vertices-present");
            __present_vertices(&mesh_handler);
        BfExecutionTime::EndTimeCut("Vertices-present");*/



        ImGui::End();
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

void BfMain::__kill()
{
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
            
            /*std::string perfomance_view_pannel_str;
            if (__gui.is_info) perfomance_view_pannel_str = "Hide perfomance/view pannel";
            else perfomance_view_pannel_str = "Show perfomance/view pannel";*/

            if (ImGui::MenuItem(bfGetMenueInfoStr(__gui).c_str())) {
                __gui.is_info = !__gui.is_info;
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
            std::vector<unsigned char> rgba_data(base.swap_chain_extent.width * base.swap_chain_extent.height * 4);

            for (int i = 0; i < base.swap_chain_extent.width * base.swap_chain_extent.height; ++i) {
                uint32_t pixel = data[i];
                rgba_data[i * 4] = (pixel >> 16) & 0xFF; // Красный канал
                rgba_data[i * 4 + 1] = (pixel >> 8) & 0xFF; // Зеленый канал
                rgba_data[i * 4 + 2] = pixel & 0xFF; // Синий канал
                rgba_data[i * 4 + 3] = (pixel >> 24) & 0xFF; // Альфа канал
            }

            if (!stbi_write_png("output.png", base.swap_chain_extent.width, base.swap_chain_extent.height, 4, rgba_data.data(), base.swap_chain_extent.width * 4)) {
                // Обработайте ошибку сохранения изображения
                std::cerr << "Ошибка сохранения изображения." << std::endl;
            }
        }

            


        //ImGui::TextWrapped(s.c_str());
    }
    ImGui::End();
}


void BfMain::__present_vertices(BfMeshHandler* handler)
{
    ImGui::Begin("Vertices");
        
        static bool is_xyz = false;
        ImGui::Checkbox("Enable [x],[y],[z]", &is_xyz);
        
        


        // id/i | x | y | z | m |proj*view*model*x | proj*view*model*y | proj*view*model*z
        if (is_xyz) {
            static bool is_mat = false;
            ImGui::Checkbox("Enable p*v*m*[]", &is_mat);
            
            if (is_mat)
                ImGui::BeginTable("Vertices", 7);
            else
                ImGui::BeginTable("Vertices", 4);

            ImGui::TableSetupColumn("id/i");
            ImGui::TableSetupColumn("[x]");
            ImGui::TableSetupColumn("[y]");
            ImGui::TableSetupColumn("[z]");

            if (is_mat) {
                ImGui::TableSetupColumn("p*v*m*[x]");
                ImGui::TableSetupColumn("p*v*m*[y]");
                ImGui::TableSetupColumn("p*v*m*[z]");
            }


            ImGui::TableHeadersRow();

            for (int i = 0; i < handler->get_allocated_meshes_count(); i++) {
                BfMesh* pMesh = handler->get_pMesh(i);

                ImGui::TableNextRow();


                ImGui::TableSetColumnIndex(0);

                std::string id_index_str = std::to_string(pMesh->id) + 
                    "/" + std::to_string(i);

                ImGui::Text(id_index_str.c_str());

                for (const auto& vertex : pMesh->vertices) {

                    glm::vec4 vec = glm::vec4(vertex.pos.x, vertex.pos.y, vertex.pos.z, 1.0f);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text(std::to_string(vec.x).c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text(std::to_string(vec.y).c_str());

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text(std::to_string(vec.z).c_str());

                    //std::string model_str = std::to_string(pMesh->model_matrix[0][0]) + ", " + std::to_string(pMesh->model_matrix[0][1]) + ", " + std::to_string(pMesh->model_matrix[0][2]) + ", " + std::to_string(pMesh->model_matrix[0][3]) + "\n" +
                    //    std::to_string(pMesh->model_matrix[1][0]) + ", " + std::to_string(pMesh->model_matrix[1][1]) + ", " + std::to_string(pMesh->model_matrix[1][2]) + ", " + std::to_string(pMesh->model_matrix[1][3]) + "\n" +
                    //    std::to_string(pMesh->model_matrix[2][0]) + ", " + std::to_string(pMesh->model_matrix[2][1]) + ", " + std::to_string(pMesh->model_matrix[2][2]) + ", " + std::to_string(pMesh->model_matrix[2][3]) + "\n" +
                    //    std::to_string(pMesh->model_matrix[3][0]) + ", " + std::to_string(pMesh->model_matrix[3][1]) + ", " + std::to_string(pMesh->model_matrix[3][2]) + ", " + std::to_string(pMesh->model_matrix[3][3]);


                    //ImGui::TableSetColumnIndex(4);
                    //ImGui::Text(model_str.c_str());

                    if (is_mat) {
                        glm::vec4 cvec = __base.window->proj * pMesh->model_matrix * __base.window->view * vec;

                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text(std::to_string(cvec.x).c_str());

                        ImGui::TableSetColumnIndex(5);
                        ImGui::Text(std::to_string(cvec.y).c_str());

                        ImGui::TableSetColumnIndex(6);
                        ImGui::Text(std::to_string(cvec.z).c_str());
                    }

                    ImGui::TableNextRow();
                }
            }


            ImGui::EndTable();
        }
        
    ImGui::End();
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
