#include "bfWindow.h"

GLFWwindow* BfWindow::pWindow = nullptr;
std::string BfWindow::name = "";
int BfWindow::width = 0;
int BfWindow::height = 0;
bool BfWindow::resized = false;


BfEvent bfCreateWindow() {
	glfwInit();
	bfvSetGLFWProperties();
	
	BfWindow::pWindow = glfwCreateWindow(
		BfWindow::width, 
		BfWindow::height, 
		BfWindow::name.c_str(),
		nullptr, 
		nullptr
	);
	
	glfwSetFramebufferSizeCallback(
		BfWindow::pWindow, 
		[](GLFWwindow* window, int width, int height){
			BfWindow::height = height;
			BfWindow::width = width;
			BfWindow::resized = true;
		}
	);

	if (BfWindow::pWindow == nullptr) {
		BfSingleEvent event{};
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_GLFW_WINDOW_FAILURE;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		
		return BfEvent(event);
	}
	else {
		BfSingleEvent event{};
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_GLFW_WINDOW_SUCCESS;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

		return BfEvent(event);
	}

}

BfEvent bfSetWindowSize(int width, int height)
{
	BfSingleEvent event{};
	if (width <= 0 or height <= 0) {
		event.action = BfEnActionType::BF_ACTION_TYPE_SET_WINDOW_SIZE_FAILURE;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		
		std::stringstream ss;
		ss << " Tried width = " << width << ", height = " << height;
		event.info = ss.str();
	}
	else {
		BfWindow::width = width;
		BfWindow::height = height;

		event.action = BfEnActionType::BF_ACTION_TYPE_SET_WINDOW_SIZE_SUCCESS;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
	}
	
	return BfEvent(event);
}

