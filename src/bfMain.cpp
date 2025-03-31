#include <imgui_impl_vulkan.h>

#include <cassert>
#include <memory>

#include "bfAllocator.h"
#include "bfAxis.h"
#include "bfBase.h"
#include "bfCamera.h"
#include "bfConfigManager.h"
#include "bfCurves3.h"
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
   BfPipelineHandler::instance()->create<BfPipelineBuilderLine>(
       BfPipelineType_Lines,
       BfPipelineLayoutType_Main,
       fs::path(BfConfigManager::getInstance()->exePath()) / "shaders/lines"
   );
   BfPipelineHandler::instance()->create<BfPipelineBuilderTriangle>(
       BfPipelineType_Triangles,
       BfPipelineLayoutType_Main,
       fs::path(BfConfigManager::getInstance()->exePath()) / "shaders/triangle"
   );
   BfPipelineHandler::instance()->create<BfPipelineBuilderTriangle>(
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

   auto test_root = std::make_shared<obj::BfDrawRootLayer>(2000, 10);
   auto test_layer2 = std::make_shared<obj::TestLayer>();
   //
   // for (int i = 0; i < 5; ++i)
   // {
   //    auto t = std::make_shared<obj::RandomPlane>();
   //    test_root->add(t);
   // }
   //
   test_root->add(test_layer2);
   test_root->make();
   test_root->control().updateBuffer();

   obj::BfDrawManager::inst().add(test_root);

   // BfLayerHandler
   // {
   //    auto otherLayer = std::make_shared<BfDrawLayer>(
   //        BfAllocator::get(),
   //        sizeof(BfVertex3),
   //        100000,
   //        10,
   //        false
   //    );
   //    __other_layer = otherLayer.get();
   //    __base.layer_handler.add(otherLayer);
   //    // __other_layer->add(
   //    // std::make_shared<BfAxis3DPack>(&__base.triangle_pipeline));
   //    __other_layer->add(std::make_shared<BfAxis3DPack>(
   //        BfPipelineHandler::instance()->getPipeline(BfPipelineType_Axis)
   //    ));
   //
   //    __other_layer->update_buffer();
   // }
   // {
   //    auto bladeBases = std::make_shared<BfDrawLayer>(
   //        BfAllocator::get(),
   //        sizeof(BfVertex3),
   //        1000,
   //        1000,
   //        false
   //    );
   //    __blade_bases = bladeBases.get();
   //    __base.layer_handler.add(bladeBases);
   // }

   bfSetOrthoLeft(__base.window);
   while (!glfwWindowShouldClose(__base.window->pWindow))
   {
      __poll_events();

      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // __present_blade_base_create_window();

      __gui.presentInfo();
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
      __gui.presentSmartLayerObserver();
      __gui.presentCameraWindow();

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
    __cam{BfCameraMode_OrthoCentered}
{
}

void
BfMain::run()
{
   __init();
   __loop();
   __kill();
}
