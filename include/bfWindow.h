#ifndef BF_WINDOW_H
#define BF_WINDOW_H

#include "bfVariative.hpp"
#include "bfEvent.h"




#ifndef BF_CLASS_BFWINDOW
#define BF_CLASS_BFSINDOW
struct BfWindow {
	GLFWwindow* pWindow;
	std::string name;
	
	double xpos;
	double ypos;
	
	glm::vec3 front;
	glm::vec3 pos;
	glm::vec3 up;

	glm::mat4 view;

	bool is_free_camera_active;
	bool firstMouse = true;

	int width;
	int height;
	bool resized;
	int holder_index;
};
#endif

void bfCalculateView(BfWindow* window);

BfEvent bfCreateWindow(BfWindow* window);
BfEvent bfSetWindowSize(BfWindow* window, int width, int height);
BfEvent bfSetWindowName(BfWindow* window, std::string name);

#endif