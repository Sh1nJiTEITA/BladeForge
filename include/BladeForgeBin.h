/*
* Auther: sNj
* Time Creation: 07.10.2023
*
*
* Description:
* Main part of application uses Vulkan API for rendering graphics.
* Class contains fundamental tools to interact with vk.
*
*/


#ifndef VULKAN_APP_H
#define VULKAN_APP_H

// Vulkan binaries
#define GLFW_INCLUDE_VULKAN
//#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

// IO-console output
#include <iostream>
#include <vector>
#include <cstring>

// Standart exception
#include <stdexcept>

// Main-function return codes
#include <cstdlib>

// Store validation layers here:
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

// Enable additional debug info.
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger
);

void DestroyDebugUtilsMessengerEXT(
	VkInstance instance, 
	VkDebugUtilsMessengerEXT debugMessenger, 
	const VkAllocationCallbacks* pAllocator
);



/*
* ������� ����, ���� ��� ����� ����������.
*/
const int WWIDTH = 800;
const int WHEIGHT = 600;



class BladeForge
{
public:

	/*
		Starts application.
	*/
	void run();


private:
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~ VARIABLES ~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	/*
	* Handles window.
	*/
	GLFWwindow* window;

	/*
	* VK-connection handler.
	*/
	VkInstance instance;

	/*
	* Debug Messenger.
	*/
	VkDebugUtilsMessengerEXT debugMessenger;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~ METHODS ~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	/*
	* Initialize Vulkan parts.
	*/
	void initVulkan();


	void setupDebugMessenger();

	/*
	* Initialize GLFW-window parts.
	*/
	void initWindow();
	
	/*
	* Render loop.
	*/
	void mainLoop();
	
	/*
	* Terminates applicaion: stops all process and cleans memory.
	*/
	void cleanup();


	/*
	* ������� Instance - ����� ����� ����������� � ���������� ����������.
	*/
	void createInstance();

	/*
	* �������� ����������������� �����
	*/
	bool checkValidationLayersSupport();

	/*
	* ���������� ������ ���������� ���������� ���������� �� ���������� �����
	*/
	std::vector<const char*> getRequiredExtensions();

	/*
	* Vk-messages printer to console
	*/
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		// ��������� ���������
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		/*
			������: 
			1. VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT - ��� ����������� ����� ����;
			2. VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT - ������������� ���������, ��� ���
			�������� �������;
			3. VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT - ��������� � ������������� ����,
			�� �� ������;
			4. VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT - invalid-behavior ��� �����;
		*/
		// ��� ���������
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		/*
			������:
			1. VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT - �����-�� event ���������, ��������
			�� ������ ���� ����;
			2. VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT - ���-�� ���������, ��� ��������
			������������ ��� ������� � ��������� ������;
			3. VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT - ������������ ����������� ������-
			������� VK;
		
		*/
		// ������ ���������
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		/*
			������� ��:
			1. pMessage(cstring) - debug message;
			2. pObjects - ������ �������� VK, ����������� � ���������;
			3. objectCount - ���������� �������� � ������� ����.

		*/
		// ���������������� ����������
		void* pUserData
	);

	void populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

};









#endif

