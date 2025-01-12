#ifndef BF_CAMERA_H
#define BF_CAMERA_H

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/vec2.hpp>
#include <stdexcept>

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
   BfCameraMode_Perspective,
   BfCameraMode_PerspectiveCentered,
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
   glm::vec3 m_posOld;

   float m_vAngle;
   float m_vAngleOld;

   glm::vec3 m_target;
   glm::vec3 m_up;

   static BfCamera* m_pInstance;

   BfCamera(BfCameraMode mode);

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

   // NOTE: Extent can be set via GLFW if
   // only single viewport is used!
   void setExtent(float x, float y);

   // Out
   glm::mat4 view();
   glm::mat4 projection();

   void update();
};

#endif
