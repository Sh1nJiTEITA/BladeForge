#include "bfBase.h"

/*
BfWindow*				 BfBase::window			 = nullptr;
VkInstance				 BfBase::instance		 = VK_NULL_HANDLE;
VkSurfaceKHR			 BfBase::surface		 = VK_NULL_HANDLE;
BfPhysicalDevice*		 BfBase::physical_device = nullptr;
VkDevice				 BfBase::device			 = VK_NULL_HANDLE;
VkSwapchainKHR			 BfBase::swap_chain		 = VK_NULL_HANDLE;
VkDebugUtilsMessengerEXT BfBase::debug_messenger = VK_NULL_HANDLE;
*/

// Function definitions
BfEvent bfCreateInstance(BfBase& base)
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

	VkResult is_instance_created = vkCreateInstance(&instanceInfo, nullptr, &base.instance);
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

BfEvent bfCreateDebugMessenger(BfBase& base)
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

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(base.instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(base.instance, &createInfo, nullptr, &base.debug_messenger);
		
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

BfEvent bfCreateSurface(BfBase& base)
{
	BfSingleEvent event{};
	if (glfwCreateWindowSurface(base.instance, base.window->pWindow, nullptr, &base.surface) != VK_SUCCESS) {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_SURFACE_FAILURE;
	}
	else
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_SURFACE_SUCCESS;
	}
	
	return BfEvent(event);
}

BfEvent bfCreatePhysicalDevice(BfBase& base)
{
	
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(base.instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		BfSingleEvent event{};
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_NO_GPU;
		return BfEvent(event);
	}
		
	// Else:
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(base.instance, &deviceCount, devices.data());

	

	// Check devices for suitablity
	for (const auto& device : devices) {
		BfPhysicalDevice bf_device{};
		bf_device.physical_device = device;
		
		bfPhysicalDeviceHolder.push_back(bf_device);
		BfPhysicalDevice* pPhysicalDevice = &bfPhysicalDeviceHolder.back();

		bool is_suitable;
		bfIsDeviceSuitable(pPhysicalDevice, base.surface, is_suitable);

		if (is_suitable) {
			base.physical_device = pPhysicalDevice;
			break;
		}
	}

	if (base.physical_device->physical_device == VK_NULL_HANDLE) {
		BfSingleEvent event{};
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_FAILURE;
		return BfEvent(event);
	} 
	else {
		BfSingleEvent event{};
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_SUCCESS;
		return BfEvent(event);
	}

}

BfEvent bfCreateLogicalDevice(BfBase& base)
{
	
	base.physical_device->queue_family_indices;
	
	std::set<uint32_t> uniqueQueueFamilies;
	std::transform(
		base.physical_device->queue_family_indices.cbegin(), 
		base.physical_device->queue_family_indices.cend(),
		std::inserter(uniqueQueueFamilies, uniqueQueueFamilies.begin()),
		[](const std::pair<BfvEnQueueType, std::optional<uint32_t>>& key_value) 
		{ return key_value.second.value(); }
	);


	float queuePriority = 1.0f;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	// SWAP-CHAIN extensions (and others)
	createInfo.enabledExtensionCount = static_cast<uint32_t>(bfvDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = bfvDeviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(bfvValidationLayers.size());
		createInfo.ppEnabledLayerNames = bfvValidationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	BfSingleEvent event{};
	if (vkCreateDevice(base.physical_device->physical_device, &createInfo, nullptr, &base.device) != VK_SUCCESS) {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_LOGICAL_DEVICE_FAILURE;
	}
	else {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_LOGICAL_DEVICE_SUCCESS;
	}

	for (auto& it : base.physical_device->queue_family_indices) {
		vkGetDeviceQueue(
			base.device, 
			it.second.value(), 
			0, 
			&base.physical_device->queues[it.first]
		);
	}
	return BfEvent(event);
}

BfEvent bfCreateSwapchain(BfBase& base)
{
	BfSwapChainSupport swapChainSupport; 
	bfGetSwapChainSupport(base.physical_device->physical_device, base.surface, swapChainSupport);

	VkSurfaceFormatKHR surfaceFormat;
	bfGetSwapSurfaceFormat(swapChainSupport, surfaceFormat);
	
	VkPresentModeKHR presentMode;
	bfGetSwapPresentMode(swapChainSupport, presentMode);
	
	VkExtent2D extent; 
	bfGetSwapExtent(swapChainSupport, base.window->pWindow, extent);
	
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (
		swapChainSupport.capabilities.maxImageCount > 0 
		&& imageCount > swapChainSupport.capabilities.maxImageCount
	) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = base.surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	/*
	uint32_t queueFamilyIndices[] = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};
	*/
	std::vector<uint32_t> queueFamilyIndices;
	for (auto& it : base.physical_device->queue_family_indices) {
		queueFamilyIndices.push_back(it.second.value());
	}


	//if (indices.graphicsFamily != indices.presentFamily) {
	if (base.physical_device->queue_family_indices[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE].value() !=
		base.physical_device->queue_family_indices[BfvEnQueueType::BF_QUEUE_PRESENT_TYPE].value() !=
		base.physical_device->queue_family_indices[BfvEnQueueType::BF_QUEUE_TRANSFER_TYPE].value()
	) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 3;
		createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
	} 
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		//createInfo.queueFamilyIndexCount = 0;
		//createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(base.device, &createInfo, nullptr, &base.swap_chain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	return BfEvent();
	/*vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;*/
}


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

