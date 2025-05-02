#pragma once
#include <fmt/base.h>
#include <imgui.h>
#include <imgui_internal.h>
#ifndef BF_HANDLE_H
#define BF_HANDLE_H

#include "bfDrawObject.h"
namespace obj
{

class BfGuiIntegration
{
public:
   void updateHoveredStatus(bool sts) noexcept { m_isHovered = sts; };
   virtual void processDragging() {}
   virtual void processInteraction() {}

protected:
   bool m_isHovered;
};

class BfHandleBehavior : public virtual BfGuiIntegration
{
   virtual inline void processHovered() {}
   virtual inline void processIsPressed()
   {
      m_isPressed = m_isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
   }
   virtual inline void processIsDown()
   {
      m_isDown = m_isPressed ||
                 (m_isDown && ImGui::IsMouseClicked(ImGuiMouseButton_Left));
   }
   virtual inline void processIsDragging() {}

   virtual inline void processDragging() override {}
   virtual void processInteraction() override
   {
      processHovered();
      processIsPressed();
      processIsDragging();
      processDragging();
   }

protected:
   bool m_isPressed;
   bool m_isDown;
};

} // namespace obj

#endif
