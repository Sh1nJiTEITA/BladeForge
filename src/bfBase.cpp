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
		
		BfHolder::__bfpHolder->physical_devices.push_back(bf_device);

		BfPhysicalDevice* pPhysicalDevice = &BfHolder::__bfpHolder->physical_devices.back();

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
		base.device, &layoutInfo, nullptr, &base.descriptor_set_layout
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

	auto bindingDescription = bfVertex::getBindingDescription();
	auto attributeDescriptions = bfVertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(
		attributeDescriptions.size());

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
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
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
	pipelineInfo.pDepthStencilState = nullptr;
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
		
		VkImageView attachments[] = {
			*base.image_packs[i].pImage_view
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = base.standart_render_pass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
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
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &base.descriptor_set_layout; // Optional
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

