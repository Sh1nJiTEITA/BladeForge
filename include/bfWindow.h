#ifndef BF_WINDOW_H
#define BF_WINDOW_H

#include "bfVariative.hpp"
#include "bfEvent.h"





struct BfWindow {
	GLFWwindow* pWindow;
	std::string name;
	int width;
	int height;
	bool resized;

	int holder_index;
};

BfEvent bfCreateWindow(BfWindow* window);
BfEvent bfSetWindowSize(BfWindow* window, int width, int height);
BfEvent bfSetWindowName(BfWindow* window, std::string name);

#endif