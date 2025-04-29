#pragma once

#ifndef BF_DESCRIPTOR_STRUCTS_H
#define BF_DESCRIPTOR_STRUCTS_H

#include "bfDrawObjectBuffer.h"
#include "bfUniforms.h"
#include "bfVariative.hpp"
#include <bfDescriptor.h>
#include <fmt/color.h>
#include <vulkan/vulkan_core.h>

namespace base
{
namespace desc
{
namespace own
{

enum class SetType
{
   Main = 0,
   Global = 1,
   Texture = 2
};

struct BfDescriptorUBO : public BfDescriptor
{
   BfDescriptorUBO()
       : BfDescriptor(
             static_cast< uint8_t >(SetType::Main),
             0,
             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             true,
             false,
             VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT |
                 VK_SHADER_STAGE_FRAGMENT_BIT
         )
   {
      fmt::println("Creating descriptor UBO");
   }

   // clang-format off
   virtual void createBuffer() override {
      m_buffer = std::make_unique< obj::BfBuffer >( 
         sizeof(BfUniformView),
         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
         VMA_MEMORY_USAGE_CPU_TO_GPU,
         0
      );
   }

   virtual void createImage() override { }
   virtual void createView() override { }
   // clang-format on

   void map(const BfUniformView& ubo)
   {
      void* data = m_buffer->map();
      {
         memcpy(data, &ubo, sizeof(ubo));
      }
      m_buffer->unmap();
   }
};

struct BfDescriptorModel : public BfDescriptor
{
   BfDescriptorModel()
       : BfDescriptor(
             static_cast< uint8_t >(SetType::Global),
             0,
             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
             true,
             false,
             VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT |
                 VK_SHADER_STAGE_FRAGMENT_BIT
         )
   {
      fmt::println("Creating descriptor MODEL");
   }

   // clang-format off
   virtual void createBuffer() override {
      m_buffer = std::make_unique< obj::BfBuffer >( 
         sizeof(BfObjectData) * maxUniqueObjects,
         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
         // VMA_MEMORY_USAGE_AUTO,
         VMA_MEMORY_USAGE_CPU_TO_GPU
      );
   }

   virtual void createImage() override { }
   virtual void createView() override { }

   void* map() { return m_buffer->map(); }
   void unmap() { m_buffer->unmap(); }

   // clang-format on

   inline static const uint32_t maxUniqueObjects = 10000;
};

struct BfDescriptorTextureTest : public BfDescriptorTexture
{
   BfDescriptorTextureTest()
       : BfDescriptorTexture{"./resources/test.jpg", 2, 0}
   {
   }
};

struct BfDescriptorPipelineDefault : public BfDescriptorPipeline
{
   static BfDescriptorManager< SetType >& manager()
   {
      static BfDescriptorManager< SetType > m{};
      return m;
   }

   virtual void create() override
   {
      // clang-format off
      fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::burly_wood),
          "=== BfDescriptorPipelineDefault === BEGIN\n"
      );
      { 
         auto& man = this->manager();
         man.add< BfDescriptorUBO >();
         man.add< BfDescriptorModel >();
         man.add< BfDescriptorTextureTest >();


         std::vector< VkDescriptorPoolSize > sizes = {
             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
             {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
             {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 10},
             {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
         };
         man.createPool(sizes);
         man.createLayouts();
         man.allocateSets();
         man.updateSets();
      }
      fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::burly_wood),
          "=== BfDescriptorPipelineDefault === END\n"
      );

      // clang-format on
   };
};

} // namespace own

}; // namespace desc
} // namespace base

#endif
