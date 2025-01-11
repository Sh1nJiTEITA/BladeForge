#include "bfCamera.h"

#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <stdexcept>

template <int KEY>
void
bfKeyUpdate(BfKeyState<KEY>& key, GLFWwindow* window)
{
   bool is = glfwGetMouseButton(window, KEY) == GLFW_PRESS;

   key.isPressedInitial = is && !key.isPressedBefore;
   key.isPressedBefore = is && key.isPressed;
   key.isHeld = is && key.isPressedBefore;
   key.isReleased = !is && key.isPressed;
   key.isPressed = is;
}

BfCamera* BfCamera::m_pInstance = nullptr;

BfCamera::BfCamera(
    const glm::vec3& pos,
    const glm::vec3& target,
    const glm::vec3& up,
    GLFWwindow* window
)
    : m_pWindow{window}
    , m_pos{pos}
    , m_target{target}
    , m_up{up}
    , m_posMouseOld{0, 0}
    , m_mouseDelta{0, 0}
    , m_middleMouseState{0, 0}
    , m_leftMouseState{0, 0}
    , m_rightMouseState{0, 0}
    , m_cameraMode{BfCameraMode_Perspective}
{
}

BfCamera::BfCamera()
    : m_pos{0.0f, 0.0f, 0.0f}
    , m_target{0.0f, 0.0f, 0.0f}
    , m_up{0.0f, 0.0f, 1.0f}
    , m_posMouseOld{0, 0}
    , m_pWindow{nullptr}
{
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

glm::mat4
BfCamera::view()
{
   switch (m_cameraMode)
   {
      case BfCameraMode_Perspective:
         return glm::lookAt(m_pos, m_pos + m_target, m_up);
   }
   throw std::runtime_error("Underfined camera mode (view)");
}

glm::mat4
BfCamera::projection()
{
   switch (m_cameraMode)
   {
      case BfCameraMode_Perspective:
         return glm::perspective(
             glm::radians(45.0f),
             m_extent.x / m_extent.y,
             0.01f,
             100.0f
         );
   }
   throw std::runtime_error("Underfined camera mode (proj)");
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

   if (m_middleMouseState.isPressedInitial)
   {
      m_posMouseOld = mousePosNew;
   }
   if (m_middleMouseState.isPressed)
   {
      m_mouseDelta = m_posMouse - m_posMouseOld;
   }
   else
   {
      m_mouseDelta = {0, 0};
   }

   float radius = glm::length(m_pos);
}
