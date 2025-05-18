#ifndef BF_PIPELINE_TRIANGLE_H
#define BF_PIPELINE_TRIANGLE_H

#include "bfPipeline.h"
#include <vulkan/vulkan_core.h>

class BfPipelineBuilderTriangle : public BfPipelineInterfaceStd
{
public:
   virtual IA_t genInputAssemblyState() override
   {
      IA_t inputAssembly_triangle{};
      inputAssembly_triangle.sType =
          VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssembly_triangle.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      inputAssembly_triangle.primitiveRestartEnable = VK_FALSE;
      return inputAssembly_triangle;
   }
};

class BfPipelineBuilderTriangleFrame : public BfPipelineInterfaceStd
{
public:
   virtual IA_t genInputAssemblyState() override
   {
      IA_t inputAssembly_triangle{};
      inputAssembly_triangle.sType =
          VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssembly_triangle.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      inputAssembly_triangle.primitiveRestartEnable = VK_FALSE;
      return inputAssembly_triangle;
   }

   virtual R_t genRasterizationState() override
   {
      R_t rasterizationState{};
      // clang-format off
      rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterizationState.depthClampEnable = VK_FALSE;
      rasterizationState.rasterizerDiscardEnable = VK_FALSE;
      rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
      rasterizationState.lineWidth = 1.5f;
      rasterizationState.cullMode = VK_CULL_MODE_NONE;
      rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
      // clang-format on
      return rasterizationState;
   }
};

#endif
