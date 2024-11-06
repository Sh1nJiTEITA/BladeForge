#include "bfWindow.h"

/*
GLFWwindow* BfWindow::pWindow = nullptr;
std::string BfWindow::name = "";
int BfWindow::width = 0;
int BfWindow::height = 0;
bool BfWindow::resized = false;
*/

void bfToggleCamParts(BfWindow* window, uint32_t cam_index, bool decision)
{
   switch (cam_index)
   {
      case 0: {  // Free cam
         if (decision)
         {
            window->is_free_camera_active = true;
            window->cam_mode              = 0;
         }
         else
         {
            window->is_free_camera_active = false;
            window->first_free            = true;
         }
         break;
      }
      case 1: {
         if (decision)
         {
            window->is_rotate_camera_active = false;
            window->cam_mode                = 1;
         }
         else
         {
            window->first_free_rotate = true;
            window->first_s_rotate    = true;
         }
         break;
      }
      case 2: {  // S cam
         if (decision)
         {
            window->is_s_camera_active = true;
            window->cam_mode           = 2;
         }
         else
         {
            window->is_s_camera_active = false;
            window->first_s            = true;
         }
         break;
      }
   }
}

void bfCalculateViewPartsFree(BfWindow* window)
{
   if (window->first_free)
   {
      window->first_free = false;
      window->lastX      = window->xpos;
      window->lastY      = window->ypos;
   }

   float xoffset = window->xpos - window->lastX;
   float yoffset = window->lastY - window->ypos;
   yoffset *= -1;

   double sensitivity;
   if (window->proj_mode == 0)
   {
      sensitivity = ZOOM_SENSITIVITY;
   }
   else
   {
      sensitivity = ZOOM_SENSITIVITY;
   }

   xoffset *= sensitivity;
   yoffset *= sensitivity;

   // static double yaw = -925.6;
   // static double pitch = 44.3;

   window->yaw += xoffset;
   window->pitch += yoffset;

   if (window->pitch > 89.0f) window->pitch = 89.0f;
   if (window->pitch < -89.0f) window->pitch = -89.0f;

   // std::cout << "y: " << yaw << ", p: " << pitch << "\n";

   window->front =
       glm::normalize(glm::vec3(glm::cos(glm::radians(window->yaw)) *
                                    glm::cos(glm::radians(window->pitch)),
                                glm::sin(glm::radians(window->pitch)),
                                glm::sin(glm::radians(window->yaw)) *
                                    glm::cos(glm::radians(window->pitch))));

   // window->view = glm::lookAt(window->pos, window->pos + window->front,
   // window->up);

   window->lastX = window->xpos;
   window->lastY = window->ypos;

   /*std::cout << "free_cam: pos:(" << window->pos.x << ", " << window->pos.y <<
      ", " << window->pos.z
           << "); front:(" << window->front.x << ", " << window->front.y << ", "
      << window->front.z
           << "); up:(" << window->up.x << ", " << window->up.y << ", " <<
      window->up.z << ")\n";*/
}

void bfCalculateViewPartsS(BfWindow* window)
{
   static float lastXs = 0;
   static float lastYs = 0;

   if (window->first_s)
   {
      // std::cout << " first s ";
      window->first_s = false;
      lastXs          = window->xpos;
      lastYs          = window->ypos;
   }

   float deltaX = lastXs - window->xpos;
   float deltaY = lastYs - window->ypos;
   // std::cout << "deltaX = " << deltaX << "; deltaY = " << deltaY << "\n";
   // std::cout << "; xpos = " << window->xpos << "; lastXs = " << lastXs << ";
   // ypos = " << "; lastYs = " << "; ypos" << "\n";

   // x-direction
   glm::vec3 x_dir = -glm::normalize(glm::cross(window->up, window->front)) *
                     (deltaX / 50.0f);

   // y-direction
   glm::vec3 y_dir =
       -glm::normalize(
           glm::cross(glm::cross(window->up, window->front), window->front)) *
       (deltaY / 50.0f);

   // z-direction
   glm::vec3 z_dir;
   if (window->is_scroll)
   {
      if (window->proj_mode == 0)
      {
         z_dir = (window->front) * static_cast<float>(window->scroll_yoffset) *
                 SCROLL_SENSITIVITY;
         window->scroll_yoffset = 0;
      }
      else if (window->proj_mode == 1)
      // z_dir = (window->front) * static_cast<float>(window->scroll_yoffset)
      // * 10.0f;
      {
         /*if (window->scroll_yoffset < 0) {
                 window->ortho_scale -= 1 / (1 +
         glm::exp(-static_cast<float>(window->scroll_yoffset)));
         }
         else {
                 window->ortho_scale += 1 / (1 +
         glm::exp(-static_cast<float>(window->scroll_yoffset)));
         }*/

         static float last_yoffset = window->scroll_yoffset;
         static float st           = 0.0f;

         float delta_yoffset       = window->scroll_yoffset - last_yoffset;
         st += delta_yoffset / 2.0;
         if (st < -10)
         {
            st = -10;
         }
         if (st > 6)
         {
            st = 6;
         }

         window->ortho_scale =
             0.1 + 32.0f / (1 + glm::exp(-static_cast<float>(st)));

         /*if (window->ortho_scale - 0.1 < 1e-4) {
                 window->scroll_yoffset = 0;
         }*/
         std::cout << st << "\n";
         last_yoffset = window->scroll_yoffset;
         z_dir        = glm::vec3(0.0f);
      }

      window->is_scroll = false;
   }
   else
   {
      z_dir = glm::vec3(0.0f);
   }

   window->pos += y_dir + x_dir + z_dir;

   lastXs = window->xpos;
   lastYs = window->ypos;
}

void bfCalculateRotateView(BfWindow* window)
{
   static float lastRx = 0;
   static float lastRy = 0;

   if (window->first_free_rotate)
   {
      window->first_free_rotate = false;
      lastRx                    = window->xpos;
      lastRy                    = window->ypos;
   }

   float xoffset = window->xpos - lastRx;
   float yoffset = lastRy - window->ypos;
   yoffset *= -1;

   double sensitivity;
   if (window->proj_mode == 0)
   {
      sensitivity = ZOOM_SENSITIVITY;
   }
   else
   {
      sensitivity = ZOOM_SENSITIVITY;
   }

   xoffset *= sensitivity;
   yoffset *= sensitivity;

   window->yaw += xoffset;
   window->pitch += yoffset;

   if (window->pitch > 89.0f) window->pitch = 89.0f;
   if (window->pitch < -89.0f) window->pitch = -89.0f;

   window->front =
       glm::normalize(glm::vec3(glm::cos(glm::radians(window->yaw)) *
                                    glm::cos(glm::radians(window->pitch)),

                                glm::sin(glm::radians(window->pitch)),

                                glm::sin(glm::radians(window->yaw)) *
                                    glm::cos(glm::radians(window->pitch))));

   /*window->lastRx = window->xpos;
   window->lastRy = window->ypos;*/
   lastRx = window->xpos;
   lastRy = window->ypos;

   // part 2

   static float lastRXs = 0;
   static float lastRYs = 0;

   if (window->first_s_rotate)
   {
      window->first_s_rotate = false;
      lastRXs                = window->xpos;
      lastRYs                = window->ypos;
   }

   float deltaX = lastRXs - window->xpos;
   float deltaY = lastRYs - window->ypos;

   // x-direction
   glm::vec3 x_dir = glm::vec3(0.0f);

   // y-direction
   glm::vec3 y_dir = glm::vec3(0.0f);

   if (!((window->pitch == -89.0f) || (window->pitch == 89.0f)))
   {
      y_dir = -glm::normalize(glm::cross(glm::cross(window->up, window->front),
                                         window->front)) *
              (deltaY / 50.0f);
      x_dir = -glm::normalize(glm::cross(window->up, window->front)) *
              (deltaX / 50.0f);
   }

   // z-direction
   glm::vec3 z_dir;
   if (window->is_scroll)
   {
      z_dir =
          (window->front) * static_cast<float>(window->scroll_yoffset) * 1.1f;
      window->is_scroll      = false;
      window->scroll_yoffset = 0;
   }
   else
   {
      z_dir = glm::vec3(0.0f);
   }

   window->pos += y_dir + x_dir + z_dir;

   lastRXs       = window->xpos;
   lastRYs       = window->ypos;

   window->lastX = window->xpos;
   window->lastY = window->ypos;
}

void bfUpdateView(BfWindow* window)
{
   window->view =
       glm::lookAt(window->pos, window->pos + window->front, window->up);
}

BfEvent bfCreateWindow(BfWindow* window)
{
   glfwInit();
   bfvSetGLFWProperties();

   // window->pos = { 2.0f, -2.0f, -1.0f};

   window->pos = {0.04, 0.72, -3.1};

   window->up  = {0.0f, 1.0f, 0.0f};
   bfCalculateViewPartsFree(window);
   // window->front = { 0.0f, 0.0f, 0.0f };
   window->is_free_camera_active = false;
   window->is_s_camera_active    = false;

   window->pWindow               = glfwCreateWindow(window->width,
                                      window->height,
                                      window->name.c_str(),
                                      nullptr,
                                      nullptr);

   glfwSetWindowUserPointer(window->pWindow, window);
   glfwSetScrollCallback(
       window->pWindow,
       [](GLFWwindow* window, double xoffset, double yoffset) {
          BfWindow* thisWindow =
              static_cast<BfWindow*>(glfwGetWindowUserPointer(window));
          if (!thisWindow->is_any_window_hovered_func())
             thisWindow->scroll_yoffset += yoffset;
          // std::cout << "xoffset = " << xoffset << "; yoffset = << " <<
          // yoffset << "soffset = " << thisWindow->scroll_xoffset << "\n";
          thisWindow->is_scroll = true;

          bfCalculateViewPartsS(thisWindow);
          ////thisWindow->scroll_xoffset = xoffset;
          // if (thisWindow->scroll_yoffset != 0) {
          //	thisWindow->scroll_xoffset = yoffset;
          //	thisWindow->is_scroll = true;
          // }
          // else {
          //	thisWindow->is_scroll = false;
          // }
       });

   glfwSetCursorPosCallback(window->pWindow,
                            [](GLFWwindow* window, double xpos, double ypos) {
                               BfWindow* thisWindow = static_cast<BfWindow*>(
                                   glfwGetWindowUserPointer(window));
                               thisWindow->xpos = xpos;
                               thisWindow->ypos = ypos;
                               // std::cout << "cb: " << thisWindow->xpos << ",
                               // " << thisWindow->ypos << "\n";
                            });
   glfwSetFramebufferSizeCallback(
       window->pWindow,
       [](GLFWwindow* window, int width, int height) {
          BfWindow* thisWindow =
              static_cast<BfWindow*>(glfwGetWindowUserPointer(window));

          thisWindow->height  = height;
          thisWindow->width   = width;
          thisWindow->resized = true;
       });

   if (window->pWindow == nullptr)
   {
      BfSingleEvent event{};
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_GLFW_WINDOW_FAILURE;
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

      return BfEvent(event);
   }
   else
   {
      BfSingleEvent event{};
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_GLFW_WINDOW_SUCCESS;
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

      return BfEvent(event);
   }
}

void bfSetOrthoUp(BfWindow* window)
{
   window->pitch = 89.0f;

   bfCalculateViewPartsFree(window);
}

void bfSetOrthoBottom(BfWindow* window)
{
   window->pitch = -89.0f;

   bfCalculateViewPartsFree(window);
}

void bfSetOrthoRight(BfWindow* window)
{
   window->yaw = -90.0f;
   bfCalculateViewPartsFree(window);
}

void bfSetOrthoLeft(BfWindow* window)
{
   window->yaw = 90.0f;
   bfCalculateViewPartsFree(window);
}

void bfSetOrthoNear(BfWindow* window)
{
   window->yaw = 0.0f;
   bfCalculateViewPartsFree(window);
}

void bfSetOrthoFar(BfWindow* window)
{
   window->yaw = 180.0f;
   bfCalculateViewPartsFree(window);
}

BfEvent bfSetWindowSize(BfWindow* window, int width, int height)
{
   BfSingleEvent event{};
   if (width <= 0 or height <= 0)
   {
      event.action = BfEnActionType::BF_ACTION_TYPE_SET_WINDOW_SIZE_FAILURE;
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

      std::stringstream ss;
      ss << " Tried width = " << width << ", height = " << height;
      event.info = ss.str();
   }
   else
   {
      window->width  = width;
      window->height = height;

      event.action   = BfEnActionType::BF_ACTION_TYPE_SET_WINDOW_SIZE_SUCCESS;
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   }

   return BfEvent(event);
}

BfEvent bfSetWindowName(BfWindow* window, std::string name)
{
   window->name = name;

   BfSingleEvent event{};
   event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   event.action = BfEnActionType::BF_ACTION_TYPE_SET_WINDOW_NAME;
   return BfEvent(event);
}

BfEvent bfBindGuiWindowHoveringFunction(BfWindow*             window,
                                        std::function<bool()> func)
{
   window->is_any_window_hovered_func = func;
   return BfEvent();
}
