#ifndef BF_MAIN_H
#define BF_MAIN_H

// #define STB_IMAGE_IMPLEMENTATION
#include "bfDrawObject2.h"
#include "bfGuiBody.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #include <stb_image.h>
#include <stb_image_write.h>

#include <random>

#include "bfBase.h"
#include "bfBladeSection.h"
#include "bfBuffer.h"
#include "bfCamera.h"
#include "bfConsole.h"
#include "bfEvent.h"
#include "bfGui.h"

#define BF_APP_NAME "BladeForge"
#define BF_APP_VERSION "0.001"
#define BF_START_W 1000
#define BF_START_H 800

class BfMain
{
private:
   BfBase m_base;
   BfHolder m_holder;
   BfGui m_gui;
   BfCamera m_cam;
   gui::MainDock m_dock;

   void _processKeys();
   void _pollEvents();
   void _processSelfInteraction();

   void _init();
   void _loop();
   void _kill();

public:
   BfMain();
   void run();
};

#endif
