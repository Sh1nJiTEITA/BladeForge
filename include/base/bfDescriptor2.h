#pragma once

#ifndef BF_DESCRIPTOR2_H
#define BF_DESCRIPTOR2_H
#include "bfDrawObjectBuffer.h"
#include "bfSingle.h"
#include "bfVariative.hpp"
#include "stb_image.h"
#include <algorithm>
#include <filesystem>
#include <fmt/format.h>
#include <iterator>
#include <memory>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace base
{
namespace desc
{

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

struct BfDescriptorPool
{
   BfDescriptorPool(const std::vector< VkDescriptorPoolSize >& sz)
   {
      VkDescriptorPoolCreateInfo poolInfo{
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .maxSets = 10,
          .poolSizeCount = static_cast< uint32_t >(sz.size()),
          .pPoolSizes = sz.data()
      };
      if (vkCreateDescriptorPool(
              base::g::device(),
              &poolInfo,
              nullptr,
              &m_pool
          ) != VK_SUCCESS)
      {
         throw std::runtime_error("Cant create descriptor pool!");
      }
   }
   ~BfDescriptorPool()
   {
      vkDestroyDescriptorPool(base::g::device(), m_pool, nullptr);
   }

   VkDescriptorPool& handle() noexcept { return m_pool; }

private:
   VkDescriptorPool m_pool;
};
//
//
//
//
//
//

struct BfDescriptor
{
   /**
    * @brief Создание дескриптора
    *
    * @param layout Индекс лейаута, к которому этот дескриптор будет
    * прикреплен
    * @param binding Индекс внутри дескриптор сета, чтобы можно было
    * пользоваться дескриптором внутри шейдеров
    * @param stages На каких стадия будет доступен дескриптор. По дефолту
    * На фрагментном и вертекс шейдере
    * @param vkdesctype Тип дескриптора по Vulkan
    */
   BfDescriptor(
       uint8_t layout,
       uint8_t binding,
       VkDescriptorType vkdesctype,
       bool has_buffer,
       bool has_image,
       VkShaderStageFlags stages = VK_SHADER_STAGE_FRAGMENT_BIT |
                                   VK_SHADER_STAGE_VERTEX_BIT
   )
       : layout{layout}
       , binding{binding}
       , vkShaderStages{stages}
       , vkType{vkdesctype}
       , hasImage{has_image}
       , hasBuffer{has_buffer}
   {
   }

   // clang-format off
   virtual void 
   createBuffer() {
      throw std::runtime_error("base::desc::BfDescriptor::createBuffer must be implemented");
   }

   virtual void 
   createImage() {
      throw std::runtime_error("base::desc::BfDescriptor::createImage must be implemented");
   }

   virtual void 
   createView() {
      throw std::runtime_error("base::desc::BfDescriptor::createView must be implemented");
   }

   VkDescriptorSetLayoutBinding 
   layoutBinding() { 
      return VkDescriptorSetLayoutBinding { 
         .binding = binding, 
         .descriptorType = vkType, 
         .descriptorCount = 1,
         .stageFlags = vkShaderStages,
         .pImmutableSamplers = nullptr
      };
   }

   virtual VkDescriptorBufferInfo bufferInfo() { 
      if (!m_buffer) { 
         // throw std::runtime_error("Buffer is not valid to get desc buffer info"); 
         return VkDescriptorBufferInfo{};
      }
      return VkDescriptorBufferInfo { 
         .buffer = m_buffer->handle(),
         .offset = 0,
         .range  = m_buffer->size()
      };
   }

   virtual VkDescriptorImageInfo imageInfo() { 
      if (!m_image) { 
         // throw std::runtime_error("Buffer is not valid to get desc buffer info"); 
         return VkDescriptorImageInfo{};
      }
      return VkDescriptorImageInfo { 
         // .sampler = m_sampler->handle(),
         .sampler = base::g::sampler(),
         .imageView = m_view->handle(),
         .imageLayout = m_image->layout()
      };
   }

   const uint8_t layout;
   const uint8_t binding;
   const VkShaderStageFlags vkShaderStages;
   const VkDescriptorType vkType;
   const bool hasBuffer;
   const bool hasImage;

protected:
   
   std::unique_ptr< obj::BfBuffer > m_buffer;
   std::unique_ptr< obj::BfImage > m_image;
   std::unique_ptr< obj::BfImageView > m_view;
   // std::shared_ptr< BfSampler > m_sampler;
};


//
//
//
//
//
struct BfDescriptorSet
{
   /**
    * @brief Для того, чтобы создать дескриптор сет (лейаут)
    * нужно знать, что будет к нему прикреплено, чтобы это было
    * доступно внутри шейдера. То есть при создании нужно указать
    * все дескрипторы, которые связаны с ним.
    */
   BfDescriptorSet(std::vector< std::unique_ptr< BfDescriptor >>&& desc)
      : m_desc{ std::move(desc) }
   {
      std::vector< VkDescriptorSetLayoutBinding > bindings;
      std::transform(m_desc.begin(), m_desc.end(), 
                     std::back_inserter(bindings),
                     [](const auto& d) { 
                        return d->layoutBinding();
                     });

      VkDescriptorSetLayoutCreateInfo l{
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .bindingCount = static_cast< uint32_t >(bindings.size()),
          .pBindings = bindings.data()
      };

      if (vkCreateDescriptorSetLayout(
              base::g::device(),
              &l,
              nullptr,
              &m_layout
          ) != VK_SUCCESS)
      {
         throw std::runtime_error("Cant create Descriptor set");
      }
   }
   ~BfDescriptorSet()
   {
      vkDestroyDescriptorSetLayout(base::g::device(), m_layout, nullptr);
      m_desc.clear();
   }
   
   void allocate(const VkDescriptorPool& pool) { 
      m_sets.clear();
      m_sets.resize(base::g::frames());
      auto frames = base::g::frames();
      for (int i = 0; i < frames; ++i) { 
         VkDescriptorSetAllocateInfo info{
            .sType =  VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &m_layout
         };
         if (vkAllocateDescriptorSets(base::g::device(), &info, &m_sets[i]) != VK_SUCCESS) { 
            throw std::runtime_error("Cant allocate descriptor set");
         }
         fmt::println("Allocated set for frame={}", i);
      }
   }

   

   std::vector<VkWriteDescriptorSet> write(
      int frame,
      std::list<VkDescriptorBufferInfo>& binfos,
      std::list<VkDescriptorImageInfo>& iinfos
   ) { 
      std::vector<VkWriteDescriptorSet> writes;
      auto& set = m_sets[frame];
      for (auto& desc : m_desc) { 
         auto& buffer_info = *binfos.insert(binfos.end(), desc->bufferInfo());
         auto& image_info = *iinfos.insert(iinfos.end(), desc->imageInfo());
         VkWriteDescriptorSet w {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr, .dstSet = set, .dstBinding = desc->binding,
            .descriptorCount = 1,
            .descriptorType = desc->vkType,
            .pImageInfo = desc->hasImage ? &image_info : nullptr,
            .pBufferInfo = desc->hasBuffer ? &buffer_info : nullptr,
         };
         writes.push_back(std::move(w));
      }
      return writes;
   }

   VkDescriptorSetLayout& layoutHandle() noexcept { return m_layout; }
   VkDescriptorSet& setHandle(int frame) noexcept { return m_sets[frame]; }
   
   template <typename T>
   T& get(uint8_t binding) { 
      return static_cast<T&>(*m_desc[binding]);
   }

private:
   std::vector< std::unique_ptr< BfDescriptor > > m_desc;
   VkDescriptorSetLayout m_layout;
   std::vector< VkDescriptorSet > m_sets;
};

//
//
//
//
//
//
//
//

struct BfDescriptorTexture : public BfDescriptor { 
   BfDescriptorTexture(
      const std::filesystem::path& path,
      uint8_t layout, 
      uint8_t binding,
      VkShaderStageFlags stages = VK_SHADER_STAGE_FRAGMENT_BIT)
   : BfDescriptor(
       layout,
       binding,
       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       false,
       true,
       stages
   )
   , m_path { path }
   {
   }

   void reload(const std::filesystem::path& newPath) {
      m_path = newPath;
      m_buffer.reset();
      m_image.reset();
      m_view.reset();
      createBuffer();
      createImage();
      createView();
   }

   // clang-format off
   virtual void 
   createBuffer() override {
      if (!std::filesystem::exists(std::filesystem::absolute(m_path))) { 
         std::string msg = fmt::format("Cant load image... path={} does not exist", m_path.string());
         throw std::runtime_error(msg.c_str());
      }

      stbi_uc* pixels = stbi_load(std::filesystem::absolute(m_path).c_str(), &m_texWidth, &m_texHeight, &m_texChannels, STBI_rgb_alpha);
      VkDeviceSize imageSize = m_texWidth * m_texHeight * 4;

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
   }

   virtual void 
   createImage() override {
      m_image = std::make_unique< obj::BfImage >(
         m_texWidth,
         m_texHeight,
         VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
         VMA_MEMORY_USAGE_AUTO
      );
      transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

      copyBufferToImage();

      transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
   }

   virtual void 
   createView() override {
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
         m_image->layout() = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      }
      else if (oldl == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newl == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
      {
         barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
         barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

         sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
         destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
         m_image->layout() = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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

   const std::filesystem::path& path() noexcept { return m_path; }
   const VkImageView& view() noexcept { return m_view->handle(); }
   const VkImage& image() noexcept { return m_image->handle(); }

protected:
   int m_texWidth;
   int m_texHeight;
   int m_texChannels;
   std::filesystem::path m_path;
};

//
//
//
//
//
//
//
//
//
//
//
//

// clang-format off
template < typename E >
struct BfDescriptorManager {
   BfDescriptorManager() {}

   template < typename T, typename... Args >
   void add(Args&&... args)
   {
      auto new_desc = std::make_unique< T >(std::forward< Args >(args)...);
      new_desc->createBuffer();
      new_desc->createImage();
      new_desc->createView();
      fmt::println("New descriptor with layout={} binding={} added",
                   static_cast<int>(new_desc->layout),
                   static_cast<int>(new_desc->binding));
      m_descStack.push(std::move(new_desc));
   }

   void createPool(const std::vector< VkDescriptorPoolSize >& sz) { 
      fmt::println("Creating descriptor pool");
      m_pool = std::make_unique<BfDescriptorPool>(sz);
   }

   void createLayouts() { 
      using set_index = uint8_t; 
      using binding_index = uint8_t;

      std::unordered_map<set_index, 
                         std::unordered_map< binding_index, 
                                             std::unique_ptr< BfDescriptor > > 
                        > m;

      while (!m_descStack.empty()) { 
         auto desc = std::move(m_descStack.top());
         m_descStack.pop();

         uint8_t layout = desc->layout;
         uint8_t binding = desc->binding;

         auto& set = m[layout];
         if (set.empty()) {
            fmt::println("New set with index={} found in descriptors data", layout);
         }
         auto [_, new_desc_in_set_success] = m.at(layout).insert({binding, std::move(desc)});
         if (!new_desc_in_set_success) { 
            std::string msg = fmt::format("Cant add descriptor with binding={} to set={}, already exists", 
                                          layout, binding);
            throw std::runtime_error(msg);
         }
         else { 
            fmt::println("New descriptor with binding={} added to set with index={}", binding, layout);
         }
      }

      for (auto& [set_i, bindings_map] : m) { 
         fmt::println("Creating descriptor-set-layout with index={} and descriptors count={}", set_i, bindings_map.size());
         std::vector< std::unique_ptr< BfDescriptor > > current_set_descriptors; 
         std::transform(
            bindings_map.begin(), 
            bindings_map.end(),
            std::back_inserter(current_set_descriptors),
            [](auto&& pair) { return std::move(pair.second); }
         );
         auto set_name = static_cast< E >( set_i );
         auto complete_set = std::make_unique<BfDescriptorSet>( std::move(current_set_descriptors) );
         m_set.insert_or_assign(set_name, std::move(complete_set) );
      }
   }

   void allocateSets() { 
      for (auto& [layout_index, set] : m_set) { 
         fmt::println("Allocating sets for layout={}", static_cast<int>(layout_index));
         set->allocate(m_pool->handle());
      }
   }
   
   void updateSets() { 
      for (int i = 0; i < base::g::frames(); ++i) { 
         std::vector<VkWriteDescriptorSet> writes;
         std::list<VkDescriptorBufferInfo> binfos;
         std::list<VkDescriptorImageInfo> iinfos;
         // FIXME: Missing texel ...

         for (auto& [layout, pset] : m_set) { 
            auto new_writes = pset->write(i, binfos, iinfos);
            std::copy(new_writes.begin(), 
                      new_writes.end(),
                      std::back_inserter(writes));

         }
         vkUpdateDescriptorSets(
            base::g::device(),
            static_cast< uint32_t >(writes.size()),
            writes.data(),
            0,
            nullptr
         );
      }
   }
   
   void bindSets(
      E layout, 
      int frame,
      VkCommandBuffer cb, 
      VkPipelineLayout pllayout
   ) { 
      vkCmdBindDescriptorSets(
         cb,
         VK_PIPELINE_BIND_POINT_GRAPHICS,
         pllayout,
         static_cast<uint32_t>(layout),
         1,
         &m_set[layout]->setHandle(frame),
         0,                  
         nullptr            
   );
   }

   auto getAllLayouts() -> std::vector< VkDescriptorSetLayout > { 
      std::vector< VkDescriptorSetLayout > l;
      for (auto& [layout, set] : m_set) { 
         l.push_back(set->layoutHandle());
      }
      return l;
   }
   
   void cleanup() { 
      m_pool.reset();
      m_set.clear();
   }

   template< typename T> 
   T& get(E layout, uint8_t binding) { 
      auto& ptr = m_set[layout];
      return ptr->template get<T&>(binding);
   }

   ~BfDescriptorManager() { cleanup(); }

private:
   std::unordered_map< E, std::unique_ptr< BfDescriptorSet > > m_set;
   std::stack< std::unique_ptr<BfDescriptor> > m_descStack;
   std::unique_ptr< BfDescriptorPool > m_pool;
};

//
//
//
//
//
//
//
//
//
//

struct BfDescriptorPipeline
{
   virtual void create() = 0;
};

}; // namespace desc
}; // namespace base

#endif
