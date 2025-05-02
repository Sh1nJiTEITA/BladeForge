#pragma once
#include <fmt/format.h>
#ifndef BF_HANDLE_H
#define BF_HANDLE_H

#include "bfCamera.h"
#include "functional"
#include <fmt/base.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
namespace obj
{

class BfGuiIntegration
{
public:
   inline static uint32_t runtimeID = 0;

   void updateHoveredStatus(bool sts) noexcept { m_isHovered = sts; };
   virtual void processDragging() {}
   virtual void presentContextMenu() {}

   virtual void processInteraction()
   {
      const std::string s(fmt::format("context-menu-id={}", runtimeID));

      if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && m_isHovered)
      {
         fmt::println("clicked");
         ImGui::OpenPopup(s.c_str());
      }

      if (ImGui::BeginPopup(s.c_str()))
      {
         ImGui::PushID(runtimeID);
         {
            presentContextMenu();
         }
         ImGui::PopID();
         ImGui::EndPopup();
      }
      runtimeID++;
   }
   virtual void presentTooltipInfo() {}

protected:
   bool m_isHovered;
};

class BfHandleBehavior : public virtual BfGuiIntegration
{
protected:
   virtual inline void processHovered() {}

   virtual inline void processIsPressed()
   {
      m_isPressed = m_isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
   }

   virtual inline void processIsDown()
   {
      m_isDown = m_isPressed ||
                 (m_isDown && ImGui::IsMouseDown(ImGuiMouseButton_Left));
   }

   virtual inline void processDragging() override
   {
      if (m_isDown)
      {
         if (m_isPressed)
         {
            m_initialMousePos = BfCamera::instance()->mouseWorldCoordinates();
         }
      }
   }

   glm::vec3 delta3D()
   {
      return BfCamera::instance()->mouseWorldCoordinates() - m_initialMousePos;
   }

   virtual void processInteraction() override
   {
      processHovered();
      processIsPressed();
      processIsDown();
      processDragging();
   }

   virtual void presentTooltipInfo() override
   {
      constexpr ImVec4 green = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
      constexpr ImVec4 red = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);

      ImGui::Text("isPressed =");
      ImGui::SameLine();
      ImGui::TextColored(
          m_isPressed ? green : red,
          m_isPressed ? "true" : "false"
      );

      ImGui::Text("isDown =");
      ImGui::SameLine();
      ImGui::TextColored(m_isDown ? green : red, m_isDown ? "true" : "false");

      auto delta = delta3D();
      ImGui::Text("delta3D = (%.3f, %.3f, %.3f)", delta.x, delta.y, delta.z);
   }

protected:
   bool m_isPressed;
   bool m_isDown;

   glm::vec3 m_initialMousePos{0.f, 0.f, 0.f};
};

} // namespace obj

#endif
