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
//#undef max
// Vulkan binaries

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



// STL
#include <iostream>
#include <vector>
#include <cstring>
#include <stdexcept> // Standart exception
#include <cstdlib> // Main-function return codes
#include <optional>
#include <set> // For queues
#include <string> // Device Extensions
#include <limits>
#include <algorithm>
#include <fstream> // Read shaderfiles
#include <sstream>
#include <chrono> // For time-stop rotation

// cocal
#include "bfVertex.h"

//const std::vector<bfVertex> vertices = {
//	{{0.0f, -0.5f}, {1.0f, 1.0f, 0.0f}},
//	{{0.5f, 0.5f}, {0.0f, 0.0f, 0.5f}},
//	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
//};

const std::vector<bfVertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};

//const std::vector<bfVertex> vertices2 = {
//	{{-0.7f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//	{{-0.2f, -0.5f}, {0.0f, 1.0f, 0.0f}},
//	{{-0.5f, -0.1f}, {0.0f, 1.0f, 1.0f}},
//};

//const std::vector<bfVertex> vertices = {
//	{{0.0, 0.0}, {1.0, 1.0, 1.0}},
//	{{0.05, 0.2}, {1.0, 1.0, 1.0}},
//	{{0.1, -0.3}, {1.0, 1.0, 1.0}},
//	{{0.15, 0.1}, {1.0, 1.0, 1.0}},
//	{{0.2, 0.4}, {1.0, 1.0, 1.0}},
//	{{0.25, -0.2}, {1.0, 1.0, 1.0}},
//	{{0.3, 0.0}, {1.0, 1.0, 1.0}},
//	{{0.35, 0.2}, {1.0, 1.0, 1.0}},
//	{{0.4, -0.4}, {1.0, 1.0, 1.0}},
//	{{0.45, -0.1}, {1.0, 1.0, 1.0}},
//	{{0.5, 0.3}, {1.0, 1.0, 1.0}},
//	{{0.55, 0.1}, {1.0, 1.0, 1.0}},
//	{{0.6, 0.2}, {1.0, 1.0, 1.0}},
//	{{0.65, -0.3}, {1.0, 1.0, 1.0}},
//	{{0.7, -0.1}, {1.0, 1.0, 1.0}},
//	{{0.75, 0.2}, {1.0, 1.0, 1.0}},
//	{{0.8, 0.4}, {1.0, 1.0, 1.0}},
//	{{0.85, -0.2}, {1.0, 1.0, 1.0}},
//	{{0.9, 0.1}, {1.0, 1.0, 1.0}},
//	{{0.95, -0.3}, {1.0, 1.0, 1.0}},
//	{{1.0, 0.0}, {1.0, 1.0, 1.0}}
//};
static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
};


// Store validation layers here:
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

// Store device extensions
const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	//GL_KHR_vulkan_glsl
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



const int MAX_FRAMES_IN_FLIGHT = 2;

std::vector<char> readFile(const std::string& filename);


struct SwapChainSupportDetails {
	// min/max number of images, min/max width height of images
	VkSurfaceCapabilitiesKHR capabilities;
	// Support formats: pixel/color space
	std::vector<VkSurfaceFormatKHR> formats;
	// ???
	std::vector<VkPresentModeKHR> presentModes;
};



struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
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
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~ VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	/* Handles window.*/
	GLFWwindow*	window;

	/* VK-connection handler. */
	VkInstance instance;

	/* Debug Messenger. */
	VkDebugUtilsMessengerEXT debugMessenger;

	/* Praphics-card Handler */
	VkPhysicalDevice physicalDevice;

	/* Logical device */
	VkDevice device;

	/* Queues */
	VkQueue	graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;

	VkSwapchainKHR		 swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat			 swapChainImageFormat;
	VkExtent2D			 swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

	// Pipeline / RenderPass / Subpass
	VkRenderPass	 renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline		 graphicsPipeline;

	// Uniforms
	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;

	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	// Framebuffers
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkFramebuffer> ImGUIFrameBuffers;

	// FrameBuffers
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	


	// Command pools
	VkCommandPool				 commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkCommandBuffer> ImGUIcommandBuffers;


	// SYNCHRONIZATION 
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence>	 inFlightFences;
	bool frameBufferResized = false;

	// ImGUI
	
	VkRenderPass imGuiRenderPass;
	uint32_t minImageCount;
	VkDescriptorPool imGuiDescriptorPool;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~ METHODS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~ MAINS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	/* Render loop. */
	void mainLoop();

	/* Terminates applicaion: stops all process and cleans memory. */
	void cleanup();

// ~~~~~~~~~~ SET UP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/* Initialize Vulkan parts. */
	void initVulkan();

	/* ... */
	void setupDebugMessenger();

	/* Initialize GLFW-window parts. */
	void initWindow();

	/* IMGUI */
	void initImGUI();
	void createImGUIRenderPass();
	void createImGUIDescriptorPool();
	void createImGUICommandBuffers();
	void createImGUIFrameBuffers();


	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	
	/* ... */
	void createInstance();

	void createLogicalDevice();

	void createSurface();

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
// ~~~~~~~~~ SWAP-CHAIN ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void createSwapChain();
	void createImageViews();
	void recreateSwapChain();
	void cleanupSwapChain();

// ~~~~~~~~~ RenderPass ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void createRenderPass();


// ~~~~~~~~~ GRAPHICS PIPELINE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void createGraphicsPipeline();
	VkShaderModule createShaderModule(const std::vector<char>& code);

// ~~~~~~~~~ UNIFORMS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void createDescriptorSetLayout();
	void createDescriptorPool();
	void createDescriptorSets();
	void createUniformBuffers();
	void updateUniformBuffer(uint32_t currentImage);


// ~~~~~~~~~ VERTEX BUFFERS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void createVertexbuffer();
	void createIndexBuffer();

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(VkDeviceSize size, 
					  VkBufferUsageFlags usage, 
					  VkMemoryPropertyFlags properties, 
					  VkBuffer& buffer, 
					  VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

// ~~~~~~~~~ FrameBuffers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void createFramebuffers();

// ~~~~~~~~~ COMMANDS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	void createCommandPool();
	void createCommandBuffers();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBuffer ImGUICommandBuffer, uint32_t imageIndex);

// ~~~~~~~~~ DRAW ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void drawFrame();
	uint32_t currentFrame = 0;

// ~~~~~~~~~ SYNCHRONIZATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void createSyncObjects();

// ~~~~~~~~~ QUEUE FAMALY & PHYSICAL ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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