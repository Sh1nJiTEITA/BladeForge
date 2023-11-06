#ifndef BF_WINDOW_H
#define BF_WINDOW_H

#include "bfVariative.hpp"
#include "bfEvent.h"




#ifndef BF_CLASS_BFWINDOW
#define BF_CLASS_BFSINDOW
struct BfWindow {
	GLFWwindow* pWindow;
	std::string name;
	int width;
	int height;
	bool resized;
	int holder_index;
};
#endif


BfEvent bfCreateWindow(BfWindow* window);
BfEvent bfSetWindowSize(BfWindow* window, int width, int height);
BfEvent bfSetWindowName(BfWindow* window, std::string name);

#endif