#include "bfBase.h"

#include <fmt/base.h>
#include <fmt/color.h>
#include <imgui_impl_vulkan.h>
#include <memory>
#include <vulkan/vulkan_core.h>

#include <stdexcept>

#include "bfAllocator.h"
#include "bfCamera.h"
#include "bfDescriptorStructs.h"
#include "bfDrawObjectManager.h"
#include "bfEvent.h"
#include "bfLayerHandler.h"
#include "bfPipeline.h"
// #include "bfTexture.h"
#include "bfViewport.h"
#include "implot.h"

#define IMGUI_ENABLE_DOCKING

// Function definitions
BfEvent
bfCreateInstance(BfBase& base)
{
   if (enableValidationLayers)
   {
      fmt::print("::DEBUG MODE::\n");
   }

   // Check Validation layers support
   bool is_validation_layers_supported;

   uint32_t layerCount;
   vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

   std::vector< VkLayerProperties > avaliableLayers(layerCount);

   vkEnumerateInstanceLayerProperties(&layerCount, avaliableLayers.data());

   // Layers:\n";
   int i = 0;
   for (const auto& layer : avaliableLayers)
   {
      fmt::print("{}. {}\n", i, layer.layerName);
      i++;
   }

   i = 0;
   is_validation_layers_supported = true;
   for (auto& vLayer : bfvValidationLayers)
   {
      bool isLayer = false;
      fmt::print("{}) {}\n", i, vLayer);
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
      throw std::runtime_error(
          "Validation layers requested, but not available."
      );
   }
   // Get Required Extensions
   uint32_t glfwExtensionCount = 0;
   const char** glfwExtensions;
   glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

   // std::vector<const char*> extensions(glfwExtensions, glfwExternsions +
   // glfwExtensionCount);
   std::vector< const char* > extensions(
       glfwExtensions,
       glfwExtensions + glfwExtensionCount
   );

   if (enableValidationLayers)
   {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      extensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
   }

   // Create VkInstance
   VkApplicationInfo appInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
   appInfo.pApplicationName = "BladeForge";
   appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
   appInfo.pEngineName = nullptr;
   appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
   appInfo.apiVersion = VK_API_VERSION_1_3;

   VkInstanceCreateInfo instanceInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
   instanceInfo.pApplicationInfo = &appInfo;
   instanceInfo.enabledExtensionCount =
       static_cast< uint32_t >(extensions.size());
   instanceInfo.ppEnabledExtensionNames = extensions.data();

   VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
   VkValidationFeaturesEXT features{};

   if (enableValidationLayers)
   {
      instanceInfo.enabledLayerCount =
          static_cast< uint32_t >(bfvValidationLayers.size());

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

   VkResult is_instance_created =
       vkCreateInstance(&instanceInfo, nullptr, &base.instance);
   if (is_instance_created != VK_SUCCESS)
   {
      BfSingleEvent event{};
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_INSTANCE_FAILURE;
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

      return BfEvent(event);
   }
   else
   {
      BfSingleEvent event{};
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_INSTANCE_SUCCESS;
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

      return BfEvent(event);
   }
}

BfEvent
bfDestroyInstance(BfBase& base)
{
   vkDestroyInstance(base.instance, nullptr);

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_INSTANCE;

   return BfEvent(event);
}

BfEvent
bfDestroyDebufMessenger(BfBase& base)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;

   if (!enableValidationLayers)
   {
      event.action = BF_ACTION_TYPE_DESTROY_DEBUG_MESSENGER_NO_INIT;
      return event;
   }
   auto func = (PFN_vkDestroyDebugUtilsMessengerEXT
   )vkGetInstanceProcAddr(base.instance, "vkDestroyDebugUtilsMessengerEXT");
   if (func != nullptr)
   {
      func(base.instance, base.debug_messenger, nullptr);
      event.action = BF_ACTION_TYPE_DESTROY_DEBUG_MESSENGER_SUCCESS;
      return event;
   }
   else
   {
      event.action = BF_ACTION_TYPE_DESTROY_DEBUG_MESSENGER_FAILURE;
      return event;
   }
}

BfEvent
bfCreateDebugMessenger(BfBase& base)
{
   // If layer is disabled, то DebugMessenger is not neccesery
   if (!enableValidationLayers)
   {
      BfSingleEvent event{};
      event.action =
          BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_NO_INIT;
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      return BfEvent(event);
   }

   // Info about mess
   VkDebugUtilsMessengerCreateInfoEXT createInfo{};
   bfPopulateMessengerCreateInfo(createInfo);

   auto func = (PFN_vkCreateDebugUtilsMessengerEXT
   )vkGetInstanceProcAddr(base.instance, "vkCreateDebugUtilsMessengerEXT");
   if (func != nullptr)
   {
      func(base.instance, &createInfo, nullptr, &base.debug_messenger);

      BfSingleEvent event{};
      event.action =
          BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_SUCCESS;
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

      return BfEvent(event);
   }
   else
   {
      BfSingleEvent event{};
      event.action =
          BfEnActionType::BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_FAILURE;
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

      return BfEvent(event);
   }
}

BfEvent
bfCreateSurface(BfBase& base)
{
   BfSingleEvent event{};
   if (glfwCreateWindowSurface(
           base.instance,
           base.window->pWindow,
           nullptr,
           &base.surface
       ) != VK_SUCCESS)
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_SURFACE_FAILURE;
   }
   else
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_SURFACE_SUCCESS;
   }

   return BfEvent(event);
}

BfEvent
bfDestroySurface(BfBase& base)
{
   vkDestroySurfaceKHR(base.instance, base.surface, nullptr);

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_SURFACE;

   return BfEvent(event);
}

BfEvent
bfCreatePhysicalDevice(BfBase& base)
{
   uint32_t deviceCount = 0;
   vkEnumeratePhysicalDevices(base.instance, &deviceCount, nullptr);
   if (deviceCount == 0)
   {
      BfSingleEvent event{};
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_NO_GPU;
      return BfEvent(event);
   }

   // Else:
   std::vector< VkPhysicalDevice > devices(deviceCount);
   vkEnumeratePhysicalDevices(base.instance, &deviceCount, devices.data());

   // Check devices for suitablity
   for (const auto& device : devices)
   {
      BfPhysicalDevice bf_device{};
      bf_device.physical_device = device;

      BfHolder::__bfpHolder->physical_devices.push_back(bf_device);

      BfPhysicalDevice* pPhysicalDevice =
          &BfHolder::__bfpHolder->physical_devices.back();

      bool is_suitable;
      bfIsDeviceSuitable(pPhysicalDevice, base.surface, is_suitable);

      if (is_suitable)
      {
         fmt::println(
             "<================== PHYSICAL DEVICE IS SUITABLE "
             "====================>"
         );
         base.physical_device = pPhysicalDevice;

         // Get chosen physical device properties
         vkGetPhysicalDeviceProperties(
             base.physical_device->physical_device,
             &base.physical_device->properties
         );
         fmt::println(
             "Anisotropy max={}",
             base.physical_device->properties.limits.maxSamplerAnisotropy
         );

         break;
      }
   }

   if (base.physical_device->physical_device == VK_NULL_HANDLE)
   {
      BfSingleEvent event{};
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action =
          BfEnActionType::BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_FAILURE;
      return BfEvent(event);
   }
   else
   {
      BfSingleEvent event{};
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action =
          BfEnActionType::BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_SUCCESS;
      return BfEvent(event);
   }
}

BfEvent
bfCreateLogicalDevice(BfBase& base)
{
   std::set< uint32_t > uniqueQueueFamilies;
   std::transform(
       base.physical_device->queue_family_indices.cbegin(),
       base.physical_device->queue_family_indices.cend(),
       std::inserter(uniqueQueueFamilies, uniqueQueueFamilies.begin()),
       [](const std::pair< BfvEnQueueType, std::optional< uint32_t > >&
              key_value) { return key_value.second.value(); }
   );

   float queuePriority = 1.0f;

   std::vector< VkDeviceQueueCreateInfo > queueCreateInfos;
   for (uint32_t queueFamily : uniqueQueueFamilies)
   {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
   }

   VkPhysicalDeviceFeatures deviceFeatures{};
   deviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
   deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
   deviceFeatures.independentBlend = VK_TRUE;
   deviceFeatures.geometryShader = VK_TRUE;
   deviceFeatures.samplerAnisotropy = VK_TRUE;
   deviceFeatures.wideLines = VK_TRUE;

   VkDeviceCreateInfo createInfo{};
   createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

   createInfo.queueCreateInfoCount =
       static_cast< uint32_t >(queueCreateInfos.size());
   createInfo.pQueueCreateInfos = queueCreateInfos.data();

   createInfo.pEnabledFeatures = &deviceFeatures;

   // SWAP-CHAIN extensions (and others)
   createInfo.enabledExtensionCount =
       static_cast< uint32_t >(bfvDeviceExtensions.size());
   createInfo.ppEnabledExtensionNames = bfvDeviceExtensions.data();

   VkPhysicalDeviceShaderDrawParametersFeatures
       shader_draw_parameters_features = {};
   shader_draw_parameters_features.sType =
       VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
   shader_draw_parameters_features.pNext = nullptr;
   shader_draw_parameters_features.shaderDrawParameters = VK_TRUE;
   createInfo.pNext = &shader_draw_parameters_features;

   if (enableValidationLayers)
   {
      createInfo.enabledLayerCount =
          static_cast< uint32_t >(bfvValidationLayers.size());
      createInfo.ppEnabledLayerNames = bfvValidationLayers.data();
   }
   else
   {
      createInfo.enabledLayerCount = 0;
   }

   BfSingleEvent event{};
   if (vkCreateDevice(
           base.physical_device->physical_device,
           &createInfo,
           nullptr,
           &base.device
       ) != VK_SUCCESS)
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_LOGICAL_DEVICE_FAILURE;
   }
   else
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_LOGICAL_DEVICE_SUCCESS;
   }

   for (auto& it : base.physical_device->queue_family_indices)
   {
      vkGetDeviceQueue(
          base.device,
          it.second.value(),
          0,
          &base.physical_device->queues[it.first]
      );
   }
   return BfEvent(event);
}

BfEvent
bfDestroyLogicalDevice(BfBase& base)
{
   vkDestroyDevice(base.device, nullptr);

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_DEVICE;

   return BfEvent(event);
}

BfEvent
bfCreateSwapchain(BfBase& base)
{
   BfSwapChainSupport swapChainSupport;
   bfGetSwapChainSupport(
       base.physical_device->physical_device,
       base.surface,
       swapChainSupport
   );

   VkSurfaceFormatKHR surfaceFormat;
   bfGetSwapSurfaceFormat(swapChainSupport, surfaceFormat);

   VkPresentModeKHR presentMode;
   bfGetSwapPresentMode(swapChainSupport, presentMode);

   VkExtent2D extent;
   bfGetSwapExtent(swapChainSupport, base.window->pWindow, extent);

   uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

   if (swapChainSupport.capabilities.maxImageCount > 0 &&
       imageCount > swapChainSupport.capabilities.maxImageCount)
   {
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

   // QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
   /*
   uint32_t queueFamilyIndices[] = {
           indices.graphicsFamily.value(),
           indices.presentFamily.value()
   };
   */
   std::vector< uint32_t > queueFamilyIndices;
   for (auto& it : base.physical_device->queue_family_indices)
   {
      queueFamilyIndices.push_back(it.second.value());
   }

   // if (indices.graphicsFamily != indices.presentFamily) {
   // clang-format off
   if (base.physical_device->queue_family_indices[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE].value() !=
       base.physical_device->queue_family_indices[BfvEnQueueType::BF_QUEUE_PRESENT_TYPE].value() ||
       base.physical_device->queue_family_indices[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE].value() !=
       base.physical_device->queue_family_indices[BfvEnQueueType::BF_QUEUE_TRANSFER_TYPE].value() ||
       base.physical_device->queue_family_indices[BfvEnQueueType::BF_QUEUE_PRESENT_TYPE].value() !=
       base.physical_device->queue_family_indices[BfvEnQueueType::BF_QUEUE_TRANSFER_TYPE].value())
   // clang-format on
   {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
      createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
   }
   else
   {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      // createInfo.queueFamilyIndexCount = 0;
      // createInfo.pQueueFamilyIndices = nullptr;
   }
   createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
   createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
   createInfo.presentMode = presentMode;

   createInfo.clipped = VK_TRUE;
   createInfo.oldSwapchain = VK_NULL_HANDLE;

   BfSingleEvent event{};
   if (vkCreateSwapchainKHR(
           base.device,
           &createInfo,
           nullptr,
           &base.swap_chain
       ) != VK_SUCCESS)
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_SWAPCHAIN_FAILURE;
   }
   else
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_SWAPCHAIN_SUCCESS;
   }

   vkGetSwapchainImagesKHR(base.device, base.swap_chain, &imageCount, nullptr);
   base.image_packs.resize(imageCount);
   bfGetpHolder()->images.resize(imageCount);
   vkGetSwapchainImagesKHR(
       base.device,
       base.swap_chain,
       &imageCount,
       bfGetpHolder()->images.data()
   );

   for (size_t i = 0; i < bfGetpHolder()->images.size(); i++)
   {
      base.image_packs[i].pImage = &bfGetpHolder()->images[i];
   }

   base.swap_chain_format = surfaceFormat.format;
   base.swap_chain_extent = extent;
   base.image_pack_count = imageCount;

   return BfEvent(event);
}

BfEvent
bfDestroySwapchain(BfBase& base)
{
   vkDestroySwapchainKHR(base.device, base.swap_chain, nullptr);

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_SWAPCHAIN;

   return BfEvent(event);
}

BfEvent
bfCreateImageViews(BfBase& base)
{
   // NUMBER OF IMAGES -> NUMBER OF VIEWS
   BfHolder* holder = bfGetpHolder();
   holder->image_views.resize(base.image_pack_count);

   for (size_t i = 0; i < base.image_pack_count; i++)
   {
      base.image_packs[i].pImage_view = &holder->image_views[i];
   }

   std::stringstream ss_s;
   std::stringstream ss_f;

   ss_s << "Successfully done image indices = [";
   ss_f << "Unsuccessfully done image indices = [";

   bool is_image_views_created = true;

   // Go through all images
   for (size_t i = 0; i < holder->image_views.size(); i++)
   {
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
      if (vkCreateImageView(
              base.device,
              &createInfo,
              nullptr,
              base.image_packs[i].pImage_view
          ) == VK_SUCCESS)
      {
         if (i != base.image_pack_count - 1)
            ss_s << i << "][";
         else
            ss_s << i << "] ";
      }
      else
      {
         is_image_views_created = false;
         if (i != base.image_pack_count - 1)
            ss_f << i << "][";
         else
            ss_f << i << "] ";
      }
   }

   BfSingleEvent event{};
   event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   if (is_image_views_created)
   {
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_IMAGE_VIEWS_SUCCESS;
      event.info = ss_s.str();
   }
   else
   {
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_IMAGE_VIEWS_FAILURE;
      ss_s << ss_f.str();
      event.info = ss_s.str();
   }
   return BfEvent(event);
}

BfEvent
bfDestroyImageViews(BfBase& base)
{
   auto holder = bfGetpHolder();
   for (size_t i = 0; i < holder->image_views.size(); i++)
   {
      vkDestroyImageView(
          base.device,
          *base.image_packs[i].pImage_view,
          nullptr
      );
   }

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_IMAGE_VIEWS;

   return BfEvent(event);
}

BfEvent
bfCreateStandartRenderPass(BfBase& base)
{
   // Color attachment
   // Id map attachment
   VkAttachmentDescription id_map_attachment{};
   id_map_attachment.format = VK_FORMAT_R32_UINT;
   id_map_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
   id_map_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
   id_map_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
   id_map_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   id_map_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
   id_map_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
   id_map_attachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

   VkAttachmentReference id_map_attachment_ref{};
   id_map_attachment_ref.attachment = 1;
   id_map_attachment_ref.layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

   // Color attachment
   VkAttachmentDescription colorAttachment{};
   colorAttachment.format = base.swap_chain_format;
   colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
   colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
   colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
   colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
   colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
   colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

   VkAttachmentReference colorAttachmentRef{};
   colorAttachmentRef.attachment = 0;
   colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

   std::vector< VkAttachmentReference > attachment_refs{
       colorAttachmentRef,
       id_map_attachment_ref
   };

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
   depth_attachment.finalLayout =
       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

   VkAttachmentReference depth_attachment_ref = {};
   depth_attachment_ref.attachment = 2;
   depth_attachment_ref.layout =
       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

   // How to use subPass
   VkSubpassDescription subpass{};
   subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
   subpass.colorAttachmentCount = attachment_refs.size();
   subpass.pColorAttachments = attachment_refs.data();
   subpass.pDepthStencilAttachment = &depth_attachment_ref;

   // Dependencies

   // id depen
   VkSubpassDependency id_dependency{};
   id_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
   id_dependency.dstSubpass = 0;
   id_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
   id_dependency.srcAccessMask = 0;
   id_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
   id_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

   // Color depen
   VkSubpassDependency dependency{};
   dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
   dependency.dstSubpass = 0;
   dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
   dependency.srcAccessMask = 0;
   dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
   dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

   // Depth depen
   VkSubpassDependency depth_dependency{};
   depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
   depth_dependency.dstSubpass = 0;
   depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                                   VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
   depth_dependency.srcAccessMask = 0;
   depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                                   VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
   depth_dependency.dstAccessMask =
       VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

   // Create render pass

   std::vector< VkAttachmentDescription > attachments{
       colorAttachment,
       id_map_attachment,
       depth_attachment
   };

   std::vector< VkSubpassDependency > dependencies{
       dependency,
       id_dependency,
       depth_dependency
   };

   VkRenderPassCreateInfo renderPassInfo{};
   renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
   renderPassInfo.subpassCount = 1;
   renderPassInfo.pSubpasses = &subpass;
   renderPassInfo.attachmentCount = attachments.size();
   renderPassInfo.pAttachments = attachments.data();
   renderPassInfo.dependencyCount = dependencies.size();
   renderPassInfo.pDependencies = dependencies.data();

   BfSingleEvent event{};
   if (vkCreateRenderPass(
           base.device,
           &renderPassInfo,
           nullptr,
           &base.standart_render_pass
       ) == VK_SUCCESS)
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_RENDER_PASS_SUCCESS;
      event.info = " Standart render pass";
   }
   else
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_RENDER_PASS_FAILURE;
      event.info = " Standart render pass";
   }

   return BfEvent(event);
}

BfEvent
bfDestroyStandartRenderPass(BfBase& base)
{
   vkDestroyRenderPass(base.device, base.standart_render_pass, nullptr);

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_MAIN_RENDER_PASS;

   return BfEvent(event);
}

BfEvent
bfCreateGUIRenderPass(BfBase& base)
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
   dependency.srcAccessMask = 0; // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
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
   if (vkCreateRenderPass(base.device, &info, nullptr, &base.gui_render_pass) ==
       VK_SUCCESS)
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_RENDER_PASS_SUCCESS;
      event.info = " GUI render pass";
   }
   else
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_INIT_RENDER_PASS_FAILURE;
      event.info = " GUI render pass";
   }

   return BfEvent(event);
}

BfEvent
bfDestroyGUIRenderPass(BfBase& base)
{
   vkDestroyDescriptorPool(base.device, base.gui_descriptor_pool, nullptr);
   vkDestroyRenderPass(base.device, base.gui_render_pass, nullptr);

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_IMGUI_RENDER_PASS;

   return BfEvent(event);
}

BfEvent
bfDestroyOwnDescriptors(BfBase& base)
{
   return BfEvent();
}

BfEvent
bfInitOwnDescriptors(BfBase& base)
{
   bfCreateBuffer(
       &base.id_image_buffer,
       BfAllocator::get(),
       3 * sizeof(uint32_t),
       VK_BUFFER_USAGE_TRANSFER_DST_BIT,
       VMA_MEMORY_USAGE_AUTO,
       VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
           VMA_ALLOCATION_CREATE_MAPPED_BIT
   );
   return BfEvent();
}

void
bfPopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
   if (!enableValidationLayers)
      return;

   createInfo = {};
   createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
   createInfo.messageSeverity = {
       VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |    // Just info (e.g.
                                                         // creation info)
       VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | // Diagnostic info
       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | // Warning (bug)
       VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT     // Warning (potential
                                                         // crush)
   };
   createInfo.messageType = {
       VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |    // Unrelated perfomance
                                                        // (mb ok)
       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | // Possiable mistake
                                                        // (not ok)
       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT  // Potential non-optimal
                                                        // use of VK
   };
   createInfo.pfnUserCallback = bfvDebugCallback;
}

BfEvent
bfaReadFile(std::vector< char >& data, const std::string& filename)
{
   std::ifstream file(
       // FILE-name
       filename,
       // Modes
       std::ios::ate |      // Read from the end
           std::ios::binary // Read file as binary (avoid text transformations)
   );

   BfSingleEvent event{};

   std::stringstream ss;
   ss << "File path = " << filename;
   event.info = ss.str();

   if (!file.is_open())
   {
      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_READ_SHADER_FILE_FAILURE;
      event.success = false;
      return BfSingleEvent(event);
   }
   else
   {
      size_t fileSize = (size_t)file.tellg();
      data.clear();
      data.resize(fileSize);

      file.seekg(0);
      file.read(data.data(), fileSize);
      file.close();

      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_READ_SHADER_FILE_SUCCESS;
      event.success = true;
      return BfSingleEvent(event);
   }
}

// BfEvent
// bfCreateGraphicsPipelines(BfBase &base)
// {
//    std::vector<VkShaderModule> modules_basic;
//    std::vector<VkPipelineShaderStageCreateInfo> infos_basic;
//    bfaCreateShaderCreateInfos(
//        base.device,
//        "shaders/basic",
//        modules_basic,
//        infos_basic
//    );
//
//    std::vector<VkShaderModule> modules_tlines;
//    std::vector<VkPipelineShaderStageCreateInfo> infos_tlines;
//    bfaCreateShaderCreateInfos(
//        base.device,
//        "shaders/thick_lines",
//        modules_tlines,
//        infos_tlines
//    );
//
//    std::vector<VkShaderModule> modules_lines;
//    std::vector<VkPipelineShaderStageCreateInfo> infos_lines;
//    bfaCreateShaderCreateInfos(
//        base.device,
//        "shaders/lines",
//        modules_lines,
//        infos_lines
//    );
//
//    // Vertex Data
//    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
//    vertexInputInfo.sType =
//        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//
//    auto bindingDescription = BfVertex3::getBindingDescription();
//    auto attributeDescriptions = BfVertex3::getAttributeDescriptions();
//
//    vertexInputInfo.vertexBindingDescriptionCount = 1;
//    vertexInputInfo.vertexAttributeDescriptionCount =
//        static_cast<uint32_t>(attributeDescriptions.size());
//    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
//    vertexInputInfo.pVertexAttributeDescriptions =
//    attributeDescriptions.data();
//
//    // How to use vertex data
//    VkPipelineInputAssemblyStateCreateInfo inputAssembly_triangles{};
//    inputAssembly_triangles.sType =
//        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//    inputAssembly_triangles.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//    inputAssembly_triangles.primitiveRestartEnable = VK_FALSE;
//
//    VkPipelineInputAssemblyStateCreateInfo inputAssembly_lines{};
//    inputAssembly_lines.sType =
//        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//    inputAssembly_lines.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
//    inputAssembly_lines.primitiveRestartEnable = VK_FALSE;
//
//    /*VkPipelineShaderStageCreateInfo lines_shaderStages[] = {
//            lines_vertShaderCreateInfo,
//            lines_geomShaderCreateInfo,
//            lines_fragShaderCreateInfo
//    };*/
//    //// Viewports
//    // VkViewport viewport{};
//    // viewport.x = 0.0f;
//    // viewport.y = 0.0f;
//
//    // viewport.width = (float)swapChainExtent.width;
//    // viewport.height = (float)swapChainExtent.height;
//
//    // viewport.minDepth = 0.0f;
//    // viewport.maxDepth = 1.0f;
//
//    //// Scissors
//    // VkRect2D scissor{};
//    // scissor.offset = { 0, 0 };
//    // scissor.extent = swapChainExtent;
//
//    // Dynamic viewport/scissors
//    std::vector<VkDynamicState> dynamicStates = {
//        VK_DYNAMIC_STATE_VIEWPORT,
//        VK_DYNAMIC_STATE_SCISSOR,
//        // VK_DYNAMIC_STATE_BLEND_CONSTANTS
//    };
//
//    VkPipelineDynamicStateCreateInfo dynamicState{};
//    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//    dynamicState.dynamicStateCount =
//    static_cast<uint32_t>(dynamicStates.size()); dynamicState.pDynamicStates =
//    dynamicStates.data();
//
//    VkPipelineViewportStateCreateInfo viewportState{};
//    viewportState.sType =
//    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//    viewportState.viewportCount = 1;
//    viewportState.scissorCount = 1;
//
//    // Rasterizer
//    VkPipelineRasterizationStateCreateInfo rasterizer{};
//    rasterizer.sType =
//        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//    rasterizer.depthClampEnable = VK_FALSE;
//    rasterizer.rasterizerDiscardEnable = VK_FALSE;
//    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//    rasterizer.lineWidth = 1.0f;
//    rasterizer.cullMode = VK_CULL_MODE_NONE;
//    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//    // rasterizer.depthBiasEnable = VK_TRUE;
//    // rasterizer.depthBiasConstantFactor = 0.0f; // Optional
//    // rasterizer.depthBiasClamp = 0.0f; // Optional
//    // rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
//
//    // Multisampling
//    VkPipelineMultisampleStateCreateInfo multisampling{};
//    multisampling.sType =
//        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//    multisampling.sampleShadingEnable = VK_FALSE;
//    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//    multisampling.minSampleShading = 1.0f;           // Optional
//    multisampling.pSampleMask = nullptr;             // Optional
//    multisampling.alphaToCoverageEnable = VK_FALSE;  // Optional
//    multisampling.alphaToOneEnable = VK_FALSE;       // Optional
//
//    // Color blending
//    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//    colorBlendAttachment.colorWriteMask = {
//        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
//        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
//    };
//    colorBlendAttachment.blendEnable = VK_FALSE;
//    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   //
//    Optional colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
//    // Optional colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; //
//    Optional colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
//    // Optional colorBlendAttachment.dstAlphaBlendFactor =
//    VK_BLEND_FACTOR_ZERO;  // Optional colorBlendAttachment.alphaBlendOp =
//    VK_BLEND_OP_ADD;              // Optional
//
//    // id Color blending
//    VkPipelineColorBlendAttachmentState idBlendAttachment{};
//    idBlendAttachment.colorWriteMask = {VK_COLOR_COMPONENT_R_BIT};
//    idBlendAttachment.blendEnable = VK_FALSE;
//    idBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
//    idBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
//    idBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
//    idBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
//    idBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
//    idBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
//
//    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments = {
//        colorBlendAttachment,
//        idBlendAttachment
//    };
//
//    VkPipelineColorBlendStateCreateInfo colorBlending{};
//    colorBlending.sType =
//        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//    colorBlending.logicOpEnable = VK_FALSE;
//    colorBlending.logicOp = VK_LOGIC_OP_COPY;  // Optional
//    colorBlending.attachmentCount = colorBlendAttachments.size();
//    colorBlending.pAttachments = colorBlendAttachments.data();
//    colorBlending.blendConstants[0] = 0.0f;  // Optional
//    colorBlending.blendConstants[1] = 0.0f;  // Optional
//    colorBlending.blendConstants[2] = 0.0f;  // Optional
//    colorBlending.blendConstants[3] = 0.0f;  // Optional
//
//    // Depth-stencil part
//    auto depth_stencil_state = bfPopulateDepthStencilStateCreateInfo(
//        true,
//        true,
//        VK_COMPARE_OP_LESS_OR_EQUAL
//    );
//
//    bool is_pipelines_created = true;
//    std::stringstream ss;
//
//    // Uniforms
//    bfaCreateGraphicsPipelineLayouts(base);
//
//    VkGraphicsPipelineCreateInfo pipelineInfo{};
//    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//    pipelineInfo.pVertexInputState = &vertexInputInfo;
//    pipelineInfo.pViewportState = &viewportState;
//    pipelineInfo.pRasterizationState = &rasterizer;
//    pipelineInfo.pMultisampleState = &multisampling;
//    pipelineInfo.pDepthStencilState = &depth_stencil_state;
//    pipelineInfo.pColorBlendState = &colorBlending;
//    pipelineInfo.pDynamicState = &dynamicState;
//    pipelineInfo.renderPass = base.standart_render_pass;
//    pipelineInfo.subpass = 0;
//    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
//    pipelineInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
//
//    // basic (triangles)
//    pipelineInfo.stageCount = infos_basic.size();
//    pipelineInfo.pStages = infos_basic.data();
//    pipelineInfo.pInputAssemblyState = &inputAssembly_triangles;
//    pipelineInfo.layout = base.triangle_pipeline_layout;
//    if (vkCreateGraphicsPipelines(
//            base.device,
//            VK_NULL_HANDLE,
//            1,
//            &pipelineInfo,
//            nullptr,
//            &base.triangle_pipeline
//        ) != VK_SUCCESS)
//    {
//       is_pipelines_created = false;
//       ss << "Base pipeline (triangles) wasn't created;";
//    }
//
//    // tline
//    pipelineInfo.stageCount = infos_tlines.size();
//    pipelineInfo.pStages = infos_tlines.data();
//    pipelineInfo.pInputAssemblyState = &inputAssembly_lines;
//    pipelineInfo.layout = base.tline_pipeline_layout;
//    if (vkCreateGraphicsPipelines(
//            base.device,
//            VK_NULL_HANDLE,
//            1,
//            &pipelineInfo,
//            nullptr,
//            &base.tline_pipeline
//        ) != VK_SUCCESS)
//    {
//       is_pipelines_created = false;
//       ss << "Base pipeline (slines) wasn't created;";
//    }
//
//    pipelineInfo.stageCount = infos_lines.size();
//    pipelineInfo.pStages = infos_lines.data();
//    pipelineInfo.pInputAssemblyState = &inputAssembly_lines;
//    pipelineInfo.layout = base.line_pipeline_layout;
//    // pipelineInfo.basePipelineIndex = -1;
//    // pipelineInfo.basePipelineHandle = base.triangle_pipeline;
//
//    if (vkCreateGraphicsPipelines(
//            base.device,
//            VK_NULL_HANDLE,
//            1,
//            &pipelineInfo,
//            nullptr,
//            &base.line_pipeline
//        ) != VK_SUCCESS)
//    {
//       is_pipelines_created = false;
//       ss << "Derivative pipeline (lines) wasn't created;";
//    }
//
//    for (auto &mod : modules_tlines)
//    {
//       vkDestroyShaderModule(base.device, mod, nullptr);
//    }
//
//    for (auto &mod : modules_basic)
//    {
//       vkDestroyShaderModule(base.device, mod, nullptr);
//    }
//
//    for (auto &mod : modules_lines)
//    {
//       vkDestroyShaderModule(base.device, mod, nullptr);
//    }
//
//    /*vkDestroyShaderModule(base.device, vertShaderModule, nullptr);
//    vkDestroyShaderModule(base.device, fragShaderModule, nullptr);
//    vkDestroyShaderModule(base.device, geomShaderModule, nullptr);*/
//
//    BfSingleEvent event{};
//    event.type =
//    BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
//
//    if (is_pipelines_created)
//    {
//       event.action =
//           BfEnActionType::BF_ACTION_TYPE_CREATE_GRAPHICS_PIPELINE_SUCCESS;
//       event.info = "All pipelines is OK";
//    }
//    else
//    {
//       event.action =
//           BfEnActionType::BF_ACTION_TYPE_CREATE_GRAPHICS_PIPELINE_FAILURE;
//       event.info = ss.str();
//    }
//    return BfEvent(event);
// }
//
// BfEvent
// bfDestroyGraphicsPipelines(BfBase &base)
// {
//    vkDestroyPipeline(base.device, base.line_pipeline, nullptr);
//    vkDestroyPipeline(base.device, base.tline_pipeline, nullptr);
//    vkDestroyPipeline(base.device, base.triangle_pipeline, nullptr);
//
//    // vkDestroyDescriptorSetLayout(base.device, base.tline_pipeline_layout,
//    // nullptr); vkDestroyDescriptorSetLayout(base.device,
//    // base.triangle_pipeline_layout, nullptr);
//    // vkDestroyDescriptorSetLayout(base.device, base.line_pipeline_layout,
//    // nullptr);
//    //
//    BfSingleEvent event{};
//    event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
//    event.action = BF_ACTION_TYPE_DESTROY_PIPELINES;
//
//    return BfEvent(event);
// }

BfEvent
bfCreateStandartFrameBuffers(BfBase& base)
{
   BfHolder* holder = bfGetpHolder();
   holder->standart_framebuffers.resize(base.image_pack_count);

   std::stringstream ss_s;
   ss_s << "Correctly created standart framebuffers indices = [";
   std::stringstream ss_f;
   ss_f << "Incorrectly created standart framebuffers indices = [";
   BfSingleEvent event{};
   event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

   bool is_all_framebuffers_was_created = true;

   for (size_t i = 0; i < base.image_pack_count; i++)
   {
      base.image_packs[i].pStandart_Buffer = &holder->standart_framebuffers[i];

      std::vector< VkImageView > attachments = {
          *base.image_packs[i].pImage_view,
          *base.image_packs[i].pImage_view_id,
          base.depth_image.view
          // base.depth_image_view
      };

      VkFramebufferCreateInfo framebufferInfo{};
      framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferInfo.renderPass = base.standart_render_pass;
      framebufferInfo.attachmentCount = attachments.size();
      framebufferInfo.pAttachments = attachments.data();
      framebufferInfo.width = base.swap_chain_extent.width;
      framebufferInfo.height = base.swap_chain_extent.height;
      framebufferInfo.layers = 1;

      if (vkCreateFramebuffer(
              base.device,
              &framebufferInfo,
              nullptr,
              base.image_packs[i].pStandart_Buffer
          ) == VK_SUCCESS)
      {
         if (i != base.image_pack_count - 1)
            ss_s << i << "][";
         else
            ss_s << i << "]";
      }
      else
      {
         is_all_framebuffers_was_created = false;

         if (i != base.image_pack_count - 1)
            ss_f << i << "][";
         else
            ss_f << i << "]";
      }
   }

   if (is_all_framebuffers_was_created)
   {
      event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_FRAME_BUFFER_SUCCESS;
      event.info = ss_s.str();
   }
   else
   {
      event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_FRAME_BUFFER_FAILURE;
      event.info = ss_s.str().append(ss_f.str());
   }

   return BfEvent(event);
}

BfEvent
bfDestroyStandartFrameBuffers(BfBase& base)
{
   auto holder = bfGetpHolder();

   for (uint32_t i = 0; i < base.image_pack_count; i++)
   {
      vkDestroyFramebuffer(
          base.device,
          holder->standart_framebuffers[i],
          nullptr
      );
   }

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_MAIN_FRAMEBUFFERS;

   return BfEvent(event);
}

BfEvent
bfCreateGUIFrameBuffers(BfBase& base)
{
   BfHolder* holder = bfGetpHolder();
   holder->gui_framebuffers.resize(base.image_pack_count);

   std::stringstream ss_s;
   ss_s << "Correctly created GUI framebuffers indices = [";
   std::stringstream ss_f;
   ss_f << "Incorrectly created GUI framebuffers indices = [";
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

      if (vkCreateFramebuffer(
              base.device,
              &info,
              nullptr,
              base.image_packs[i].pGUI_buffer
          ) == VK_SUCCESS)
      {
         if (i != base.image_pack_count - 1)
            ss_s << i << "][";
         else
            ss_s << i << "]";
      }
      else
      {
         is_all_framebuffers_was_created = false;

         if (i != base.image_pack_count - 1)
            ss_f << i << "][";
         else
            ss_f << i << "]";
      }
   }

   if (is_all_framebuffers_was_created)
   {
      event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_FRAME_BUFFER_SUCCESS;
      event.info = ss_s.str();
   }
   else
   {
      event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_FRAME_BUFFER_FAILURE;
      event.info = ss_s.str().append(ss_f.str());
   }

   return BfEvent(event);
}

BfEvent
bfDestroyGUIFrameBuffers(BfBase& base)
{
   auto holder = bfGetpHolder();

   for (uint32_t i = 0; i < base.image_pack_count; i++)
   {
      vkDestroyFramebuffer(base.device, holder->gui_framebuffers[i], nullptr);
   }

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_IMGUI_FRAMEBUFFERS;

   return BfEvent(event);
}

BfEvent
bfCreateCommandPool(BfBase& base)
{
   VkCommandPoolCreateInfo poolInfo{};
   poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
   poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
   poolInfo.queueFamilyIndex =
       base.physical_device
           ->queue_family_indices[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE]
           .value();

   BfSingleEvent event{};
   if (vkCreateCommandPool(
           base.device,
           &poolInfo,
           nullptr,
           &base.command_pool
       ) == VK_SUCCESS)
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_COMMAND_POOL_SUCCESS;
   }
   else
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_CREATE_COMMAND_POOL_FAILURE;
   }

   return BfEvent(event);
}

BfEvent
bfDestroyCommandPool(BfBase& base)
{
   vkDestroyCommandPool(base.device, base.command_pool, nullptr);

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_COMMAND_POOL;

   return BfEvent(event);
}

BfEvent
bfCreateGUIDescriptorPool(BfBase& base)
{
   VkDescriptorPoolSize pool_sizes[] = {
       {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
   };
   VkDescriptorPoolCreateInfo pool_info = {};
   pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
   pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
   pool_info.maxSets = 1;
   pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
   pool_info.pPoolSizes = pool_sizes;
   if (vkCreateDescriptorPool(
           base.device,
           &pool_info,
           nullptr,
           &base.gui_descriptor_pool
       ) != VK_SUCCESS)
   {
      throw std::runtime_error("ImGUI descriptor pool wasn't created");
   }
   return BfEvent();
}
BfEvent
bfDestroyGUIDescriptorPool(BfBase& base)
{
   vkDestroyDescriptorPool(base.device, base.gui_descriptor_pool, nullptr);

   // vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout
   // descriptorSetLayout, const VkAllocationCallbacks *pAllocator)

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_IMGUI_DESCRIPTOR_POOL;

   return BfEvent(event);
}

BfEvent
bfCreateStandartCommandBuffers(BfBase& base)
{
   BfHolder* holder = bfGetpHolder();

   if (holder->standart_command_buffers.size() != MAX_FRAMES_IN_FLIGHT)
   {
      holder->standart_command_buffers.resize(MAX_FRAMES_IN_FLIGHT);
   }
   if (base.frame_pack.size() != MAX_FRAMES_IN_FLIGHT)
   {
      base.frame_pack.resize(MAX_FRAMES_IN_FLIGHT);
   }

   VkCommandBufferAllocateInfo allocInfo{};
   allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
   allocInfo.commandPool = base.command_pool;
   allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
   allocInfo.commandBufferCount =
       static_cast< uint32_t >(holder->standart_command_buffers.size());

   if (vkAllocateCommandBuffers(
           base.device,
           &allocInfo,
           holder->standart_command_buffers.data()
       ) != VK_SUCCESS)
   {
      throw std::runtime_error("Command buffer was not allocated");
   }

   for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
   {
      base.frame_pack[i].standart_command_buffer =
          &holder->standart_command_buffers[i];
   }

   return BfEvent();
}

BfEvent
bfDestroyStandartCommandBuffers(BfBase& base)
{
   auto holder = bfGetpHolder();

   vkFreeCommandBuffers(
       base.device,
       base.command_pool,
       holder->standart_command_buffers.size(),
       holder->standart_command_buffers.data()
   );

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_MAIN_COMMAND_BUFFERS;

   return BfEvent(event);
}

BfEvent
bfCreateGUICommandBuffers(BfBase& base)
{
   BfHolder* holder = bfGetpHolder();

   if (holder->gui_command_buffers.size() != MAX_FRAMES_IN_FLIGHT)
   {
      holder->gui_command_buffers.resize(MAX_FRAMES_IN_FLIGHT);
   }

   VkCommandBufferAllocateInfo allocInfo{};
   allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
   allocInfo.commandPool = base.command_pool;
   allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
   allocInfo.commandBufferCount =
       static_cast< uint32_t >(holder->gui_command_buffers.size());

   if (vkAllocateCommandBuffers(
           base.device,
           &allocInfo,
           holder->gui_command_buffers.data()
       ) != VK_SUCCESS)
   {
      throw std::runtime_error("ImGUI Command buffer was not allocated");
   }

   for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
   {
      base.frame_pack[i].gui_command_buffer = &holder->gui_command_buffers[i];
   }

   return BfEvent();
}

BfEvent
bfDestroyGUICommandBuffers(BfBase& base)
{
   auto holder = bfGetpHolder();

   vkFreeCommandBuffers(
       base.device,
       base.command_pool,
       holder->gui_command_buffers.size(),
       holder->gui_command_buffers.data()
   );

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_IMGUI_COMMAND_BUFFERS;

   return BfEvent(event);
}

BfEvent
bfCreateSyncObjects(BfBase& base)
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

   for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
   {
      base.frame_pack[i].available_image_semaphore =
          &holder->available_image_semaphores[i];
      base.frame_pack[i].finish_render_image_semaphore =
          &holder->finish_render_image_semaphores[i];
      base.frame_pack[i].frame_in_flight_fence =
          &holder->frame_in_flight_fences[i];

      if (vkCreateSemaphore(
              base.device,
              &semaphoreInfo,
              nullptr,
              base.frame_pack[i].available_image_semaphore
          ) != VK_SUCCESS ||

          vkCreateSemaphore(
              base.device,
              &semaphoreInfo,
              nullptr,
              base.frame_pack[i].finish_render_image_semaphore
          ) != VK_SUCCESS ||

          vkCreateFence(
              base.device,
              &fenceInfo,
              nullptr,
              base.frame_pack[i].frame_in_flight_fence
          ) != VK_SUCCESS)
      {
         throw std::runtime_error("Semaphore or fence weren't created");
      }
   }

   return BfEvent();
}

BfEvent
bfDestorySyncObjects(BfBase& base)
{
   auto holder = bfGetpHolder();
   for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
   {
      vkDestroySemaphore(
          base.device,
          holder->available_image_semaphores[i],
          nullptr
      );
      vkDestroySemaphore(
          base.device,
          holder->finish_render_image_semaphores[i],
          nullptr
      );
      vkDestroyFence(base.device, holder->frame_in_flight_fences[i], nullptr);
   }

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_SYNC_OBJECTS;

   return BfEvent(event);
}

BfEvent
bfInitImGUI(BfBase& base)
{
   // Setup Dear ImGui context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImPlot::CreateContext();

   return BfEvent();
}

BfEvent
bfPostInitImGui(BfBase& base)
{
   ImGuiIO& io = ImGui::GetIO();
   (void)io;
   io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

#if defined(_WIN32)
   io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif

   ImGui::StyleColorsClassic();

   // Setup Platform/Renderer bindings
   ImGui_ImplGlfw_InitForVulkan(base.window->pWindow, true);
   ImGui_ImplVulkan_InitInfo init_info = {};
   init_info.Instance = base.instance;
   init_info.PhysicalDevice = base.physical_device->physical_device;
   init_info.Device = base.device;
   init_info.QueueFamily =
       base.physical_device
           ->queue_family_indices[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE]
           .value();
   init_info.Queue =
       base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE];
   init_info.PipelineCache = VK_NULL_HANDLE;
   init_info.DescriptorPool = base.gui_descriptor_pool;
   init_info.Allocator = nullptr;
   init_info.RenderPass = base.gui_render_pass;

   BfSwapChainSupport swapChainSupport;
   bfGetSwapChainSupport(
       base.physical_device->physical_device,
       base.surface,
       swapChainSupport
   );

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

   if (true == false)
   { // FONTS TODO: RECREATE LOGIC IN OTHER FUNCTION
      //

      ImFontConfig config;

      config.GlyphOffset.y = 2.0f;
      config.SizePixels = 10.0f;

      io.Fonts->AddFontFromFileTTF(
          "./resources/fonts/Cousine-Regular.ttf",
          16.0f,
          &config
      );

      config.MergeMode = true;
      config.GlyphMinAdvanceX = 13.0f;

      //
      static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
      io.Fonts->AddFontFromFileTTF(
          "./resources/fonts/fa-solid-900.ttf",
          16.0f,
          &config,
          icon_ranges
      );
   }

   // ????????????????????????????????????
   // ImGui_ImplVulkan_CreateFontsTexture();
   ImGui_ImplVulkan_Init(&init_info);

   return BfEvent();
}

BfEvent
bfDestroyImGUI(BfBase& base)
{
   ImPlot::DestroyContext();
   ImGui_ImplVulkan_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_IMGUI;

   return BfEvent(event);
}

BfEvent
bfCreateDepthBuffer(BfBase& base)
{
   VkExtent3D depthImageExtent =
       {base.swap_chain_extent.width, base.swap_chain_extent.height, 1};

   base.depth_format = VK_FORMAT_D32_SFLOAT;

   VkImageCreateInfo dimg_info = bfPopulateDepthImageCreateInfo(
       base.depth_format,
       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
       depthImageExtent
   );

   VmaAllocationCreateInfo dimg_allocinfo{};
   dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
   dimg_allocinfo.requiredFlags =
       VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

   auto res = vmaCreateImage(
       BfAllocator::get(),
       &dimg_info,
       &dimg_allocinfo,
       &base.depth_image.image,
       &base.depth_image.allocation,
       nullptr
   );
   base.depth_image.allocator = BfAllocator::get();

   VkImageViewCreateInfo dview_info = bfPopulateDepthImageViewCreateInfo(
       base.depth_format,
       base.depth_image.image,
       VK_IMAGE_ASPECT_DEPTH_BIT
   );

   BfSingleEvent event{};
   if (vkCreateImageView(
           base.device,
           &dview_info,
           nullptr,
           &base.depth_image.view
       ) == VK_SUCCESS)
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_CREATE_DEPTH_IMAGE_VIEW_SUCCESS;
   }
   else
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action = BfEnActionType::BF_CREATE_DEPTH_IMAGE_VIEW_FAILURE;
   }

   return BfEvent(event);
}

BfEvent
bfDestroyDepthBuffer(BfBase& base)
{
   bfDestroyImageView(&base.depth_image, base.device);
   // vkDestroyImageView(base.device, base.depth_image.view, nullptr);
   bfDestroyImage(&base.depth_image);

   // vmaDestroyImage(base.allocator, base.depth_image.image, nullptr);

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_DEPTH_BUFFER;

   return BfEvent(event);
}

BfEvent
bfCreateIDMapImage(BfBase& base)
{
   VkImageCreateInfo id_image_create_info{};
   // info.flags = VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
   id_image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
   id_image_create_info.pNext = nullptr;
   id_image_create_info.imageType = VK_IMAGE_TYPE_2D;
   id_image_create_info.format = VK_FORMAT_R32_UINT; // base.swap_chain_format;
   id_image_create_info.extent =
       {base.swap_chain_extent.width, base.swap_chain_extent.height, 1};
   id_image_create_info.mipLevels = 1;
   id_image_create_info.arrayLayers = 1;
   id_image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
   id_image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
   id_image_create_info.usage =
       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
       VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
       VK_IMAGE_USAGE_SAMPLED_BIT;

   id_image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
   id_image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

   VmaAllocationCreateInfo allocinfo{};
   allocinfo.usage = VMA_MEMORY_USAGE_AUTO;
   // allocinfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
   // VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

   VkImageViewUsageCreateInfo id_image_usage_info{};
   id_image_usage_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO;
   id_image_usage_info.usage =
       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
       VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
       VK_IMAGE_USAGE_SAMPLED_BIT;

   VkImageViewCreateInfo id_image_info = {};
   id_image_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
   id_image_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
   id_image_info.format = VK_FORMAT_R32_UINT;
   id_image_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
   id_image_info.subresourceRange.baseMipLevel = 0;
   id_image_info.subresourceRange.levelCount = 1;
   id_image_info.subresourceRange.baseArrayLayer = 0;
   id_image_info.subresourceRange.layerCount = 1;
   id_image_info.pNext = &id_image_usage_info;

   auto holder = bfGetpHolder();
   holder->images_id.resize(base.image_pack_count);

   bool success = 1;
   for (int frame_index = 0; frame_index < base.image_pack_count; frame_index++)
   {
      auto image_event = bfCreateImage(
          &holder->images_id[frame_index],
          // base.allocator,
          BfAllocator::get(),
          &id_image_create_info,
          &allocinfo
      );
      id_image_info.image = holder->images_id[frame_index].image;

      auto view_event = bfCreateImageView(
          &holder->images_id[frame_index],
          base.device,
          &id_image_info
      );

      base.image_packs[frame_index].pImage_id =
          &holder->images_id[frame_index].image;
      base.image_packs[frame_index].pImage_view_id =
          &holder->images_id[frame_index].view;

      success *=
          image_event.single_event.success * view_event.single_event.success;
   }

   BfSingleEvent event{};
   {
      event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      if (success)
      {
         event.action = BF_ACTION_TYPE_CREATE_IDMAP_SUCCESS;
         event.success = true;
      }
      else
      {
         event.action = BF_ACTION_TYPE_CREATE_IDMAP_FAILURE;
         event.success = false;
      }
   }
   return BfEvent(event);
}

BfEvent
bfDestroyIDMapImage(BfBase& base)
{
   auto holder = bfGetpHolder();
   for (int frame_index = 0; frame_index < base.image_pack_count; frame_index++)
   {
      bfDestroyImage(&holder->images_id[frame_index]);
      bfDestroyImageView(&holder->images_id[frame_index], base.device);

      base.image_packs[frame_index].pImage_id = nullptr;
      base.image_packs[frame_index].pImage_view_id = nullptr;
   }

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_IDMAP;

   return BfEvent(event);
}

BfEvent
bfCreateAllocator(BfBase& base)
{
   BfAllocator::create(
       base.device,
       base.instance,
       base.physical_device->physical_device
   );
   // base.allocator = BfAllocator::get();

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

   // VmaAllocatorCreateInfo info{};
   // info.device = base.device;
   // info.instance = base.instance;
   // info.physicalDevice = base.physical_device->physical_device;
   //
   // if (vmaCreateAllocator(&info, &base.allocator) != VK_SUCCESS)
   // {
   //    event.action = BF_ACTION_TYPE_CREATE_VMA_ALLOCATOR_FAILURE;
   //    event.success = false;
   // }
   // else
   // {
   event.action = BF_ACTION_TYPE_CREATE_VMA_ALLOCATOR_SUCCESS;
   event.success = true;
   // }
   return BfEvent(event);
}

BfEvent
bfDestroyAllocator(BfBase& base)
{
   BfAllocator::destroy();
   // vmaDestroyAllocator(base.allocator);
   // //
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_VMA_ALLOCATOR;
   //
   return BfEvent(event);
}

BfEvent
bfCreateTextureLoader(BfBase& base)
{
   // base.texture_loader = BfTextureLoader(
   //     base.physical_device,
   //     &base.device,
   //     &BfAllocator::pget(),
   //     &base.command_pool
   // );
   //
   // base.texture_loader.create_imgui_descriptor_pool();
   // base.texture_loader.create_imgui_descriptor_set_layout();
   //
   // BfSingleEvent event{};
   // event.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
   // event.action = BF_ACTION_TYPE_CREATE_TEXTURE_LOADER_SUCCESS;
   // return event;
   return BfEvent{};
}

BfEvent
bfDestroyTextureLoader(BfBase& base)
{
   return BfSingleEvent();
}

BfEvent
bfLoadTextures(BfBase& base)
{
   // BfTexture button =
   // base.texture_loader.load("./resources/buttons/test.png");
   return BfEvent();
}

BfEvent
bfBindAllocatorToLayerHandler(BfBase& base)
{
   VmaAllocator alloc = BfAllocator::get();
   return base.layer_handler.bind_allocator(&alloc);
}

// BfEvent
// bfBindTrianglePipelineToLayerHandler(BfBase &base)
// {
//    // return
//    base.layer_handler.bind_trianle_pipeline(&base.triangle_pipeline); return
//    base.layer_handler.bind_trianle_pipeline(
//        BfPipelineHandler::instance()->getPipeline(BfPipelineType_Triangles)
//    );
// }
//
// BfEvent
// bfBindLinePipelineToLayerHandler(BfBase &base)
// {
//    // return base.layer_handler.bind_line_pipeline(&base.line_pipeline);
//    return base.layer_handler.bind_trianle_pipeline(
//        BfPipelineHandler::instance()->getPipeline(BfPipelineType_Lines)
//    );
// }

// void bfAllocateBuffersForDynamicMesh(BfBase& base)
//{
//	uint32_t MAX_VERTICES = 10000;
//
//	bfCreateBuffer(&base.dynamic_vertex_buffer,
//					base.allocator,
//					sizeof(bfVertex) * MAX_VERTICES,
//					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//		VMA_MEMORY_USAGE_CPU_TO_GPU);
//
//	bfCreateBuffer(&base.dynamic_index_buffer,
//					base.allocator,
//					sizeof(uint32_t) * MAX_VERTICES,
//					VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
//					VMA_MEMORY_USAGE_CPU_TO_GPU);
// }

// void bfUploadDynamicMesh(BfBase& base, BfMesh& mesh)
//{
//	if (base.is_resized) {
//		void* vertex_data;
//		vmaMapMemory(base.allocator,
// base.dynamic_vertex_buffer.allocation, &vertex_data);
// memcpy(vertex_data, mesh.vertices.data(), sizeof(bfVertex) *
// mesh.vertices.size()); 		vmaUnmapMemory(base.allocator,
// base.dynamic_vertex_buffer.allocation);
//
//		void* index_data;
//		vmaMapMemory(base.allocator,
// base.dynamic_index_buffer.allocation, &index_data);
// memcpy(index_data, mesh.indices.data(), sizeof(uint32_t) *
// mesh.indices.size()); 		vmaUnmapMemory(base.allocator,
// base.dynamic_index_buffer.allocation);
//
//		base.is_resized = false;
//	}
// }

// void bfUploadMesh(BfBase &base, BfMesh &mesh)
// {
//    bfUploadVertices(base, mesh);
//    bfUploadIndices(base, mesh);
// }
//
// void bfUploadVertices(BfBase &base, BfMesh &mesh)
// {
//    VkBuffer      local_buffer;
//    VmaAllocation local_allocation;
//
//    VkBufferCreateInfo bufferInfo{};
//    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    bufferInfo.size  = mesh.vertices.size() * sizeof(bfVertex);
//    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//
//    VmaAllocationCreateInfo allocationInfo{};
//    allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
//    allocationInfo.flags =
//        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
//        VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;
//
//    if (vmaCreateBuffer(base.allocator,
//                        &bufferInfo,
//                        &allocationInfo,
//                        &local_buffer,
//                        &local_allocation,
//                        nullptr) != VK_SUCCESS)
//    {
//       throw std::runtime_error("vmaCrateBuffer didn't work");
//    }
//
//    void *data;
//    vmaMapMemory(base.allocator, local_allocation, &data);
//    memcpy(data, mesh.vertices.data(), bufferInfo.size);
//    vmaUnmapMemory(base.allocator, local_allocation);
//
//    bufferInfo.usage =
//        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
//    allocationInfo.flags = 0;
//
//    if (vmaCreateBuffer(base.allocator,
//                        &bufferInfo,
//                        &allocationInfo,
//                        &mesh.vertex_buffer.buffer,
//                        &mesh.vertex_buffer.allocation,
//                        nullptr) != VK_SUCCESS)
//    {
//       throw std::runtime_error("vmaCrateBuffer didn't work");
//    }
//
//    VkCommandBufferAllocateInfo allocInfo{};
//    allocInfo.sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    allocInfo.level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    allocInfo.commandPool = base.command_pool;
//    allocInfo.commandBufferCount = 1;
//
//    VkCommandBuffer commandBuffer;
//    vkAllocateCommandBuffers(base.device, &allocInfo, &commandBuffer);
//
//    VkCommandBufferBeginInfo beginInfo{};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//    vkBeginCommandBuffer(commandBuffer, &beginInfo);
//    VkBufferCopy copyRegion{};
//    copyRegion.srcOffset = 0;  // Optional
//    copyRegion.dstOffset = 0;  // Optional
//    copyRegion.size      = bufferInfo.size;
//    vkCmdCopyBuffer(commandBuffer,
//                    local_buffer,
//                    mesh.vertex_buffer.buffer,
//                    1,
//                    &copyRegion);
//    vkEndCommandBuffer(commandBuffer);
//
//    VkSubmitInfo submitInfo{};
//    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//    submitInfo.commandBufferCount = 1;
//    submitInfo.pCommandBuffers    = &commandBuffer;
//
//    vkQueueSubmit(
//        base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE],
//        1,
//        &submitInfo,
//        VK_NULL_HANDLE);
//    vkQueueWaitIdle(
//        base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE]);
//
//    vkFreeCommandBuffers(base.device, base.command_pool, 1, &commandBuffer);
//
//    vmaDestroyBuffer(base.allocator, local_buffer, local_allocation);
// }
//
// void bfUploadIndices(BfBase &base, BfMesh &mesh)
// {
//    VkBuffer      local_buffer;
//    VmaAllocation local_allocation;
//
//    VkBufferCreateInfo bufferInfo{};
//    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    bufferInfo.size  = mesh.indices.size() * sizeof(uint32_t);
//    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//    // bufferInfo.flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
//    //  VMA_ALLOCATION_CREATE_HOST_ACCESS_
//
//    VmaAllocationCreateInfo allocationInfo{};
//    allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
//    allocationInfo.flags =
//        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
//        VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;
//
//    if (vmaCreateBuffer(base.allocator,
//                        &bufferInfo,
//                        &allocationInfo,
//                        &local_buffer,
//                        &local_allocation,
//                        nullptr) != VK_SUCCESS)
//    {
//       throw std::runtime_error("vmaCrateBuffer didn't work");
//    }
//
//    void *data;
//    vmaMapMemory(base.allocator, local_allocation, &data);
//    memcpy(data, mesh.indices.data(), bufferInfo.size);
//    vmaUnmapMemory(base.allocator, local_allocation);
//
//    bufferInfo.usage =
//        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
//    allocationInfo.flags = 0;
//
//    if (vmaCreateBuffer(base.allocator,
//                        &bufferInfo,
//                        &allocationInfo,
//                        &mesh.index_buffer.buffer,
//                        &mesh.index_buffer.allocation,
//                        nullptr) != VK_SUCCESS)
//    {
//       throw std::runtime_error("vmaCrateBuffer didn't work");
//    }
//
//    VkCommandBufferAllocateInfo allocInfo{};
//    allocInfo.sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    allocInfo.level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    allocInfo.commandPool = base.command_pool;
//    allocInfo.commandBufferCount = 1;
//
//    VkCommandBuffer commandBuffer;
//    vkAllocateCommandBuffers(base.device, &allocInfo, &commandBuffer);
//
//    VkCommandBufferBeginInfo beginInfo{};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//    vkBeginCommandBuffer(commandBuffer, &beginInfo);
//    VkBufferCopy copyRegion{};
//    copyRegion.srcOffset = 0;  // Optional
//    copyRegion.dstOffset = 0;  // Optional
//    copyRegion.size      = bufferInfo.size;
//    vkCmdCopyBuffer(commandBuffer,
//                    local_buffer,
//                    mesh.index_buffer.buffer,
//                    1,
//                    &copyRegion);
//    vkEndCommandBuffer(commandBuffer);
//
//    VkSubmitInfo submitInfo{};
//    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//    submitInfo.commandBufferCount = 1;
//    submitInfo.pCommandBuffers    = &commandBuffer;
//
//    vkQueueSubmit(
//        base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE],
//        1,
//        &submitInfo,
//        VK_NULL_HANDLE);
//    vkQueueWaitIdle(
//        base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE]);
//
//    vkFreeCommandBuffers(base.device, base.command_pool, 1, &commandBuffer);
//
//    vmaDestroyBuffer(base.allocator, local_buffer, local_allocation);
// }

BfEvent
bfaCreateShaderModule(
    VkShaderModule& module, VkDevice device, const std::vector< char >& data
)
{
   VkShaderModuleCreateInfo createInfo{};
   createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
   createInfo.codeSize = data.size();
   createInfo.pCode = reinterpret_cast< const uint32_t* >(data.data());

   BfSingleEvent event{};
   if (vkCreateShaderModule(device, &createInfo, nullptr, &module) ==
       VK_SUCCESS)
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action =
          BfEnActionType::BF_ACTION_TYPE_CREATE_SHADER_MODULE_SUCCESS;
   }
   else
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action =
          BfEnActionType::BF_ACTION_TYPE_CREATE_SHADER_MODULE_FAILURE;
   }

   return BfEvent(event);
}

// BfEvent
// bfaCreateGraphicsPipelineLayouts(BfBase &base)
// {
//    std::vector<VkDescriptorSetLayout> layouts;
//    layouts.reserve(10);
//    for (auto &pack : base.descriptor.__desc_layout_packs_map)
//    {
//       layouts.push_back(pack.second.desc_set_layout);
//    }
//    //{
//    //	base.global_set_layout,
//    //	base.main_set_layout
//    //};
//
//    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
//    pipelineLayoutCreateInfo.sType =
//        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//    pipelineLayoutCreateInfo.setLayoutCount =
//        static_cast<uint32_t>(layouts.size());
//    pipelineLayoutCreateInfo.pSetLayouts = layouts.data();   // Optional
//    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;     // Optional
//    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;  // Optional
//
//    bool is_pipeline_layouts_done = true;
//
//    std::stringstream ss;
//
//    if (vkCreatePipelineLayout(
//            base.device,
//            &pipelineLayoutCreateInfo,
//            nullptr,
//            &base.tline_pipeline_layout
//        ) == VK_SUCCESS)
//    {
//       ss << "SLine pipeline layout is done;";
//    }
//    else
//    {
//       ss << "SLine pipeline layout isn't done;";
//       is_pipeline_layouts_done = false;
//    }
//    if (vkCreatePipelineLayout(
//            base.device,
//            &pipelineLayoutCreateInfo,
//            nullptr,
//            &base.triangle_pipeline_layout
//        ) == VK_SUCCESS)
//    {
//       ss << "Triangle pipeline layout is done;";
//    }
//    else
//    {
//       ss << "Triangle pipeline layout isn't done;";
//       is_pipeline_layouts_done = false;
//    }
//    if (vkCreatePipelineLayout(
//            base.device,
//            &pipelineLayoutCreateInfo,
//            nullptr,
//            &base.line_pipeline_layout
//        ) == VK_SUCCESS)
//    {
//       ss << "Line pipeline layout is done;";
//    }
//    else
//    {
//       ss << "Line pipeline layout isn't done;";
//       is_pipeline_layouts_done = false;
//    }
//
//    BfSingleEvent event{};
//    event.type =
//    BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT; if
//    (is_pipeline_layouts_done)
//    {
//       event.action =
//           BfEnActionType::BF_ACTION_TYPE_CREATE_PIPELINE_LAYOUT_SUCCESS;
//    }
//    else
//    {
//       event.action =
//           BfEnActionType::BF_ACTION_TYPE_CREATE_PIPELINE_LAYOUT_FAILURE;
//    }
//    event.info = ss.str();
//
//    return BfEvent(event);
// }

// BfEvent
// bfDestoryGraphicsPipelineLayouts(BfBase &base)
// {
//    vkDestroyPipelineLayout(base.device, base.triangle_pipeline_layout,
//    nullptr); vkDestroyPipelineLayout(base.device, base.tline_pipeline_layout,
//    nullptr); vkDestroyPipelineLayout(base.device, base.line_pipeline_layout,
//    nullptr);
//
//    BfSingleEvent event{};
//    event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
//    event.action = BF_ACTION_TYPE_DESTROY_PIPELINE_LAYOUT;
//    return event;
// }

BfEvent
bfaRecreateSwapchain(BfBase& base)
{
   int width = 0;
   int height = 0;
   glfwGetFramebufferSize(base.window->pWindow, &width, &height);
   while (width == 0 || height == 0)
   {
      glfwGetFramebufferSize(base.window->pWindow, &width, &height);
      glfwWaitEvents();
   }

   ImGui_ImplVulkan_SetMinImageCount(base.image_pack_count);

   vkDeviceWaitIdle(base.device);

   bfDestroyIDMapImage(base);
   bfCleanUpSwapchain(base);

   bfCreateSwapchain(base);
   bfCreateDepthBuffer(base);
   bfCreateImageViews(base);
   bfCreateIDMapImage(base);
   bfCreateStandartFrameBuffers(base);
   bfCreateGUIFrameBuffers(base);

   base.window->resized = false;
   // base.window->ortho_right = (float)base.swap_chain_extent.width;
   // base.window->ortho_top = (float)base.swap_chain_extent.height;

   auto camera = BfCamera::instance();
   if (camera)
   {
      camera->setExtent(
          base.swap_chain_extent.width,
          base.swap_chain_extent.height
      );
   }

   base::viewport::ViewportManager::update();

   return BfEvent();
}

BfEvent
bfaCreateShaderCreateInfos(
    VkDevice device,
    std::string path,
    std::vector< VkShaderModule >& modules,
    std::vector< VkPipelineShaderStageCreateInfo >& out
)
{
   /*
           "*.vert"
           "*.frag"
           "*.geom"
           "*.glsl"
           "*.comp"
           "*.tesse"
           "*.tessc"
   */

   static std::map< std::string, VkShaderStageFlagBits > stage_bits = {
       {"vert", VK_SHADER_STAGE_VERTEX_BIT},
       {"frag", VK_SHADER_STAGE_FRAGMENT_BIT},
       {"geom", VK_SHADER_STAGE_GEOMETRY_BIT},
       {"comp", VK_SHADER_STAGE_COMPUTE_BIT},
       {"tesse", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
       {"tessc", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT}
   };

   BfSingleEvent event{.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT};
   std::stringstream ss;
   ss << " ";

   std::vector< std::string > file_names;

   for (const auto& entry : std::filesystem::directory_iterator(path))
   {
      if (entry.path().extension().string() != ".spv")
         continue;
      file_names.push_back(entry.path().filename().string());
   }

   modules.clear();
   modules.reserve(file_names.size());

   for (size_t i = 0; i < file_names.size(); ++i)
   {
      std::vector< char > code;
      BfEvent e = bfaReadFile(code, path + "/" + file_names[i]);

      if (e.single_event.success)
      {
         modules.emplace_back();
         bfaCreateShaderModule(modules[i], device, code);
         ss << "shader file " << path << "/" << file_names[i]
            << " was processed ";
      }
      else
      {
         ss << "shader file " << path << "/" << file_names[i]
            << " wasn't processed";
         event.action = BF_ACTION_TYPE_CREATE_SHADER_MODULE_PACK_FAILURE;
         event.success = false;
         event.info = ss.str();
         return event;
      }

      VkPipelineShaderStageCreateInfo info{};
      info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      info.pName = "main";
      info.module = modules[i];
      info.stage =
          stage_bits[std::filesystem::path(file_names[i]).stem().string()];

      out.push_back(info);
   }

   {
      event.action = BF_ACTION_TYPE_CREATE_SHADER_MODULE_PACK_SUCCESS;
      event.success = true;
      event.info = ss.str();
      return event;
   }
}

void
bfBeginSingleTimeCommands(BfBase& base, VkCommandBuffer& commandBuffer)
{
   VkCommandBufferAllocateInfo allocInfo{};
   allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
   allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
   allocInfo.commandPool = base.command_pool;
   allocInfo.commandBufferCount = 1;

   // VkCommandBuffer commandBuffer;
   vkAllocateCommandBuffers(base.device, &allocInfo, &commandBuffer);

   VkCommandBufferBeginInfo beginInfo{};
   beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
   beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

   vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void
bfEndSingleTimeCommands(BfBase& base, VkCommandBuffer& commandBuffer)
{
   vkEndCommandBuffer(commandBuffer);

   VkSubmitInfo submitInfo{};
   submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
   submitInfo.commandBufferCount = 1;
   submitInfo.pCommandBuffers = &commandBuffer;

   vkQueueSubmit(
       base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE],
       1,
       &submitInfo,
       VK_NULL_HANDLE
   );
   vkQueueWaitIdle(
       base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE]
   );

   vkFreeCommandBuffers(base.device, base.command_pool, 1, &commandBuffer);
}

void
bfDrawFrame(BfBase& base)
{
   // VK_TRUE - wait all fences
   // vkWaitForFences(base.device, 1,
   // base.frame_pack[base.current_frame].frame_in_flight_fence, VK_TRUE,
   // UINT64_MAX);

   VkSemaphore local_available_image_semaphore =
       *base.frame_pack[base.current_frame].available_image_semaphore;
   VkSemaphore local_finish_render_image_semaphore =
       *base.frame_pack[base.current_frame].finish_render_image_semaphore;
   VkFence local_fence_in_flight =
       *base.frame_pack[base.current_frame].frame_in_flight_fence;

   VkCommandBuffer local_standart_command_bufffer =
       *base.frame_pack[base.current_frame].standart_command_buffer;
   VkCommandBuffer local_gui_command_buffer =
       *base.frame_pack[base.current_frame].gui_command_buffer;

   VkResult result = vkAcquireNextImageKHR(
       base.device,
       base.swap_chain,
       UINT64_MAX,
       local_available_image_semaphore,
       VK_NULL_HANDLE,
       &base.current_image
   );

   if (result == VK_ERROR_OUT_OF_DATE_KHR)
   {
      bfaRecreateSwapchain(base);

      return;
   }
   else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
   {
      throw std::runtime_error("Failed to acquire swap chain image");
   }

   bfUpdateUniformBuffer(base);

   vkResetFences(base.device, 1, &local_fence_in_flight);

   vkResetCommandBuffer(local_standart_command_bufffer, 0);

   bfMainRecordCommandBuffer(base);

   VkSubmitInfo submitInfo{};
   submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

   VkSemaphore waitSemaphores[] = {local_available_image_semaphore};
   VkPipelineStageFlags waitStages[] = {
       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
   };
   submitInfo.waitSemaphoreCount = 1;
   submitInfo.pWaitSemaphores = waitSemaphores;
   submitInfo.pWaitDstStageMask = waitStages;

   std::array< VkCommandBuffer, 2 > submitCommandBuffers = {
       local_standart_command_bufffer,
       local_gui_command_buffer
   };

   submitInfo.pCommandBuffers = submitCommandBuffers.data();
   submitInfo.commandBufferCount =
       static_cast< uint32_t >(submitCommandBuffers.size());

   VkSemaphore signalSemaphores[] = {local_finish_render_image_semaphore};
   submitInfo.signalSemaphoreCount = 1;
   submitInfo.pSignalSemaphores = signalSemaphores;

   if (vkQueueSubmit(
           base.physical_device->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE],
           1,
           &submitInfo,
           local_fence_in_flight
       ) != VK_SUCCESS)
   {
      throw std::runtime_error("Failed to submit draw command buffer");
   }

   VkPresentInfoKHR presentInfo{};
   presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

   presentInfo.waitSemaphoreCount = 1;
   presentInfo.pWaitSemaphores = signalSemaphores;

   VkSwapchainKHR swapChains[] = {base.swap_chain};
   presentInfo.swapchainCount = 1;
   presentInfo.pSwapchains = swapChains;
   presentInfo.pImageIndices = &base.current_image;

   result = vkQueuePresentKHR(
       base.physical_device->queues[BfvEnQueueType::BF_QUEUE_PRESENT_TYPE],
       &presentInfo
   );
   if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
       base.window->resized)
   {
      ImGui_ImplVulkan_SetMinImageCount(base.image_pack_count);
      bfaRecreateSwapchain(base);
   }
   else if (result != VK_SUCCESS)
   {
      throw std::runtime_error("failed to present swap chain image!");
   }

   vkWaitForFences(base.device, 1, &local_fence_in_flight, VK_TRUE, UINT64_MAX);
   base.layer_killer.kill();

   base.current_frame = (base.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void
bfRenderDefault(
    BfBase& base,
    VkCommandBuffer& command_buffer,
    bool is_left,
    float x,
    float y,
    float w,
    float h
)
{
   VkDeviceSize offsets[] = {0};

   VkViewport viewport{};
   viewport.x = x;
   viewport.y = y;
   viewport.width = w;
   viewport.height = h;
   viewport.minDepth = 0.0f;
   viewport.maxDepth = 1.0f;
   vkCmdSetViewport(command_buffer, 0, 1, &viewport);

   VkRect2D scissor{};
   scissor.offset = {static_cast< int32_t >(x), static_cast< int32_t >(y)};
   scissor.extent = {static_cast< uint32_t >(w), static_cast< uint32_t >(h)};
   vkCmdSetScissor(command_buffer, 0, 1, &scissor);

   auto& man = base::desc::own::BfDescriptorPipelineDefault::manager();
   man.bindSets(
       base::desc::own::SetType::Main,
       is_left ? 0 : 1,
       command_buffer,
       *BfPipelineHandler::instance()->getLayout(BfPipelineLayoutType_Main)
   );
   man.bindSets(
       base::desc::own::SetType::Global,
       base.current_frame,
       command_buffer,
       *BfPipelineHandler::instance()->getLayout(BfPipelineLayoutType_Main)
   );
   man.bindSets(
       base::desc::own::SetType::Texture,
       base.current_frame,
       command_buffer,
       *BfPipelineHandler::instance()->getLayout(BfPipelineLayoutType_Main)
   );
   obj::BfDrawManager::inst().draw(command_buffer);
}

void
bfPickID(BfBase& base, VkCommandBuffer& command_buffer)
{

   VkImageSubresourceLayers sub{};
   sub.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
   sub.mipLevel = 0;
   sub.layerCount = 1;
   sub.baseArrayLayer = 0;

   VkBufferImageCopy region = {};
   region.bufferOffset = 0;
   region.bufferRowLength = base.swap_chain_extent.width;
   region.bufferImageHeight = base.swap_chain_extent.height;
   region.imageSubresource = sub;
   region.imageExtent = {1, 1, 1};

   int x = 0;
   if ((int)base.window->xpos >= base.swap_chain_extent.width)
   {
      x = base.swap_chain_extent.width - 1;
   }
   else if ((int)base.window->xpos <= 0)
   {
      x = 1;
   }
   else
   {
      x = (int)base.window->xpos;
   }

   int y = 0;
   if ((int)base.window->ypos >= base.swap_chain_extent.height)
   {
      y = base.swap_chain_extent.height - 1;
   }
   else if ((int)base.window->ypos <= 0)
   {
      y = 1;
   }
   else
   {
      y = (int)base.window->ypos;
   }

   region.imageOffset = {x, y, 0};

   vkCmdCopyImageToBuffer(
       command_buffer,
       // base.descriptor.get_image(BfDescriptorPosPickUsage,
       // base.current_frame)->image,
       *base.image_packs[base.current_image].pImage_id,
       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
       base.id_image_buffer.buffer,
       1,
       &region
   );
}

void
bfRenderGUI(BfBase& base, VkCommandBufferBeginInfo& beginInfo)
{
   auto& local_buffer = *base.frame_pack[base.current_frame].gui_command_buffer;

   // IMGUI
   if (vkBeginCommandBuffer(local_buffer, &beginInfo) != VK_SUCCESS)
   {
      throw std::runtime_error("Failed to begin recoding command buffer");
   }

   VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

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

   if (vkEndCommandBuffer(local_buffer) != VK_SUCCESS)
   {
      throw std::runtime_error("failed to record command buffer!");
   }
}
void
bfMainRecordCommandBuffer(BfBase& base)
{
   // Depth buffer
   VkCommandBufferBeginInfo beginInfo{};
   beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
   beginInfo.flags = 0;
   beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
   beginInfo.pInheritanceInfo = nullptr;

   VkClearValue depth_clear;
   depth_clear.depthStencil.depth = 1.f;

   if (vkBeginCommandBuffer(
           *base.frame_pack[base.current_frame].standart_command_buffer,
           &beginInfo
       ) != VK_SUCCESS)
   {
      throw std::runtime_error("Failed to begin recoding command buffer");
   }

   VkRenderPassBeginInfo renderPassInfo{};
   renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
   renderPassInfo.renderPass = base.standart_render_pass;
   renderPassInfo.framebuffer =
       *base.image_packs[base.current_image].pStandart_Buffer;
   renderPassInfo.renderArea.offset = {0, 0};
   renderPassInfo.renderArea.extent = base.swap_chain_extent;

   VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
   renderPassInfo.clearValueCount = 3;

   std::vector< VkClearValue > clear_values{
       clearColor,
       clearColor,
       depth_clear
   };

   renderPassInfo.pClearValues = clear_values.data();
   VkCommandBuffer local_buffer =
       *base.frame_pack[base.current_frame].standart_command_buffer;

   vkCmdBeginRenderPass(
       local_buffer,
       &renderPassInfo,
       VK_SUBPASS_CONTENTS_INLINE
   );
   {
      // clang-format off
      //
      auto& extent = base.swap_chain_extent;
      bfUpdateUniformViewExt(base, base.current_frame, {extent.width, extent.height});
      bfRenderDefault(base, local_buffer, base.current_frame, 0.0f, 0.0f, extent.width, extent.height);
      // Compute left/right widths based on ratio
      float ratio = std::clamp(base.viewport_ratio, 0.0f, 1.0f);
      float left_width = extent.width * ratio;
      float right_width = extent.width - left_width;

      // // Left side
      // bfUpdateUniformViewExt(base, 0, {left_width, extent.height});
      // bfRenderDefault(base, local_buffer, 0, 0.0f, 0.0f, left_width, extent.height);
      //
      // // Right side
      // bfUpdateUniformViewExt(base, 1, {right_width, extent.height});
      // bfRenderDefault(base, local_buffer, 1, left_width, 0.0f, right_width, extent.height);
      // clang-format on
   }
   vkCmdEndRenderPass(local_buffer);
   bfPickID(base, local_buffer);

   if (vkEndCommandBuffer(local_buffer) != VK_SUCCESS)
   {
      throw std::runtime_error("Failed to begin recoding command buffer");
   }
   bfRenderGUI(base, beginInfo);
}

uint32_t
bfGetCurrentObjectId(BfBase& base)
{
   uint32_t id_on_cursor;
   void* id_on_cursor_ = base.id_image_buffer.allocation_info.pMappedData;
   memcpy(&id_on_cursor, id_on_cursor_, sizeof(uint32_t));
   return id_on_cursor;
}

void
bfUpdateImGuiPlatformWindows()
{
   ImGuiIO& io = ImGui::GetIO();
   (void)io;
   if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
   {
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
   }
}

size_t
bfPadUniformBufferSize(
    const BfPhysicalDevice* physical_device, size_t original_size
)
{
   size_t min_uniform_buffer_offset_allignment =
       physical_device->properties.limits.minUniformBufferOffsetAlignment;
   size_t alligned_size = original_size;
   if (min_uniform_buffer_offset_allignment > 0)
   {
      alligned_size =
          (alligned_size + min_uniform_buffer_offset_allignment + 1) &
          ~(min_uniform_buffer_offset_allignment - 1);
   }

   return alligned_size;
}

VkDescriptorSetLayoutBinding
bfGetDescriptorSetLayoutBinding(
    VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t binding
)
{
   VkDescriptorSetLayoutBinding setbind = {};
   setbind.binding = binding;
   setbind.descriptorCount = 1;
   setbind.descriptorType = type;
   setbind.pImmutableSamplers = nullptr;
   setbind.stageFlags = stage_flags;

   return setbind;
}

VkWriteDescriptorSet
bfWriteDescriptorBuffer(
    VkDescriptorType type,
    VkDescriptorSet dstSet,
    VkDescriptorBufferInfo* bufferInfo,
    uint32_t binding
)
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

VkImageCreateInfo
bfPopulateDepthImageCreateInfo(
    VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent
)
{
   VkImageCreateInfo info{};
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

VkImageViewCreateInfo
bfPopulateDepthImageViewCreateInfo(
    VkFormat format, VkImage image, VkImageAspectFlags aspect_flags
)
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

VkPipelineDepthStencilStateCreateInfo
bfPopulateDepthStencilStateCreateInfo(
    bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp
)
{
   VkPipelineDepthStencilStateCreateInfo info = {};
   info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
   info.pNext = nullptr;

   info.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE;
   info.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE;
   info.depthCompareOp =
       VK_COMPARE_OP_LESS; // bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
   info.depthBoundsTestEnable = VK_FALSE;
   // info.minDepthBounds = -10.0f; // Optional
   // info.maxDepthBounds = 10.0f; // Optional
   info.stencilTestEnable = VK_FALSE;

   return info;
}

BfEvent
bfCleanUpSwapchain(BfBase& base)
{
   BfHolder* holder = bfGetpHolder();

   bfDestroyStandartFrameBuffers(base);
   bfDestroyGUIFrameBuffers(base);
   bfDestroyImageViews(base);
   bfDestroyDepthBuffer(base);
   bfDestroySwapchain(base);

   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_DESTROY_EVENT;
   event.action = BF_ACTION_TYPE_DESTROY_SWAPCHAIN_PARTS;

   return BfEvent(event);
}

void
bfUpdateUniformViewNew(BfBase& base)
{
   BfUniformView ubo{};

   ubo.view = BfCamera::instance()->view();
   ubo.proj = BfCamera::instance()->projection();

   ubo.cursor_pos = {base.window->xpos, base.window->ypos};
   ubo.id_on_cursor = obj::BfDrawManager::getHovered();
   ubo.camera_pos = base.window->pos;
   ubo.model = BfCamera::instance()->m_scale;

   auto& man = base::desc::own::BfDescriptorPipelineDefault::manager();
   auto& desc = man.get< base::desc::own::BfDescriptorUBO >(
       base::desc::own::SetType::Main,
       0
   );
   desc.map(ubo);
}

void
bfUpdateUniformViewExt(BfBase& base, bool is_left, const glm::vec2& ext)
{
   BfUniformView ubo{};

   ubo.view = BfCamera::instance()->view();
   ubo.proj = BfCamera::instance()->projection(ext);

   ubo.cursor_pos = {base.window->xpos, base.window->ypos};
   ubo.id_on_cursor = obj::BfDrawManager::getHovered();
   ubo.camera_pos = base.window->pos;
   ubo.model = BfCamera::instance()->m_scale;

   auto& man = base::desc::own::BfDescriptorPipelineDefault::manager();
   auto& desc = man.getForFrame< base::desc::own::BfDescriptorUBO >(
       is_left ? 0 : 1,
       base::desc::own::SetType::Main,
       0
   );
   desc.map(ubo);
}

void
bfUpdateUniformBuffer(BfBase& base)
{
   // bfUpdateUniformViewNew(base);
   obj::BfDrawManager::inst().mapModels(base.current_frame);
}

void
bfCreateSampler(BfBase& base)
{
   // VkSamplerCreateInfo samplerInfo{};
   // samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
   // samplerInfo.magFilter = VK_FILTER_LINEAR;
   // samplerInfo.minFilter = VK_FILTER_LINEAR;
   // samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
   // samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
   // samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
   // samplerInfo.anisotropyEnable = VK_FALSE;
   // samplerInfo.maxAnisotropy = 16;
   // samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
   // samplerInfo.unnormalizedCoordinates = VK_FALSE;
   // samplerInfo.compareEnable = VK_FALSE;
   // samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
   // samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
   // samplerInfo.mipLodBias = 0.0f;
   // samplerInfo.minLod = 0.0f;
   // samplerInfo.maxLod = 0.0f;
   //
   // VkSampler textureSampler;
   // if (vkCreateSampler(base.device, &samplerInfo, nullptr, &base.sampler) !=
   //     VK_SUCCESS)
   // {
   //    throw std::runtime_error("Failed to create texture sampler!");
   // }
   base.sampler = std::make_unique< base::desc::BfSampler >();
   base::g::bindSampler(&base.sampler->handle());
}

// void
// bfDestorySampler(BfBase& base)
// {
//    vkDestroySampler(base.device, base.sampler, nullptr);
// }
//
void
bfBindBase(BfBase* new_base)
{
   __pBase = new_base;
}

BfBase*
bfGetBase()
{
   if (!__pBase)
   {
      throw std::runtime_error("No BfBase was bound to static pointer");
   }
   return __pBase;
}
