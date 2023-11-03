#ifndef BF_WINDOW_H
#define BF_WINDOW_H

#include "bfVariative.hpp"
#include "bfEvent.h"


struct BfWindow {
	static GLFWwindow* pWindow;
	static std::string name;
	static int width;
	static int height;
	static bool resized;
};

BfEvent bfCreateWindow();
BfEvent bfSetWindowSize(int width, int height);

#endif