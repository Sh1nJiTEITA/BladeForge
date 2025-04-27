#pragma once
#include <vulkan/vulkan_core.h>
#ifndef BF_SINGLE_H
#define BF_SINGLE_H
#include "bfPhysicalDevice.h"
#include "bfVariative.hpp"
#include <map>
#include <stdexcept>

namespace base
{

class g
{
public:
   static void create(
       VkInstance* vkinst,
       BfPhysicalDevice* bfphdevice,
       VkDevice* vkdevice,
       VkCommandPool* vkcmpool
       // VkSampler* vksampler
   )
   {
      auto& base = g::inst();
      base.m_vkinst = vkinst;
      base.m_bfphdevice = bfphdevice;
      base.m_vkdevice = vkdevice;
      base.m_vkcmpool = vkcmpool;
      // base.m_vksampler = vksampler;
   }

   static void bindSampler(VkSampler* vksampler)
   {
      g::inst().m_vksampler = vksampler;
   }

   static void validate()
   {
      // clang-format off
      auto& base = g::inst();
      if (!base.m_bfphdevice) throw std::runtime_error("BfPhysicalDevice not bound to g");
      if (!base.m_vkdevice) throw std::runtime_error("VkDevice not bound to g");
      if (!base.m_vkinst) throw std::runtime_error("VkInstance not bound to g");
      // clang-format on
   }

   static g& inst() noexcept
   {
      static g _;
      return _;
   }

   // clang-format off
   static VkDevice& device() { return *g::inst().m_vkdevice; }
   static VkInstance& instance() { return *g::inst().m_vkinst; }
   static VkPhysicalDevice& phdevice() { return g::inst().m_bfphdevice->physical_device; }
   static VkCommandPool& cmpool() { return *g::inst().m_vkcmpool; }
   static VkSampler& sampler() { return *g::inst().m_vksampler; }
   static VkPhysicalDeviceProperties& phdeviceprop() { return g::inst().m_bfphdevice->properties; }
   // clang-format on

   static VkCommandBuffer beginSingleTimeCommands()
   {
      VkCommandBufferAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      allocInfo.commandPool = g::cmpool();
      allocInfo.commandBufferCount = 1;

      // VkCommandBuffer commandBuffer;
      VkCommandBuffer buf;
      vkAllocateCommandBuffers(g::device(), &allocInfo, &buf);

      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

      vkBeginCommandBuffer(buf, &beginInfo);
      return buf;
   }

   static void endSingleTimeCommands(VkCommandBuffer buf)
   {
      auto& base = g::inst();
      vkEndCommandBuffer(buf);

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &buf;

      vkQueueSubmit(
          base.m_bfphdevice->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE],
          1,
          &submitInfo,
          VK_NULL_HANDLE
      );
      vkQueueWaitIdle(
          base.m_bfphdevice->queues[BfvEnQueueType::BF_QUEUE_GRAPHICS_TYPE]
      );

      vkFreeCommandBuffers(*base.m_vkdevice, *base.m_vkcmpool, 1, &buf);
   }

private:
   VkInstance* m_vkinst;
   BfPhysicalDevice* m_bfphdevice;
   VkDevice* m_vkdevice;
   VkCommandPool* m_vkcmpool;
   VkSampler* m_vksampler;
};

} // namespace base

#endif
