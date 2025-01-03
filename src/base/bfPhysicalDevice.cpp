#include "bfPhysicalDevice.h"

// Static definitions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

// static VkPhysicalDevice physical_device;
// static std::map<BfvEnQueueType, std::optional<uint32_t>>
// queue_family_indices;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Static
// definitions //

BfEvent
bfIsQueueFamilyIndicesCorrect(
    const BfPhysicalDevice* bf_physical_device, bool& is_suitable
)
{
   int decision = true;
   std::for_each(
       bf_physical_device->queue_family_indices.begin(),
       bf_physical_device->queue_family_indices.end(),
       [&](const std::pair<BfvEnQueueType, std::optional<uint32_t>>& pair) {
          if (!pair.second.has_value()) decision = false;
       }
   );

   BfSingleEvent event{};
   if (decision)
   {
      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
      event.action =
          BfEnActionType::BF_ACTION_TYPE_SEARCH_NEEDED_QUEUES_SUCCESS;
   }
   else
   {
      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
      event.action =
          BfEnActionType::BF_ACTION_TYPE_SEARCH_NEEDED_QUEUES_FAILURE;
   }
   is_suitable = decision;
   return BfEvent(event);
}

BfEvent
bfCheckDeviceExtensionSupport(
    BfPhysicalDevice* physical_device, bool& is_suitable
)
{
   uint32_t extensionsCount;
   vkEnumerateDeviceExtensionProperties(
       physical_device->physical_device,
       nullptr,
       &extensionsCount,
       nullptr
   );

   std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
   vkEnumerateDeviceExtensionProperties(
       physical_device->physical_device,
       nullptr,
       &extensionsCount,
       availableExtensions.data()
   );

   std::set<std::string> requiredExtensions(
       bfvDeviceExtensions.begin(),
       bfvDeviceExtensions.end()
   );

   for (auto& extension : availableExtensions)
   {
      requiredExtensions.erase(extension.extensionName);
   }
   is_suitable = requiredExtensions.empty();

   BfSingleEvent event{};
   if (is_suitable)
   {
      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
      event.action =
          BfEnActionType::BF_ACTION_TYPE_CHECK_EXTENSION_SUPPORT_SUCCESS;
   }
   else
   {
      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
      event.action =
          BfEnActionType::BF_ACTION_TYPE_CHECK_EXTENSION_SUPPORT_FAILURE;
   }
   return BfEvent(event);
}

BfEvent
bfFindQueueFamilyIndices(
    BfPhysicalDevice* bf_physical_device, VkSurfaceKHR surface
)
{
   uint32_t queueFamiliesCount = 0;
   vkGetPhysicalDeviceQueueFamilyProperties(
       bf_physical_device->physical_device,
       &queueFamiliesCount,
       nullptr
   );

   std::vector<VkQueueFamilyProperties> queueFamilies(queueFamiliesCount);
   vkGetPhysicalDeviceQueueFamilyProperties(
       bf_physical_device->physical_device,
       &queueFamiliesCount,
       queueFamilies.data()
   );

   int i = 0;
   for (const auto& queueFamily : queueFamilies)
   {
      if (bfvEnabledQueueTypes.contains(BF_QUEUE_GRAPHICS_TYPE) &&
          !bf_physical_device->queue_family_indices[BF_QUEUE_GRAPHICS_TYPE]
               .has_value() &&
          (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
      {
         bf_physical_device->queue_family_indices[BF_QUEUE_GRAPHICS_TYPE] = i;
      }

      if (bfvEnabledQueueTypes.contains(BF_QUEUE_PRESENT_TYPE) &&
          !bf_physical_device->queue_family_indices[BF_QUEUE_PRESENT_TYPE]
               .has_value())
      {
         VkBool32 presentSupport = false;
         vkGetPhysicalDeviceSurfaceSupportKHR(
             bf_physical_device->physical_device,
             i,
             surface,
             &presentSupport
         );

         if (presentSupport)
         {
            bf_physical_device->queue_family_indices[BF_QUEUE_PRESENT_TYPE] = i;
         }
      }

      if (bfvEnabledQueueTypes.contains(BF_QUEUE_TRANSFER_TYPE) &&
          !bf_physical_device->queue_family_indices[BF_QUEUE_TRANSFER_TYPE]
               .has_value() &&
          (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT))
      {
         bf_physical_device->queue_family_indices[BF_QUEUE_TRANSFER_TYPE] = i;
      }

      if (bfvEnabledQueueTypes.contains(BF_QUEUE_COMPUTE_TYPE) &&
          !bf_physical_device->queue_family_indices[BF_QUEUE_COMPUTE_TYPE]
               .has_value() &&
          (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
      {
         bf_physical_device->queue_family_indices[BF_QUEUE_COMPUTE_TYPE] = i;
      }

      i++;
   }

   int decision = true;
   std::for_each(
       bf_physical_device->queue_family_indices.begin(),
       bf_physical_device->queue_family_indices.end(),
       [&](const std::pair<BfvEnQueueType, std::optional<uint32_t>>& pair) {
          if (!pair.second.has_value()) decision = false;
       }
   );

   BfSingleEvent event{};
   if (decision)
   {
      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_CHECK_QUEUE_SUPPORT_SUCCESS;
   }
   else
   {
      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_CHECK_QUEUE_SUPPORT_FAILURE;
   }
   return BfEvent(event);
}

BfEvent
bfIsDeviceSuitable(
    BfPhysicalDevice* bf_physical_device,
    VkSurfaceKHR surface,
    bool& is_suitable
)
{
   bfFindQueueFamilyIndices(bf_physical_device, surface);

   bool extensionsSupport;
   bfCheckDeviceExtensionSupport(bf_physical_device, extensionsSupport);
   bool swapChainAdequate = false;
   if (extensionsSupport)
   {
      BfSwapChainSupport swapChainSupport;
      bfGetSwapChainSupport(
          bf_physical_device->physical_device,
          surface,
          swapChainSupport
      );

      swapChainAdequate = !swapChainSupport.formats.empty() &&
                          !swapChainSupport.presentModes.empty();
   }

   bool isQueuesCorrect;
   bfIsQueueFamilyIndicesCorrect(bf_physical_device, isQueuesCorrect);

   BfSingleEvent event{};
   if (isQueuesCorrect && extensionsSupport && swapChainAdequate)
   {
      is_suitable = true;
      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
      event.action = BfEnActionType::
          BF_ACTION_TYPE_CHECK_PHYSICAL_DEVICE_SUITABILITY_SUCCESS;
   }
   else
   {
      is_suitable = false;
      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CHECK_EVENT;
      event.action = BfEnActionType::
          BF_ACTION_TYPE_CHECK_PHYSICAL_DEVICE_SUITABILITY_FAILURE;
   }

   return BfEvent(event);
}
