#include "bfPipeline.h"

#include <memory>
#include <utility>

std::unique_ptr<BfPipelineHandler> BfPipelineHandler::__instance =
    std::unique_ptr<BfPipelineHandler>(new BfPipelineHandler);

BfPipelineHandler*
BfPipelineHandler::instance()
{
   return __instance.get();
}

void
BfPipelineHandler::load(BfPipelineType type, const fs::path& shader_path)
{
}

VkPipeline*
BfPipelineHandler::get(BfPipelineType type)
{
   auto pair = __pipelines.find(type);
   if (pair != __pipelines.end())
   {
      return &pair->second;
   }
   else
   {
      return nullptr;
   }
}
void
BfPipelineHandler::check(BfPipelineType type)
{
}

BfPipelineHandler::BfPipelineHandler() {}
BfPipelineHandler::~BfPipelineHandler() {}

void
BfPipelineHandler::__findPipeline()
{
}
void
BfPipelineHandler::__destroyPipelines()
{
}
void
BfPipelineHandler::__createShaderCreateInfos()
{
}

void
BfPipelineCreateStruct::genVertexInputState()
{
   // clang-format off
   __vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

   __bindingDescription = BfVertex3::getBindingDescription();
   __attributeDescriptions = BfVertex3::getAttributeDescriptions();

   __vertexInputState.vertexBindingDescriptionCount = 1;
   __vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(
      __attributeDescriptions.size()
   );
   __vertexInputState.pVertexBindingDescriptions = &__bindingDescription;
   __vertexInputState.pVertexAttributeDescriptions = __attributeDescriptions.data();
   // clang-format on
}

void
BfPipelineCreateStruct::genInputAssemblyState(VkPrimitiveTopology topology)
{
   // clang-format off
   __inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
   __inputAssemblyState.topology = topology;
   __inputAssemblyState.primitiveRestartEnable = VK_FALSE;
   // clang-format on
}

void
BfPipelineCreateStruct::genDynamicState()
{
   __dynamicStates = {
       VK_DYNAMIC_STATE_VIEWPORT,
       VK_DYNAMIC_STATE_SCISSOR,
       // VK_DYNAMIC_STATE_BLEND_CONSTANTS // Было ранее включено
   };

   // clang-format off
   __dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
   __dynamicState.dynamicStateCount = static_cast<uint32_t>(__dynamicStates.size());
   __dynamicState.pDynamicStates = __dynamicStates.data();
   // clang-format on
}

void
BfPipelineCreateStruct::genRasterizationState()
{
   // clang-format off
   __rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
   __rasterizationState.depthClampEnable = VK_FALSE;
   __rasterizationState.rasterizerDiscardEnable = VK_FALSE;
   __rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
   __rasterizationState.lineWidth = 1.0f;
   __rasterizationState.cullMode = VK_CULL_MODE_NONE;
   __rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
   // clang-format on
}

void
BfPipelineCreateStruct::genMultisampleState()
{
   // clang-format off
   __multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
   __multisampleState.sampleShadingEnable = VK_FALSE;
   __multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
   __multisampleState.minSampleShading = 1.0f;           // Optional
   __multisampleState.pSampleMask = nullptr;             // Optional
   __multisampleState.alphaToCoverageEnable = VK_FALSE;  // Optional
   __multisampleState.alphaToOneEnable = VK_FALSE;       // Optional
   // clang-format on
}

// void
// BfPipelineCreateStruct::genColorBlendAttachmentState()
// {
//
// }
