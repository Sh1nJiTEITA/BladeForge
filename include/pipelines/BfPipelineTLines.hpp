#ifndef BF_PIPELINE_TLINE_H
#define BF_PIPELINE_TLINE_H

#include "bfPipeline.h"
#include <vulkan/vulkan_core.h>

class BfPipelineBuilderTLine : public BfPipelineInterfaceStd
{
public:
   virtual IA_t genInputAssemblyState() override
   {
      IA_t inputAssembly_triangle{};
      inputAssembly_triangle.sType =
          VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssembly_triangle.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,

      // inputAssembly_triangle.topology =
      // VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
          inputAssembly_triangle.primitiveRestartEnable = VK_FALSE;
      return inputAssembly_triangle;
   }
};

#endif
