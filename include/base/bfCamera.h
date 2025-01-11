#ifndef BF_CAMERA_H
#define BF_CAMERA_H

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

template <int KEY>
struct BfKeyState
{
   bool isPressed;  /** Нажата ли кнопка в целом? */
   bool isReleased; /** Отпущена ли кнопка? */
   bool isHeld;     /** Зажимается ли кнопка? */

   bool isPressedBefore;
   bool isPressedInitial;
};

template <int KEY>
void bfKeyUpdate(BfKeyState<KEY>& key, GLFWwindow* window);

typedef uint32_t BfCameraMode;
enum BfCameraMode_ : BfCameraMode
{
   BfCameraMode_Perspective
};

class BfCamera
{
public:
   GLFWwindow* m_pWindow;
   glm::vec2 m_extent;

   BfCameraMode m_cameraMode;

   glm::vec2 m_posMouseOld;
   glm::vec2 m_posMouse;
   glm::vec2 m_mouseDelta;

   BfKeyState<GLFW_MOUSE_BUTTON_MIDDLE> m_middleMouseState;
   BfKeyState<GLFW_MOUSE_BUTTON_LEFT> m_leftMouseState;
   BfKeyState<GLFW_MOUSE_BUTTON_RIGHT> m_rightMouseState;

   glm::vec3 m_pos;
   glm::vec3 m_target;
   glm::vec3 m_up;

   static BfCamera* m_pInstance;

   BfCamera(
       const glm::vec3& pos,
       const glm::vec3& target,
       const glm::vec3& up,
       GLFWwindow* window
   );

   BfCamera();

   // Creation
   static BfCamera* instance() noexcept;
   static void bindInstance(BfCamera*);
   void bindWindow(GLFWwindow* window);
   void setExtent(float x, float y);

   // Out
   glm::mat4 view();
   glm::mat4 projection();

   void update();
};

#endif
