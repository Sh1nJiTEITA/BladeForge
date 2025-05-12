#pragma once
#include "bfCamera.h"
#include "bfSingle.h"
#include <cmath>
#include <imgui.h>
#include <stack>
#ifndef BF_VIEWPORT_CUSTOM_H
#define BF_VIEWPORT_CUSTOM_J

#include <fmt/base.h>
#include <memory>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <bfVertex2.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>

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
       const glm::vec2& pos,
       const glm::vec2& extent
   )
       : m_pos{pos}
       , m_extent{extent}
       , m_splitType{SplitDirection::None}
       , m_isLeaf{ true } 
   {
   }

public: // PUBLIC VARS
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

   auto split(SplitDirection dir, float ratio = 0.5f) -> void { 
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
      
      m_FNode = std::make_unique<ViewPortNode>(newpos.first, newext.first);
      m_SNode = std::make_unique<ViewPortNode>(newpos.second, newext.second);
      
      m_ratio = ratio;
   }

   auto update() -> void { 
      if (m_isLeaf) return;

      auto newpos = calcNewPos(m_splitType, m_ratio);
      auto newext = calcNewExtent(m_splitType, m_ratio);

      m_FNode->pos() = newpos.first;
      m_FNode->ext() = newext.first;

      m_SNode->pos() = newpos.second;
      m_SNode->ext() = newext.second;

      m_camera.setExtent(m_extent.x, m_extent.y);
   }
   
   auto isHovered(const glm::vec2& mouse_pos) -> bool { 
      return (mouse_pos.x > m_pos.x && mouse_pos.x < m_pos.x + m_extent.x) &&
             (mouse_pos.y > m_pos.y && mouse_pos.y < m_pos.y + m_extent.y);
   }
   
   auto pos() -> glm::vec2& { return m_pos; }
   auto ext() -> glm::vec2& { return m_extent; }
   auto ratio() -> float& { return m_ratio; }
   auto camera() -> BfCamera&  { return m_camera; }

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
      if (ImGui::Button(button_title.c_str(), button_sz)) {
         
      }

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

private:
   BfCamera m_camera;

   SplitDirection m_splitType;

   bool m_isLeaf;
   glm::vec2 m_pos;
   glm::vec2 m_extent;

   float m_ratio = std::nanf("");

   std::unique_ptr< ViewPortNode > m_FNode;
   std::unique_ptr< ViewPortNode > m_SNode;
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
   static auto root() -> ViewPortNode& { 
      auto& self = ViewportManager::inst();
      return *self.m_root;
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

   }
   static auto currentHoveredNode() -> std::optional<std::reference_wrapper<ViewPortNode>> 
   { 
      auto& self = ViewportManager::inst();
      auto& root = self.root();
      auto it = root.iter();
      const ImVec2 _mouse_pos = ImGui::GetMousePos();
      const glm::vec2 mouse_pos = { _mouse_pos.x, _mouse_pos.y };
      while (it.hasNext()) { 
         auto node = it.next();
         if (node->isHovered(mouse_pos) && node->isLeaf()) { 
            return {*node};
         }
      }
      return {}; 
   }

private:
   auto _makeScreenViewportNode() -> std::unique_ptr< ViewPortNode >
   {
      const glm::vec2 main_extent = rootWindowExtent();
      return std::make_unique< ViewPortNode >(glm::vec2(0.f, 0.f), main_extent);
   }

private:
   std::unique_ptr< ViewPortNode > m_root;
   GLFWwindow* m_pGLFWwindow;
};

inline auto bfCreateViewports() -> void { 
   base::viewport::ViewportManager::init(base::g::glfwwin());
   auto& root = base::viewport::ViewportManager::root();
   root.split(base::viewport::SplitDirection::V);
   root.right().split(base::viewport::SplitDirection::H);

}

}; // namespace viewport
}; // namespace base

#endif
