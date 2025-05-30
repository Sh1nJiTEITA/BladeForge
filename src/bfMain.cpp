﻿#include <imgui_impl_vulkan.h>

#include <cassert>
#include <imgui_internal.h>
#include <memory>
#include <nfd.h>

#include "bfAllocator.h"
#include "bfAxis.h"
#include "bfBase.h"
#include "bfBladeBody.h"
#include "bfBladeSection2.h"
#include "bfCamera.h"
#include "bfConfigManager.h"
#include "bfCurves3.h"
#include "bfCurves4.h"
#include "bfDescriptorStructs.h"
#include "bfDrawObject2.h"
#include "bfDrawObjectManager.h"
#include "bfGuiBody.h"
#include "bfHandle.h"
#include "bfPipeline.h"
#include "bfSingle.h"
#include "bfViewport.h"
#include "bfWindow.h"
#include "imgui.h"

#define VMA_IMPLEMENTATION

#include "BfMain.h"
#include "BfPipelineLine.hpp"
#include "BfPipelineTriangle.hpp"
#include "bfPipeline.h"
#include "vk_mem_alloc.h"

void
BfMain::_processKeys()
{
   // clang-format off
   bool is_ctrl_down = 0
      || ImGui::IsKeyDown(ImGuiKey_LeftCtrl) 
      || ImGui::IsKeyDown(ImGuiKey_RightCtrl);

   if (is_ctrl_down && ImGui::IsKeyPressed(ImGuiKey_E, false))
   {
      m_gui.toggleRenderCreateWindow();
   }

   if (is_ctrl_down && ImGui::IsKeyPressed(ImGuiKey_V, false)) { 
      auto node = base::viewport::ViewportManager::currentHoveredNode();
      if (node.has_value()) { 
         node.value().get().split(base::viewport::SplitDirection::V);
      }
   }

   if (is_ctrl_down && ImGui::IsKeyPressed(ImGuiKey_H, false)) { 
      auto node = base::viewport::ViewportManager::currentHoveredNode();
      if (node.has_value()) { 
         node.value().get().split(base::viewport::SplitDirection::H);
      }
   }

   if (is_ctrl_down && ImGui::IsKeyPressed(ImGuiKey_W, false)) { 
      auto node = base::viewport::ViewportManager::currentHoveredNode();
      if (node.has_value()) { 
         node.value().get().close();
      }
   }

   // clang-format on
}

void
BfMain::_pollEvents()
{
   glfwPollEvents();
   auto& io = ImGui::GetIO();
   if (io.WantCaptureMouse || io.WantCaptureKeyboard)
   {
      obj::BfDrawManager::inst().setHovered(0);
      base::viewport::ViewportManager::setHoveredID(0);
   }
   else
   {
      auto hovered = bfGetCurrentObjectId(m_base);
      obj::BfDrawManager::inst().setHovered(hovered);
      base::viewport::ViewportManager::setHoveredID(hovered);
      base::viewport::ViewportManager::update();
   }

   _processKeys();

   // __gui.updateFonts();
   m_gui.pollEvents();
   m_cam.update();

   // TODO: REMAKE !
   obj::BfGuiIntegration::runtimeID = 0;
}

void
BfMain::_processSelfInteraction()
{
   while (!base::g::intrstack().empty())
   {
      base::g::intrstack().top()();
      base::g::intrstack().pop();
   }
}

void
BfMain::_init()
{
   NFD_Init();

   BfConfigManager::createInstance();

   bfBindBase(&m_base);
   bfBindHolder(&m_holder);

   bfHoldWindow(m_base.window);

   bfHoldPhysicalDevice(m_base.physical_device);

   bfSetWindowSize(m_base.window, BF_START_W, BF_START_H);
   bfSetWindowName(m_base.window, BF_APP_NAME);
   bfCreateWindow(m_base.window);

   bfBindGuiWindowHoveringFunction(m_base.window, []() {
      return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
   });

   bfCreateInstance(m_base);
   bfCreateDebugMessenger(m_base);
   bfCreateSurface(m_base);
   bfCreatePhysicalDevice(m_base);
   bfCreateLogicalDevice(m_base);
   bfCreateAllocator(m_base);

   base::g::create(
       MAX_FRAMES_IN_FLIGHT,
       &m_base.current_frame,
       m_base.window->pWindow,
       &m_base.instance,
       m_base.physical_device,
       &m_base.device,
       &m_base.command_pool
       // &__base.sampler->handle()
   );
   base::g::bindStack(&m_base.self_interaction);

   bfCreateSampler(m_base);

   bfCreateSwapchain(m_base);

   // Setup camera
   BfCamera::bindInstance(&m_cam);
   BfCamera::instance()->bindWindow(m_base.window->pWindow);
   // NOTE: WAS ADDITIONALY SET IN RECREATE SWAP-CHAIN FUNCTION
   BfCamera::instance()->setExtent(
       m_base.swap_chain_extent.width,
       m_base.swap_chain_extent.height
   );

   bfCreateImageViews(m_base);
   bfCreateDepthBuffer(m_base);
   bfCreateStandartRenderPass(m_base);
   bfCreateGUIRenderPass(m_base);

   bfCreateIDMapImage(m_base);

   // bfCreateTextureLoader(__base);
   bfCreateCommandPool(m_base);

   bfInitOwnDescriptors(m_base);

   base::desc::own::BfDescriptorPipelineDefault story;
   story.create();

   BfPipelineHandler::instance()->createLayout(
       BfPipelineLayoutType_Main,
       story.manager().getAllLayouts()
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
   BfPipelineHandler::instance()->create< BfPipelineBuilderTriangleFrame >(
       BfPipelineType_TrianglesFramed,
       BfPipelineLayoutType_Main,
       fs::path(BfConfigManager::getInstance()->exePath()) / "shaders/triangle"
   );

   BfPipelineHandler::instance()->create< BfPipelineBuilderTriangle >(
       BfPipelineType_Axis,
       BfPipelineLayoutType_Main,
       fs::path(BfConfigManager::getInstance()->exePath()) / "shaders/axis"
   );
   BfPipelineHandler::instance()->create< BfPipelineBuilderTriangle >(
       BfPipelineType_LoadedImage,
       BfPipelineLayoutType_Main,
       fs::path(BfConfigManager::getInstance()->exePath()) /
           "shaders/loaded_image"
   );
   BfPipelineHandler::instance()->create< BfPipelineBuilderTriangle >(
       BfPipelineType_Handles,
       BfPipelineLayoutType_Main,
       fs::path(BfConfigManager::getInstance()->exePath()) / "shaders/handles"
   );

   bfCreateStandartFrameBuffers(m_base);
   bfCreateGUIFrameBuffers(m_base);

   bfCreateGUIDescriptorPool(m_base);
   bfCreateStandartCommandBuffers(m_base);
   bfCreateGUICommandBuffers(m_base);
   bfCreateSyncObjects(m_base);

   bfBindAllocatorToLayerHandler(m_base);

   m_gui.bindSettings(
       BfConfigManager::getInstance()->exePath() / "scripts/guiconfig.lua"
   );
   bfInitImGUI(m_base);
   {
      m_gui.bindDefaultFont();
      m_gui.bindIconFont();
      m_gui.bindGreekFont();
   }
   bfPostInitImGui(m_base);

   m_gui.bindBase(&m_base);
   m_gui.bindHolder(&m_holder);
}

void
BfMain::_kill()
{
   obj::BfDrawManager::inst().kill();

   m_dock.kill();

   bfDestroyImGUI(m_base);
   bfDestorySyncObjects(m_base);
   bfDestroyGUICommandBuffers(m_base);
   bfDestroyStandartCommandBuffers(m_base);
   bfDestroyCommandPool(m_base);
   bfDestroyGUIFrameBuffers(m_base);
   bfDestroyStandartFrameBuffers(m_base);
   bfDestroyDepthBuffer(m_base);
   BfPipelineHandler::instance()->kill();
   base::desc::own::BfDescriptorPipelineDefault::manager().cleanup();
   bfDestroyOwnDescriptors(m_base);
   bfDestroyIDMapImage(m_base);
   bfDestroyGUIRenderPass(m_base);
   bfDestroyStandartRenderPass(m_base);
   bfDestroyImageViews(m_base);
   bfDestroySwapchain(m_base);
   m_base.sampler.reset();
   // bfDestorySampler(__base);
   // BUG: Not destorying vulkan objects inside
   // bfDestroyTextureLoader(__base);
   // id buffer
   bfDestroyBuffer(&m_base.id_image_buffer);
   m_base.layer_handler.kill();
   m_base.layer_killer.kill();
   bfDestroySurface(m_base);
   bfDestroyAllocator(m_base);
   bfDestroyLogicalDevice(m_base);
   bfDestroyDebufMessenger(m_base);
   bfDestroyInstance(m_base);

   NFD_Quit();
}

/*

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


*/

void
createRootLayers()
{
   const auto mtype = obj::BfDrawManager::RootType::MAIN;
   obj::BfDrawManager::inst().addRoot< mtype >();

   // const auto ttype = obj::BfDrawManager::RootType::IMAGE_LOAD;
   // obj::BfDrawManager::inst().addRoot< ttype >();
};

void
BfMain::_loop()
{
   m_base.current_frame = 0;
   m_base.is_resized = true;

   createRootLayers();

   // clang-format off
   auto mainRoot = obj::BfDrawManager::inst().get< obj::BfDrawManager::RootType::MAIN >();

   auto body = std::make_shared< obj::body::BfBladeBody >(1);
   mainRoot->add(body);
   mainRoot->control().updateBuffer();
   m_dock.bindBody(body);
   

   base::viewport::bfCreateViewports();

   while (!glfwWindowShouldClose(m_base.window->pWindow))
   {
      _pollEvents();

      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      _processSelfInteraction();
      base::viewport::bfRenderViewpors(obj::BfDrawManager::getHovered());

      m_gui.presentInfo();
      m_gui.presentMenuBar();
      m_gui.presentEventLog();
      m_gui.presentTooltip();
      m_gui.presentSettings();
      m_gui.presentFileDialog();
      m_dock.draw();

      ImGui::Render();
      bfUpdateImGuiPlatformWindows();
      bfDrawFrame(m_base);
   }

   vkDeviceWaitIdle(m_base.device);
}

/*
   Тут есть списочное объявление в конструкторе, чтобы
   разные instance сгенеровались 1 раз, чтобы каждый раз
   для каждого такого класса не прописывать bind
*/
BfMain::BfMain()
    : m_base{}, m_holder{}, m_cam{BfCameraMode_Ortho}
{
}

void
BfMain::run()
{
   _init();
   _loop();
   _kill();
}
