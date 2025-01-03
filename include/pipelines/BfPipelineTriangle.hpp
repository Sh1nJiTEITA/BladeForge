#ifndef BF_PIPELINE_TRIANGLE_H
#define BF_PIPELINE_TRIANGLE_H

#include "bfPipeline.h"

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

#endif
