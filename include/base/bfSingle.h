#pragma once
#include <vulkan/vulkan_core.h>
#ifndef BF_SINGLE_H
#define BF_SINGLE_H
#include "bfPhysicalDevice.h"
#include "bfVariative.hpp"
#include <functional>
#include <map>
#include <stack>
#include <stdexcept>

namespace base
{

class g
{
public:
   static void create(
       int frames,
       uint32_t* pcurrent_frame,
       VkInstance* vkinst,
       BfPhysicalDevice* bfphdevice,
       VkDevice* vkdevice,
       VkCommandPool* vkcmpool
       // VkSampler* vksampler
   )
   {
      auto& base = g::inst();
      base.m_frames = frames;
      base.m_vkinst = vkinst;
      base.m_bfphdevice = bfphdevice;
      base.m_vkdevice = vkdevice;
      base.m_vkcmpool = vkcmpool;
      base.m_currentframe = pcurrent_frame;
      // base.m_vksampler = vksampler;
   }

   static void bindSampler(VkSampler* vksampler)
   {
      g::inst().m_vksampler = vksampler;
   }

   static void bindStack(std::stack< std::function< void() > >* m_intrstack)
   {
      g::inst().m_intrstack = m_intrstack;
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
   static int frames() noexcept { return g::inst().m_frames; }
   static uint32_t currentFrame() noexcept { return *g::inst().m_currentframe; }
   
   static std::stack< std::function< void() > >& intrstack() { return *g::inst().m_intrstack; }
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
   int m_frames;
   uint32_t* m_currentframe;
   std::stack< std::function< void() > >* m_intrstack;

   VkInstance* m_vkinst;
   BfPhysicalDevice* m_bfphdevice;
   VkDevice* m_vkdevice;
   VkCommandPool* m_vkcmpool;
   VkSampler* m_vksampler;
};

} // namespace base

#endif
