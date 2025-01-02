/**
 * @file File for working with shaders
 * @brief ...
 */

#ifndef BF_PIPELINE_H
#define BF_PIPELINE_H

#include <vulkan/vulkan_core.h>

#include <bfVertex2.hpp>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <unordered_map>

#include "bfDescriptor.h"
#include "bfVariative.hpp"

namespace fs = std::filesystem;

class BfPipelineInterface;
class BfPipelineInterfaceStd;

typedef uint32_t BfPipelineType;
enum BfPipelineType_ : BfPipelineType
{
   BfPipelineType_Lines,
   BfPipelineType_Triangles,
   BfPipelineType_Axis
};

/**
 * @class BfPipelineHandler
 * @brief [Singleton] Vulkan pipeline handler to load/unload
 * shaders to make pipelines
 *
 */

class BfPipelineHandler
{
   static std::unique_ptr<BfPipelineHandler> __instance;
   //

   struct __PipelineData
   {
      VkPipeline pipeline;
      VkPipelineLayout layout;
   };
   std::unordered_map<BfPipelineType, __PipelineData> __pipelines;

public:
   static BfPipelineHandler* instance();

   // INTERACTION //
   template <class T>
   typename std::enable_if<std::is_base_of<BfPipelineInterface, T>::value>::type
   create(
       BfPipelineType type,
       const fs::path& shaders_path,
       const VkDevice& device,
       const VkRenderPass& render_pass,
       const BfDescriptor& desc
   )
   {
      auto builder = T();

      auto [iPipelineStruct, insertSuccess] = __pipelines.emplace();
      if (!insertSuccess)
      {
         throw std::runtime_error(
             "Cant emplace new pipeline to pipeline-storage"
         );
      }

      // LOADING SHADERS;
      std::vector<VkShaderModule> modules;
      std::vector<VkPipelineShaderStageCreateInfo> infos;

      __createShaderCreateInfos(device, shaders_path, modules, infos);

      auto pipelineLayoutCreateInfo = builder.genLayout(device, desc);
      if (vkCreatePipelineLayout(
              device,
              &pipelineLayoutCreateInfo,
              nullptr,
              &iPipelineStruct->second.layout
          ) == VK_SUCCESS)
      {
      }
      else
      {
         throw std::runtime_error("Line pipeline layout isn't done;");
      }

      auto VertexInputState = builder.genVertexInputState();
      auto DynamicState = builder.genDynamicState();
      auto ViewportState = builder.genViewportState();
      auto RasterizationState = builder.genRasterizationState();
      auto MultisampleState = builder.genMultisampleState();
      auto DepthStencilState = builder.genDepthStencilState();
      auto ColorBlendState = builder.genColorBlendAttachmentState();
      auto InputAssemblyState = builder.genInputAssemblyState();

      VkGraphicsPipelineCreateInfo pipelineInfo{};
      pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineInfo.pVertexInputState = &VertexInputState;
      pipelineInfo.pViewportState = &ViewportState;
      pipelineInfo.pRasterizationState = &RasterizationState;
      pipelineInfo.pMultisampleState = &MultisampleState;
      pipelineInfo.pDepthStencilState = &DepthStencilState;
      pipelineInfo.pColorBlendState = &ColorBlendState;
      pipelineInfo.pDynamicState = &DynamicState;
      pipelineInfo.pInputAssemblyState = &InputAssemblyState;
      pipelineInfo.renderPass = render_pass;
      pipelineInfo.subpass = 0;
      pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
      pipelineInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
      pipelineInfo.layout = iPipelineStruct->second.layout;
      pipelineInfo.stageCount = infos.size();
      pipelineInfo.pStages = infos.data();

      if (vkCreateGraphicsPipelines(
              device,
              VK_NULL_HANDLE,
              1,
              &pipelineInfo,
              nullptr,
              &iPipelineStruct->second.pipeline
          ) != VK_SUCCESS)
      {
         throw std::runtime_error("Base pipeline (triangles) wasn't created;");
      }

      for (auto& mod : modules)
      {
         vkDestroyShaderModule(device, mod, nullptr);
      }
   }

   VkPipeline* get(BfPipelineType type);
   void check(BfPipelineType type);
   // INSTANCE //

   ~BfPipelineHandler();

private:
   // clang-format off
   // PRIVATE INIT //
   BfPipelineHandler();
   BfPipelineHandler(const BfPipelineHandler&) = delete;
   BfPipelineHandler(BfPipelineHandler&&) = delete;
   BfPipelineHandler& operator=(BfPipelineHandler&&) = delete;
   BfPipelineHandler& operator=(const BfPipelineHandler&) = delete;
   // OTHER //
   void __findPipeline();
   void __destroyPipelines();

   BfEvent __readShaderFile(std::vector<char>& data, const fs::path& path);
   BfEvent __createShaderModule(VkShaderModule& module, VkDevice device, const std::vector<char>& data);
   BfEvent __createShaderCreateInfos(
       VkDevice device,
       const fs::path& path,
       std::vector<VkShaderModule>& modules,
       std::vector<VkPipelineShaderStageCreateInfo>& out
   );

   // PIPELINE CREATION //

   // clang-format on
};

struct BfPipelineInterface
{
   using VI_t = VkPipelineVertexInputStateCreateInfo;
   using IA_t = VkPipelineInputAssemblyStateCreateInfo;
   using D_t = VkPipelineDynamicStateCreateInfo;
   using V_t = VkPipelineViewportStateCreateInfo;
   using R_t = VkPipelineRasterizationStateCreateInfo;
   using M_t = VkPipelineMultisampleStateCreateInfo;
   using CB_t = VkPipelineColorBlendStateCreateInfo;
   using DS_t = VkPipelineDepthStencilStateCreateInfo;
   using L_t = VkPipelineLayoutCreateInfo;

   /**
    * @brief Creates a VkPipelineVertexInputStateCreateInfo structure.
    * This structure is used to define the vertex input state of a pipeline.
    * It specifies how vertex data is fetched, including the binding
    * descriptions (how vertex data is stored in memory) and attribute
    * descriptions (how vertex data is mapped to shader inputs).
    *
    * @return VkPipelineVertexInputStateCreateInfo => a structure defining the
    * vertex input state.
    */
   virtual VI_t genVertexInputState() = 0;

   /**
    * @brief Creates a VkPipelineInputAssemblyStateCreateInfo structure.
    * This structure is used to describe the input assembly state of a pipeline.
    * It defines how primitives are assembled from vertex data, including the
    * topology (such as points, lines, or triangles) and whether the input
    * should be restarted after reaching the last vertex in a primitive.
    *
    * @param topology The primitive topology to be used for the input assembly
    * stage.
    * @return VkPipelineInputAssemblyStateCreateInfo => a structure defining the
    * input assembly state.
    */
   virtual IA_t genInputAssemblyState() = 0;

   /**
    * @brief Creates a VkPipelineDynamicStateCreateInfo structure.
    * This structure is used to describe the dynamic states of a pipeline.
    * Dynamic states allow modifying pipeline parameters at the command buffer
    * level, without the need to recreate the pipeline itself. This includes,
    * for example, adjusting the viewport, scissors, or blend constants during
    * the rendering process.
    *
    * @return VkPipelineDynamicStateCreateInfo => a structure containing dynamic
    * state information.
    */
   virtual D_t genDynamicState() = 0;

   virtual V_t genViewportState() = 0;

   /**
    * @brief Creates a VkPipelineRasterizationStateCreateInfo structure.
    * This structure defines the rasterization state of a pipeline, which is
    * responsible for converting primitives into fragments. It includes
    * settings for polygon modes, culling, depth clamping, and line width.
    *
    * @return VkPipelineRasterizationStateCreateInfo => a structure defining
    * the rasterization state.
    */
   virtual R_t genRasterizationState() = 0;

   /**
    * @brief Creates a VkPipelineMultisampleStateCreateInfo structure.
    * This structure specifies multisampling parameters for the pipeline.
    * Multisampling is used to perform anti-aliasing by sampling a pixel
    * multiple times and averaging the results. This state determines how
    * multisampling is applied during rendering.
    *
    * @return VkPipelineMultisampleStateCreateInfo => a structure defining the
    * multisample state.
    */
   virtual M_t genMultisampleState() = 0;

   /**
    * @brief Creates a VkPipelineColorBlendAttachmentState structure.
    * This structure defines how color blending is applied to a specific
    * attachment in the pipeline. It includes parameters for enabling blending,
    * specifying blend factors, and configuring blend operations for RGB and
    * alpha components.
    *
    * @return VkPipelineColorBlendAttachmentState => a structure containing
    * color blend attachment state information.
    */
   virtual CB_t genColorBlendAttachmentState() = 0;

   virtual DS_t genDepthStencilState() = 0;

   /**
    * @brief Creates a VkPipelineLayoutCreateInfo structure.
    * This structure describes the pipeline layout, which specifies how
    * descriptor sets and push constants are organized and accessed by the
    * shaders. It serves as the interface between the pipeline and shader
    * resources.
    *
    * @return VkPipelineLayoutCreateInfo => a structure defining the pipeline
    * layout.
    */
   virtual L_t genLayout(const VkDevice& device, const BfDescriptor& desc) = 0;
};

class BfPipelineInterfaceStd : public BfPipelineInterface
{
   VkVertexInputBindingDescription __bindingDescription;
   std::array<VkVertexInputAttributeDescription, 3> __attributeDescriptions;
   std::vector<VkDynamicState> __dynamicStates;
   std::vector<VkPipelineColorBlendAttachmentState> __colorBlendAttachments;
   std::vector<VkDescriptorSetLayout> __descriptorSetLayouts;

public:
   // clang-format off
   virtual VI_t genVertexInputState() override;
   // virtual IA_t genInputAssemblyState() = 0;
   virtual D_t genDynamicState() override;
   virtual V_t genViewportState() override;
   virtual R_t genRasterizationState() override;
   virtual M_t genMultisampleState() override;
   virtual CB_t genColorBlendAttachmentState() override;
   virtual DS_t genDepthStencilState() override;
   virtual L_t genLayout(const VkDevice& device, const BfDescriptor& desc) override;
   // clang-format on
};

#endif
