#pragma once
#include "bfDescriptor.h"
#include "bfDrawObjectBuffer.h"
#include "bfTexture.h"
#include <algorithm>
#include <fmt/format.h>
#include <iterator>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <vulkan/vulkan_core.h>
#ifndef BF_DESCRIPTOR2_H
#define BF_DESCRIPTOR2_H

#include "bfSingle.h"
#include "bfVariative.hpp"
#include <memory>
#include <vector>

namespace base
{
namespace desc
{

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
         .sampler = m_sampler->handle(),
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
   std::shared_ptr< texture::BfSampler > m_sampler;
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
