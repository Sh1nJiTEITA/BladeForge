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
		extensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
	}
	
	// Create VkInstance
	VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	appInfo.pApplicationName = "BladeForge";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = nullptr;
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;


	VkInstanceCreateInfo instanceInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceInfo.ppEnabledExtensionNames = extensions.data();



	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	VkValidationFeaturesEXT features{};

	if (enableValidationLayers)
	{
	
		instanceInfo.enabledLayerCount = static_cast<uint32_t>(bfvValidationLayers.size());

	
		instanceInfo.ppEnabledLayerNames = bfvValidationLayers.data();

		bfPopulateMessengerCreateInfo(debugCreateInfo);

		
		features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		features.enabledValidationFeatureCount = bfvValidationFeatures.size();
		features.pEnabledValidationFeatures = bfvValidationFeatures.data();
		features.pNext = (VkDebugUtilsMessengerEXT*)&debugCreateInfo;
		instanceInfo.pNext = (VkValidationFeaturesEXT*)&features;
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
	// If layer is disabled, �� DebugMessenger is not neccesery
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
		
		BfHolder::__bfpHolder->physical_devices.push_back(bf_device);

		BfPhysicalDevice* pPhysicalDevice = &BfHolder::__bfpHolder->physical_devices.back();

		bool is_suitable;
		bfIsDeviceSuitable(pPhysicalDevice, base.surface, is_suitable);

		if (is_suitable) {
			base.physical_device = pPhysicalDevice;
			
			// Get chosen physical device properties
			vkGetPhysicalDeviceProperties(base.physical_device->physical_device, &base.physical_device->properties);

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

	VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features = {};
	shader_draw_parameters_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
	shader_draw_parameters_features.pNext = nullptr;
	shader_draw_parameters_features.shaderDrawParameters = VK_TRUE;
	createInfo.pNext = &shader_draw_parameters_features;


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

	BfSingleEvent event{};
	if (vkCreateSwapchainKHR(base.device, &createInfo, nullptr, &base.swap_chain) != VK_SUCCESS) {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_SWAPCHAIN_FAILURE;
	}
	else {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_SWAPCHAIN_SUCCESS;
	}

	vkGetSwapchainImagesKHR(base.device, base.swap_chain, &imageCount, nullptr);
	base.image_packs.resize(imageCount);
	bfGetpHolder()->images.resize(imageCount);
	vkGetSwapchainImagesKHR(base.device, base.swap_chain, &imageCount, bfGetpHolder()->images.data());

	for (size_t i = 0; i < bfGetpHolder()->images.size(); i++) {
		base.image_packs[i].pImage = &bfGetpHolder()->images[i];
	}

	base.swap_chain_format = surfaceFormat.format;
	base.swap_chain_extent = extent;
	base.image_pack_count = imageCount;

	return BfEvent(event);
}

BfEvent bfCreateImageViews(BfBase& base)
{
	// NUMBER OF IMAGES -> NUMBER OF VIEWS
	BfHolder* holder = bfGetpHolder();
	holder->image_views.resize(base.image_pack_count);
	
	for (size_t i = 0; i < base.image_pack_count; i++) {
		base.image_packs[i].pImage_view = &holder->image_views[i];
	}

	std::stringstream ss_s;
	std::stringstream ss_f;

	ss_s << "Successfully done image indices = [";
	ss_f << "Unsuccessfully done image indices = [";

	bool is_image_views_created = true;

	// Go through all images
	for (size_t i = 0; i < holder->image_views.size(); i++) {
		// Create single image-view inside loop-iteration
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = *base.image_packs[i].pImage;

		// 1D, 2D, 3D textures ...
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = base.swap_chain_format;

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

		BfSingleEvent event{};
		if (vkCreateImageView(base.device, &createInfo, nullptr, base.image_packs[i].pImage_view) == VK_SUCCESS) {
			if (i != base.image_pack_count - 1)
				ss_s << i << "][";
			else
				ss_s << i << "] ";
		}
		else {
			is_image_views_created = false;
			if (i != base.image_pack_count - 1)
				ss_f << i << "][";
			else
				ss_f << i << "] ";
		}
	}

	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
	if (is_image_views_created) {
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_IMAGE_VIEWS_SUCCESS;
		event.info = ss_s.str();
	}
	else {
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_IMAGE_VIEWS_FAILURE;
		ss_s << ss_f.str();
		event.info = ss_s.str();
	}
	return BfEvent(event);
}

BfEvent bfCreateStandartRenderPass(BfBase& base)
{
	


	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = base.swap_chain_format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	//VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	
	

// Depth buffer
	VkAttachmentDescription depth_attachment{};
	depth_attachment.flags = 0;
	depth_attachment.format = base.depth_format;
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_ref = {};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	// How to use subPass
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;


// Dependencies
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkSubpassDependency depth_dependency{};
	depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	depth_dependency.dstSubpass = 0;
	depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	depth_dependency.srcAccessMask = 0;
	depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

// Create render pass

	std::vector<VkAttachmentDescription> attachments {
		colorAttachment,
		depth_attachment
	};

	std::vector<VkSubpassDependency> dependencies{
		dependency,
		depth_dependency
	};

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.dependencyCount = 2;
	renderPassInfo.pDependencies = dependencies.data();


	BfSingleEvent event{};
	if (vkCreateRenderPass(base.device, &renderPassInfo, nullptr, &base.standart_render_pass) == VK_SUCCESS) {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_RENDER_PASS_SUCCESS;
		event.info = " Standart render pass";
	}
	else {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_RENDER_PASS_FAILURE;
		event.info = " Standart render pass";
	}
	
	return BfEvent(event);
}

BfEvent bfCreateGUIRenderPass(BfBase& base)
{
	VkAttachmentDescription attachment = {};
	attachment.format = base.swap_chain_format;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment = {};
	color_attachment.attachment = 0;
	color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.attachmentCount = 1;
	info.pAttachments = &attachment;
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.dependencyCount = 1;
	info.pDependencies = &dependency;

	BfSingleEvent event{};
	if (vkCreateRenderPass(base.device, &info, nullptr, &base.gui_render_pass) == VK_SUCCESS) {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_RENDER_PASS_SUCCESS;
		event.info = " GUI render pass";
	}
	else {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_RENDER_PASS_FAILURE;
		event.info = " GUI render pass";
	}
	
	return BfEvent(event);
}

BfEvent bfCreateDescriptorSetLayout(BfBase& base)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	BfSingleEvent event{};
	if (vkCreateDescriptorSetLayout(
		base.device, &layoutInfo, nullptr, &base.global_set_layout
	) == VK_SUCCESS) {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_DESCRIPTOR_SET_LAYOUT_SUCCESS;
	}
	else {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_INIT_DESCRIPTOR_SET_LAYOUT_FAILURE;
	}
	return BfEvent(event);
}

BfEvent bfInitDescriptors(BfBase& base)
{
// BfEvent recording
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
	
	bool is_descriptors_init_successfull = true;
	// Additional Infolog for bfSingleEvent
	std::stringstream ss;

// BfHolder bindings
	BfHolder* pHolder = bfGetpHolder();

// Resize holder storages ---------------------------------------------------------------

	
	if (base.frame_pack.size() != MAX_FRAMES_IN_FLIGHT) { // FrameHolder
		base.frame_pack.resize(MAX_FRAMES_IN_FLIGHT);
	}

	// pack
	if (pHolder->uniform_view_buffers.size() != MAX_FRAMES_IN_FLIGHT) { // UniformBuffer's
		pHolder->uniform_view_buffers.resize(MAX_FRAMES_IN_FLIGHT);
	}
	if (pHolder->bezier_points_buffers.size() != MAX_FRAMES_IN_FLIGHT) {
		pHolder->bezier_points_buffers.resize(MAX_FRAMES_IN_FLIGHT);
	}
	if (pHolder->model_buffers.size() != MAX_FRAMES_IN_FLIGHT) {
		pHolder->model_buffers.resize(MAX_FRAMES_IN_FLIGHT);
	}

	// sets 
	if (pHolder->global_descriptor_sets.size() != MAX_FRAMES_IN_FLIGHT) { // DescriptorSet's
		pHolder->global_descriptor_sets.resize(MAX_FRAMES_IN_FLIGHT);
	}

	if (pHolder->main_descriptor_sets.size() != MAX_FRAMES_IN_FLIGHT) {
		pHolder->main_descriptor_sets.resize(MAX_FRAMES_IN_FLIGHT);
	}


// Dynamic uniform buffer preparations
	const size_t bezier_properties_uniform_buffer_size = MAX_FRAMES_IN_FLIGHT * 
		bfPadUniformBufferSize(base.physical_device, sizeof(BfUniformBezierProperties));

	// Test buffer (bezier-properties)
	bfCreateBuffer(&base.bezier_properties_uniform_buffer,
					base.allocator,
					bezier_properties_uniform_buffer_size,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VMA_MEMORY_USAGE_CPU_TO_GPU);

	const int MAX_BEZIER_POINTS = 1000;
	const int MAX_OBJECTS = 10000;
	// Bezier-points
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		// Create storage buffer for bezier_points
		bfCreateBuffer(&pHolder->bezier_points_buffers[i],
					   base.allocator, 
					   sizeof(BfStorageBezierPoints) * MAX_BEZIER_POINTS,
					   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
					   VMA_MEMORY_USAGE_CPU_TO_GPU);

		// Create storage buffer for object model matrices
		bfCreateBuffer(&pHolder->model_buffers[i],
						base.allocator,
						sizeof(BfObjectData) * MAX_OBJECTS,
						VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
						VMA_MEMORY_USAGE_CPU_TO_GPU);


		// Create buffer for view uniform 
		bfCreateBuffer(&pHolder->uniform_view_buffers[i],//*camUniformBuffer, 
			base.allocator,
			sizeof(BfUniformView),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VMA_MEMORY_USAGE_CPU_TO_GPU);
	}



// Global descriptor set layout ----------------------------------------------------------
	// Uniform-bindings enumeration 

	// View-uniform 
	//VkDescriptorSetLayoutBinding camBufferBinding{};
	/*camBufferBinding.binding		 = 0;
	camBufferBinding.descriptorCount = 1;
	camBufferBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	camBufferBinding.stageFlags		 = VK_SHADER_STAGE_VERTEX_BIT;*/

	
	VkDescriptorSetLayoutBinding viewBufferBinding = bfGetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
	VkDescriptorSetLayoutBinding bezierPropertinesBufferbinding = bfGetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT, 1);

	// Bindings to desctiptor set layout
	std::vector<VkDescriptorSetLayoutBinding> global_bindings = {
		viewBufferBinding,
		bezierPropertinesBufferbinding
	};

	// Create desctiptor-pool layout 
	// Descriptor global set layout
	VkDescriptorSetLayoutCreateInfo globalDescriptorSetLayoutInfo{};
	globalDescriptorSetLayoutInfo.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	globalDescriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(global_bindings.size());
	globalDescriptorSetLayoutInfo.pNext		   = nullptr;
	globalDescriptorSetLayoutInfo.flags		   = 0;
	globalDescriptorSetLayoutInfo.pBindings	   = global_bindings.data();

	if (vkCreateDescriptorSetLayout(base.device, 
									&globalDescriptorSetLayoutInfo,
									nullptr, 
									&base.global_set_layout) == VK_SUCCESS) {
		ss << "Global VkDescriptorSetLayout for " << global_bindings.size() << "bindings was"
			"created successfully;";
	}
	else {
		is_descriptors_init_successfull = false;
		ss << "Global VkDescriptorSetLayout for " << global_bindings.size() << "bindings wasn't"
			"created;";
	}

// Main descriptor set layout ------------------------------------------------------------

	VkDescriptorSetLayoutBinding bezierPointsBinding = bfGetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
	VkDescriptorSetLayoutBinding modelMatrixBinding = bfGetDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);

	// Bindings to main desctiptor set layout
	std::vector<VkDescriptorSetLayoutBinding> main_bindings = {
		bezierPointsBinding,
		modelMatrixBinding
	};

	VkDescriptorSetLayoutCreateInfo mainSetLayourCreateInfo{};
	mainSetLayourCreateInfo.sType		 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	mainSetLayourCreateInfo.pNext		 = nullptr;
	mainSetLayourCreateInfo.flags		 = 0;
	mainSetLayourCreateInfo.pBindings	 = main_bindings.data();
	mainSetLayourCreateInfo.bindingCount = static_cast<uint32_t>(main_bindings.size());

	if (vkCreateDescriptorSetLayout(base.device,
		&mainSetLayourCreateInfo,
		nullptr,
		&base.main_set_layout) == VK_SUCCESS) {
		ss << "Main VkDescriptorSetLayout for " << main_bindings.size() << "bindings was"
			"created successfully;";
	}
	else {
		is_descriptors_init_successfull = false;
		ss << "Main VkDescriptorSetLayout for " << main_bindings.size() << "bindings wasn't"
			"created;";
	}


// Create descriptor pool ----------------------------------------------------------------
	std::vector<VkDescriptorPoolSize> sizes = {
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10}
	};

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.pNext		   = nullptr;
	poolInfo.flags		   = 0;
	poolInfo.maxSets	   = 10;
	poolInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
	poolInfo.pPoolSizes	   = sizes.data();

	if (vkCreateDescriptorPool(base.device,
							   &poolInfo,
							   nullptr,
							   &base.standart_descriptor_pool) == VK_SUCCESS) {
		ss << "VkDescriptorPool for " 
		   << poolInfo.maxSets
		   << " sets was created successfully";	
	}
	else {
		is_descriptors_init_successfull = false;
		
		ss << "VkDescriptorPool for "
		   << poolInfo.maxSets
		   << " sets wasn't created";
	}


	
// Allocate descriptors from desctiptor pool for each frame ------------------------------
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		
		// Point to uniform buffer in holder
		base.frame_pack[i].uniform_view_buffer   = &pHolder->uniform_view_buffers[i];
		base.frame_pack[i].bezier_points_buffer  = &pHolder->bezier_points_buffers[i];
		base.frame_pack[i].model_matrix_buffer   = &pHolder->model_buffers[i];

		base.frame_pack[i].global_descriptor_set = &pHolder->global_descriptor_sets[i];
		base.frame_pack[i].main_descriptor_set   = &pHolder->main_descriptor_sets[i];


		// Use needed uniform buffer value
		BfAllocatedBuffer* camUniformBuffer    = base.frame_pack[i].uniform_view_buffer;
		BfAllocatedBuffer* bezierStorageBuffer = base.frame_pack[i].bezier_points_buffer;
		BfAllocatedBuffer* modelMatrixBuffer = base.frame_pack[i].model_matrix_buffer;
		

	// Allocate global descriptor sets ---------------------------------------------------

		VkDescriptorSetAllocateInfo globalDescriptorSetAllocInfo{};
		globalDescriptorSetAllocInfo.sType			    = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		globalDescriptorSetAllocInfo.pNext			    = nullptr;
		globalDescriptorSetAllocInfo.descriptorPool	    = base.standart_descriptor_pool;
		globalDescriptorSetAllocInfo.descriptorSetCount = 1; // 1 for each frame
		globalDescriptorSetAllocInfo.pSetLayouts		= &base.global_set_layout;

		if (vkAllocateDescriptorSets(base.device, 
									 &globalDescriptorSetAllocInfo,
									 base.frame_pack[i].global_descriptor_set) == VK_SUCCESS) {
			ss << "Global VkDescriptorsSet for frame " << i << "was allocated;";
		}
		else {
			is_descriptors_init_successfull = false;
			ss << "Global VkDescriptorsSet for frame " << i << "wasn't allocated;";
		}

	// Allocate main descriptor sets -----------------------------------------------------

		VkDescriptorSetAllocateInfo mainDescriptorSetAllocInfo{};
		mainDescriptorSetAllocInfo.sType			  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		mainDescriptorSetAllocInfo.pNext			  = nullptr;
		mainDescriptorSetAllocInfo.descriptorPool	  = base.standart_descriptor_pool;
		mainDescriptorSetAllocInfo.descriptorSetCount = 1; // 1 for each frame
		mainDescriptorSetAllocInfo.pSetLayouts		  =	&base.main_set_layout;

		if (vkAllocateDescriptorSets(base.device,
			&mainDescriptorSetAllocInfo,
			base.frame_pack[i].main_descriptor_set) == VK_SUCCESS) {
			ss << "Main VkDescriptorsSet for frame " << i << "was allocated;";
		}
		else {
			is_descriptors_init_successfull = false;
			ss << "Main VkDescriptorsSet for frame " << i << "wasn't allocated;";
		}

// Make descriptors point to uniform buffers (data) --------------------------------------

		// View Uniform buffer
		VkDescriptorBufferInfo viewUniformbufferInfo{};
		viewUniformbufferInfo.buffer = camUniformBuffer->buffer;
		viewUniformbufferInfo.offset = 0;						// From start
		viewUniformbufferInfo.range  = sizeof(BfUniformView);  // Length in bites of data

		// Bezier properties Uniform buffer
		VkDescriptorBufferInfo bezierPropertiesUniformbufferInfo{};
		bezierPropertiesUniformbufferInfo.buffer = base.bezier_properties_uniform_buffer.buffer;
		bezierPropertiesUniformbufferInfo.offset = 0; 
		bezierPropertiesUniformbufferInfo.range = sizeof(BfUniformBezierProperties);  // Length in bites of data

		// Bezier points storage buffer
		VkDescriptorBufferInfo bezierPointStoragebufferInfo{};
		bezierPointStoragebufferInfo.buffer = bezierStorageBuffer->buffer;
		bezierPointStoragebufferInfo.offset = 0;
		bezierPointStoragebufferInfo.range = sizeof(BfStorageBezierPoints) * MAX_BEZIER_POINTS;

		VkDescriptorBufferInfo modelStoragebufferInfo{};
		modelStoragebufferInfo.buffer = modelMatrixBuffer->buffer;
		modelStoragebufferInfo.offset = 0;
		modelStoragebufferInfo.range = sizeof(BfObjectData) * MAX_OBJECTS;



		VkWriteDescriptorSet viewWrite = bfWriteDescriptorBuffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
																*base.frame_pack[i].global_descriptor_set, 
																&viewUniformbufferInfo, 
																 viewBufferBinding.binding);

		VkWriteDescriptorSet bezierPropertiesWrite = bfWriteDescriptorBuffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
																			*base.frame_pack[i].global_descriptor_set,
																			&bezierPropertiesUniformbufferInfo, 
																			 bezierPropertinesBufferbinding.binding);

		VkWriteDescriptorSet bezierPointsPropertiesWrite = bfWriteDescriptorBuffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
																				  *base.frame_pack[i].main_descriptor_set, 
																				  &bezierPointStoragebufferInfo, 
																				   bezierPointsBinding.binding);

		VkWriteDescriptorSet modelWrite = bfWriteDescriptorBuffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
																  *base.frame_pack[i].main_descriptor_set,
																  &modelStoragebufferInfo,
																  modelMatrixBinding.binding);

		std::vector<VkWriteDescriptorSet> setWrites = {
			viewWrite,
			bezierPropertiesWrite,
			bezierPointsPropertiesWrite,
			modelWrite
		};

		vkUpdateDescriptorSets(base.device, static_cast<uint32_t>(setWrites.size()), setWrites.data(), 0, nullptr);

		if (vmaMapMemory(base.allocator,
						 base.frame_pack[i].uniform_view_buffer->allocation,
						&base.frame_pack[i].uniform_view_data) == VK_SUCCESS) {
			ss << "vmaMapMemory was successful for View-Uniform descriptor of frame " << i;
		}
		else {
			is_descriptors_init_successfull = false;
			ss << "vmaMapMemory wasn't successful for View-Uniform descriptor of frame " << i;
		}

		/*if (vmaMapMemory(base.allocator,
			base.bezier_properties_uniform_buffer.allocation,
			(void**)&base.bezier_data) == VK_SUCCESS) {
			ss << "vmaMapMemory was successful for View-Uniform descriptor of frame " << i;
		}
		else {
			is_descriptors_init_successfull = false;
			ss << "vmaMapMemory wasn't successful for View-Uniform descriptor of frame " << i;
		}*/

	}

	if (is_descriptors_init_successfull) {
		event.action = BfEnActionType::BF_ACTION_INIT_WHOLE_DISCRIPTORS_SUCCESS;
	}
	else {
		event.action = BfEnActionType::BF_ACTION_INIT_WHOLE_DISCRIPTORS_FAILURE;
	}
	event.info = ss.str();

	return BfEvent(event);
}


void bfPopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	if (!enableValidationLayers) return;

	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = {
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |	// Just info (e.g. creation info)
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

BfEvent bfaReadFile(std::vector<char>& data, const std::string& filename)
{
	std::ifstream file(
		// FILE-name
		filename,
		// Modes
		std::ios::ate |  // Read from the end
		std::ios::binary // Read file as binary (avoid text transformations)
	);

	BfSingleEvent event{};

	std::stringstream ss;
	ss << "File path = " << filename;
	event.info = ss.str();
	
	if (!file.is_open()) {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_READ_SHADER_FILE_SUCCESS;
		return BfSingleEvent(event);
	}
	else {
		

		size_t fileSize = (size_t)file.tellg();
		data.clear();
		data.resize(fileSize);

		file.seekg(0);
		file.read(data.data(), fileSize);
		file.close();

		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_READ_SHADER_FILE_SUCCESS;
		return BfSingleEvent(event);
	}
}

BfEvent bfCreateGraphicsPipelines(BfBase& base, std::string vert_shader_path, std::string frag_shader_path)
{
	// Shader Modules Creation
	std::vector<char> vertShaderCode;
	std::vector<char> fragShaderCode;
	
	if (!vert_shader_path.empty()) {
		bfaReadFile(vertShaderCode, vert_shader_path);
	}
	if (!vert_shader_path.empty()) {
		bfaReadFile(fragShaderCode, frag_shader_path);
	}
	
	
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

	bfaCreateShaderModule(vertShaderModule, base.device, vertShaderCode);
	bfaCreateShaderModule(fragShaderModule, base.device, fragShaderCode);

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

	auto bindingDescription = BfVertex3::getBindingDescription();
	auto attributeDescriptions = BfVertex3::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	// How to use vertex data
	VkPipelineInputAssemblyStateCreateInfo inputAssembly_triangles{};
	inputAssembly_triangles.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly_triangles.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly_triangles.primitiveRestartEnable = VK_FALSE;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly_lines{};
	inputAssembly_lines.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly_lines.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	inputAssembly_lines.primitiveRestartEnable = VK_FALSE;

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
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	//rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	//rasterizer.depthBiasClamp = 0.0f; // Optional
	//rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

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

	// Depth-stencil part
	auto depth_stencil_state = bfPopulateDepthStencilStateCreateInfo(
		true,
		true,
		VK_COMPARE_OP_LESS_OR_EQUAL
	);



	// Uniforms
	bfaCreateGraphicsPipelineLayouts(base);

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depth_stencil_state;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.renderPass = base.standart_render_pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	//pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
	// basic
	pipelineInfo.pInputAssemblyState = &inputAssembly_triangles;
	pipelineInfo.layout = base.triangle_pipeline_layout;
	pipelineInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;


	bool is_pipelines_created = true;
	std::stringstream ss;




	if (vkCreateGraphicsPipelines(
		base.device,
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr,
		&base.triangle_pipeline
	) != VK_SUCCESS) {
		is_pipelines_created = false;
		ss << "Base pipeline (triangles) wasn't created;";
	}
	
	pipelineInfo.pInputAssemblyState = &inputAssembly_lines;
	pipelineInfo.layout = base.line_pipeline_layout;
	pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
	pipelineInfo.basePipelineIndex = -1;
	pipelineInfo.basePipelineHandle = base.triangle_pipeline;

	if (vkCreateGraphicsPipelines(
		base.device,
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr,
		&base.line_pipeline
	) != VK_SUCCESS) {
		is_pipelines_created = false;
		ss << "Derivative pipeline (lines) wasn't created;";
	}

	vkDestroyShaderModule(base.device, vertShaderModule, nullptr);
	vkDestroyShaderModule(base.device, fragShaderModule, nullptr);

	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
	
	if (is_pipelines_created) {
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_GRAPHICS_PIPELINE_SUCCESS;
		event.info = "All pipelines is OK";
	}
	else {
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_GRAPHICS_PIPELINE_FAILURE;
		event.info = ss.str();
	}
	return BfEvent(event);
}

BfEvent bfCreateStandartFrameBuffers(BfBase& base)
{
	BfHolder* holder = bfGetpHolder();
	holder->standart_framebuffers.resize(base.image_pack_count);

	std::stringstream ss_s; ss_s << "Correctly created standart framebuffers indices = [";
	std::stringstream ss_f; ss_f << "Incorrectly created standart framebuffers indices = [";
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

	bool is_all_framebuffers_was_created = true;

	for (size_t i = 0; i < base.image_pack_count; i++) {
		base.image_packs[i].pStandart_Buffer = &holder->standart_framebuffers[i];
		
		std::vector<VkImageView> attachments = {
			*base.image_packs[i].pImage_view,
			base.depth_image_view
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = base.standart_render_pass;
		framebufferInfo.attachmentCount = 2;
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = base.swap_chain_extent.width;
		framebufferInfo.height = base.swap_chain_extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(base.device,
								&framebufferInfo,
								nullptr,
								base.image_packs[i].pStandart_Buffer) == VK_SUCCESS) {
			if (i != base.image_pack_count - 1) ss_s << i << "]["; 
			else								ss_s << i << "]";
		}
		else {
			is_all_framebuffers_was_created = false;
			
			if (i != base.image_pack_count - 1) ss_f << i << "][";
			else								ss_f << i << "]";
		}

	}

	if (is_all_framebuffers_was_created) {
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_FRAME_BUFFER_SUCCESS;
		event.info = ss_s.str();
	}
	else {
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_FRAME_BUFFER_FAILURE;
		event.info = ss_s.str().append(ss_f.str());
	}


	return BfEvent(event);
}

BfEvent bfCreateGUIFrameBuffers(BfBase& base)
{
	BfHolder* holder = bfGetpHolder();
	holder->gui_framebuffers.resize(base.image_pack_count);

	std::stringstream ss_s; ss_s << "Correctly created GUI framebuffers indices = [";
	std::stringstream ss_f; ss_f << "Incorrectly created GUI framebuffers indices = [";
	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

	VkImageView attachment[1];

	VkFramebufferCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.renderPass = base.gui_render_pass;
	info.attachmentCount = 1;
	info.pAttachments = attachment;
	info.width = base.swap_chain_extent.width;
	info.height = base.swap_chain_extent.height;
	info.layers = 1;
	
	bool is_all_framebuffers_was_created = true;

	for (uint32_t i = 0; i < base.image_pack_count; i++)
	{
		base.image_packs[i].pGUI_buffer = &holder->gui_framebuffers[i];
		attachment[0] = *base.image_packs[i].pImage_view;

		if (vkCreateFramebuffer(base.device, 
								&info, 
								nullptr, 
								base.image_packs[i].pGUI_buffer) == VK_SUCCESS) {
			if (i != base.image_pack_count - 1) ss_s << i << "][";
			else								ss_s << i << "]";
		}
		else {
			is_all_framebuffers_was_created = false;

			if (i != base.image_pack_count - 1) ss_f << i << "][";
			else								ss_f << i << "]";
		}
	}
	
	if (is_all_framebuffers_was_created) {
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_FRAME_BUFFER_SUCCESS;
		event.info = ss_s.str();
	}
	else {
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_FRAME_BUFFER_FAILURE;
		event.info = ss_s.str().append(ss_f.str());
	}

	return BfEvent(event);
}

BfEvent bfCreateCommandPool(BfBase& base)
{
	

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = base.physical_device->queue_family_indices[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE].value();

	BfSingleEvent event{};
	if (vkCreateCommandPool(base.device, &poolInfo, nullptr, &base.command_pool) == VK_SUCCESS) {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_COMMAND_POOL_SUCCESS;
	}
	else {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_COMMAND_POOL_FAILURE;
	}
	
	return BfEvent(event);
}

BfEvent bfCreateUniformBuffers(BfBase& base)
{
	BfHolder* holder = bfGetpHolder();
	holder->uniform_view_buffers.resize(MAX_FRAMES_IN_FLIGHT);

	if (base.frame_pack.size() != MAX_FRAMES_IN_FLIGHT) {
		base.frame_pack.resize(MAX_FRAMES_IN_FLIGHT);
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		base.frame_pack[i].uniform_view_buffer = &holder->uniform_view_buffers[i];

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(BfUniformView);
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

		if (vmaCreateBuffer(base.allocator, 
							&bufferInfo,
							&allocationInfo, 
							&base.frame_pack[i].uniform_view_buffer->buffer, 
							&base.frame_pack[i].uniform_view_buffer->allocation, 
							nullptr) != VK_SUCCESS) 
		{ 
			throw std::runtime_error("vmaCrateBuffer didn't work"); 
		}

		/*vmaMapMemory(base.allocator, 
					 base.frame_pack[i].uniform_view_buffer->allocation, 
					 &base.frame_pack[i].uniform_view_buffer->data);*/

		/*glm::mat4 local_mat = glm::mat4(1.0f);
		BfUniformView uniform{ local_mat,local_mat,local_mat };

		memcpy(base.frame_pack[i].uniform_view_buffer->data, &uniform, sizeof(BfUniformView));*/
	}

	return BfEvent();
}

BfEvent bfCreateStandartDescriptorPool(BfBase& base)
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(base.device, &poolInfo, nullptr, &base.standart_descriptor_pool) != VK_SUCCESS) {
		throw std::runtime_error("Descriptor pool wasn't created");
	}
	return BfEvent();
}

BfEvent bfCreateGUIDescriptorPool(BfBase& base)
{
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1;
	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	if (vkCreateDescriptorPool(base.device, &pool_info, nullptr, &base.gui_descriptor_pool) != VK_SUCCESS) {
		throw std::runtime_error("ImGUI descriptor pool wasn't created");
	}
	return BfEvent();
}
//
//BfEvent bfCreateDescriptorSets(BfBase& base)
//{
//	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, base.global_set_layout);
//	
//	VkDescriptorSetAllocateInfo allocInfo{};
//	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//	allocInfo.descriptorPool = base.standart_descriptor_pool;
//	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//	allocInfo.pSetLayouts = layouts.data();
//
//	BfHolder* holder = bfGetpHolder();
//	if (holder->uniform_view_descriptor_set.size() != MAX_FRAMES_IN_FLIGHT) {
//		holder->uniform_view_descriptor_set.resize(MAX_FRAMES_IN_FLIGHT);
//	}
//	
//	if (vkAllocateDescriptorSets(base.device, &allocInfo, holder->uniform_view_descriptor_set.data()) != VK_SUCCESS)
//	{
//		throw std::runtime_error("Descriptor sets wasn't created");
//	}
//
//	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//		holder->uniform_view_buffers[i].descriptor_set = &holder->uniform_view_descriptor_set[i];
//		
//		VkDescriptorBufferInfo bufferInfo{};
//		bufferInfo.buffer = base.frame_pack[i].uniform_view_buffer->buffer;
//		bufferInfo.offset = 0;
//		bufferInfo.range = sizeof(BfUniformView);
//
//
//		VkWriteDescriptorSet descriptorWrite{};
//		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//		descriptorWrite.dstSet = *base.frame_pack[i].uniform_view_buffer->descriptor_set;
//		descriptorWrite.dstBinding = 0;
//		descriptorWrite.dstArrayElement = 0;
//		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		descriptorWrite.descriptorCount = 1;
//		descriptorWrite.pBufferInfo = &bufferInfo;
//
//		vkUpdateDescriptorSets(base.device, 1, &descriptorWrite, 0, nullptr);
//	}
//	
//	return BfEvent();
//}


BfEvent bfCreateStandartCommandBuffers(BfBase& base) {
	BfHolder* holder = bfGetpHolder();
	
	if (holder->standart_command_buffers.size() != MAX_FRAMES_IN_FLIGHT) {
		holder->standart_command_buffers.resize(MAX_FRAMES_IN_FLIGHT);
	}

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = base.command_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(holder->standart_command_buffers.size());

	if (vkAllocateCommandBuffers(base.device, &allocInfo, holder->standart_command_buffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Command buffer was not allocated");
	}
	
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		base.frame_pack[i].standart_command_buffer = &holder->standart_command_buffers[i];
	}

	return BfEvent();
}

BfEvent bfCreateGUICommandBuffers(BfBase& base)
{
	BfHolder* holder = bfGetpHolder();

	if (holder->gui_command_buffers.size() != MAX_FRAMES_IN_FLIGHT) {
		holder->gui_command_buffers.resize(MAX_FRAMES_IN_FLIGHT);
	}

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = base.command_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(holder->gui_command_buffers.size());

	if (vkAllocateCommandBuffers(base.device, &allocInfo, holder->gui_command_buffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("ImGUI Command buffer was not allocated");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		base.frame_pack[i].gui_command_buffer = &holder->gui_command_buffers[i];
	}

	
	return BfEvent();
}

BfEvent bfCreateSyncObjects(BfBase& base)
{
	BfHolder* holder = bfGetpHolder();
	
	if (holder->available_image_semaphores.size() != MAX_FRAMES_IN_FLIGHT)
		holder->available_image_semaphores.resize(MAX_FRAMES_IN_FLIGHT);

	if (holder->finish_render_image_semaphores.size() != MAX_FRAMES_IN_FLIGHT)
		holder->finish_render_image_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	
	if (holder->frame_in_flight_fences.size() != MAX_FRAMES_IN_FLIGHT)
		holder->frame_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);


	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // For first frame render

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		base.frame_pack[i].available_image_semaphore = &holder->available_image_semaphores[i];
		base.frame_pack[i].finish_render_image_semaphore = &holder->finish_render_image_semaphores[i];
		base.frame_pack[i].frame_in_flight_fence = &holder->frame_in_flight_fences[i];
		
		if (vkCreateSemaphore(base.device, &semaphoreInfo, nullptr,
			base.frame_pack[i].available_image_semaphore) != VK_SUCCESS ||

			vkCreateSemaphore(base.device, &semaphoreInfo, nullptr,
			base.frame_pack[i].finish_render_image_semaphore) != VK_SUCCESS ||

			vkCreateFence(base.device, &fenceInfo, nullptr,
			base.frame_pack[i].frame_in_flight_fence) != VK_SUCCESS)
		{
			throw std::runtime_error("Semaphore or fence weren't created");
		}
	}

	return BfEvent();
}

BfEvent bfInitImGUI(BfBase& base)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForVulkan(base.window->pWindow, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = base.instance;
	init_info.PhysicalDevice = base.physical_device->physical_device;
	init_info.Device = base.device;
	init_info.QueueFamily = base.physical_device->queue_family_indices[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE].value();
	init_info.Queue = base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE];
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = base.gui_descriptor_pool;
	init_info.Allocator = nullptr;

	BfSwapChainSupport swapChainSupport; 
	bfGetSwapChainSupport(base.physical_device->physical_device, base.surface, swapChainSupport);
	
	VkSurfaceFormatKHR surfaceFormat;
	bfGetSwapSurfaceFormat(swapChainSupport, surfaceFormat);

	VkPresentModeKHR presentMode;
	bfGetSwapPresentMode(swapChainSupport, presentMode);

	VkExtent2D extent;
	bfGetSwapExtent(swapChainSupport, base.window->pWindow, extent);

	uint32_t imageCount = base.image_pack_count;

	/*if (
		swapChainSupport.capabilities.maxImageCount > 0
		&& imageCount > swapChainSupport.capabilities.maxImageCount
		) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}


	init_info.MinImageCount = imageCount;

	this->minImageCount = imageCount;*/

	init_info.ImageCount = imageCount;
	init_info.MinImageCount = imageCount;
	init_info.CheckVkResultFn = check_vk_result;


	ImGui_ImplVulkan_Init(&init_info, base.gui_render_pass);

	VkCommandBuffer command_buffer; 
	bfBeginSingleTimeCommands(base, command_buffer);
		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
	bfEndSingleTimeCommands(base, command_buffer);
	return BfEvent();
}

BfEvent bfCreateDepthBuffer(BfBase& base)
{
	VkExtent3D depthImageExtent = {
		base.swap_chain_extent.width,
		base.swap_chain_extent.height,
		1
	};
	
	base.depth_format = VK_FORMAT_D32_SFLOAT;

	VkImageCreateInfo dimg_info = bfPopulateDepthImageCreateInfo(base.depth_format, 
																 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
																 depthImageExtent);

	VmaAllocationCreateInfo dimg_allocinfo{};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vmaCreateImage(base.allocator, 
				   &dimg_info, 
				   &dimg_allocinfo, 
				   &base.depth_image.image, 
				   &base.depth_image.allocation, nullptr);

	VkImageViewCreateInfo dview_info = bfPopulateDepthImageViewCreateInfo(base.depth_format, 
																		  base.depth_image.image, 
																		  VK_IMAGE_ASPECT_DEPTH_BIT);

	BfSingleEvent event{};
	if (vkCreateImageView(base.device, &dview_info, nullptr, &base.depth_image_view) == VK_SUCCESS) {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_CREATE_DEPTH_IMAGE_VIEW_SUCCESS;
	}
	else {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_CREATE_DEPTH_IMAGE_VIEW_FAILURE;
	}

	return BfEvent(event);
}

void bfCreateAllocator(BfBase& base)
{
	VmaAllocatorCreateInfo info{};
	info.device = base.device;
	info.instance = base.instance;
	info.physicalDevice = base.physical_device->physical_device;
	
	if (vmaCreateAllocator(&info, &base.allocator) != VK_SUCCESS) {
		throw std::runtime_error("allocator wasn't created");
	}
}

void bfAllocateBuffersForDynamicMesh(BfBase& base)
{
	uint32_t MAX_VERTICES = 10000;
	
	bfCreateBuffer(&base.dynamic_vertex_buffer,
					base.allocator,
					sizeof(bfVertex) * MAX_VERTICES,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VMA_MEMORY_USAGE_CPU_TO_GPU);

	bfCreateBuffer(&base.dynamic_index_buffer,
					base.allocator,
					sizeof(uint32_t) * MAX_VERTICES,
					VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
					VMA_MEMORY_USAGE_CPU_TO_GPU);
}

void bfUploadDynamicMesh(BfBase& base, BfMesh& mesh)
{
	if (base.is_resized) {
		void* vertex_data;
		vmaMapMemory(base.allocator, base.dynamic_vertex_buffer.allocation, &vertex_data);
			memcpy(vertex_data, mesh.vertices.data(), sizeof(bfVertex) * mesh.vertices.size());
		vmaUnmapMemory(base.allocator, base.dynamic_vertex_buffer.allocation);

		void* index_data;
		vmaMapMemory(base.allocator, base.dynamic_index_buffer.allocation, &index_data);
			memcpy(index_data, mesh.indices.data(), sizeof(uint32_t) * mesh.indices.size());
		vmaUnmapMemory(base.allocator, base.dynamic_index_buffer.allocation);

		base.is_resized = false;
	}
}

void bfUploadMesh(BfBase& base, BfMesh& mesh)
{
	bfUploadVertices(base, mesh);
	bfUploadIndices(base, mesh);
}

void bfUploadVertices(BfBase& base, BfMesh& mesh)
{
	VkBuffer local_buffer;
	VmaAllocation local_allocation;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = mesh.vertices.size() * sizeof(bfVertex);
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	
	VmaAllocationCreateInfo allocationInfo{};
	allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

	if (vmaCreateBuffer(base.allocator, &bufferInfo, &allocationInfo, &local_buffer, &local_allocation, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("vmaCrateBuffer didn't work");
	}

	void* data;
	vmaMapMemory(base.allocator, local_allocation, &data);
		memcpy(data, mesh.vertices.data(), bufferInfo.size);
	vmaUnmapMemory(base.allocator, local_allocation);


	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	allocationInfo.flags = 0;

	if (vmaCreateBuffer(base.allocator, &bufferInfo, &allocationInfo, &mesh.vertex_buffer.buffer, &mesh.vertex_buffer.allocation, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("vmaCrateBuffer didn't work");
	}

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = base.command_pool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(base.device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = bufferInfo.size;
	vkCmdCopyBuffer(commandBuffer, local_buffer, mesh.vertex_buffer.buffer, 1, &copyRegion);
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE], 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE]);

	vkFreeCommandBuffers(base.device, base.command_pool, 1, &commandBuffer);

	vmaDestroyBuffer(base.allocator, local_buffer, local_allocation);
}

void bfUploadIndices(BfBase& base, BfMesh& mesh)
{
	VkBuffer local_buffer;
	VmaAllocation local_allocation;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = mesh.indices.size() * sizeof(uint32_t);
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	//bufferInfo.flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	// VMA_ALLOCATION_CREATE_HOST_ACCESS_

	VmaAllocationCreateInfo allocationInfo{};
	allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

	if (vmaCreateBuffer(base.allocator, &bufferInfo, &allocationInfo, &local_buffer, &local_allocation, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("vmaCrateBuffer didn't work");
	}

	void* data;
	vmaMapMemory(base.allocator, local_allocation, &data);
	memcpy(data, mesh.indices.data(), bufferInfo.size);
	vmaUnmapMemory(base.allocator, local_allocation);


	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	allocationInfo.flags = 0;

	if (vmaCreateBuffer(base.allocator, &bufferInfo, &allocationInfo, &mesh.index_buffer.buffer, &mesh.index_buffer.allocation, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("vmaCrateBuffer didn't work");
	}

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = base.command_pool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(base.device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = bufferInfo.size;
		vkCmdCopyBuffer(commandBuffer, local_buffer, mesh.index_buffer.buffer, 1, &copyRegion);
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE], 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE]);

	vkFreeCommandBuffers(base.device, base.command_pool, 1, &commandBuffer);

	vmaDestroyBuffer(base.allocator, local_buffer, local_allocation);
}

BfEvent bfaCreateShaderModule(VkShaderModule& module, VkDevice device, const std::vector<char>& data)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = data.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(data.data());

	BfSingleEvent event{};
	if (vkCreateShaderModule(device, &createInfo, nullptr, &module) == VK_SUCCESS) {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_SHADER_MODULE_SUCCESS;
	}
	else {
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_SHADER_MODULE_FAILURE;
	}

	return BfEvent(event);
}

BfEvent bfaCreateGraphicsPipelineLayouts(BfBase& base)
{
	std::vector<VkDescriptorSetLayout> layouts = {
		base.global_set_layout,
		base.main_set_layout
	};
	
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
	pipelineLayoutCreateInfo.pSetLayouts = layouts.data(); // Optional
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

	bool is_pipeline_layouts_done = true;

	std::stringstream ss;
	
	if (vkCreatePipelineLayout(base.device, &pipelineLayoutCreateInfo, nullptr, &base.triangle_pipeline_layout) == VK_SUCCESS) {
		ss << "Triangle pipeline layout is done;";
	}
	else {
		ss << "Triangle pipeline layout isn't done;";
		is_pipeline_layouts_done = false;
	}
	if (vkCreatePipelineLayout(base.device, &pipelineLayoutCreateInfo, nullptr, &base.line_pipeline_layout) == VK_SUCCESS) {
		ss << "Line pipeline layout is done;";
	}
	else {
		ss << "Line pipeline layout isn't done;";
		is_pipeline_layouts_done = false;
	}

	BfSingleEvent event{};
	event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
	if (is_pipeline_layouts_done) {
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_PIPELINE_LAYOUT_SUCCESS;
	} 
	else {
		event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_PIPELINE_LAYOUT_FAILURE;
	}
	event.info = ss.str();

	return BfEvent(event);
}

BfEvent bfaRecreateSwapchain(BfBase& base)
{
	int width = 0; int height = 0;
	glfwGetFramebufferSize(base.window->pWindow, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(base.window->pWindow, &width, &height);
		glfwWaitEvents();
	}
	

	ImGui_ImplVulkan_SetMinImageCount(base.image_pack_count);

	vkDeviceWaitIdle(base.device);

	bfCleanUpSwapchain(base);

	bfCreateSwapchain(base);
	bfCreateDepthBuffer(base);
	bfCreateImageViews(base);
	bfCreateStandartFrameBuffers(base);
	bfCreateGUIFrameBuffers(base);
	
	base.window->resized = false;
	//base.window->ortho_right = (float)base.swap_chain_extent.width;
	//base.window->ortho_top = (float)base.swap_chain_extent.height;

	return BfEvent();
}

void bfBeginSingleTimeCommands(BfBase& base, VkCommandBuffer& commandBuffer)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = base.command_pool;
	allocInfo.commandBufferCount = 1;

	//VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(base.device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void bfEndSingleTimeCommands(BfBase& base, VkCommandBuffer& commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE], 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE]);

	vkFreeCommandBuffers(base.device, base.command_pool, 1, &commandBuffer);
}

void bfDrawFrame(BfBase& base, BfMesh& mesh, BfMeshHandler& handler)
{
	// VK_TRUE - wait all fences
	vkWaitForFences(base.device, 1, base.frame_pack[base.current_frame].frame_in_flight_fence, VK_TRUE, UINT64_MAX);

	VkSemaphore		local_available_image_semaphore		= *base.frame_pack[base.current_frame].available_image_semaphore;
	VkSemaphore		local_finish_render_image_semaphore = *base.frame_pack[base.current_frame].finish_render_image_semaphore;
	VkFence			local_fence_in_flight				= *base.frame_pack[base.current_frame].frame_in_flight_fence;
	
	VkCommandBuffer local_standart_command_bufffer		= *base.frame_pack[base.current_frame].standart_command_buffer;
	VkCommandBuffer local_gui_command_buffer			= *base.frame_pack[base.current_frame].gui_command_buffer;

	VkResult result = vkAcquireNextImageKHR(base.device,
		base.swap_chain,
		UINT64_MAX,
		local_available_image_semaphore,
		VK_NULL_HANDLE,
		&base.current_image);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		bfaRecreateSwapchain(base);

		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image");
	}

	BfExecutionTime::BeginTimeCut("uniform");
		bfUpdateUniformBuffer(base);
	BfExecutionTime::EndTimeCut("uniform");

	BfExecutionTime::BeginTimeCut("dynamic-mesh");
		bfUploadDynamicMesh(base, mesh);
	BfExecutionTime::EndTimeCut("dynamic-mesh");

	vkResetFences(base.device, 1, &local_fence_in_flight);

	vkResetCommandBuffer(local_standart_command_bufffer, 0);
	bfMainRecordCommandBuffer(base, mesh, handler);


	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { local_available_image_semaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	std::array<VkCommandBuffer, 2> submitCommandBuffers = {
		local_standart_command_bufffer, local_gui_command_buffer
	};

	submitInfo.pCommandBuffers = submitCommandBuffers.data();
	submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());

	VkSemaphore signalSemaphores[] = { local_finish_render_image_semaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE], 1, &submitInfo, local_fence_in_flight) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { base.swap_chain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &base.current_image;

	result = vkQueuePresentKHR(base.physical_device->queues[BfvEnQueueType::BF_QUEUE_PRESENT_TYPE], &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR ||
		result == VK_SUBOPTIMAL_KHR ||
		base.window->resized)
	{
		ImGui_ImplVulkan_SetMinImageCount(base.image_pack_count);
		bfaRecreateSwapchain(base);
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}


	base.current_frame = (base.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void bfMainRecordCommandBuffer(BfBase& base, BfMesh& mesh, BfMeshHandler& handler)
{
	// Depth buffer
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//beginInfo.flags = 0;
	beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;


	VkClearValue depth_clear;
	depth_clear.depthStencil.depth = 1.f;

	if (vkBeginCommandBuffer(*base.frame_pack[base.current_frame].standart_command_buffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recoding command buffer");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = base.standart_render_pass;
	renderPassInfo.framebuffer = *base.image_packs[base.current_image].pStandart_Buffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = base.swap_chain_extent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 2;
	
	std::vector<VkClearValue> clear_values{
		clearColor,
		depth_clear
	};

	renderPassInfo.pClearValues = clear_values.data();//&clearColor;

	VkCommandBuffer local_buffer = *base.frame_pack[base.current_frame].standart_command_buffer;
	vkCmdBeginRenderPass(local_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	{
		//vkCmdBindPipeline(local_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, base.triangle_pipeline);
		//vkCmdBindPipeline(local_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, base.line_pipeline);

		VkBuffer vertexBuffers[] = { 
			//mesh.vertex_buffer.buffer
			base.dynamic_vertex_buffer.buffer 
		};
		VkDeviceSize offsets[] = { 0 };

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(base.swap_chain_extent.width);
		viewport.height = static_cast<float>(base.swap_chain_extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(local_buffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = base.swap_chain_extent;
		vkCmdSetScissor(local_buffer, 0, 1, &scissor);

		//vkCmdBindVertexBuffers(local_buffer, 0, 1, vertexBuffers, offsets);
		//vkCmdBindIndexBuffer(local_buffer, base.dynamic_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
		
		uint32_t uniform_offset = bfPadUniformBufferSize(base.physical_device, sizeof(BfUniformBezierProperties)) * (base.current_frame % MAX_FRAMES_IN_FLIGHT);
		
		vkCmdBindDescriptorSets(
			local_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			base.triangle_pipeline_layout,
			0,
			1,
			base.frame_pack[base.current_frame].global_descriptor_set,//base.frame_pack[base.current_frame].uniform_view_buffer->descriptor_set,
			1,
			&uniform_offset
		);
		vkCmdBindDescriptorSets(
			local_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			base.triangle_pipeline_layout,
			1,
			1,
			base.frame_pack[base.current_frame].main_descriptor_set,//base.frame_pack[base.current_frame].uniform_view_buffer->descriptor_set,
			0,
			nullptr
		);


		//vkCmdDraw(local_buffer, base.vert_number, 1, 0, 0);
		//vkCmdDraw(local_buffer, base.vert_number, 1, 0, 0);
		//vkCmdDrawIndexed(local_buffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
		vkCmdBindPipeline(local_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, base.line_pipeline);
		/*
		for (size_t i = 0; i < handler.get_allocated_meshes_count(); i++) {
			BfMesh* mesh = handler.get_pMesh(i);
			if (mesh->type == BF_MESH_TYPE_CURVE) {
				handler.bind_mesh(local_buffer, i);
				handler.draw_indexed(local_buffer, i);
			}
		}
		
		vkCmdBindPipeline(local_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, base.triangle_pipeline);
		for (size_t i = 0; i < handler.get_allocated_meshes_count(); i++) {
			BfMesh* mesh = handler.get_pMesh(i);
			if (mesh->type != BF_MESH_TYPE_CURVE) {
				handler.bind_mesh(local_buffer, i);
				handler.draw_indexed(local_buffer, i);
			}
		}*/
		
		BfCurveHolder* pCurveHolder = bfGetpCurveHolder();

		pCurveHolder->draw_indexed(local_buffer);

		//handler.bind_mesh(local_buffer, 0);
		//handler.draw_indexed(local_buffer, 0);
	}
	vkCmdEndRenderPass(local_buffer);

	if (vkEndCommandBuffer(local_buffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recoding command buffer");
	}

	local_buffer = *base.frame_pack[base.current_frame].gui_command_buffer;

	// IMGUI
	if (vkBeginCommandBuffer(local_buffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recoding command buffer");
	}

	VkRenderPassBeginInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.renderPass = base.gui_render_pass;
	info.framebuffer = *base.image_packs[base.current_image].pGUI_buffer;
	info.renderArea.extent = base.swap_chain_extent;
	info.clearValueCount = 1;
	info.pClearValues = &clearColor;

	vkCmdBeginRenderPass(local_buffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), local_buffer);
	vkCmdEndRenderPass(local_buffer);

	if (vkEndCommandBuffer(local_buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

}

size_t bfPadUniformBufferSize(const BfPhysicalDevice* physical_device, size_t original_size)
{
	size_t min_uniform_buffer_offset_allignment = physical_device->properties.limits.minUniformBufferOffsetAlignment;
	size_t alligned_size = original_size;
	if (min_uniform_buffer_offset_allignment > 0) {
		alligned_size = (alligned_size + min_uniform_buffer_offset_allignment + 1) & 
					   ~(min_uniform_buffer_offset_allignment - 1);
	}

	return alligned_size;
}

VkDescriptorSetLayoutBinding bfGetDescriptorSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t binding)
{
	VkDescriptorSetLayoutBinding setbind = {};
	setbind.binding = binding;
	setbind.descriptorCount = 1;
	setbind.descriptorType = type;
	setbind.pImmutableSamplers = nullptr;
	setbind.stageFlags = stage_flags;

	return setbind;
}

VkWriteDescriptorSet bfWriteDescriptorBuffer(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo, uint32_t binding)
{
	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;

	write.dstBinding = binding;
	write.dstSet = dstSet;
	write.descriptorCount = 1;
	write.descriptorType = type;
	write.pBufferInfo = bufferInfo;

	return write;
}

VkImageCreateInfo bfPopulateDepthImageCreateInfo(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent)
{
	VkImageCreateInfo info {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.pNext = nullptr;

	info.imageType = VK_IMAGE_TYPE_2D;

	info.format = format;
	info.extent = extent;

	info.mipLevels = 1;
	info.arrayLayers = 1;
	info.samples = VK_SAMPLE_COUNT_1_BIT;
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.usage = usage_flags;

	return info;
}

VkImageViewCreateInfo bfPopulateDepthImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspect_flags)
{
	VkImageViewCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = nullptr;

	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.image = image;
	info.format = format;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;
	info.subresourceRange.aspectMask = aspect_flags;

	return info;
}

VkPipelineDepthStencilStateCreateInfo bfPopulateDepthStencilStateCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp)
{
	VkPipelineDepthStencilStateCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	info.pNext = nullptr;

	info.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE;
	info.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE;
	info.depthCompareOp = VK_COMPARE_OP_LESS;//bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
	info.depthBoundsTestEnable = VK_FALSE;
	//info.minDepthBounds = -10.0f; // Optional
	//info.maxDepthBounds = 10.0f; // Optional
	info.stencilTestEnable = VK_FALSE;

	return info;
}

BfEvent bfCleanUpSwapchain(BfBase& base)
{
	BfHolder* holder = bfGetpHolder();
	
	for (size_t i = 0; i < holder->standart_framebuffers.size(); i++) {
		vkDestroyFramebuffer(base.device, holder->standart_framebuffers[i], nullptr);
	}
	for (size_t i = 0; i < holder->gui_framebuffers.size(); i++) {
		vkDestroyFramebuffer(base.device, holder->gui_framebuffers[i], nullptr);
	}
	for (size_t i = 0; i < holder->image_views.size(); i++) {
		vkDestroyImageView(base.device, holder->image_views[i], nullptr);
	}

	vkDestroyImageView(base.device, base.depth_image_view, nullptr);
	vmaDestroyImage(base.allocator, base.depth_image.image, base.depth_image.allocation);

	vkDestroySwapchainKHR(base.device, base.swap_chain, nullptr);
	
	return BfEvent();
}

void bfUpdateUniformBuffer(BfBase& base)
{

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	BfUniformView ubo{};

	ubo.model = glm::scale(glm::mat4(1.0f), glm::vec3(base.x_scale, base.y_scale, 1.0f));
	
	//ubo.model = glm::rotate(ubo.model, time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 1.0f));


	//ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	
	//bfCalculateView(base.window);
	//std::cout << base.window->cam_mode << "\n";
	switch (base.window->cam_mode) {
	case 0:
		bfCalculateViewPartsFree(base.window);
		break;
	case 1:
		bfCalculateRotateView(base.window);
		//bfCalculateViewPartsS(base.window);
		//bfCalculateViewPartsFree(base.window);
		break;
	case 2:
		bfCalculateViewPartsS(base.window);
		break;
	}
	base.window->cam_mode = 99;
	
	
	//if (base.window->is_free_camera_active) {
	//	bfCalculateViewPartsFree(base.window);
	//	//std::cout << "FreeCam" << "\n";
	//}
	//else if (base.window->is_rotate_camera_active) {
	//	bfCalculateViewPartsFree(base.window);
	//	bfCalculateViewPartsS(base.window);
	//	//std::cout << "RotateCam" << "\n";
	//}
	//else if (base.window->is_s_camera_active || base.window->is_scroll) {
	//	bfCalculateViewPartsS(base.window);
	//	//std::cout << "SCam" << "\n";

	//}
	bfUpdateView(base.window);

	
	static float counter = -10.0f;

	ubo.view = base.window->view;
	

	//float R = 2.0f;

	//ubo.view = glm::lookAt(glm::vec3{ R * cos(glm::radians(counter/100)), R * sin(glm::radians(counter / 100)), -2.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f});


	/*std::cout << "pos:(" << base.window->pos.x << ", " << base.window->pos.y << ", " << base.window->pos.z
		<< "); front:(" << base.window->front.x << ", " << base.window->front.y << ", " << base.window->front.z
		<< "); up:(" << base.window->up.x << ", " << base.window->up.y << ", " << base.window->up.z << ")\n";*/
	
	

	if (base.window->proj_mode == 0) {
		ubo.proj = glm::perspective(glm::radians(45.0f), (float)base.swap_chain_extent.width / (float)base.swap_chain_extent.height, 0.01f, 100.0f);
	}
	else if (base.window->proj_mode == 1) {
		if (base.window->is_asp) {
			float asp = (float)base.swap_chain_extent.height / (float)base.swap_chain_extent.width;
			ubo.proj = glm::ortho(
								  base.window->ortho_left,
								  base.window->ortho_right,
								  base.window->ortho_bottom * asp,
								  base.window->ortho_top * asp,
								  base.window->ortho_near,
								  base.window->ortho_far
				/*0.0f,
				(float)(base.swap_chain_extent.width),
				(float)(base.swap_chain_extent.height),
				0.0f,
				0.1f,
				1000.0f*/

			);
			/*std::cout << "ASP" << "\n";
			std::cout << ubo.proj[0][0] << ", " << ubo.proj[0][1] << ", " << ubo.proj[0][2] << ", " << ubo.proj[0][3] << "\n";
			std::cout << ubo.proj[1][0] << ", " << ubo.proj[1][1] << ", " << ubo.proj[1][2] << ", " << ubo.proj[1][3] << "\n";
			std::cout << ubo.proj[2][0] << ", " << ubo.proj[2][1] << ", " << ubo.proj[2][2] << ", " << ubo.proj[2][3] << "\n";
			std::cout << ubo.proj[3][0] << ", " << ubo.proj[3][1] << ", " << ubo.proj[3][2] << ", " << ubo.proj[3][3] << "\n";*/
		}
		else {
			ubo.proj = glm::ortho(base.window->ortho_left,
								  base.window->ortho_right,
								  base.window->ortho_bottom,
								  base.window->ortho_top,
								  base.window->ortho_near,
								  base.window->ortho_far);
			/*std::cout << "no ASP" << "\n";
			std::cout << ubo.proj[0][0] << ", " << ubo.proj[0][1] << ", " << ubo.proj[0][2] << ", " << ubo.proj[0][3] << "\n";
			std::cout << ubo.proj[1][0] << ", " << ubo.proj[1][1] << ", " << ubo.proj[1][2] << ", " << ubo.proj[1][3] << "\n";
			std::cout << ubo.proj[2][0] << ", " << ubo.proj[2][1] << ", " << ubo.proj[2][2] << ", " << ubo.proj[2][3] << "\n";
			std::cout << ubo.proj[3][0] << ", " << ubo.proj[3][1] << ", " << ubo.proj[3][2] << ", " << ubo.proj[3][3] << "\n";*/
		}
		
		
	}
	
	
	
	//ubo.proj[1][1] *= -1;
	// uniformBuffersMapped[currentImage]
	//BfHolder* holder = bfGetpHolder();

	


	memcpy(base.frame_pack[base.current_frame].uniform_view_data, &ubo, sizeof(ubo));

	static int local_frame = base.current_frame;

	BfUniformBezierProperties prop{};
	prop.points_count = 3;
	base.vert_number = prop.points_count;
	
	int frameIndex = local_frame % MAX_FRAMES_IN_FLIGHT;
	
	char* bezier_data;
	vmaMapMemory(base.allocator, base.bezier_properties_uniform_buffer.allocation, (void**)&bezier_data);
	{
		bezier_data += bfPadUniformBufferSize(base.physical_device, sizeof(BfUniformBezierProperties)) * frameIndex;

		memcpy(bezier_data, &prop, sizeof(BfUniformBezierProperties));

	}
	vmaUnmapMemory(base.allocator, base.bezier_properties_uniform_buffer.allocation);

	void* bezier_points_data;

	std::vector<BfStorageBezierPoints> localvec = base.storage;

	for (auto& it : localvec) {
		it.coo += glm::vec2(base.px, base.py);
	}
	base.vert_number = static_cast<uint32_t>(localvec.size());
	/*for (int i = 0; i < prop.points_count; i++) {
		glm::vec2 lVec = glm::vec2((float)i/10, (float)i/10);
		localvec.push_back(BfStorageBezierPoints(lVec));
	}*/

	/*std::vector<BfStorageBezierPoints> localvec = {
		{{base.px, base.py}},
		{{base.px, base.py}},
		{{base.px, base.py}},
	};*/

	

	vmaMapMemory(base.allocator, base.frame_pack[base.current_frame].bezier_points_buffer->allocation, &bezier_points_data);
	{
		/*BfStorageBezierPoints* bezier_points_ssbo = (BfStorageBezierPoints*)bezier_points_data;
		for (int i = 0; i < prop.points_count; i++) {
			glm::vec2& lVec = localvec[i];
			
			bezier_points_ssbo[i].coo = localvec[i];
		}*/
		memcpy(bezier_points_data, localvec.data(), sizeof(BfStorageBezierPoints) * localvec.size());
	}
	vmaUnmapMemory(base.allocator, base.frame_pack[base.current_frame].bezier_points_buffer->allocation);

	// Onject data
	//BfMeshHandler* pHandler = BfMeshHandler::get_bound_handler();
	//std::vector<BfObjectData> objects_data(10);
		//pHandler->get_allocated_meshes_count());
	
	BfCurveHolder* pCurveHolder = bfGetpCurveHolder();

	pCurveHolder->update_obj_data(base.frame_pack[base.current_frame].model_matrix_buffer->allocation);
	
	//objects_data[0].model_matrix = glm::mat4(1.0f);//glm::scale(glm::mat4(1.0f), glm::vec3(base.x_scale, base.y_scale, 1.0f));
	//objects_data[1].model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(base.x_scale, base.y_scale, 1.0f));

	/*void* pobjects_data;
	vmaMapMemory(base.allocator, base.frame_pack[base.current_frame].model_matrix_buffer->allocation, &pobjects_data);
	{
		memcpy(pobjects_data, objects_data.data(), sizeof(BfObjectData) * objects_data.size());
	}
	vmaUnmapMemory(base.allocator, base.frame_pack[base.current_frame].model_matrix_buffer->allocation);*/
	counter += 1.0f;
	local_frame++;


	base.window->proj = ubo.proj;
	base.window->view = ubo.view;
}

