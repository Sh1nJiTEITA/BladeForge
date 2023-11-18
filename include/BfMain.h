#ifndef BF_MAIN_H
#define BF_MAIN_H

#include "bfBase.h"

#define BF_APP_NAME "BladeForge"
#define BF_APP_VERSION "0.001"
#define BF_START_W 800
#define BF_START_H 600



class BfMain {
private:
	
	BfBase __base;
	BfHolder __holder;
	
	



	void __process_keys();

	void __init();
	void __start_loop();
	void __kill();
	
public:
	BfMain();
	void run();



};




#endif