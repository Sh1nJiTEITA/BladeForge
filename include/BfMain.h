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
#include "bfConsole.h"
#include "bfCurves.hpp"
#include "bfCurves2.h"
#include "bfCurves3.h"
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
   BfBase   __base;
   BfHolder __holder;
   BfGui    __gui;

   BfDrawLayer *__blade_bases = nullptr;
   BfDrawLayer *__other_layer = nullptr;

   void __process_keys();
   void __poll_events();

   void __init();
   void __start_loop();
   void __kill();

   // Presents //
   void __present_tooltype();
   void __present_info();
   void __present_id_map(BfBase &base, std::vector<uint32_t> image_data_);

   // void __present_blade_section_create_window();
   void __present_blade_base_create_window();

public:
   BfMain();
   void run();
};

#endif
