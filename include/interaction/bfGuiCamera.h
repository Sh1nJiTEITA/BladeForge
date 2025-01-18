#ifndef BF_GUI_CAMERA_H
#define BF_GUI_CAMERA_H

#include "bfCamera.h"
#include "bfGuiWindow.h"

class BfGuiCameraWindow : public BfGuiWindow
{
protected:
   virtual void __preRender() override;
   virtual void __bodyRender() override;
   virtual void __postRender() override;

public:
   BfGuiCameraWindow();
};

#endif
