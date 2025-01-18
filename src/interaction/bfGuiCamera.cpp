#include "bfGuiCamera.h"

#include "bfCamera.h"
#include "imgui.h"

void
BfGuiCameraWindow::__preRender()
{
}

#define BGUICAMERAWINDOW_STRINGIFY(NAME) #NAME

void
BfGuiCameraWindow::__bodyRender()
{
   static auto choice = [](BfCameraMode mode) {
      if (ImGui::RadioButton(
              bfGetCameraModeStr(mode),
              BfCamera::instance()->m_mode == mode
          ))
      {
         BfCamera::instance()->setMode(mode);
      }
   };

   for (uint32_t mode = 0; mode < _BfCameraMode_End; ++mode)
   {
      choice(mode);
   }
}

void
BfGuiCameraWindow::__postRender()
{
}

BfGuiCameraWindow::BfGuiCameraWindow()
    : BfGuiWindow{BfGuiWindowCreateInfo{
          .name = "Camera",
          .isRender = true,
          .toggleRenderMapping = {ImGuiKey_LeftCtrl, ImGuiKey_C}
      }}
{
}
