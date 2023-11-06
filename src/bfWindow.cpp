#include "bfWindow.h"

/*
GLFWwindow* BfWindow::pWindow = nullptr;
std::string BfWindow::name = "";
int BfWindow::width = 0;
int BfWindow::height = 0;
bool BfWindow::resized = false;
*/

BfEvent bfCreateWindow(BfWindow* window) {
	glfwInit();
	bfvSetGLFWProperties();

	window->pWindow = glfwCreateWindow(
		window->width,
		window->height,
		window->name.c_str(),
		nullptr, 
		nullptr
	);
	
	glfwSetWindowUserPointer(window->pWindow, &window);

	glfwSetFramebufferSizeCallback(
		window->pWindow,
		[](GLFWwindow* window, int width, int height){
			BfWindow* thisWindow = static_cast<BfWindow*>(glfwGetWindowUserPointer(window));
			
			thisWindow->height = height;
			thisWindow->width = width;
			thisWindow->resized = true;
		}
	);

	if (window->pWindow == nullptr) {
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

BfEvent bfSetWindowSize(BfWindow* window, int width, int height)
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
		window->width = width;
		window->height = height;

		event.action = BfEnActionType::BF_ACTION_TYPE_SET_WINDOW_SIZE_SUCCESS;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
	}
	
	return BfEvent(event);
}

BfEvent bfSetWindowName(BfWindow* window, std::string name)
{
	window->name = name;

	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
	event.action = BfEnActionType::BF_ACTION_TYPE_SET_WINDOW_NAME;
	return BfEvent(event);
}

