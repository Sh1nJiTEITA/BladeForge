#ifndef BF_MAIN_H
#define BF_MAIN_H

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#include "bfBase.h"
#include "bfCurves.hpp"
#include "bfCurves2.h"
#include "bfGUI.h"
#include "bfCurves3.h"

#define BF_APP_NAME "BladeForge"
#define BF_APP_VERSION "0.001"
#define BF_START_W 1600
#define BF_START_H 1024




class BfMain {
private:
	
	BfBase __base;
	BfHolder __holder;
	BfGeometryHolder __geometry_holder;
	BfGUI __gui;



	void __process_keys();

	void __init();
	void __start_loop();
	void __kill();

	// Presents // 
	void __present_vertices(BfMeshHandler* handler);
	void __present_info(double currentTime, uint32_t id_map);
	void __present_camera();
	void __present_menu_bar();
	void __present_id_map(BfBase& base, std::vector<uint32_t> image_data_);

public:
	BfMain();
	void run();



};




#endif