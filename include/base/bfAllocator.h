#ifndef BF_CUSTOM_ALLOCATOR_H
#define BF_CUSTOM_ALLOCATOR_H

#include <vulkan/vulkan_core.h>

#include <stdexcept>

#include "bfVariative.hpp"

class BfAllocator
{
   BfAllocator()
       : m_allocator{nullptr}
   {
   }

public:
   static BfAllocator& inst()
   {
      static BfAllocator m{};
      return m;
   }

   static void create(
       VkDevice device, VkInstance instance, VkPhysicalDevice pdevice
   )
   {
      VmaAllocatorCreateInfo info{
          .physicalDevice = pdevice,
          .device = device,
          .instance = instance,
      };
      if (vmaCreateAllocator(&info, &inst().m_allocator) != VK_SUCCESS)
      {
         throw std::runtime_error("Allocator was not created");
      }
      std::cout << "Allocator created... " << inst().m_allocator << "\n";
   }

   static void destroy() { vmaDestroyAllocator(inst().m_allocator); }
   static VmaAllocator get() { return inst().m_allocator; }

private:
   VmaAllocator m_allocator;
};

#endif
