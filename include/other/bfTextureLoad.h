#ifndef BF_TEXTURE_LOAD_H
#define BF_TEXTURE_LOAD_H

#include "bfBuffer.h"
#include "bfCurves2.h"
#include "bfVariative.hpp"
#include "vk_mem_alloc.h"

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

   void open();
   void load(VmaAllocator allocator, VkDevice device);

   bool               is_ok() const;
   uint32_t           id() const;
   const std::string& path() const;
   VmaAllocator       allocator() const;
   uint32_t           size() const;

   int width() const;
   int height() const;
   int channels() const;

   BfAllocatedImage* image();
   uint8_t*          data();
};

class BfTextureLoader
{
   VkDevice*     __pDevice;
   VmaAllocator* __pAllocator;

   VkSampler __sampler;

   void __create_sampler();

public:
   BfTextureLoader();
   BfTextureLoader(VkDevice* device, VmaAllocator* allocator);
   ~BfTextureLoader();

   BfTexture load(const std::string& path);
};
#endif
