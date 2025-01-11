#ifndef BF_MAIN_H
#define BF_MAIN_H

// #define STB_IMAGE_IMPLEMENTATION
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
#include "bfTextureLoad.h"

#define BF_APP_NAME "BladeForge"
#define BF_APP_VERSION "0.001"
#define BF_START_W 1000
#define BF_START_H 800

class BfMain
{
private:
   BfBase __base;
   BfHolder __holder;
   BfGui __gui;
   BfCamera __cam;

   BfDrawLayer *__blade_bases = nullptr;
   BfDrawLayer *__other_layer = nullptr;

   void __process_keys();
   void __poll_events();

   void __init();
   void __loop();
   void __kill();

public:
   BfMain();
   void run();
};

#endif
