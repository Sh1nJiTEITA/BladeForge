#include "bfBase.h"

BfWindow*				 BfBase::window			 = nullptr;
VkInstance				 BfBase::instance		 = VK_NULL_HANDLE;
VkSurfaceKHR			 BfBase::surface		 = VK_NULL_HANDLE;
VkPhysicalDevice		 BfBase::physical_device = VK_NULL_HANDLE;
VkDevice				 BfBase::device			 = VK_NULL_HANDLE;
VkSwapchainKHR			 BfBase::swap_chain		 = VK_NULL_HANDLE;
VkDebugUtilsMessengerEXT BfBase::debug_messenger = VK_NULL_HANDLE;


// Function definitions
BfEvent bfCreateInstance()
{
	if (enableValidationLayers)
	{
		std::cout << "::DEBUG MODE::" << std::endl;
	}
	
	// Check Validation layers support
	bool is_validation_layers_supported;

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> avaliableLayers(layerCount);


	vkEnumerateInstanceLayerProperties(&layerCount, avaliableLayers.data());

	//std::cout << "Checking validation layers support. " << "Available Layers:\n";
	int i = 0;
	for (const auto& layer : avaliableLayers)
	{
		std::cout << i << ". " << layer.layerName << '\n';
		i++;
	}
	//std::cout << std::endl;

	//std::cout << "Required layers:\n";

	i = 0;
	is_validation_layers_supported = true;
	for (auto& vLayer : bfvValidationLayers)
	{
		bool isLayer = false;
		std::cout << i << ") " << vLayer << '\n';

		for (const auto& layerProperties : avaliableLayers)
		{
			if (strcmp(vLayer, layerProperties.layerName) == 0)
			{
				isLayer = true;
				break;
			}
		}

		if (!isLayer)
		{
			is_validation_layers_supported = false;
			break;
			
		}
	}


	if (enableValidationLayers && !is_validation_layers_supported)
	{
		throw std::runtime_error("Validation layers requested, but not available.");
	}
	// Get Required Extensions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//std::vector<const char*> extensions(glfwExtensions, glfwExternsions + glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	
	// Create VkInstance
	VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	appInfo.pApplicationName = "BladeForge";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = nullptr;
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;


	VkInstanceCreateInfo instanceInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceInfo.ppEnabledExtensionNames = extensions.data();



	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{}; 
	if (enableValidationLayers)
	{
	
		instanceInfo.enabledLayerCount = static_cast<uint32_t>(bfvValidationLayers.size());

	
		instanceInfo.ppEnabledLayerNames = bfvValidationLayers.data();

		bfPopulateMessengerCreateInfo(debugCreateInfo);
		instanceInfo.pNext = (VkDebugUtilsMessengerEXT*)&debugCreateInfo;
	}
	else 
	{
		instanceInfo.enabledLayerCount = 0;

		instanceInfo.pNext = nullptr;
	}

	VkResult is_instance_created = vkCreateInstance(&instanceInfo, nullptr, &BfBase::instance);
	if (is_instance_created != VK_SUCCESS)
	{
		BfSingleEvent event{};
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_INSTANCE_FAILURE;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

		return BfEvent(event);
	}
	else
	{
		BfSingleEvent event{};
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_INSTANCE_SUCCESS;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

		return BfEvent(event);
	}
}

BfEvent bfCreateDebugMessenger()
{
	// If layer is disabled, то DebugMessenger is not neccesery
	if (!enableValidationLayers) {
		BfSingleEvent event{};
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_NO_INIT;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		return BfEvent(event);
	}

	// Info about mess
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	bfPopulateMessengerCreateInfo(createInfo);

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(BfBase::instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(BfBase::instance, &createInfo, nullptr, &BfBase::debug_messenger);
		
		BfSingleEvent event{};
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_SUCCESS;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		
		return BfEvent(event);
	}
	else {
		BfSingleEvent event{};
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_FAILURE;
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

		return BfEvent(event);
	}
}

BfEvent bfCreateSurface()
{
	if (glfwCreateWindowSurface(BfBase::instance, BfBase::window->pWindow, nullptr, &BfBase::surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
	
	return BfEvent();
}

//BfEvent bfCreatePhysicalDevice()
//{
//	
//}

void bfPopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	if (!enableValidationLayers) return;

	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = {
		//VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |	// Just info (e.g. creation info)
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |	// Diagnostic info
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |	// Warning (bug)
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT		// Warning (potential crush)
	};
	createInfo.messageType = {
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |		// Unrelated perfomance (mb ok)
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |	// Possiable mistake (not ok)
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT		// Potential non-optimal use of VK
	};
	createInfo.pfnUserCallback = bfvDebugCallback;
}
