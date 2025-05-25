#pragma once

#ifndef BF_VIEWPORT_CUSTOM_H
#define BF_VIEWPORT_CUSTOM_H

#include "bfCamera.h"
// #include "bfDrawObjectManager.h"
#include "bfDescriptorStructs.h"
#include "bfPipeline.h"
#include "bfSingle.h"
#include "bfUniforms.h"

#include <GLFW/glfw3.h>
#include <bfVertex2.hpp>
#include <cmath>
#include <fmt/base.h>
#include <functional>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <imgui.h>
#include <memory>
#include <stack>
#include <stdexcept>
#include <strings.h>
#include <vulkan/vulkan_core.h>

namespace base
{
namespace viewport
{

enum class SplitDirection
{
   H,
   V,
   None
};

class ViewPortNode
{
public: // ASSIGMENT
        // clang-format off
   ViewPortNode(
       uint8_t index,
       const glm::vec2& pos,
       const glm::vec2& extent
   )
       : m_pos{pos}
       , m_index{index}
       , m_extent{extent}
       , m_splitType{SplitDirection::None}
       , m_isLeaf{ true } 
       , m_camera(BfCameraMode_Ortho)
   {
      m_camera.bindWindow(base::g::glfwwin());
   }

public: // PUBLIC METHODS
   auto up() -> ViewPortNode& { return *m_FNode; }
   auto left() -> ViewPortNode& { return *m_FNode; }
   auto down() -> ViewPortNode& { return *m_SNode; }
   auto right() -> ViewPortNode& { return *m_SNode; }
   auto isLeaf() -> bool { return m_isLeaf; } 

   auto calcNewPos(SplitDirection dir, float ratio) -> std::pair< glm::vec2, glm::vec2 > { 
      const glm::vec2 half = m_extent * ratio;
      switch (dir) { 
         case SplitDirection::H: return { 
            m_pos,                                
            glm::vec2(m_pos.x, m_pos.y + half.y) 
         };
         case SplitDirection::V: return { 
            m_pos,                                
            glm::vec2(m_pos.x + half.x, m_pos.y) 
         };
         case SplitDirection::None: throw std::runtime_error(
            "Cant calc position for None split type"
         );
      }
      return {};
   };

   auto calcNewExtent(SplitDirection dir, float ratio) -> std::pair< glm::vec2, glm::vec2 > {
      const glm::vec2 half = m_extent * ratio;
      switch (dir)
      {
         case SplitDirection::H: {
            const float top_h = m_extent.y * ratio;
            const float bot_h = m_extent.y - top_h;
            return {
               glm::vec2(m_extent.x, top_h), 
               glm::vec2(m_extent.x, bot_h)
            };
         }
         case SplitDirection::V: {
            const float left_w = m_extent.x * ratio;
            const float right_w = m_extent.x - left_w;
            return {
               glm::vec2(left_w, m_extent.y),
               glm::vec2(right_w, m_extent.y)
            };
         }
         case SplitDirection::None: throw std::runtime_error(
            "Cant calc position for None split type"
         );
      }
      return {};
   }

   virtual auto split(SplitDirection dir, float ratio = 0.5f) -> void { 
      if (!m_isLeaf) throw std::runtime_error(
         "Can't split a non-leaf node"
      );
      if (dir == SplitDirection::None) throw std::runtime_error(
         "Can't use None direction for split"
      );

      m_splitType = dir;
      m_isLeaf = false;
      
      auto newpos = calcNewPos(dir, ratio);
      auto newext = calcNewExtent(dir, ratio);
      
      m_FNode = std::make_unique<ViewPortNode>(2 * m_index + 1, newpos.first, newext.first);
      m_SNode = std::make_unique<ViewPortNode>(2 * m_index + 2, newpos.second, newext.second);

      m_FNode->m_parent = this;
      m_SNode->m_parent = this;
      
      m_ratio = ratio;
   }
   
   auto close() -> void { 
      if (!m_isLeaf || !m_parent) {
           return;
       }

       ViewPortNode* parent = m_parent;
       
       // Check which child this node is
       if (parent->m_FNode.get() == this || parent->m_SNode.get() == this) {
           parent->m_FNode.reset();
           parent->m_SNode.reset();
           parent->m_isLeaf = true;
           parent->m_splitType = SplitDirection::None;
           parent->m_ratio = std::nanf("");
           parent->m_camera.setExtent(parent->m_extent.x, parent->m_extent.y);
       }
   };

   auto update() -> void { 
      if (m_isLeaf) return;

      auto newpos = calcNewPos(m_splitType, m_ratio);
      auto newext = calcNewExtent(m_splitType, m_ratio);

      m_FNode->pos() = newpos.first;
      m_FNode->ext() = newext.first;

      m_SNode->pos() = newpos.second;
      m_SNode->ext() = newext.second;

      m_camera.setExtent(m_extent.x, m_extent.y);
      // FIXME: REMOVE SINGLETON FROM HERE
   }

   virtual auto updateCam() -> void { 
      m_camera.update();
   }
   
   auto isHovered(const glm::vec2& mouse_pos) -> bool { 
      return (mouse_pos.x > m_pos.x && mouse_pos.x < m_pos.x + m_extent.x) &&
             (mouse_pos.y > m_pos.y && mouse_pos.y < m_pos.y + m_extent.y);
   }
   
   auto pos() -> glm::vec2& { return m_pos; }
   auto ext() -> glm::vec2& { return m_extent; }
   auto ratio() -> float& { return m_ratio; }
   auto camera() -> BfCamera&  { return m_camera; }
   auto index() -> uint8_t { return m_index; }
   auto setParent(ViewPortNode* node) -> void { 
      m_parent = node;
   } 

   auto presentRect() -> void { 
      auto list = ImGui::GetBackgroundDrawList();   
      const float padding = 20.f;
      list->AddRect(
         { m_pos.x + padding, m_pos.y + padding },
         { m_pos.x + m_extent.x - padding, m_pos.y + m_extent.y - padding},
         IM_COL32(140, 140, 140, 100)
      );
   }

   

   auto presentButton() -> void { 
      if (m_isLeaf || m_splitType == SplitDirection::None) {
      return; 
      }

      const auto extent = m_extent;
      const float button_w = 8.f;

      
      ImVec2 button_pos{};
      ImVec2 button_sz{};

      switch (m_splitType) {
      case SplitDirection::V: { 
         float center_x = m_pos.x + m_extent.x * m_ratio; 
         button_pos = ImVec2(center_x - button_w * 0.5f, m_pos.y);
         button_sz = ImVec2(button_w, m_extent.y);
         break;
      }
      case SplitDirection::H: { 
         float center_y = m_pos.y + m_extent.y * m_ratio;
         button_pos = ImVec2(m_pos.x, center_y - button_w * 0.5f);
         button_sz = ImVec2(m_extent.x, button_w);
         break;
      }
      default: return;
      }

      const auto flags =
         ImGuiWindowFlags_NoTitleBar |
         ImGuiWindowFlags_NoDecoration |
         ImGuiWindowFlags_NoBackground |
         ImGuiWindowFlags_NoMove |
         ImGuiWindowFlags_NoCollapse |
         ImGuiWindowFlags_NoResize |
         ImGuiWindowFlags_NoScrollWithMouse |
         ImGuiWindowFlags_NoNavInputs |
         ImGuiWindowFlags_NoNavFocus |
         ImGuiWindowFlags_NoBringToFrontOnFocus;

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});

      ImGui::SetNextWindowPos(button_pos, ImGuiCond_Always);
      ImGui::SetNextWindowSize(button_sz, ImGuiCond_Always);

      const std::string title = 
         std::string("##splitter_button") + 
         std::to_string(reinterpret_cast<std::uintptr_t>(this));
      ImGui::Begin(title.c_str(), nullptr, flags);
      
      const std::string button_title = 
         std::string("##splitter_button_inner") + 
         std::to_string(reinterpret_cast<std::uintptr_t>(this));

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.87f, 0.86f, 0.82f, 0.80f));
      if (ImGui::Button(button_title.c_str(), button_sz)) { }
      ImGui::PopStyleColor();

      if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
         const auto& io = ImGui::GetIO();
         float delta = (m_splitType == SplitDirection::V)
            ? io.MouseDelta.x / extent.x
            : io.MouseDelta.y / extent.y;
         
         
         m_ratio += delta;
         m_ratio = std::clamp(m_ratio, 0.05f, 0.95f);

         update();
      }

      ImGui::End();
      
      ImGui::PopStyleVar(2); 
   }

   class ViewPortNodeIterator {
   public:
       ViewPortNodeIterator(ViewPortNode* root) {
           if (root) stack.push(root);
       }

       bool hasNext() const {
           return !stack.empty();
       }

       ViewPortNode* next() {
           if (stack.empty()) return nullptr;
           ViewPortNode* current = stack.top();
           stack.pop();
           if (!current->isLeaf()) {
               stack.push(&current->down());
               stack.push(&current->up());
           }
           return current;
       }
   private:
       std::stack<ViewPortNode*> stack;
   };
   
   auto iter() -> ViewPortNodeIterator { return ViewPortNodeIterator(this); }

   virtual auto pushViewConstants(VkCommandBuffer command_buffer) -> void { 
      BfPushConstants c{ .viewport_index = m_index };
      vkCmdPushConstants(
          command_buffer,
          *BfPipelineHandler::instance()->getLayout(BfPipelineLayoutType_Main),
          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
          0,
          sizeof(BfPushConstants),
          &c
      );
   }

   //! FIXME: Uses for each node separate vkCmdSetViewport, but can be refractored to use 
   //! Single command call
   auto appRender(VkCommandBuffer command_buffer, std::function<void()> func) -> void { 
      VkDeviceSize offsets[] = {0};
      VkViewport viewport{};
      viewport.x = m_pos.x;
      viewport.y = m_pos.y;
      viewport.width = m_extent.x;
      viewport.height = m_extent.y;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(command_buffer, 0, 1, &viewport);

      VkRect2D scissor{};
      scissor.offset = {static_cast< int32_t >(m_pos.x), static_cast< int32_t >(m_pos.y)};
      scissor.extent = {static_cast< uint32_t >(m_extent.x), static_cast< uint32_t >(m_extent.y)};
      vkCmdSetScissor(command_buffer, 0, 1, &scissor); 

      auto& man = base::desc::own::BfDescriptorPipelineDefault::manager();
      man.bindSets(
          base::desc::own::SetType::Main,
          base::g::currentFrame(),
          command_buffer,
          *BfPipelineHandler::instance()->getLayout(BfPipelineLayoutType_Main)
      );
      man.bindSets(
          base::desc::own::SetType::Global,
          base::g::currentFrame(),
          command_buffer,
          *BfPipelineHandler::instance()->getLayout(BfPipelineLayoutType_Main)
      );
      man.bindSets(
          base::desc::own::SetType::Texture,
          base::g::currentFrame(),
          command_buffer,
          *BfPipelineHandler::instance()->getLayout(BfPipelineLayoutType_Main)
      );
      func();
   };

protected:
   BfCamera m_camera;

   SplitDirection m_splitType;

   bool m_isLeaf;
   uint8_t m_index;
   glm::vec2 m_pos;
   glm::vec2 m_extent;

   float m_ratio = std::nanf("");

   ViewPortNode* m_parent = nullptr;
   std::unique_ptr< ViewPortNode > m_FNode;
   std::unique_ptr< ViewPortNode > m_SNode;
};

enum class ViewportEnum : uint32_t { 
   Formatting = 0,
   Channel = 1,
   Body = 2
};

class ViewPortNodeTyped : public ViewPortNode { 
public:
   ViewPortNodeTyped (
       uint8_t index,
       const glm::vec2& pos,
       const glm::vec2& extent
   )
      : ViewPortNode(index, pos, extent)
   {
   }
   
   virtual auto split(SplitDirection dir, float ratio = 0.5f) -> void override { 
      if (!m_isLeaf) throw std::runtime_error(
         "Can't split a non-leaf node"
      );
      if (dir == SplitDirection::None) throw std::runtime_error(
         "Can't use None direction for split"
      );

      m_splitType = dir;
      m_isLeaf = false;
      
      auto newpos = calcNewPos(dir, ratio);
      auto newext = calcNewExtent(dir, ratio);
      
      auto f = std::make_unique<ViewPortNodeTyped>(2 * m_index + 1, newpos.first, newext.first);
      auto s = std::make_unique<ViewPortNodeTyped>(2 * m_index + 2, newpos.second, newext.second);

      f->setParent(this);
      s->setParent(this);

      f->setType(m_type);
      s->setType(m_type);
      
      m_FNode = std::move(f);
      m_SNode = std::move(s);

      m_ratio = ratio;
   }  
   
   virtual auto updateCam() -> void override  { 
      m_camera.update();
   }


   auto setType(ViewportEnum type) -> void { m_type = type; }
   auto type() -> ViewportEnum& { return m_type; }

private:
   ViewportEnum m_type = ViewportEnum::Formatting; 
};


class ViewportManager
{

private: // ASSIGNMENT
   ViewportManager() { }

public:
   static auto inst() -> ViewportManager&  {
      static ViewportManager man{};
      return man;
   }
   static auto init(GLFWwindow* win) -> void { 
      auto& self = ViewportManager::inst();
      self.m_pGLFWwindow = win;
      self.m_root = self._makeScreenViewportNode(); 
      fmt::println("Initializing viewport manager "
                   "with main root with pos={}, ext={}",
                   self.m_root->pos(), self.m_root->ext());
   }
   static auto glfwwin() -> GLFWwindow* { 
      auto& self = ViewportManager::inst();
      return self.m_pGLFWwindow;
   }
   static auto rootWindowExtent() -> glm::vec2
   {
      glm::ivec2 sz;
      glfwGetWindowSize(glfwwin(), &sz.x, &sz.y);
      return sz;
   };
   static auto root() -> ViewPortNodeTyped& { 
      auto& self = ViewportManager::inst();
      return *static_cast<ViewPortNodeTyped*>(self.m_root.get());
   }
   static auto update() -> void { 
      auto& self = ViewportManager::inst();
      auto& root = self.root();
      root.ext() = rootWindowExtent();
      auto it = root.iter();
      while (it.hasNext()) { 
         auto next = it.next();
         next->update();
      }
      auto current = currentHoveredNode();
      if (current.has_value()) { 
         current.value().get().updateCam();
      }

   }
   static auto currentHoveredNode() -> std::optional<std::reference_wrapper<ViewPortNodeTyped>> 
   { 
      auto& self = ViewportManager::inst();
      auto& root = self.root();
      auto it = root.iter();
      const ImVec2 _mouse_pos = ImGui::GetMousePos();
      const glm::vec2 mouse_pos = { _mouse_pos.x, _mouse_pos.y };
      while (it.hasNext()) { 
         auto node = it.next();
         if (self.m_currentContextMenuOpenedIndex.has_value() && 
            node->index() == self.m_currentContextMenuOpenedIndex.value()
         ) { 
            return {*static_cast<ViewPortNodeTyped*>(node)};
         }

         if (node->isHovered(mouse_pos) && node->isLeaf()) { 
            return {*static_cast<ViewPortNodeTyped*>(node)};
         }
      }
      return {}; 
   }
   static auto mousePos() -> glm::vec2 { 
      auto& self = ViewportManager::inst();
      glfwGetCursorPos(self.m_pGLFWwindow, &self.m_lastMousePos.x, &self.m_lastMousePos.y);
      return static_cast< glm::vec2 >(self.m_lastMousePos);
   }
   static auto mouseWorldPos() -> glm::vec3 { 
      auto& self = ViewportManager::inst();
      auto _current = self.currentHoveredNode();
      if (!_current.has_value()) return glm::vec3{0,0,0}; 
      auto& current = _current.value().get();
      switch (current.camera().m_mode)
      { // clang-format off
         case BfCameraMode_Perspective: return glm::vec3(0.0f);
         case BfCameraMode_PerspectiveCentered: return glm::vec3(0.0f);
         case BfCameraMode_Ortho: {
            // TODO: CHANGE getting mouse pos from imgui to glfw
            ImVec2 mousePos = ImGui::GetMousePos();
            glm::vec2 worldMousePos;
            worldMousePos.x = (2.0f * mousePos.x) / current.ext().x - 1.0f;
            worldMousePos.y = (2.0f * mousePos.y) / current.ext().y - 1.0f;
            glm::vec4 ndcPos(worldMousePos.x, worldMousePos.y, 0.0f, 1.0f);
            glm::mat4 invProj = glm::inverse(current.camera().projection(current.ext()));
            glm::mat4 invView = glm::inverse(current.camera().view());
            glm::mat4 invModel = glm::inverse(current.camera().m_scale);
            glm::vec4 worldPos = invView * invProj * invModel * ndcPos;
            glm::vec2 finalWorldPos(worldPos.x, worldPos.y);
            return {finalWorldPos, 0.0f};
         }
         case BfCameraMode_OrthoCentered: return glm::vec3(0.0f);
      } // clang-format on
      throw std::runtime_error("Underfined camera mode");
   }
   static auto mapUBO() -> void
   { // clang-format off
      auto& self = ViewportManager::inst();
      auto& desc = base::desc::own::BfDescriptorPipelineDefault::getMultiViewportUBO();
      auto it = self.root().iter(); 
      void* data = desc.map();
      size_t offset = 0;
      while (it.hasNext()) { 
         auto viewport = it.next();
         if (viewport->isLeaf()) { 
            auto& cam = viewport->camera();

            BfUniformView ubo { 
               .model = glm::mat4(1.0f),
               .view = cam.view(),
               .proj = cam.projection(viewport->ext()),
               .scale = cam.m_scale,
               .cursor_pos = self.m_lastMousePos,
               .camera_pos = cam.m_pos,
               .id_on_cursor = self.m_hoveredID
            };

            memcpy(
               reinterpret_cast< char* >(data) + offset,
               &ubo,
               sizeof(BfUniformView )
            );
            offset += sizeof(BfUniformView );
         }
      }
      desc.unmap();
   }

   // FIXME: This method must be locate at Draw manager (it does)
   // but if BfDrawManager import exists -> recursive error, 
   // should be fullt refractored 
   static auto setHoveredID(uint32_t ID) -> void { 
      auto& self = ViewportManager::inst();
      self.m_hoveredID = ID;
   }
   
   static auto presentContextMenu() -> void { 
      auto& self = ViewportManager::inst();
      
      // if (auto hovered_id = obj::BfDrawManager::getHovered()) { 
      //    return;
      // }
      
      auto current = self.currentHoveredNode() ;
      if (!current.has_value()) return;

      auto& cam = current.value().get().camera();

      if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
         ImGui::OpenPopup("ViewportContextMenu");
      }

      if (ImGui::BeginPopup("ViewportContextMenu"))
      {
         auto& node = current.value().get();
         ImGui::Text("Extent: [%f.1f, %f.1f]", node.ext().x, node.ext().y);
         ImGui::Text("Viewport index: %i", node.index());
         if (ImGui::BeginMenu("Change camera projection"))
         {
              const bool is_pers = cam.m_mode == BfCameraMode_Perspective;
              if (ImGui::RadioButton("Perspective", is_pers))
              {
                  cam.m_mode = BfCameraMode_Perspective;
              }

              const bool is_persc = cam.m_mode == BfCameraMode_PerspectiveCentered;
              if (ImGui::RadioButton("Perspective centered", is_persc))
              {
                  cam.m_mode = BfCameraMode_PerspectiveCentered;
              }

              const bool is_ortho = cam.m_mode == BfCameraMode_Ortho;
              if (ImGui::RadioButton("Orthographic", is_ortho))
              {
                  cam.m_mode = BfCameraMode_Ortho;
              }

              const bool is_orthoc = cam.m_mode == BfCameraMode_OrthoCentered;
              if (ImGui::RadioButton("Orthographic centered", is_orthoc))
              {
                  cam.m_mode = BfCameraMode_OrthoCentered;
              }

              ImGui::EndMenu();
         }

         if (ImGui::BeginMenu("Viewport type"))
         {
              const bool is_formatting = (node.type() == ViewportEnum::Formatting);
              if (ImGui::RadioButton("Formatting", is_formatting))
              {
                  node.type() = ViewportEnum::Formatting;
              }

              const bool is_channel = (node.type() == ViewportEnum::Channel);
              if (ImGui::RadioButton("Channel", is_channel))
              {
                  node.type() = ViewportEnum::Channel;
              }

              const bool is_body = (node.type() == ViewportEnum::Body);
              if (ImGui::RadioButton("Body", is_body))
              {
                  node.type() = ViewportEnum::Body;
              }
              ImGui::EndMenu();
         }
         // if (ImGui::MenuItem("Delete Selected"))
         // {
         //    // your logic
         // }
         ImGui::EndPopup();
         self.m_currentContextMenuOpenedIndex = node.index();
      }
      else { 
         self.m_currentContextMenuOpenedIndex.reset();
      }
      // clang-format off
   };

private:
   auto _makeScreenViewportNode() -> std::unique_ptr< ViewPortNode >
   { // clang-format off
      const glm::vec2 main_extent = rootWindowExtent();
      auto node = std::make_unique< ViewPortNodeTyped >(0, glm::vec2(0.f, 0.f), main_extent);
      node->type() = ViewportEnum::Formatting;
      return std::move(node);
   } // clang-format on

private:
   std::unique_ptr< ViewPortNode > m_root;
   GLFWwindow* m_pGLFWwindow;
   glm::dvec2 m_lastMousePos;
   uint32_t m_hoveredID;
   std::optional< uint32_t > m_currentContextMenuOpenedIndex = {};
};

inline auto
bfCreateViewports() -> void
{
   base::viewport::ViewportManager::init(base::g::glfwwin());
   auto& root = base::viewport::ViewportManager::root();
   root.split(base::viewport::SplitDirection::V);
   // root.right().split(base::viewport::SplitDirection::H);
   // root.right().right().split(base::viewport::SplitDirection::V);
}

}; // namespace viewport
}; // namespace base

#endif
