#ifndef BF_TEXTURE_LOAD_H
#define BF_TEXTURE_LOAD_H

#include <vulkan/vulkan_core.h>

#include "bfBuffer.h"
#include "bfCurves2.h"
#include "bfPhysicalDevice.h"
#include "bfVariative.hpp"
#include "vk_mem_alloc.h"

class BfTextureLoader;

class BfTexture
{
   std::string __path;

   int      __width;
   int      __height;
   int      __channels;
   uint8_t* __data;

   BfAllocatedImage __image;

   uint32_t __id;

public:
   BfTexture(std::string path);
   ~BfTexture();

   // Use STB-image to open image data
   void open();
   // Close image to free data inside __data
   void close();

   void load(VmaAllocator allocator, VkDevice device);

   bool               is_ok() const;
   uint32_t           id() const;
   const std::string& path() const;
   VmaAllocator       allocator() const;
   uint32_t           size() const;
   
   BfAllocatedImage* image();

   int width() const;
   int height() const;
   int channels() const;

   friend BfTextureLoader;
};

class BfTextureLoader
{
   BfPhysicalDevice* __pPhysicalDevice;
   VkDevice*         __pDevice;
   VmaAllocator*     __pAllocator;
   VkCommandPool*    __pCommandPool;

   VkSampler       __sampler;
   VkCommandBuffer __commandBuffer;

   void __create_temp_buffer(BfAllocatedBuffer* buffer);
   void __map_temp_buffer(BfAllocatedBuffer* buffer, BfTexture* texture

   );
   void __create_sampler();
   void __create_texture_image(BfTexture* texture);

   void __transition_image(BfTexture* image,
                           VkImageLayout     o,
                           VkImageLayout     n);

   void __copy_buffer_to_image(BfAllocatedBuffer* buffer,
                               BfAllocatedImage*  image,
                               uint32_t           w,
                               uint32_t           h);

   void __create_texture_image_view(BfTexture* texture);

public:
   BfTextureLoader();
   BfTextureLoader(BfPhysicalDevice* physical_device,
                   VkDevice*         device,
                   VmaAllocator*     allocator,
                   VkCommandPool*    pool);
   void kill();
   ~BfTextureLoader();

   void __begin_single_time_command();
   void __end_single_time_command();

   BfTexture load(const std::string& path);
};
#endif
