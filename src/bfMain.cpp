#include <imgui_impl_vulkan.h>

#include <cassert>
#include <memory>

#include "bfAllocator.h"
#include "bfAxis.h"
#include "bfBase.h"
#include "bfBladeSection2.h"
#include "bfCamera.h"
#include "bfConfigManager.h"
#include "bfCurves3.h"
#include "bfCurves4.h"
#include "bfDrawObject2.h"
#include "bfDrawObjectManager.h"
#include "bfPipeline.h"
#include "bfWindow.h"
#include "imgui.h"

#define VMA_IMPLEMENTATION

#include "BfMain.h"
#include "BfPipelineLine.hpp"
#include "BfPipelineTriangle.hpp"
#include "bfPipeline.h"
#include "vk_mem_alloc.h"

void
BfMain::__process_keys()
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

   if (  //! ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) &&
       (glfwGetMouseButton(__base.window->pWindow, GLFW_MOUSE_BUTTON_3) ==
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

   if (  //! ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) &&
       glfwGetMouseButton(__base.window->pWindow, GLFW_MOUSE_BUTTON_3) ==
       GLFW_PRESS)
   {
      bfToggleCamParts(__base.window, 2, true);
      return;
   }
   else
   {
      bfToggleCamParts(__base.window, 2, false);
   }

   if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
        ImGui::IsKeyDown(ImGuiKey_RightCtrl)) &&
       ImGui::IsKeyPressed(ImGuiKey_E, false))
   {
      __gui.toggleRenderCreateWindow();
   }
}

void
BfMain::__poll_events()
{
   glfwPollEvents();
   auto& io = ImGui::GetIO();

   if (io.WantCaptureMouse || io.WantCaptureKeyboard)
   {
   }
   else
   {
      auto hovered = bfGetCurrentObjectId(__base);
      __base.pos_id = hovered;

      obj::BfDrawManager::inst().setHovered(hovered);
   }

   __process_keys();

   // __gui.updateFonts();
   __gui.pollEvents();
   __cam.update();
}

void
BfMain::__init()
{
   BfConfigManager::createInstance();

   bfBindBase(&__base);
   bfBindHolder(&__holder);

   bfHoldWindow(__base.window);

   bfHoldPhysicalDevice(__base.physical_device);

   bfSetWindowSize(__base.window, BF_START_W, BF_START_H);
   bfSetWindowName(__base.window, BF_APP_NAME);
   bfCreateWindow(__base.window);

   bfBindGuiWindowHoveringFunction(__base.window, []() {
      return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
   });

   bfCreateInstance(__base);
   bfCreateDebugMessenger(__base);
   bfCreateSurface(__base);
   bfCreatePhysicalDevice(__base);
   bfCreateLogicalDevice(__base);
   bfCreateAllocator(__base);

   bfCreateSwapchain(__base);

   // Setup camera
   BfCamera::bindInstance(&__cam);
   BfCamera::instance()->bindWindow(__base.window->pWindow);
   // NOTE: WAS ADDITIONALY SET IN RECREATE SWAP-CHAIN FUNCTION
   BfCamera::instance()->setExtent(
       __base.swap_chain_extent.width,
       __base.swap_chain_extent.height
   );

   bfCreateImageViews(__base);
   bfCreateDepthBuffer(__base);
   bfCreateStandartRenderPass(__base);
   bfCreateGUIRenderPass(__base);

   bfCreateIDMapImage(__base);

   // bfCreateTextureLoader(__base);
   bfCreateCommandPool(__base);
   bfInitOwnDescriptors(__base);

   BfPipelineHandler::instance()->createLayout(
       BfPipelineLayoutType_Main,
       __base.descriptor.getAllLayouts()
   );
   BfPipelineHandler::instance()->create< BfPipelineBuilderLine >(
       BfPipelineType_Lines,
       BfPipelineLayoutType_Main,
       fs::path(BfConfigManager::getInstance()->exePath()) / "shaders/lines"
   );
   BfPipelineHandler::instance()->create< BfPipelineBuilderTriangle >(
       BfPipelineType_Triangles,
       BfPipelineLayoutType_Main,
       fs::path(BfConfigManager::getInstance()->exePath()) / "shaders/triangle"
   );
   BfPipelineHandler::instance()->create< BfPipelineBuilderTriangle >(
       BfPipelineType_Axis,
       BfPipelineLayoutType_Main,
       fs::path(BfConfigManager::getInstance()->exePath()) / "shaders/axis"
   );

   bfCreateStandartFrameBuffers(__base);
   bfCreateGUIFrameBuffers(__base);

   bfCreateGUIDescriptorPool(__base);
   bfCreateStandartCommandBuffers(__base);
   bfCreateGUICommandBuffers(__base);
   bfCreateSyncObjects(__base);

   bfBindAllocatorToLayerHandler(__base);

   __gui.bindSettings(
       BfConfigManager::getInstance()->exePath() / "scripts/guiconfig.lua"
   );
   bfInitImGUI(__base);
   {
      __gui.bindDefaultFont();
      __gui.bindIconFont();
      __gui.bindGreekFont();
   }
   bfPostInitImGui(__base);

   bfCreateSampler(__base);

   __gui.bindBase(&__base);
   __gui.bindHolder(&__holder);
}

void
BfMain::__kill()
{
   obj::BfDrawManager::inst().kill();

   bfDestroyImGUI(__base);
   bfDestorySyncObjects(__base);
   bfDestroyGUICommandBuffers(__base);
   bfDestroyStandartCommandBuffers(__base);
   bfDestroyCommandPool(__base);
   bfDestroyGUIFrameBuffers(__base);
   bfDestroyStandartFrameBuffers(__base);
   bfDestroyDepthBuffer(__base);
   BfPipelineHandler::instance()->kill();
   bfDestroyOwnDescriptors(__base);
   bfDestroyIDMapImage(__base);
   bfDestroyGUIRenderPass(__base);
   bfDestroyStandartRenderPass(__base);
   bfDestroyImageViews(__base);
   bfDestroySwapchain(__base);
   bfDestorySampler(__base);
   // BUG: Not destorying vulkan objects inside
   // bfDestroyTextureLoader(__base);
   // id buffer
   bfDestroyBuffer(&__base.id_image_buffer);
   __base.layer_handler.kill();
   __base.layer_killer.kill();
   bfDestroySurface(__base);
   bfDestroyAllocator(__base);
   bfDestroyLogicalDevice(__base);
   bfDestroyDebufMessenger(__base);
   bfDestroyInstance(__base);
}

void
BfMain::__loop()
{
   __base.current_frame = 0;
   __base.is_resized = true;

   // auto xAxis = std::make_shared<BfSingleLine>(
   //     glm::vec3{0.0f, 0.0f, 0.0},
   //     glm::vec3{1.0f, 0.0f, 0.0f}
   // );
   // xAxis->set_color({1.0f, 0.0f, 0.0f});
   // xAxis->createVertices();
   // xAxis->createIndices();
   // // xAxis->bind_pipeline(&__base.line_pipeline);
   // xAxis->bind_pipeline(
   //     BfPipelineHandler::instance()->getPipeline(BfPipelineType_Triangles)
   // );
   // auto yAxis = std::make_shared<BfSingleLine>(
   //     glm::vec3{0.0f, 0.0f, 0.0},
   //     glm::vec3{0.0f, 1.0f, 0.0f}
   // );
   // yAxis->set_color({1.0f, 0.0f, 0.0f});
   // yAxis->createVertices();
   // yAxis->createIndices();
   // auto zAxis = std::make_shared<BfSingleLine>(
   //     glm::vec3{0.0f, 0.0f, 0.0},
   //     glm::vec3{0.0f, 0.0f, 1.0f}
   // );
   // zAxis->set_color({1.0f, 0.0f, 0.0f});
   // zAxis->createVertices();
   // zAxis->createIndices();

   auto test_root = std::make_shared< obj::BfDrawRootLayer >(2000, 10);

   // auto filled_circle = std::make_shared<obj::curves::BfCircleCenterFilled>(
   //     BfVertex3({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}),
   //     0.1f
   // );
   // filled_circle->make();
   // auto circle = std::make_shared<obj::curves::BfHandle>(
   //     BfVertex3({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}),
   //     0.1f
   // );
   // circle->make();

   // auto bez = std::make_shared< obj::curves::BfBezierWH >(
   //     BfVertex3Uni{BfVertex3(
   //         glm::vec3(0.0f),
   //         glm::vec3(1.0f),
   //         glm::vec3{0.0f, 0.0f, 1.0f}
   //     )},
   //     BfVertex3Uni{BfVertex3(
   //         glm::vec3(.5f, .5f, 0.0f),
   //         glm::vec3(1.0f),
   //         glm::vec3{0.0f, 0.0f, 1.0f}
   //     )},
   //     BfVertex3Uni{BfVertex3(
   //         glm::vec3(1.f, 0.0f, 0.0f),
   //         glm::vec3(1.0f),
   //         glm::vec3{0.0f, 0.0f, 1.0f}
   //     )}
   // );
   // bez->make();

   // test_root->add(circle);
   // test_root->add(bez);

   // auto cirhan = std::make_shared<obj::curves::BfCircleCenterWH>(
   //     BfVertex3{
   //         glm::vec3(-0.5f),
   //         glm::vec3(1.0f),
   //         glm::vec3{0.0f, 0.0f, 1.0f}
   //     },
   //     0.5f
   // );
   // cirhan->make();
   // test_root->add(cirhan);

   // auto l = std::make_shared<obj::curves::BfSingleLineWH>(&v1, &v2);
   // l->make();
   // test_root->add(l);
   auto info =  obj::section::SectionCreateInfo{};

   auto bs = std::make_shared< obj::section::BfBladeSection >(
      &info
   );
   bs->make();
   test_root->add(bs);

   test_root->control().updateBuffer();
   obj::BfDrawManager::inst().add(test_root);

   // bfSetOrthoLeft(__base.window);

   // int bez_order = bez->size();

   auto _tmpcol = [&bs, &test_root](const char* name, float* v) { 
         ImGui::TableNextRow();
         ImGui::TableSetColumnIndex(0);
         ImGui::Text("%s", name);

         ImGui::TableSetColumnIndex(1);
         if (ImGui::InputFloat((std::string("##___") + std::string(name)).c_str(), v)) { 
            bs->make();
            test_root->control().updateBuffer();
         }
   };
   #define tmpcol(NAME) _tmpcol(#NAME, &info.NAME)

   while (!glfwWindowShouldClose(__base.window->pWindow))
   {
      __poll_events();

      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // __present_blade_base_create_window();

      __gui.presentInfo();
      // __gui.presentTopDock();
      // __gui.presentLeftDock();

      __gui.presentMenuBar();
      __gui.presentCamera();
      __gui.presentLayerHandler();
      __gui.presentEventLog();
      __gui.presentToolType();
      __gui.presentSettings();
      // __gui.presentLuaInteraction();
      __gui.presentFileDialog();
      __gui.presentCreateWindow();
      __gui.presentSmartLayerObserver();
      __gui.presentCameraWindow();
      __gui.presentIds();
   
      ImGui::Begin("BladeSection CI");
      {
         if (ImGui::BeginTable("dasd12312312dsadas 12- ", 2) ) {
            ImGui::TableSetupColumn("VarName");
            ImGui::TableSetupColumn("VarValue");
            ImGui::TableHeadersRow();
            
            tmpcol(chord);
            tmpcol(inletAngle);
            tmpcol(outletAngle);
            tmpcol(inletRadius);
            tmpcol(outletRadius);
            int i = 0;
            for (auto& c : info.centerCircles) { 
               _tmpcol((std::string("Circle_") + std::to_string(i)).c_str(), &c.relativePos);
               i++;
            }

            ImGui::EndTable();
         }
      }
      ImGui::End();

      // ImGui::ShowDemoWindow();

      // ImGui::Begin("Bezier");
      //
      // if (ImGui::InputInt("Order of bezier", &bez_order))
      // {
      //    if (bez_order > bez->size()) { 
      //       bez->elevateOrder();
      //       test_root->make();
      //       test_root->control().updateBuffer();
      //    }
      //    else { 
      //       bez->lowerateOrder();
      //       test_root->make();
      //       test_root->control().updateBuffer();
      //    }
      //    
      // }
      // if (ImGui::Button("toggle")) { 
      //    bez->toggleBoundHandles();
      // }

      // ImGui::End();

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

/*
   Тут есть списочное объявление в конструкторе, чтобы
   разные instance сгенеровались 1 раз, чтобы каждый раз
   для каждого такого класса не прописывать bind
*/
BfMain::BfMain()
    : __base{}
    , __holder{}
    ,
    // __cam{{0, 0, -3}, {0, 0, 1}, {0, 1, 0}, nullptr}
    __cam{BfCameraMode_Ortho}
{
}

void
BfMain::run()
{
   __init();
   __loop();
   __kill();
}
