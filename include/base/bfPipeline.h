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

#include "bfBase.h"
#include "bfVariative.hpp"

namespace fs = std::filesystem;

class BfPipelineInterface;
class BfPipelineInterfaceStd;

typedef uint32_t BfPipelineType;
enum BfPipelineType_ : BfPipelineType
{
   BfPipelineType_Lines,
   BfPipelineType_Triangles,
   BfPipelineType_Axis,
   BfPipelineType_LoadedImage,
   BfPipelineType_Handles,
};

typedef uint32_t BfPipelineLayoutType;
enum BfPipelineLayoutType_ : BfPipelineLayoutType
{
   BfPipelineLayoutType_Main,
};

/**
 * @class BfPipelineHandler
 * @brief [Singleton] Vulkan pipeline handler to load/unload
 * shaders to make pipelines
 *
 */

struct BfPipelineData
{
   VkPipeline pipeline;
   VkPipelineLayout* pLayout;
};

class BfPipelineHandler
{
   static std::unique_ptr< BfPipelineHandler > __instance;

   // clang-format off
   std::unordered_map<BfPipelineType, BfPipelineData> __pipelines;
   std::unordered_map<BfPipelineLayoutType, VkPipelineLayout> __pipeline_layouts;
   // clang-format on

public:
   static BfPipelineHandler* instance();

   /**
    * @brief Creates pipeline layout.
    * Layouts can be used multiple times for different pipelines.
    * Binding pipeline-layout to pipeline provides descriptor-data (shader
    * variables) to pipelines. Usualy (as i suppose) pipelines want to get
    * camera-data (view), model-data (model, id and etc).
    *
    * @param type => some item of BfPipelineLayoutType-enum
    * @param descSetLayouts => group of descriptor-set-layouts
    * in context of BfDescriptor each descriptor-set-layout is bound to
    * element of 'BfEnDescriptorSetLayoutType' enum. Each element has its own
    * set index (inside shaders layout(set = <INT-index>, binding = <INT>)
    */
   void createLayout(
       BfPipelineLayoutType type,
       const std::vector< VkDescriptorSetLayout >& descSetLayouts
   );

   /**
    * @brief Creates pipeline.
    * Created pipeline adds to inner storage. Access to it can be get by
    * 'get(type)' method. Each created pipeline must be provided with value-type
    * of 'BfPipelineType_' struct.
    * Pipeline describes how Vulkan should use input-vertex,index data to render
    * it.
    *
    * @tparam T => derivative of BfPipelineInterface
    * @param type => value of 'BfPipelineType_' struct
    * @param shaders_path => path to shader directory ('PATH/TO/SHADER_DIR')
    * where inside SHADER_DIR: SHADER_DIR.vert, SHADER_DIR.frag etc
    * @param layout_type => layout type to bind it to pipeline (check
    * 'createLayout' description)
    * @param device => VkDevice handle
    * @param render_pass => VkRenderPass handle
    * @return void
    */
   template < class T >
   typename std::enable_if<
       std::is_base_of< BfPipelineInterface, T >::value >::type
   create(
       BfPipelineType type,
       BfPipelineLayoutType layout_type,
       const fs::path& shaders_path,
       const VkDevice& device,
       const VkRenderPass& render_pass
   )
   {
      auto builder = T();
      __create(&builder, type, layout_type, shaders_path, device, render_pass);
   }

   /**
    * @brief Short version of 'create' method.
    * Check other 'create' description.
    * Uses render-pass, device, descriptor from bound BfBase via 'bfGetBase()'
    * function
    *
    * @param type
    * @param layout_type
    * @param shaders_path
    * @return void
    */
   template < class T >
   typename std::enable_if<
       std::is_base_of< BfPipelineInterface, T >::value >::type
   create(
       BfPipelineType type,
       BfPipelineLayoutType layout_type,
       const fs::path& shaders_path
   )
   {
      // clang-format off
      auto pBase = bfGetBase();
      auto builder = T();
      __create(&builder, type, layout_type, shaders_path, pBase->device, pBase->standart_render_pass);
      // clang-format on
   }

   /**
    * @brief Finds created inner pipeline data by type
    *
    * @param type => value of 'BfPipelineType_' struct
    * @return pointer to __PipelineStruct of saved created pipelines
    */
   BfPipelineData* get(BfPipelineType type);

   /**
    * @brief Finds created inner VkPipeline by type
    *
    * @param type => value of 'BfPipelineType_' struct
    * @return pointer to VkPipeline of saved created pipelines
    */
   VkPipeline* getPipeline(BfPipelineType type);

   /**
    * @brief Finds created VkPipelineLayoyt by type
    *
    * @param type => value of 'BfPipelineLayoutType_' struct
    * @return pointer to VkPipelineLayout of saved created pipelines
    */
   VkPipelineLayout* getLayout(BfPipelineType type);

   /**
    * @brief Checks if pipeline
    *
    * @param type
    * @return
    */
   bool check(BfPipelineType type);

   /**
    * @brief Destroys all created Vulkan handles: vk-pipeline-layouts,
    * vk-pipelines
    *
    */
   void kill();
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
   BfEvent __readShaderFile(std::vector<char>& data, const fs::path& path);
   BfEvent __createShaderModule(VkShaderModule& module, VkDevice device, const std::vector<char>& data);
   BfEvent __createShaderCreateInfos(
       VkDevice device,
       const fs::path& path,
       std::vector<VkShaderModule>& modules,
       std::vector<VkPipelineShaderStageCreateInfo>& out
   );
   // clang-format on
   void __create(
       BfPipelineInterface* builder,
       BfPipelineType type,
       BfPipelineLayoutType layout_type,
       const fs::path& shaders_path,
       const VkDevice& device,
       const VkRenderPass& render_pass
   );
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
   virtual L_t genLayout(const VkDevice& device) = 0;
};

class BfPipelineInterfaceStd : public BfPipelineInterface
{
   VkVertexInputBindingDescription m_bindingDescription;
   std::array< VkVertexInputAttributeDescription, 3 > m_attributeDescriptions;
   std::vector< VkDynamicState > m_dynamicStates;
   std::vector< VkPipelineColorBlendAttachmentState > m_colorBlendAttachments;
   std::vector< VkDescriptorSetLayout > m_descriptorSetLayouts;
   VkPushConstantRange m_pushConstantRange;

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
   virtual L_t genLayout(const VkDevice& device) override;
   // clang-format on
};

#endif
