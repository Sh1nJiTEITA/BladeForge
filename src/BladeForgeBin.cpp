#include "BladeForgeBin.h"

void BladeForge::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}


void BladeForge::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
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
	/*VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);*/

	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupport = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if (extensionsSupport) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}


	return indices.isComplete() && extensionsSupport && swapChainAdequate;
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
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}
	
	vkDestroySwapchainKHR(device, swapChain, nullptr);
	vkDestroyDevice(device, nullptr);
	
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	
	
	vkDestroySurfaceKHR(instance, surface, nullptr);
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

void BladeForge::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

	float queuePriority = 1.0f;
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
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device.");
	}

	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void BladeForge::createSurface()
{
	/*VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = glfwGetWin32Window(window);
	createInfo.hinstance = GetModuleHandle(nullptr);

	if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface");
	}*/
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

bool BladeForge::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionsCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, availableExtensions.data());
	
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	
	if (requiredExtensions.empty()) {
		std::cout << "\nChosen device has all required extensions.\n";
	}


	return requiredExtensions.empty();
}

SwapChainSupportDetails BladeForge::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}



	return details;
}

VkSurfaceFormatKHR BladeForge::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (
			availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && 
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR BladeForge::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& presentMode : availablePresentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D BladeForge::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	
	//size_t maxuint32_t = (std::numeric_limits<size_t>::max)();

	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height),
		};
		actualExtent.width = std::clamp(
			actualExtent.width, 
			capabilities.minImageExtent.width, 
			capabilities.maxImageExtent.width
		);
		actualExtent.height = std::clamp(
			actualExtent.height, 
			capabilities.minImageExtent.height, 
			capabilities.maxImageExtent.height
		);

		return actualExtent;
	}

}

void BladeForge::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
	
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (
		swapChainSupport.capabilities.maxImageCount > 0 
		&& imageCount > swapChainSupport.capabilities.maxImageCount
	) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
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

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

}

void BladeForge::createImageViews()
{
	// NUMBER OF IMAGES -> NUMBER OF VIEWS
	swapChainImageViews.resize(swapChainImages.size());

	// Go through all images
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		// Create single image-view inside loop-iteration
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];

		// 1D, 2D, 3D textures ...
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;

		// Color mapping
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// What image for?
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image views");
		}

	}

}

void BladeForge::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// What image/buffer to use?
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// How to use subPass
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("RenderPass wasn't created");
	}
}

void BladeForge::createGraphicsPipeline()
{
	// Shader Modules Creation
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	// Pipeline creation
	VkPipelineShaderStageCreateInfo vertShaderCreateInfo{};
	vertShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderCreateInfo.module = vertShaderModule;
	vertShaderCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderCreateInfo{};
	fragShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderCreateInfo.module = fragShaderModule;
	fragShaderCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertShaderCreateInfo,
		fragShaderCreateInfo
	};

	// Vertex Data
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

	// How to use vertex data
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	//// Viewports 
	//VkViewport viewport{};
	//viewport.x = 0.0f;
	//viewport.y = 0.0f;

	//viewport.width = (float)swapChainExtent.width;
	//viewport.height = (float)swapChainExtent.height;

	//viewport.minDepth = 0.0f;
	//viewport.maxDepth = 1.0f;

	//// Scissors
	//VkRect2D scissor{};
	//scissor.offset = { 0, 0 };
	//scissor.extent = swapChainExtent;
	
	// Dynamic viewport/scissors
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	// Color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = {
		VK_COLOR_COMPONENT_R_BIT | 
		VK_COLOR_COMPONENT_G_BIT | 
		VK_COLOR_COMPONENT_B_BIT | 
		VK_COLOR_COMPONENT_A_BIT 
	};
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	// Uniforms
	
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 0;
	pipelineLayoutCreateInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional
	
	if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Pipeline layout was't created");
	}
	
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(
		device,
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr,
		&graphicsPipeline
	) != VK_SUCCESS) {
		throw std::runtime_error("Graphics Pipeline wasn't created.");
	}



	vkDestroyShaderModule(device, vertShaderModule, nullptr);
	vkDestroyShaderModule(device, fragShaderModule, nullptr);
}

VkShaderModule BladeForge::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Shader module was't created");
	}
	
	return shaderModule;
}

QueueFamilyIndices BladeForge::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamiliesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamiliesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		
		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}
	std::cout << "\n";
	std::cout << "Found chosen physical device properties:\n";
	std::cout << "minImageTransferGranularity (depth, height, width): ("
		<< queueFamilies[i].minImageTransferGranularity.depth << ", "
		<< queueFamilies[i].minImageTransferGranularity.height << ", "
		<< queueFamilies[i].minImageTransferGranularity.width << ")\n";
	std::cout << "queueCount: " << queueFamilies[i].queueCount << "\n";
	std::cout << "queueFlags: " << queueFamilies[i].queueFlags << "\n";
	std::cout << "timestampValidBits: " << queueFamilies[i].timestampValidBits << "\n";


	return indices;
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

std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(
		// FILE-name
		filename, 
		// Modes
			std::ios::ate |  // Read from the end
			std::ios::binary // Read file as binary (avoid text transformations)
	);

	if (!file.is_open()) {
		std::stringstream ss;
		ss << "Failed to open file " << filename;
		throw std::runtime_error(ss.str());
	}
	else {
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}
}
