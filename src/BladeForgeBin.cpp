#include "BladeForgeBin.h"

void BladeForge::run()
{
	initVulkan();
	mainLoop();
	cleanup();
}


void BladeForge::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	pickPhysicalDevice();
}

void BladeForge::setupDebugMessenger()
{
	// If layer is disabled, то DebugMessenger is not neccesery
	if (!enableValidationLayers) return;

	// Info about mess
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to set up debug messenger");
	}

}


void BladeForge::pickPhysicalDevice()
{
	physicalDevice = VK_NULL_HANDLE;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	// If no GPU's
	if (deviceCount == 0)
		throw std::runtime_error("No GPU with Vulkan support detected :/");
	// Else:
	std::vector<VkPhysicalDevice> devices(deviceCount);
	
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	// Check devices for suitablity
	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("There are some devices with Vulkan support, but no suitable.");
	}

}


// TODO Device selection
bool BladeForge::isDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);



	return true;
}


void BladeForge::initWindow()
{
	
	// Инициализация GLFW.
	glfwInit();
	
	/*
	* Так как основное API - vk, то необходимо выключить
	* встроенное подключение к OpenGL.
	*/
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// Размеры окна меняться не будут.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WWIDTH, WHEIGHT, "BladeForge", nullptr, nullptr);

}

void BladeForge::mainLoop()
{
	// Main Render Loop.
	while (!glfwWindowShouldClose(this->window))
	{
		glfwPollEvents();
	}

}

void BladeForge::cleanup()
{
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	
	glfwTerminate();
}

void BladeForge::createInstance()
{
	if (enableValidationLayers)
	{
		std::cout << "::DEBUG MODE::" << std::endl;
	}
	/* 
	* Проверяем, доступность всех подключенных слоев:
	*/
	if (enableValidationLayers && !checkValidationLayersSupport())
	{
		throw std::runtime_error("Validation layers requested, but not available.");
	}

	
	/*
	* Необходимо заполнить информацию о приложении,
	* так как это может послужить инструкцией 
	* графическим драйверам, что может улучшить дальнейшую
	* работу.
	*/
	VkApplicationInfo appInfo{};
	
	// Тип заполняемой информации
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	
	// Название приложения
	appInfo.pApplicationName = "BladeForge";
	
	// Версия приложения (Вулкана), стандартная, поддерживается везде
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	
	// Название Движка *(пока что нету)*
	appInfo.pEngineName = nullptr;
	
	// Версия движка, так как его нет, будет такой же как и версия Вулкана
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	
	// Версия API, под которую написано приложение
	appInfo.apiVersion = VK_API_VERSION_1_0;


	/*
	* Заполнение информации об Instance.
	*/
	VkInstanceCreateInfo instanceInfo{};
	// Тип информации
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	
	// Информация о приложении
	instanceInfo.pApplicationInfo = &appInfo;

	// Указание всех дополнений
	auto extensions = getRequiredExtensions();
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceInfo.ppEnabledExtensionNames = extensions.data();


	// Слои (ЕСЛИ ВКЛЮЧЕНЫЕ ИЗНАЧАЛЬНО)
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{}; // Для проверки при создании Instance
	if (enableValidationLayers)
	{
		// Количество слоев
		instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());

		// Массив на названия включенных (и найденных при проверке) слоев
		instanceInfo.ppEnabledLayerNames = validationLayers.data();

		populateMessengerCreateInfo(debugCreateInfo);
		instanceInfo.pNext = (VkDebugUtilsMessengerEXT*)&debugCreateInfo;
	}
	else // Иначе слоев нет.
	{
		instanceInfo.enabledLayerCount = 0;

		instanceInfo.pNext = nullptr;
	}

	


	// Создание Instance
	if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Vulkan instance");
	}

	std::cout << "Vulkan initialization is done." << std::endl;
}

bool BladeForge::checkValidationLayersSupport()
{
	// Число включенных слоев
	uint32_t layerCount;
	// Получить число включенных слоев
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	// Массив для хранения свойств всех доступных слоев
	std::vector<VkLayerProperties> avaliableLayers(layerCount);
	
	// Заполняем массив слоев информацией
	vkEnumerateInstanceLayerProperties(&layerCount, avaliableLayers.data());

	std::cout << "Checking validation layers support. "
		<< "Available Layers:\n";
	int i = 0;
	for (const auto& layer : avaliableLayers)
	{
		std::cout << i << ". " << layer.layerName << '\n';
		i++;
	}
	std::cout << std::endl;

	std::cout << "Required layers:\n";

	// Проверяем, все ли доступные слои совпадают с необходимыми
	
	i = 0;
	for (auto& vLayer : validationLayers)
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
			return false;
		}
		
	}


	std::cout << "Validation layer support is enabled and works fine." << std::endl;

	return true;
}

std::vector<const char*> BladeForge::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	//std::vector<const char*> extensions(glfwExtensions, glfwExternsions + glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL BladeForge::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
	VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
	void* pUserData
) 
{
	char* severity_type{};
	switch (messageSeverity)
	{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: 
			severity_type = const_cast<char*>("[INFO]"); break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: 
			severity_type = const_cast<char*>("[VERB]"); break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			severity_type = const_cast<char*>("[WARN]"); break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			severity_type = const_cast<char*>("[ERR]"); break;
	}
	
	std::cerr << severity_type << " Validation layer: " << pCallbackData->pMessage << std::endl;


	return VK_FALSE;
}

void BladeForge::populateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	if (!enableValidationLayers) return;
	
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = {
		//VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |		// Just info (e.g. creation info)
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |	// Diagnostic info
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |	// Warning (bug)
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT		// Warning (potential crush)
	};
	createInfo.messageType = {
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |		// Unrelated perfomance (mb ok)
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |	// Possiable mistake (not ok)
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT		// Potential non-optimal use of VK
	};
	createInfo.pfnUserCallback = debugCallback;
	//createInfo.pUserData = nullptr; // Optional
}


VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance, 
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
	const VkAllocationCallbacks* pAllocator, 
	VkDebugUtilsMessengerEXT* pDebugMessenger
) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(
	VkInstance instance, 
	VkDebugUtilsMessengerEXT debugMessenger, 
	const VkAllocationCallbacks* pAllocator
) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}
