/**
 * @file File for working with shaders
 * @brief ...
 */

#ifndef BF_PIPELINE_H
#define BF_PIPELINE_H

#include <bfVertex2.hpp>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <unordered_map>

#include "bfVariative.hpp"

namespace fs = std::filesystem;

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

   std::unordered_map<BfPipelineType, VkPipeline> __pipelines;

public:
   static BfPipelineHandler* instance();

   // INTERACTION //
   void load(BfPipelineType type, const fs::path& shader_path);
   VkPipeline* get(BfPipelineType type);
   void check(BfPipelineType type);
   // INSTANCE //

   ~BfPipelineHandler();

private:
   // PRIVATE INIT //
   BfPipelineHandler();
   BfPipelineHandler(const BfPipelineHandler&) = delete;
   BfPipelineHandler(BfPipelineHandler&&) = delete;
   BfPipelineHandler& operator=(BfPipelineHandler&&) = delete;
   BfPipelineHandler& operator=(const BfPipelineHandler&) = delete;
   // OTHER //
   void __findPipeline();
   void __destroyPipelines();
   void __createShaderCreateInfos();

   // PIPELINE CREATION //

   // clang-format on
};

class BfPipelineCreateStruct
{
   // STORING SOME TMP DATA //
   VkVertexInputBindingDescription __bindingDescription;
   std::array<VkVertexInputAttributeDescription, 3> __attributeDescriptions;
   std::vector<VkDynamicState> __dynamicStates;

   // INFOS //
   VkPipelineVertexInputStateCreateInfo __vertexInputState;
   VkPipelineInputAssemblyStateCreateInfo __inputAssemblyState;
   VkPipelineDynamicStateCreateInfo __dynamicState;
   VkPipelineRasterizationStateCreateInfo __rasterizationState;
   VkPipelineMultisampleStateCreateInfo __multisampleState;
   VkPipelineColorBlendAttachmentState __colorBlendAttachmentState{};

public:
   BfPipelineCreateStruct();

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
   void genVertexInputState();

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
   void genInputAssemblyState(VkPrimitiveTopology topology);

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
   void genDynamicState();

   void genRasterizationState();
   void genMultisampleState();
   void genColorBlendAttachmentState();
};

#endif
