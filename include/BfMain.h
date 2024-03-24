#ifndef BF_MAIN_H
#define BF_MAIN_H

//#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include <stb_image.h>
#include <stb_image_write.h>

#include "bfBase.h"
#include "bfCurves.hpp"
#include "bfCurves2.h"
#include "bfGUI.h"
#include "bfCurves3.h"
#include "bfConsole.h"
#include "bfEvent.h"
#include "bfBladeSection.h"

#include <random>

#define BF_APP_NAME "BladeForge"
#define BF_APP_VERSION "0.001"
#define BF_START_W 1000
#define BF_START_H 800




class BfMain {
private:
	
	BfBase __base;
	BfHolder __holder;
	BfGeometryHolder __geometry_holder;
	BfGUI __gui;


	void __process_keys();
	void __poll_events();

	void __init();
	void __start_loop();
	void __kill();

	// Presents // 
	void __present_tooltype();
	void __present_info();
	void __present_camera();
	void __present_menu_bar();
	void __present_id_map(BfBase& base, std::vector<uint32_t> image_data_);
	void __present_event_log();

public:
	BfMain();
	void run();



};




#endif