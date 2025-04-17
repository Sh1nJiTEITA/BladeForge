#ifndef BF_HOLDER_H
#define BF_HOLDER_H

#include <memory>
#include <vector>

#include "bfBuffer.h"
#include "bfDescriptor.h"
#include "bfEvent.h"
#include "bfPhysicalDevice.h"
#include "bfVertex2.hpp"
#include "bfWindow.h"

struct BfHolder
{
   std::vector< BfWindow > windows;
   std::vector< BfPhysicalDevice > physical_devices;

   // Image-pack arrays
   std::vector< VkImage > images;
   std::vector< BfAllocatedImage > images_id;
   std::vector< VkImageView > image_views;
   std::vector< VkFramebuffer > standart_framebuffers;
   std::vector< VkFramebuffer > gui_framebuffers;

   // Frame-pack arrays
   // Uniforms
   std::vector< BfAllocatedBuffer > bezier_points_buffers;
   std::vector< BfAllocatedBuffer > model_buffers;
   std::vector< BfAllocatedBuffer > pos_pick_buffers;

   std::vector< VkDescriptorSet > global_descriptor_sets;
   std::vector< VkDescriptorSet > main_descriptor_sets;

   std::vector< VkCommandBuffer > standart_command_buffers;
   std::vector< VkCommandBuffer > gui_command_buffers;
   // Sync
   std::vector< VkSemaphore > available_image_semaphores;
   std::vector< VkSemaphore > finish_render_image_semaphores;
   std::vector< VkFence > frame_in_flight_fences;

   static BfHolder* __bfpHolder;

   BfHolder();
};

BfEvent bfBindHolder(BfHolder* holder);
BfEvent bfHoldWindow(BfWindow*& window);
BfEvent bfHoldPhysicalDevice(BfPhysicalDevice*& window);
BfHolder* bfGetpHolder();

#endif
