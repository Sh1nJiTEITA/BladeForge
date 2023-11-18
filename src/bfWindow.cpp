#include "bfWindow.h"

/*
GLFWwindow* BfWindow::pWindow = nullptr;
std::string BfWindow::name = "";
int BfWindow::width = 0;
int BfWindow::height = 0;
bool BfWindow::resized = false;
*/


void bfCalculateView(BfWindow* window)
{

	static double lastX = 0;
	static double lastY = 0;
	
	

	if (window->firstMouse) {
		window->firstMouse = false;
		lastX = window->xpos;
		lastY = window->ypos;
	}


	float xoffset = window->xpos - lastX;
	float yoffset = lastY - window->ypos;  

	

	double sensitivity = 0.1f;

	xoffset *= sensitivity;
	yoffset *= sensitivity;
	
	static double yaw = -925.6;
	static double pitch = 44.3;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// std::cout << "y: " << yaw << ", p: " << pitch << "\n";

	window->front = glm::normalize(glm::vec3(
		glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)),
		glm::sin(glm::radians(pitch)),
		glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch))
	));

	window->view = glm::lookAt(window->pos, window->pos + window->front, window->up);

	lastX = window->xpos;
	lastY = window->ypos;
	/*std::cout << "pos:(" << window->pos.x << ", " << window->pos.y << ", " << window->pos.z
		<< "); front:(" << window->front.x << ", " << window->front.y << ", " << window->front.z
		<< "); up:(" << window->up.x << ", " << window->up.y << ", " << window->up.z << ")\n";*/
}

BfEvent bfCreateWindow(BfWindow* window) {
	glfwInit();
	bfvSetGLFWProperties();

	window->pos = { 2.0f, -2.0f, -1.0f};
	
	window->front = glm::normalize(glm::vec3(
		glm::cos(glm::radians(-925.6)) * glm::cos(glm::radians(44.3)),
		glm::sin(glm::radians(44.3)),
		glm::sin(glm::radians(-925.6)) * glm::cos(glm::radians(44.3))
	));
	window->up = { 0.0f, 1.0f, 0.0f };
	bfCalculateView(window);
	
	window->is_free_camera_active = false;

	window->pWindow = glfwCreateWindow(
		window->width,
		window->height,
		window->name.c_str(),
		nullptr, 
		nullptr
	);
	
	glfwSetWindowUserPointer(window->pWindow, window);
	glfwSetCursorPosCallback(window->pWindow, 
		[](GLFWwindow* window, double xpos, double ypos) {
			BfWindow* thisWindow = static_cast<BfWindow*>(glfwGetWindowUserPointer(window));
			thisWindow->xpos = xpos;
			thisWindow->ypos = ypos;
			//std::cout << "cb: " << thisWindow->xpos << ", " << thisWindow->ypos << "\n";
		});
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

