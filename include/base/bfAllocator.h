#ifndef BF_CUSTOM_ALLOCATOR_H
#define BF_CUSTOM_ALLOCATOR_H

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

   static void
   create(VkDevice device, VkInstance instance, VkPhysicalDevice pdevice)
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
      fmt::printf(
          "Allocator created... {}\n",
          static_cast< void* >(inst().m_allocator)
      );
   }

   static void destroy() { vmaDestroyAllocator(inst().m_allocator); }
   static VmaAllocator get() { return inst().m_allocator; }

private:
   VmaAllocator m_allocator;
};

#endif
