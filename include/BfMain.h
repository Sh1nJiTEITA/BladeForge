#ifndef BF_MAIN_H
#define BF_MAIN_H

#include "bfBase.h"
#include "bfCurves.hpp"

#define BF_APP_NAME "BladeForge"
#define BF_APP_VERSION "0.001"
#define BF_START_W 800
#define BF_START_H 600



class BfMain {
private:
	
	BfBase __base;
	BfHolder __holder;
	BfGeometryHolder __geometry_holder;
	



	void __process_keys();

	void __init();
	void __start_loop();
	void __kill();

	// Presents // 
	void __present_vertices(BfMeshHandler* handler);
	void __present_info(double currentTime);
	void __present_camera();

public:
	BfMain();
	void run();



};




#endif