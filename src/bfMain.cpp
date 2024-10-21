﻿#include <imgui_impl_vulkan.h>

#include "bfBase.h"
#include "bfConfigManager.h"
#include "bfGuiFileDialog.h"
#define VMA_IMPLEMENTATION
#include <memory>

#include "BfMain.h"
#include "imgui.h"
#include "vk_mem_alloc.h"

void BfMain::__process_keys()
{
   if (glfwGetKey(__base.window->pWindow, GLFW_KEY_C) == GLFW_PRESS)
   {
      bfToggleCamParts(__base.window, 0, true);
      return;
   }
   else
   {
      bfToggleCamParts(__base.window, 0, false);
   }

   if ((glfwGetMouseButton(__base.window->pWindow, GLFW_MOUSE_BUTTON_3) ==
        GLFW_PRESS) &&
       (glfwGetKey(__base.window->pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
   {
      bfToggleCamParts(__base.window, 1, true);
      return;
   }
   else
   {
      bfToggleCamParts(__base.window, 1, false);
   }

   if (glfwGetMouseButton(__base.window->pWindow, GLFW_MOUSE_BUTTON_3) ==
       GLFW_PRESS)
   {
      bfToggleCamParts(__base.window, 2, true);
      return;
   }
   else
   {
      bfToggleCamParts(__base.window, 2, false);
   }
}

void BfMain::__poll_events()
{
   glfwPollEvents();
   auto &io = ImGui::GetIO();
   if (io.WantCaptureMouse || io.WantCaptureKeyboard)
   {
   }
   else
   {
      __base.pos_id = bfGetCurrentObjectId(__base);
   }

   __process_keys();

   // __gui.updateFonts();
   __gui.pollEvents();
}

void BfMain::__init()
{
   BfConfigManager::createInstance();

   bfBindHolder(&__holder);

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

   bfCreateTextureLoader(__base);
   bfCreateCommandPool(__base);
   bfInitOwnDescriptors(__base);

   bfCreateGraphicsPipelines(__base);
   bfCreateStandartFrameBuffers(__base);
   bfCreateGUIFrameBuffers(__base);

   bfCreateGUIDescriptorPool(__base);
   bfCreateStandartCommandBuffers(__base);
   bfCreateGUICommandBuffers(__base);
   bfCreateSyncObjects(__base);

   __gui.bindSettings("./scripts/guiconfig.lua");
   bfInitImGUI(__base);
   {
      __gui.bindDefaultFont();
      __gui.bindIconFont();
   }
   bfPostInitImGui(__base);

   bfCreateSampler(__base);

   __gui.bindBase(&__base);
   __gui.bindHolder(&__holder);
}

void BfMain::__kill()
{
   bfDestroyImGUI(__base);
   bfDestorySyncObjects(__base);
   bfDestroyGUICommandBuffers(__base);
   bfDestroyStandartCommandBuffers(__base);
   bfDestroyCommandPool(__base);
   bfDestroyGUIFrameBuffers(__base);
   bfDestroyStandartFrameBuffers(__base);
   bfDestroyDepthBuffer(__base);
   bfDestoryGraphicsPipelineLayouts(__base);
   bfDestroyGraphicsPipelines(__base);
   bfDestroyOwnDescriptors(__base);
   bfDestroyIDMapImage(__base);
   bfDestroyGUIRenderPass(__base);
   bfDestroyStandartRenderPass(__base);
   bfDestroyImageViews(__base);
   bfDestroySwapchain(__base);
   bfDestorySampler(__base);
   bfDestroyTextureLoader(__base);
   // id buffer
   bfDestroyBuffer(&__base.id_image_buffer);
   __base.layer_handler.kill();
   __base.layer_killer.kill();

   // std::cout << BfAllocationViewer::print_info();

   bfDestroySurface(__base);
   bfDestroyAllocator(__base);
   bfDestroyLogicalDevice(__base);
   bfDestroyDebufMessenger(__base);
   bfDestroyInstance(__base);

   // __base.layer_killer.kill();
   // std::cout << BfAllocationViewer::print_info();
}

void BfMain::__loop()
{
   __base.current_frame = 0;
   __base.is_resized    = true;

   // BfLayerHandler
   {
      auto otherLayer = std::make_shared<BfDrawLayer>(__base.allocator,
                                                      sizeof(BfVertex3),
                                                      100000,
                                                      10,
                                                      false);
      __other_layer   = otherLayer.get();
      __base.layer_handler.add(otherLayer);
   }
   //
   {
      auto bladeBases = std::make_shared<BfDrawLayer>(__base.allocator,
                                                      sizeof(BfVertex3),
                                                      1000,
                                                      1000,
                                                      false);
      __blade_bases   = bladeBases.get();
      __base.layer_handler.add(bladeBases);
   }

   bfSetOrthoLeft(__base.window);

   while (!glfwWindowShouldClose(__base.window->pWindow))
   {
      __poll_events();

      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      __present_blade_base_create_window();

      __gui.presentTopDock();
      __gui.presentLeftDock();

      __gui.presentMenuBar();
      __gui.presentCamera();
      __gui.presentLayerHandler();
      __gui.presentEventLog();
      __gui.presentToolType();
      __gui.presentSettings();
      // __gui.presentLuaInteraction();
      __gui.presentFileDialog();
      __gui.presentCreateWindow();

      // ImGui::ShowDemoWindow();

      ImGui::Render();
      bfUpdateImGuiPlatformWindows();
      bfDrawFrame(__base);
   }

   auto layer_killer = BfLayerKiller::get_root();
   for (size_t i = 0; i < __base.layer_handler.get_layer_count(); ++i)
   {
      auto buffer = __base.layer_handler.get_layer_by_index(i);
      layer_killer->add(__base.layer_handler.get_layer_by_index(i));
   }
   layer_killer->kill();

   vkDeviceWaitIdle(__base.device);
}

void BfMain::__present_id_map(BfBase &base, std::vector<uint32_t> data)
{
   ImGui::Begin("IdMap");
   {
      if (ImGui::Button("Show Id map"))
      {
         std::ofstream outFile("id_map.txt");

         if (outFile.is_open())
         {
            for (int i = 0; i < base.swap_chain_extent.height; i++)
            {
               for (int j = 0; j < base.swap_chain_extent.width; j++)
               {
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
         std::mt19937       mt(rd());

         std::map<uint32_t, glm::vec4> id_color_map;
         for (int i = 0;
              i < base.swap_chain_extent.width * base.swap_chain_extent.height;
              ++i)
         {
            if (!id_color_map.contains((int)data[i]))
            {
               float red                  = (float)(mt() % 255);
               float green                = (float)(mt() % 255);
               float blue                 = (float)(mt() % 255);

               id_color_map[(int)data[i]] = glm::vec4(red, green, blue, 1.0f);

               std::cout << id_color_map[(int)data[i]].r << ", "
                         << id_color_map[(int)data[i]].g << ", "
                         << id_color_map[data[i]].b << "\n";
            }
         }

         std::vector<uint8_t> rgba_data(base.swap_chain_extent.width *
                                        base.swap_chain_extent.height * 3);
         size_t               index = 0;
         for (int i = 0; i < base.swap_chain_extent.width *
                                 base.swap_chain_extent.height * 3;
              i += 3)
         {
            rgba_data[i + 0] = int(id_color_map[(int)data[index]].r);
            rgba_data[i + 1] = int(id_color_map[(int)data[index]].g);
            rgba_data[i + 2] = int(id_color_map[(int)data[index]].b);
            // rgba_data[i + 3] = id_color_map[data[i]].a;

            index++;
         }

         if (!stbi_write_png("output.png",
                             base.swap_chain_extent.width,
                             base.swap_chain_extent.height,
                             3,
                             rgba_data.data(),
                             base.swap_chain_extent.width * 3))
         {
            // Обработайте ошибку сохранения изображения
            // std::cerr << "Ошибка сохранения изображения." << std::endl;
         }
      }

      // ImGui::TextWrapped(s.c_str());
   }
   ImGui::End();
}

// void BfMain::__present_blade_section_create_window() {
//
//     static int inputFloatMode = 0;
//     auto make_row = [](std::string n,
//                        std::string d,
//                        std::string dim,
//                        float* value,
//                        float left,
//                        float right)
//     {
//         static int count = 0;
//
//         ImGui::TableNextRow();
//         ImGui::TableSetColumnIndex(0);
//         ImGui::Text(n.c_str());
//         ImGui::TableSetColumnIndex(1);
//         ImGui::Text(d.c_str());
//         ImGui::TableSetColumnIndex(2);
//         std::string dsadasd = dim + "##float" + (char)(count);
//
//         switch (inputFloatMode) {
//         case 0:
//             ImGui::InputFloat(dim.c_str(), value);
//             break;
//         case 1:
//             ImGui::SliderFloat(dim.c_str(), value, left, right);
//             break;
//         }
//
//
//         count++;
//     };
//
//     static BfBladeSectionCreateInfo old{};
//
//     BfDrawLayerCreateInfo linfo{
//         /*.allocator = __base.allocator,
//         .vertex_size = sizeof(BfVertex3),
//         .max_vertex_count = 10000,
//         .max_reserved_count = 1000,
//         .is_nested = false*/
//     };
//     linfo.is_nested = true;
//
//     static BfBladeSectionCreateInfo info{
//         .layer_create_info = linfo,
//
//         .width = 1.0f,
//         .install_angle = 102.0f,
//
//         .inlet_angle = 25.0f,
//         .outlet_angle = 42.0f,
//
//         .inlet_surface_angle = 15.0f,
//         .outlet_surface_angle = 15.0f,
//
//         .inlet_radius = 0.025f,
//         .outlet_radius = 0.005f,
//
//         .border_length = 20.0f,
//
//         .is_triangulate = false,
//         .is_center = true,
//
//         .l_pipeline = __base.line_pipeline,
//         .t_pipeline = __base.triangle_pipeline,
//
//     };
//
//     ImGuiWindowFlags window_flags =
//         ImGuiWindowFlags_NoCollapse |
//         ImGuiWindowFlags_NoResize |
//         //ImGuiWindowFlags_NoMove |
//         ImGuiWindowFlags_NoBringToFrontOnFocus |
//         ImGuiWindowFlags_NoNavFocus |
//         ImGuiWindowFlags_AlwaysAutoResize;
//
//
//     static bool is_center_changed;
//     static bool is_triangulate;
//
//     ImGui::Begin("Create blade section", nullptr, window_flags);
//     {
//
//         ImGui::BeginGroup();
//         {
//             ImGui::Text("Input parameters mode");
//
//             ImGui::RadioButton("Input float", &inputFloatMode, 0);
//             ImGui::SameLine();
//             ImGui::RadioButton("Slider", &inputFloatMode, 1);
//
//         }
//         ImGui::EndGroup();
//
//         int flags = ImGuiTableFlags_NoHostExtendX |
//                     ImGuiTableFlags_SizingFixedFit;
//
//         ImGui::BeginTable("FloatTable", 3, flags);
//         {
//             ImGui::TableSetupColumn("Parameter name");
//             ImGui::TableSetupColumn("Description");
//             ImGui::TableSetupColumn("Value");
//             ImGui::TableHeadersRow();
//
//             make_row("Width",               "B",        "[m]##0",
//             &info.width, 0.0f, 10.0f); make_row("Install angle", "alpha_y",
//             "[deg]##1", &info.install_angle, -180.0f, 180.0f);
//             make_row("Inlet angle",         "beta_1",   "[deg]##2",
//             &info.inlet_angle, -180.0f, 180.0f); make_row("Outlet angle",
//             "beta_2",   "[deg]##3", &info.outlet_angle, -180.0f, 180.0f);
//             make_row("Inlet surface angle", "omega_1",  "[deg]##4",
//             &info.inlet_surface_angle, -45.0f, 45.0f); make_row("Outlet
//             surface angle","omega_2", "[deg]##5", &info.outlet_surface_angle,
//             -45.0f, 45.0f); make_row("Inlet radius",        "r_1", "[m]##6",
//             &info.inlet_radius, 0.00001, 0.05); make_row("Outlet radius",
//             "r_2",      "[m]##7",   &info.outlet_radius, 0.00001, 0.05);
//         }
//         ImGui::EndTable();
//
//         ImGui::BeginGroup();
//         {
//             ImGui::Text("Start point");
//             ImGui::SliderFloat("X", &info.start_vertex.x, -10.0f, 10.0f);
//             ImGui::SliderFloat("Y", &info.start_vertex.y, -10.0f, 10.0f);
//             ImGui::SliderFloat("Z", &info.start_vertex.z, -10.0f, 10.0f);
//         }
//         ImGui::EndGroup();
//
//
//         ImGui::BeginGroup();
//         {
//             ImGui::Text("Options");
//
//             if (ImGui::Checkbox("Calculate center", &info.is_center))
//                 is_center_changed = true;
//             else
//                 is_center_changed = false;
//
//             if (ImGui::Checkbox("Triangulate", &info.is_triangulate))
//                 is_triangulate = true;
//             else
//                 is_triangulate = false;
//
//         }
//         ImGui::EndGroup();
//     }
//
//     static int sec_id = -1;
//
//     ImGui::End();
//     if (!bfCheckBladeSectionCreateInfoEquality(info, old) ||
//     is_center_changed || is_triangulate) {
//
//         if (sec_id > 0) __base.section_layer->del(sec_id);
//
//         auto blade_section_1 = std::make_shared<BfBladeSection>(
//             info
//         );
//         sec_id = blade_section_1->id.get();
//
//         __base.section_layer->add(blade_section_1);
//         __base.section_layer->update_buffer();
//
//     }
//     old = info;
// }

void BfMain::__present_blade_base_create_window()
{
   static int sec_count                                   = 0;
   static int new_sec_count                               = 0;

   static std::shared_ptr<BfBladeBase> base_section_layer = nullptr;
   static int                          base_id            = -1;

   static int shape_id                                    = -1;

   static bool is_first_frame                             = true;
   if (is_first_frame)
   {
      BfBladeBaseCreateInfo blade_base_info{};
      blade_base_info.layer_create_info.allocator          = __base.allocator;
      blade_base_info.layer_create_info.is_nested          = true;
      blade_base_info.layer_create_info.max_vertex_count   = 1000;
      blade_base_info.layer_create_info.max_reserved_count = 1000;
      blade_base_info.layer_create_info.vertex_size        = sizeof(BfVertex3);
      // blade_base_info.layer_create_info.id_type = 0xA1;

      base_section_layer = std::make_shared<BfBladeBase>(blade_base_info);

      __blade_bases->add(base_section_layer);

      base_id        = base_section_layer->id.get();
      is_first_frame = false;
   }

   static std::vector<BfBladeSectionCreateInfo> old_infos;
   static std::vector<BfBladeSectionCreateInfo> infos;

   bool is_new_tab_item = false;
   ImGui::Begin("Create blade base");
   {
      if (ImGui::InputInt("Section count", &new_sec_count))
      {
         // -
         if (new_sec_count - sec_count < 0)
         {
            infos.pop_back();
         }
         // +
         else if (new_sec_count - sec_count > 0)
         {
            BfBladeSectionCreateInfo ns{};
            ns.layer_create_info.is_nested = true;
            // ns.layer_create_info.id_type = 0xA1;
            ns.width = 1.0f, ns.install_angle = 102.0f, ns.inlet_angle = 25.0f,
            ns.outlet_angle        = 42.0f,

            ns.inlet_surface_angle = 15.0f, ns.outlet_surface_angle = 15.0f,

            ns.inlet_radius = 0.025f, ns.outlet_radius = 0.005f,

            ns.border_length  = 20.0f,

            ns.is_triangulate = false, ns.is_center = true,

            ns.l_pipeline = __base.line_pipeline,
            ns.t_pipeline = __base.triangle_pipeline,

            infos.push_back(std::move(ns));

            is_new_tab_item = true;
         }

         sec_count = new_sec_count;
      }

      if (ImGui::BeginTabBar("MyTabBar"))
      {
         for (size_t i = 0; i < sec_count; i++)
         {
            std::string tab_name = "Section " + std::to_string(i + 1);
            if ((i == sec_count - 1) and is_new_tab_item)
            {
               if (ImGui::BeginTabItem(tab_name.c_str(),
                                       nullptr,
                                       ImGuiTabItemFlags_SetSelected))
               {
                  bfPresentBladeSectionInside(base_section_layer.get(),
                                              &infos[i],
                                              &old_infos[i]);
                  ImGui::EndTabItem();
               }
            }
            else
            {
               if (ImGui::BeginTabItem(tab_name.c_str()))
               {
                  bfPresentBladeSectionInside(base_section_layer.get(),
                                              &infos[i],
                                              &old_infos[i]);
                  ImGui::EndTabItem();
               }
            }
         }

         ImGui::EndTabBar();
      }

      if (ImGui::Button("Create shape"))
      {
         if (shape_id != -1) __other_layer->del(shape_id);

         auto base = std::static_pointer_cast<BfBladeBase>(
             __blade_bases->get_layer_by_index(0));
         auto shape = base->create_shape();
         shape->bind_pipeline(&__base.triangle_pipeline);
         __other_layer->add(shape);
         shape_id = shape->id.get();

         __other_layer->update_buffer();
      }
   }
   ImGui::End();

   bool is_changed = false;
   for (size_t i = 0; i < infos.size(); i++)
   {
      if (infos.size() != old_infos.size())
      {
         is_changed = true;
         break;
      }

      if (!bfCheckBladeSectionCreateInfoEquality(infos[i], old_infos[i]))
      {
         is_changed = true;
         break;
      }
   }

   if (is_changed)
   {
      __blade_bases->del(base_id);

      BfBladeBaseCreateInfo blade_base_info{};
      blade_base_info.layer_create_info.is_nested = true;
      blade_base_info.section_infos               = infos;
      base_section_layer = std::make_shared<BfBladeBase>(blade_base_info);

      base_id            = base_section_layer->id.get();
      __blade_bases->add(base_section_layer);

      __blade_bases->update_buffer();

      old_infos = infos;
   }
}

BfMain::BfMain()
    : __base{}, __holder{}
{
}

void BfMain::run()
{
   __init();
   __loop();
   __kill();
}
