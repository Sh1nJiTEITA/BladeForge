#include <imgui_impl_vulkan.h>

#include <cassert>
#include <imgui_internal.h>
#include <memory>

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

   // clang-format on
}

void
BfMain::_pollEvents()
{
   glfwPollEvents();
   auto& io = ImGui::GetIO();

   if (io.WantCaptureMouse || io.WantCaptureKeyboard)
   {
   }
   else
   {
      auto hovered = bfGetCurrentObjectId(m_base);
      obj::BfDrawManager::inst().setHovered(hovered);
      base::viewport::ViewportManager::setHoveredID(hovered);
   }

   _processKeys();

   // __gui.updateFonts();
   m_gui.pollEvents();
   m_cam.update();

   // TODO: REMAKE !
   obj::BfGuiIntegration::runtimeID = 0;

   base::viewport::ViewportManager::update();
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

   const auto ttype = obj::BfDrawManager::RootType::IMAGE_LOAD;
   obj::BfDrawManager::inst().addRoot< ttype >();
};

void
BfMain::_loop()
{
   m_base.current_frame = 0;
   m_base.is_resized = true;

   createRootLayers();

   // clang-format off
   auto mainRoot = obj::BfDrawManager::inst().get< obj::BfDrawManager::RootType::MAIN >();
   auto textRoot = obj::BfDrawManager::inst().get< obj::BfDrawManager::RootType::IMAGE_LOAD >();
   // clang-format on

   // auto info = obj::section::SectionCreateInfo{};
   // auto bs = std::make_shared< obj::section::BfBladeSection >(&info);
   // bs->make();
   // mainRoot->add(bs);

   auto tp = std::make_shared< obj::curves::BfTexturePlane >(0.5f, 0.5f);
   tp->make();
   textRoot->add(tp);
   //
   auto body = std::make_shared< obj::body::BfBladeBody >(2);
   mainRoot->add(body);
   //

   mainRoot->control().updateBuffer();
   textRoot->control().updateBuffer();

   m_dock.bindBody(body);

   // auto top = std::make_shared< obj::BfDrawRootLayer >(3000, 30);
   // top->addf< obj::curves::BfSingleLineWH >(BfVertex3{}, BfVertex3{});
   // auto other_buf = top->addf< obj::BfDrawRootLayer >(2000, 5);
   // auto other_buf2 = other_buf->addf< obj::BfDrawRootLayer >(2000, 5);

   // top->make();
   // top->control().updateBuffer();

   base::viewport::bfCreateViewports();

   while (!glfwWindowShouldClose(m_base.window->pWindow))
   {
      _pollEvents();

      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      _processSelfInteraction();

      auto& root = base::viewport::ViewportManager::root();
      auto it = root.iter();
      glm::vec2 cursor_pos = {ImGui::GetMousePos().x, ImGui::GetMousePos().y};
      int i = 0;
      while (it.hasNext())
      {
         auto node = it.next();
         // fmt::println(
         //     "{}|Node.isHovered={} | mpos={} | npos={} | next={}",
         //     i++,
         //     node->isHovered(cursor_pos),
         //     cursor_pos,
         //     node->pos(),
         //     node->ext()
         // );
         node->presentButton();
         node->presentRect();
      }

      m_gui.presentInfo();
      m_gui.presentMenuBar();
      m_gui.presentEventLog();
      m_gui.presentTooltip();
      m_gui.presentSettings();
      m_gui.presentFileDialog();
      m_gui.presentCameraWindow();
      // m_gui.presentViewportRatioButton(m_base);

      // ImGui::ShowDemoWindow();
      m_dock.draw();

      // ImGui::Begin("Choose image");
      // {
      //    bool is_changed = false;
      //    const char* var[]{
      //        "./resources/test.jpg",
      //        "./resources/test2.png",
      //        "./resources/R.jpg",
      //    };
      //    static int current = 0;
      //    if (ImGui::RadioButton(var[0], current == 0))
      //    {
      //       is_changed = true;
      //       current = 0;
      //    }
      //    if (ImGui::RadioButton(var[1], current == 1))
      //    {
      //       is_changed = true;
      //       current = 1;
      //    }
      //    if (ImGui::RadioButton(var[2], current == 2))
      //    {
      //       is_changed = true;
      //       current = 2;
      //    }
      //    if (is_changed)
      //    {
      //       auto& man =
      //       base::desc::own::BfDescriptorPipelineDefault::manager(); auto&
      //       texture = man.get< base::desc::own::BfDescriptorTextureTest >(
      //           base::desc::own::SetType::Texture,
      //           0
      //       );
      //       texture.reload(var[current]);
      //       man.updateSets();
      //    }
      // }
      // ImGui::End();

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
