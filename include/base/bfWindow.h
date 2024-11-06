#ifndef BF_WINDOW_H
#define BF_WINDOW_H

// #define VMA_IMPLEMENTATION
#include <functional>

#include "bfEvent.h"
#include "bfVariative.hpp"

#define ZOOM_SENSITIVITY 0.1f
#define SCROLL_SENSITIVITY 0.1f

#ifndef BF_CLASS_BFWINDOW
#define BF_CLASS_BFSINDOW

struct BfWindow
{
   GLFWwindow *pWindow;
   std::string name;

   double xpos;
   double ypos;

   inline static double lastX = 0;
   inline static double lastY = 0;

   // inline static double lastRx = 0;
   // inline static double lastRy = 0;

   double scroll_xoffset = 0;
   double scroll_yoffset = 0;

   double yaw;    // = -925.6f;
   double pitch;  // = 44.3f;

   float cam_radius;

   glm::vec3 front;
   glm::vec3 pos;
   glm::vec3 up;

   glm::mat4 proj;
   glm::mat4 view;

   float ortho_scale = 1.0f;
   //
   std::function<bool()> is_any_window_hovered_func = nullptr;
   //
   bool is_scroll               = false;

   bool is_free_camera_active   = false;
   bool first_free              = true;

   bool is_s_camera_active      = false;
   bool first_s                 = false;

   bool is_rotate_camera_active = false;
   bool first_free_rotate       = true;
   bool first_s_rotate          = true;

   uint32_t cam_mode            = -1;

   uint32_t proj_mode;

   float ortho_left   = 0.0f;
   float ortho_right  = 1.0f;
   float ortho_bottom = 0.0f;
   float ortho_top    = 1.0f;
   float ortho_near   = 0.1f;
   float ortho_far    = 100.0f;
   bool  is_asp       = true;

   int  width;
   int  height;
   bool resized;
   int  holder_index;
};
#endif

void bfToggleCamParts(BfWindow *window, uint32_t cam_index, bool decision);

void bfCalculateViewPartsFree(BfWindow *window);
void bfCalculateViewPartsS(BfWindow *window);
void bfCalculateRotateView(BfWindow *window);
void bfUpdateView(BfWindow *window);

void bfSetOrthoUp(BfWindow *window);
void bfSetOrthoBottom(BfWindow *window);
void bfSetOrthoRight(BfWindow *window);
void bfSetOrthoLeft(BfWindow *window);
void bfSetOrthoNear(BfWindow *window);
void bfSetOrthoFar(BfWindow *window);

BfEvent bfCreateWindow(BfWindow *window);
BfEvent bfSetWindowSize(BfWindow *window, int width, int height);
BfEvent bfSetWindowName(BfWindow *window, std::string name);
BfEvent bfBindGuiWindowHoveringFunction(BfWindow             *window,
                                        std::function<bool()> func);

#endif
