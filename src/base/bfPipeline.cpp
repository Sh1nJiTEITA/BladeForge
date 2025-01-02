#include "bfPipeline.h"

#include <vulkan/vulkan_core.h>

#include <memory>
#include <stdexcept>
#include <utility>

std::unique_ptr<BfPipelineHandler> BfPipelineHandler::__instance =
    std::unique_ptr<BfPipelineHandler>(new BfPipelineHandler);

BfPipelineHandler*
BfPipelineHandler::instance()
{
   return __instance.get();
}

// void
// BfPipelineHandler::create(
//     BfPipelineType type,
//     BfPipelineInterface& builder,
//     const fs::path& shaders_path,
//     const VkDevice& device,
//     const VkRenderPass& render_pass,
//     const BfDescriptor& desc
// )

VkPipeline*
BfPipelineHandler::get(BfPipelineType type)
{
   auto pair = __pipelines.find(type);
   if (pair != __pipelines.end())
   {
      return &pair->second.pipeline;
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

BfEvent
BfPipelineHandler::__readShaderFile(
    std::vector<char>& data, const fs::path& path
)
{
   std::ifstream file(
       path,
       std::ios::ate |       // Read from the end
           std::ios::binary  // Read file as binary (avoid text transformations)
   );

   BfSingleEvent event{};

   std::stringstream ss;
   ss << "File path = " << path;
   event.info = ss.str();

   if (!file.is_open())
   {
      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_READ_SHADER_FILE_FAILURE;
      event.success = false;
      return BfSingleEvent(event);
   }
   else
   {
      size_t fileSize = (size_t)file.tellg();
      data.clear();
      data.resize(fileSize);

      file.seekg(0);
      file.read(data.data(), fileSize);
      file.close();

      event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT;
      event.action = BfEnActionType::BF_ACTION_TYPE_READ_SHADER_FILE_SUCCESS;
      event.success = true;
      return BfSingleEvent(event);
   }
}

BfEvent
BfPipelineHandler::__createShaderModule(
    VkShaderModule& module, VkDevice device, const std::vector<char>& data
)
{
   VkShaderModuleCreateInfo createInfo{};
   createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
   createInfo.codeSize = data.size();
   createInfo.pCode = reinterpret_cast<const uint32_t*>(data.data());

   BfSingleEvent event{};
   if (vkCreateShaderModule(device, &createInfo, nullptr, &module) ==
       VK_SUCCESS)
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action =
          BfEnActionType::BF_ACTION_TYPE_CREATE_SHADER_MODULE_SUCCESS;
   }
   else
   {
      event.type =
          BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
      event.action =
          BfEnActionType::BF_ACTION_TYPE_CREATE_SHADER_MODULE_FAILURE;
   }

   return BfEvent(event);
}

BfEvent
BfPipelineHandler::__createShaderCreateInfos(
    VkDevice device,
    const fs::path& path,
    std::vector<VkShaderModule>& modules,
    std::vector<VkPipelineShaderStageCreateInfo>& out
)
{
   /*
              "*.vert"
              "*.frag"
              "*.geom"
              "*.glsl"
              "*.comp"
              "*.tesse"
              "*.tessc"
      */

   static std::map<std::string, VkShaderStageFlagBits> stage_bits = {
       {"vert", VK_SHADER_STAGE_VERTEX_BIT},
       {"frag", VK_SHADER_STAGE_FRAGMENT_BIT},
       {"geom", VK_SHADER_STAGE_GEOMETRY_BIT},
       {"comp", VK_SHADER_STAGE_COMPUTE_BIT},
       {"tesse", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
       {"tessc", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT}
   };

   BfSingleEvent event{.type = BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT};
   std::stringstream ss;
   ss << " ";

   std::vector<std::string> file_names;

   for (const auto& entry : std::filesystem::directory_iterator(path))
   {
      if (entry.path().extension().string() != ".spv") continue;
      file_names.push_back(entry.path().filename().string());
   }

   modules.clear();
   modules.reserve(file_names.size());

   for (size_t i = 0; i < file_names.size(); ++i)
   {
      std::vector<char> code;
      BfEvent e = __readShaderFile(code, path / file_names[i]);

      if (e.single_event.success)
      {
         modules.emplace_back();
         __createShaderModule(modules[i], device, code);
         ss << "shader file " << path / file_names[i] << " was processed ";
      }
      else
      {
         ss << "shader file " << path / file_names[i] << " wasn't processed";
         event.action = BF_ACTION_TYPE_CREATE_SHADER_MODULE_PACK_FAILURE;
         event.success = false;
         event.info = ss.str();
         return event;
      }

      VkPipelineShaderStageCreateInfo info{};
      info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      info.pName = "main";
      info.module = modules[i];
      info.stage =
          stage_bits[std::filesystem::path(file_names[i]).stem().string()];

      out.push_back(info);
   }

   {
      event.action = BF_ACTION_TYPE_CREATE_SHADER_MODULE_PACK_SUCCESS;
      event.success = true;
      event.info = ss.str();
      return event;
   }
}

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
// BfPipelineInterfaceStd //

BfPipelineInterface::VI_t
BfPipelineInterfaceStd::genVertexInputState()
{
   VI_t vertexInputState{};
   // clang-format off
   vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

   __bindingDescription = BfVertex3::getBindingDescription();
   __attributeDescriptions = BfVertex3::getAttributeDescriptions();

   vertexInputState.vertexBindingDescriptionCount = 1;
   vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(
      __attributeDescriptions.size()
   );
   vertexInputState.pVertexBindingDescriptions = &__bindingDescription;
   vertexInputState.pVertexAttributeDescriptions = __attributeDescriptions.data();
   // clang-format on
   return vertexInputState;
}

BfPipelineInterface::D_t
BfPipelineInterfaceStd::genDynamicState()
{
   __dynamicStates = {
       VK_DYNAMIC_STATE_VIEWPORT,
       VK_DYNAMIC_STATE_SCISSOR,
       // VK_DYNAMIC_STATE_BLEND_CONSTANTS // Было ранее включено
   };

   VkPipelineDynamicStateCreateInfo dynamicState{};

   // clang-format off
   dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
   dynamicState.dynamicStateCount = static_cast<uint32_t>(__dynamicStates.size());
   dynamicState.pDynamicStates = __dynamicStates.data();
   // clang-format on

   return dynamicState;
}

BfPipelineInterface::V_t
BfPipelineInterfaceStd::genViewportState()
{
   VkPipelineViewportStateCreateInfo viewportState{};
   viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
   viewportState.viewportCount = 1;
   viewportState.scissorCount = 1;
   return viewportState;
}

BfPipelineInterface::R_t
BfPipelineInterfaceStd::genRasterizationState()
{
   R_t rasterizationState{};
   // clang-format off
   rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
   rasterizationState.depthClampEnable = VK_FALSE;
   rasterizationState.rasterizerDiscardEnable = VK_FALSE;
   rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
   rasterizationState.lineWidth = 1.0f;
   rasterizationState.cullMode = VK_CULL_MODE_NONE;
   rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
   // clang-format on
   return rasterizationState;
}

BfPipelineInterface::M_t
BfPipelineInterfaceStd::genMultisampleState()
{
   VkPipelineMultisampleStateCreateInfo multisampleState{};
   // clang-format off
   multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
   multisampleState.sampleShadingEnable = VK_FALSE;
   multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
   multisampleState.minSampleShading = 1.0f;           // Optional
   multisampleState.pSampleMask = nullptr;             // Optional
   multisampleState.alphaToCoverageEnable = VK_FALSE;  // Optional
   multisampleState.alphaToOneEnable = VK_FALSE;       // Optional
   // clang-format on
   return multisampleState;
}

BfPipelineInterface::CB_t
BfPipelineInterfaceStd::genColorBlendAttachmentState()
{
   // clang-format off
   // Color blending
   VkPipelineColorBlendAttachmentState colorBlendAttachment{};
   colorBlendAttachment.colorWriteMask = {
       VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
       VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
   };
   colorBlendAttachment.blendEnable = VK_FALSE;
   colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
   colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
   colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
   colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
   colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
   colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

   // id Color blending
   VkPipelineColorBlendAttachmentState idBlendAttachment{};
   idBlendAttachment.colorWriteMask = {VK_COLOR_COMPONENT_R_BIT};
   idBlendAttachment.blendEnable = VK_FALSE;
   idBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
   idBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
   idBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
   idBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
   idBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
   idBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

   __colorBlendAttachments = {colorBlendAttachment, idBlendAttachment};

   VkPipelineColorBlendStateCreateInfo colorBlending{};
   colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
   colorBlending.logicOpEnable = VK_FALSE;
   colorBlending.logicOp = VK_LOGIC_OP_COPY;  // Optional
   colorBlending.attachmentCount = __colorBlendAttachments.size();
   colorBlending.pAttachments = __colorBlendAttachments.data();
   colorBlending.blendConstants[0] = 0.0f;  // Optional
   colorBlending.blendConstants[1] = 0.0f;  // Optional
   colorBlending.blendConstants[2] = 0.0f;  // Optional
   colorBlending.blendConstants[3] = 0.0f;  // Optional
   // clang-format on
   return colorBlending;
}

BfPipelineInterface::DS_t
BfPipelineInterfaceStd::genDepthStencilState()
{
   // TODO: OLD COPPIED CODE
   VkPipelineDepthStencilStateCreateInfo info = {};
   info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
   info.pNext = nullptr;

   info.depthTestEnable = true ? VK_TRUE : VK_FALSE;
   info.depthWriteEnable = true ? VK_TRUE : VK_FALSE;
   info.depthCompareOp =
       VK_COMPARE_OP_LESS;  // bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
   info.depthBoundsTestEnable = VK_FALSE;
   // info.minDepthBounds = -10.0f; // Optional
   // info.maxDepthBounds = 10.0f; // Optional
   info.stencilTestEnable = VK_FALSE;

   return info;
}

BfPipelineInterface::L_t
BfPipelineInterfaceStd::genLayout(
    const VkDevice& device, const BfDescriptor& desc
)
{
   // clang-format off
   __descriptorSetLayouts = std::move(desc.getAllLayouts());
   VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
   pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
   pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(__descriptorSetLayouts.size());
   pipelineLayoutCreateInfo.pSetLayouts = __descriptorSetLayouts.data();
   pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
   pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
   // clang-format on
   return pipelineLayoutCreateInfo;
}
