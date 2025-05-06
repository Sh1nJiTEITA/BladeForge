#include "bfCamera.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <stdexcept>

template < int KEY >
void
bfKeyUpdate(BfKeyState< KEY >& key, GLFWwindow* window)
{
   bool is = glfwGetMouseButton(window, KEY) == GLFW_PRESS;

   key.isPressedInitial = is && !key.isPressedBefore;
   key.isPressedBefore = is && key.isPressed;
   key.isHeld = is && key.isPressedBefore;
   key.isReleased = !is && key.isPressed;
   key.isPressed = is;
}

BfCamera* BfCamera::m_pInstance = nullptr;

BfCamera::BfCamera(BfCameraMode mode)
    : BfCamera()
{
   m_mode = mode;
   _assignMainVectors();
}

BfCamera::BfCamera()
    : m_mode{BfCameraMode_PerspectiveCentered}
{
   _assignMainVectors();
}

BfCamera*
BfCamera::instance() noexcept
{
   return BfCamera::m_pInstance;
}

void
BfCamera::bindInstance(BfCamera* pCam)
{
   if (!pCam)
   {
      throw std::runtime_error("Cant bind nullptr BfCamera!");
   }
   BfCamera::m_pInstance = pCam;
}

void
BfCamera::bindWindow(GLFWwindow* window)
{
   if (!window)
   {
      throw std::runtime_error("Cant bind nullptr window!");
   }
   m_pWindow = window;
}

void
BfCamera::setExtent(float x, float y)
{
   m_extent = {x, y};
}

void
BfCamera::setMode(BfCameraMode mode)
{
   m_mode = mode;
   _assignMainVectors();
}

glm::mat4
BfCamera::view()
{
   switch (m_mode)
   {
   case BfCameraMode_Perspective:
      return glm::lookAt(m_pos, m_pos + m_target, m_up);
   case BfCameraMode_PerspectiveCentered:
      return glm::lookAt(m_pos, m_target, m_up);
   case BfCameraMode_Ortho:
      return glm::lookAt(m_pos, m_pos + m_target, m_up);
   case BfCameraMode_OrthoCentered:
      return glm::lookAt(m_pos, m_target, m_up);
   }
   throw std::runtime_error("Underfined camera mode (view)");
}

glm::mat4
BfCamera::projection()
{
   float ratio =
       static_cast< double >(m_extent.y) / static_cast< double >(m_extent.x);
   switch (m_mode)
   {
   case BfCameraMode_Perspective:
      return glm::perspective(
          glm::radians(45.0f),
          m_extent.x / m_extent.y,
          0.01f,
          100.0f
      );
   case BfCameraMode_PerspectiveCentered:
      return glm::perspective(
          glm::radians(45.0f),
          m_extent.x / m_extent.y,
          0.01f,
          100.0f
      );
   case BfCameraMode_Ortho:
      return static_cast< glm::mat4 >(
          glm::ortho(-1.0, 1.0, 1.0 * ratio, -1.0 * ratio, -1000.0, 1000.0)
      );
   case BfCameraMode_OrthoCentered:
      return static_cast< glm::mat4 >(
          // glm::ortho(-0.5, 0.5, 0.5 * ratio, -0.5 * ratio, -1000.0,
          // 1000.0)
          glm::ortho(-1.0, 1.0, 1.0 * ratio, -1.0 * ratio, -1000.0, 1000.0)
      );
   }
   throw std::runtime_error("Underfined camera mode (proj)");
}

glm::vec3
BfCamera::mouseWorldCoordinates()
{
   switch (m_mode)
   { // clang-format off
      case BfCameraMode_Perspective: return glm::vec3(0.0f);
      case BfCameraMode_PerspectiveCentered: return glm::vec3(0.0f);
      case BfCameraMode_Ortho: {
         auto extent = BfCamera::instance()->m_extent;
         // TODO: CHANGE getting mouse pos from imgui to glfw
         ImVec2 mousePos = ImGui::GetMousePos();
         glm::vec2 worldMousePos;
         worldMousePos.x = (2.0f * mousePos.x) / extent.x - 1.0f;
         worldMousePos.y = (2.0f * mousePos.y) / extent.y - 1.0f;
         glm::vec4 ndcPos(worldMousePos.x, worldMousePos.y, 0.0f, 1.0f);
         glm::mat4 invProj = glm::inverse(projection());
         glm::mat4 invView = glm::inverse(view());
         glm::mat4 invModel = glm::inverse(m_scale);
         glm::vec4 worldPos = invModel * invView * invProj * ndcPos;
         glm::vec2 finalWorldPos(worldPos.x, worldPos.y);
         return {finalWorldPos, 0.0f};
      }
      case BfCameraMode_OrthoCentered: return glm::vec3(0.0f);
   } // clang-format on
   throw std::runtime_error("Underfined camera mode");
}

void
BfCamera::update()
{
   bfKeyUpdate(m_middleMouseState, m_pWindow);
   bfKeyUpdate(m_leftMouseState, m_pWindow);
   bfKeyUpdate(m_rightMouseState, m_pWindow);

   // get new mouse position on screen
   glm::dvec2 mousePosNew;
   glfwGetCursorPos(m_pWindow, &mousePosNew.x, &mousePosNew.y);
   m_posMouse = mousePosNew;

   switch (m_mode)
   {
   case BfCameraMode_Perspective: {
      if (m_yScroll != m_yScrollOld)
      {
         const float scrollSen = 0.05f;
         glm::vec3 to_target = m_target - m_pos;
         m_pos += (m_yScroll - m_yScrollOld) * to_target * scrollSen;
      }
      m_yScrollOld = m_yScroll;
   }
   case BfCameraMode_PerspectiveCentered: {
      if (m_yScroll != m_yScrollOld)
      {
         const float scrollSen = 0.05f;
         glm::vec3 to_target = m_target - m_pos;
         m_pos += (m_yScroll - m_yScrollOld) * to_target * scrollSen;
      }
      m_yScrollOld = m_yScroll;
   }
   case BfCameraMode_Ortho: {
      if (m_yScroll != m_yScrollOld)
      {
         const float scrollSen = 0.05f;
         m_scale = glm::scale(
             glm::mat4(1.0f),
             glm::vec3((m_yScroll - m_yScrollOld) * scrollSen)
         );
         m_descale = glm::scale(
             glm::mat4(1.0f),
             1.f / glm::vec3((m_yScroll - m_yScrollOld) * scrollSen)
         );
      }
   }
   case BfCameraMode_OrthoCentered: {
      if (m_yScroll != m_yScrollOld)
      {
         const float scrollSen = 0.05f;
         float nonlinearZoomFactor = std::exp(m_yScroll * scrollSen);
         nonlinearZoomFactor = std::clamp(nonlinearZoomFactor, 0.01f, 1000.0f);
         glm::vec3 zoom = glm::vec3(nonlinearZoomFactor);
         m_scale = glm::scale(glm::mat4(1.0f), zoom);
      }
   }
   };

   // Process scroll

   if (m_middleMouseState.isPressedInitial)
   {
      m_posMouseOld = mousePosNew;
      m_posOld = m_pos;
      m_vAngleOld = m_vAngle;
      m_yScrollOld = m_yScroll;
   }
   if (m_middleMouseState.isPressed)
   { // clang-format off
      switch (m_mode) { 
         case BfCameraMode_Perspective: { 
            const float sen_x = 0.01;
            const float sen_y = 0.005;
            m_mouseDelta = m_posMouse - m_posMouseOld;
            m_pos = m_posOld
                + glm::vec3(1.0f, 0.0f, 0.0f) * m_mouseDelta.x * sen_x
                - glm::vec3(0.0f, 1.0f, 0.0f) * m_mouseDelta.y * sen_y
            ;
            break;
         }
         case BfCameraMode_PerspectiveCentered: { 
            const float sen_x = 0.01f;
            const float sen_y = 0.005f;
            m_mouseDelta = m_posMouse - m_posMouseOld;

            m_vAngle = m_vAngleOld - m_mouseDelta.y * sen_y;
            m_vAngle = std::clamp(m_vAngle, glm::radians(-89.9999f), glm::radians(89.9999f));

            glm::vec3 right = glm::normalize(glm::cross(m_up, glm::normalize(m_posOld)));
            glm::quat horizontalRotation = glm::angleAxis(-m_mouseDelta.x * sen_x, m_up);
            glm::quat verticalRotation = glm::angleAxis(m_vAngleOld - m_vAngle, right);
            glm::quat combinedRotation = horizontalRotation * verticalRotation;

            m_pos = combinedRotation * m_posOld;
            break;
         }
         case BfCameraMode_Ortho: { 
            const float sen_x = 0.01;
            const float sen_y = 0.005;
            m_mouseDelta = -(m_posMouse - m_posMouseOld);
            m_pos = m_posOld
                + glm::vec3(1.0f, 0.0f, 0.0f) * m_mouseDelta.x * sen_x
                - glm::vec3(0.0f, 1.0f, 0.0f) * m_mouseDelta.y * sen_y
            ;
            break;
         }
         case BfCameraMode_OrthoCentered: { 
            const float sen_x = 0.01f;
            const float sen_y = 0.005f;
            m_mouseDelta = m_posMouse - m_posMouseOld;

            m_vAngle = m_vAngleOld + m_mouseDelta.y * sen_y;
            m_vAngle = std::clamp(m_vAngle, glm::radians(-89.9999f), glm::radians(89.9999f));

            glm::vec3 right = glm::normalize(glm::cross(m_up, glm::normalize(m_posOld)));
            glm::quat horizontalRotation = glm::angleAxis(-m_mouseDelta.x * sen_x, m_up);
            glm::quat verticalRotation = glm::angleAxis(m_vAngleOld - m_vAngle, right);
            glm::quat combinedRotation = horizontalRotation * verticalRotation;

            m_pos = combinedRotation * m_posOld;
            break;
         }
      }
      
   } // clang-format on 
   else
   {
      m_mouseDelta = {0, 0};
   }
}


void 
BfCamera::_assignMainVectors() 
{ 
   switch (m_mode)
   {
      case BfCameraMode_Perspective:
         m_pos = {0.0, 0.0, -3.0};
         m_target = {0.0, 0.0, 1.0};
         m_up = {0.0, 1.0, 0.0};
         break;
      case BfCameraMode_PerspectiveCentered:
         m_pos = {0.0, 0.0, -3.0};
         m_target = {0.0, 0.0, 0.0};
         m_up = {0.0, 1.0, 0.0};
         break;
      case BfCameraMode_Ortho:
         m_pos = {0.0, 0.0, 3.0};
         m_target = {0.0, 0.0, -1.0};
         m_up = {0.0, 1.0, 0.0};
         break;
      case BfCameraMode_OrthoCentered:
         m_pos = {0.0, 0.0, 3.0};
         m_target = {0.0, 0.0, 0.0};
         m_up = {0.0, 1.0, 0.0};
         break;
   }
}
