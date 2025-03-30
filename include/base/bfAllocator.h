#ifndef BF_CUSTOM_ALLOCATOR_H
#define BF_CUSTOM_ALLOCATOR_H

#include <vulkan/vulkan_core.h>

#include <stdexcept>

#include "bfVariative.hpp"

class BfAllocator
{
   BfAllocator() {}

public:
   static BfAllocator& inst()
   {
      static BfAllocator m{};
      return m;
   }

   void create(VkDevice device, VkInstance instance, VkPhysicalDevice pdevice)
   {
      VmaAllocatorCreateInfo info{
          .physicalDevice = pdevice,
          .device = device,
          .instance = instance,
      };
      // VmaAllocatorCreateInfo info{};
      // info.device = device;
      // info.instance = instance;
      // info.physicalDevice = pdevice;

      if (vmaCreateAllocator(&info, &m_allocator) != VK_SUCCESS)
      {
         throw std::runtime_error("Allocator was not created");
      }
   }
   void destroy() { vmaDestroyAllocator(m_allocator); }

   VmaAllocator get() { return m_allocator; }

private:
   VmaAllocator m_allocator;
};

#endif
