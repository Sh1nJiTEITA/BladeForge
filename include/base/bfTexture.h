#pragma once
#include <vulkan/vulkan_core.h>
#ifndef BF_TEXTURE_LOAD_NEW_H
#define BF_TEXTURE_LOAD_NEW_H

#include "bfDrawObjectBuffer.h"
#include "bfSingle.h"
#include "bfVariative.hpp"
#include "stb_image.h"
#include <filesystem>
#include <fmt/base.h>
#include <fmt/color.h>
#include <memory>
#include <stdexcept>

namespace fs = std::filesystem;
namespace base
{
namespace texture
{

// class BfTextureBuffer
// {
// public:
//    BfTextureBuffer(size_t size) {
//
//    }
// };

class BfTexture
{
public:
   template < typename PATH >
   BfTexture(PATH&& path)
       : m_path{std::forward< PATH >(path)}
   {
      // clang-format off
      if (!fs::exists(fs::absolute(m_path))) { 
         std::string msg = fmt::format("Cant load image... path={} does not exist", m_path.string());
         throw std::runtime_error(msg.c_str());
      }

      int texWidth, texHeight, texChannels;
      stbi_uc* pixels = stbi_load(fs::absolute(m_path).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
      VkDeviceSize imageSize = texWidth * texHeight * 4;

      if (!pixels)
      {
         throw std::runtime_error("failed to load texture image!");
      }
      
      m_buffer = std::make_unique< obj::BfBuffer >(
         static_cast<size_t>(imageSize), 
         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
         VMA_MEMORY_USAGE_AUTO,
         VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | 
         VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
      );

      void* data = m_buffer->map(); 
      {
         memcpy(data, pixels, static_cast<size_t>(imageSize)); 
      }
      m_buffer->unmap();
      stbi_image_free(pixels);

      m_image = std::make_unique< obj::BfImage >(
         texWidth,
         texHeight,
         VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
         VMA_MEMORY_USAGE_AUTO
      );
      transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

      copyBufferToImage();

      transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

      createView(); 
   }

   void createView() { 
      VkImageViewCreateInfo viewInfo{};
      viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      viewInfo.image = m_image->handle();
      viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
      viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      viewInfo.subresourceRange.baseMipLevel = 0;
      viewInfo.subresourceRange.levelCount = 1;
      viewInfo.subresourceRange.baseArrayLayer = 0;
      viewInfo.subresourceRange.layerCount = 1;
      m_view = std::make_unique<obj::BfImageView>(viewInfo);
   }


   void transitionImageLayout(VkImageLayout oldl, VkImageLayout newl) {
      // clang-format on
      VkCommandBuffer cb = base::g::beginSingleTimeCommands();

      VkImageMemoryBarrier barrier{};
      barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier.oldLayout = oldl;
      barrier.newLayout = newl;

      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

      barrier.image = m_image->handle();
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      barrier.srcAccessMask = 0; // TODO
      barrier.dstAccessMask = 0; // TODO

      VkPipelineStageFlags sourceStage;
      VkPipelineStageFlags destinationStage;

      if (oldl == VK_IMAGE_LAYOUT_UNDEFINED &&
          newl == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
      {
         barrier.srcAccessMask = 0;
         barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

         sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
         destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      }
      else if (oldl == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newl == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
      {
         barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
         barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

         sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
         destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      }
      else
      {
         throw std::invalid_argument("unsupported layout transition!");
      }

      vkCmdPipelineBarrier(
          cb,
          sourceStage,
          destinationStage,
          0,
          0,
          nullptr,
          0,
          nullptr,
          1,
          &barrier
      );
      base::g::endSingleTimeCommands(cb);
   };

   // clang-format on
   void copyBufferToImage()
   {
      VkCommandBuffer cb = base::g::beginSingleTimeCommands();
      {

         VkBufferImageCopy region{};
         region.bufferOffset = 0;
         region.bufferRowLength = 0;
         region.bufferImageHeight = 0;
         region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
         region.imageSubresource.mipLevel = 0;
         region.imageSubresource.baseArrayLayer = 0;
         region.imageSubresource.layerCount = 1;
         region.imageOffset = {0, 0, 0};
         region.imageExtent = {
             static_cast< uint32_t >(m_image->width()),
             static_cast< uint32_t >(m_image->height()),
             1
         };

         vkCmdCopyBufferToImage(
             cb,
             m_buffer->handle(),
             m_image->handle(),
             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
             1,
             &region
         );
      }
      base::g::endSingleTimeCommands(cb);
   }

   ~BfTexture()
   {
      fmt::println("============== DESCTRUCTING TEXTURE BUFFER ==============");
      fmt::println("with path = {}", m_path.string());
   }

   const fs::path& path() noexcept { return m_path; }
   const VkImageView& view() noexcept { return m_view->handle(); }
   const VkImage& image() noexcept { return m_image->handle(); }

private:
   const fs::path m_path;
   std::unique_ptr< obj::BfBuffer > m_buffer;
   std::unique_ptr< obj::BfImage > m_image;
   std::unique_ptr< obj::BfImageView > m_view;
};

class BfSampler
{
public:
   BfSampler()
   {
      VkSamplerCreateInfo samplerInfo{};
      samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerInfo.magFilter = VK_FILTER_LINEAR;
      samplerInfo.minFilter = VK_FILTER_LINEAR;

      samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

      samplerInfo.anisotropyEnable = VK_TRUE;
      samplerInfo.maxAnisotropy =
          base::g::phdeviceprop().limits.maxSamplerAnisotropy;

      samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
      samplerInfo.unnormalizedCoordinates = VK_FALSE;

      samplerInfo.compareEnable = VK_FALSE;
      samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

      samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerInfo.mipLodBias = 0.0f;
      samplerInfo.minLod = 0.0f;
      samplerInfo.maxLod = 0.0f;

      if (vkCreateSampler(
              base::g::device(),
              &samplerInfo,
              nullptr,
              &m_sampler
          ) != VK_SUCCESS)
      {
         throw std::runtime_error("failed to create texture sampler!");
      }
   }

   ~BfSampler() { vkDestroySampler(base::g::device(), m_sampler, nullptr); }

   VkSampler& handle() { return m_sampler; }

private:
   VkSampler m_sampler;
};

}; // namespace texture
}; // namespace base

#endif
