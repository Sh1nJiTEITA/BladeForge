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

// STL
#include <iostream>
#include <vector>
#include <cstring>
#include <stdexcept> // Standart exception
#include <cstdlib> // Main-function return codes
#include <optional>

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


// Creates debug-messenger
VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger
);

// Destroys debug-messenger
void DestroyDebugUtilsMessengerEXT(
	VkInstance instance, 
	VkDebugUtilsMessengerEXT debugMessenger, 
	const VkAllocationCallbacks* pAllocator
);




struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() {
		return graphicsFamily.has_value();
	}
};





/*
* Размеры окна, пока что будут константой.
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
	/* Handles window.*/
	GLFWwindow* window;

	/* VK-connection handler. */
	VkInstance instance;

	/* Debug Messenger. */
	VkDebugUtilsMessengerEXT debugMessenger;

	/* Praphics-card Handler */
	VkPhysicalDevice physicalDevice;

	/* Logical device */
	VkDevice device;

	VkQueue graphicsQueue;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~ METHODS ~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~ MAINS ~~~~~~~~~~~~~~
	/* Render loop. */
	void mainLoop();

	/* Terminates applicaion: stops all process and cleans memory. */
	void cleanup();

// ~~~~~~~~~~ SET UP ~~~~~~~~~~~~~

	/* Initialize Vulkan parts. */
	void initVulkan();

	/* ... */
	void setupDebugMessenger();

	/* Initialize GLFW-window parts. */
	void initWindow();
	
	/* ... */
	void createInstance();

	void createLogicalDevice();

// ~~~~~~~~~ QUEUE FAMALY & PHYSICAL DEVICE ~~~~~
	/* Finds QueueFamilies supported by device */
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	/* Choose graphics card */
	void pickPhysicalDevice();
	
	/* Checks if graphics card is suitable with vk */
	bool isDeviceSuitable(VkPhysicalDevice device);


	// ~~~~~~~~~~ DEBUG HELP ~~~~~~~~~
	/* Check if debug-layers is supported */
	bool checkValidationLayersSupport();

	/* Provides list of needed extetnsions */
	std::vector<const char*> getRequiredExtensions();

	/* Messanger-info filler */
	void populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	/* Vk-messages printer to console */
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		// Строгость сообщения
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		/*
			Бывают: 
			1. VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT - при диагностике много слов;
			2. VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT - информативное сообщение, как при
			создании ресурса;
			3. VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT - сообщение о потенциальном баге,
			НЕ об ошибке;
			4. VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT - invalid-behavior или крашы;
		*/
		// Тип сообщения
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		/*
			Бывают:
			1. VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT - какой-то event произошел, которого
			не должно было быть;
			2. VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT - что-то произошло, что нарушает
			спецификацию или говорит о возможной ошибке;
			3. VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT - потенциально неоптиманое исполь-
			зование VK;
		
		*/
		// Детали сообщения
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		/*
			Состоит из:
			1. pMessage(cstring) - debug message;
			2. pObjects - массив объектов VK, относящихся к сообщению;
			3. objectCount - количество объектов в массиве выше.

		*/
		// Пользовательская информация
		void* pUserData
	);

	
};




#endif