#pragma once
#include <cmath>
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
         case SplitDirection::H: return {
            glm::vec2(m_extent.x, half.y), 
            glm::vec2(m_extent.x, half.y)
         };
         case SplitDirection::V: return {
            glm::vec2(half.x, m_extent.y),
            glm::vec2(half.x, m_extent.y)
         };
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

   auto updateNodes() -> void { 
      if (m_isLeaf) return;

      auto newpos = calcNewPos(m_splitType, m_ratio);
      auto newext = calcNewExtent(m_splitType, m_ratio);

      m_FNode->pos() = newpos.first;
      m_FNode->ext() = newext.first;

      m_SNode->pos() = newpos.second;
      m_SNode->ext() = newext.second;
   }
   
   auto isHovered(const glm::vec2& mouse_pos) -> bool { 
      return (mouse_pos.x > m_pos.x && mouse_pos.x < m_pos.x + m_extent.x) &&
             (mouse_pos.y > m_pos.y && mouse_pos.y < m_pos.y + m_extent.y);
   }
   
   auto pos() -> glm::vec2& { return m_pos; }
   auto ext() -> glm::vec2& { return m_extent; }
   auto ratio() -> float& { return m_ratio; }

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
   
   auto iter() -> ViewPortNodeIterator { 
      return ViewPortNodeIterator(this);
   }

private:
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
      auto it = self.root().iter();
      while (it.hasNext()) { 
         auto next = it.next();
         next->updateNodes();
      }

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

}


// class CameraManager
// {
// public:
//    using CameraType = uint8_t;
//    enum CameraTypeEnum : CameraType
//    {
//       Single,
//       Left,
//       Right,
//    };
//
// public:
//    CameraManager()
//    {
//       m_cameras[Single] = {};
//       m_cameras[Left] = {};
//       m_cameras[Right] = {};
//    }
//
//    static CameraManager& inst()
//    {
//       static CameraManager c{};
//       return c;
//    }
//
//    template < CameraTypeEnum E >
//    static BfCamera& get()
//    {
//       return inst().m_cameras[E];
//    }
//
// private:
//    std::unordered_map< CameraType, BfCamera > m_cameras;
// };

}; // namespace viewport
}; // namespace base

#endif
