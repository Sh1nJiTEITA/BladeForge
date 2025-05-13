#include "bfPipeline.h"

#include <vulkan/vulkan_core.h>

#include <memory>
#include <stdexcept>
#include <utility>

#include "bfBase.h"
#include "bfDescriptorStructs.h"
#include "bfUniforms.h"

std::unique_ptr< BfPipelineHandler > BfPipelineHandler::__instance =
    std::unique_ptr< BfPipelineHandler >(new BfPipelineHandler);

BfPipelineHandler*
BfPipelineHandler::instance()
{
   return __instance.get();
}

void
BfPipelineHandler::createLayout(
    BfPipelineLayoutType type,
    const std::vector< VkDescriptorSetLayout >& descSetLayouts
)
{
   // clang-format off
   auto [iPipelineLayout, insertSuccess] = __pipeline_layouts.insert({type, {}});
   if (!insertSuccess)
   {
      throw std::runtime_error("Cant insert new pipeline to pipeline-storage");
   }

   auto rangePC = VkPushConstantRange { 
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      .offset = 0,
      .size = sizeof(BfViewPC)
   };

   auto handlesPC = VkPushConstantRange { 
      .stageFlags =  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      .offset = 128,
      .size = sizeof(BfViewHandlesPC)
   };

   std::vector< VkPushConstantRange > PC { 
      rangePC, handlesPC
   };

   VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
   pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
   pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descSetLayouts.size());
   pipelineLayoutCreateInfo.pSetLayouts = descSetLayouts.data();
   pipelineLayoutCreateInfo.pushConstantRangeCount = PC.size();
   pipelineLayoutCreateInfo.pPushConstantRanges = PC.data();
   // clang-format on
   if (vkCreatePipelineLayout(
           bfGetBase()->device,
           &pipelineLayoutCreateInfo,
           nullptr,
           &iPipelineLayout->second
       ) != VK_SUCCESS)
   {
      throw std::runtime_error("Line pipeline layout isn't done;");
   }
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

BfPipelineData*
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

VkPipeline*
BfPipelineHandler::getPipeline(BfPipelineType type)
{
   BfPipelineData* found = get(type);
   return found ? &found->pipeline : nullptr;
}

VkPipelineLayout*
BfPipelineHandler::getLayout(BfPipelineLayoutType type)
{
   auto pair = __pipeline_layouts.find(type);
   if (pair != __pipeline_layouts.end())
   {
      return &pair->second;
   }
   else
   {
      return nullptr;
   }
}

bool
BfPipelineHandler::check(BfPipelineType type)
{
   auto pipeline_struct = this->get(type);

   return pipeline_struct && pipeline_struct != VK_NULL_HANDLE;
}

void
BfPipelineHandler::kill()
{
   auto pBase = bfGetBase();
   for (auto [type, layout] : __pipeline_layouts)
   {
      vkDestroyPipelineLayout(pBase->device, layout, nullptr);
   }

   for (auto [type, pipeline_struct] : __pipelines)
   {
      vkDestroyPipeline(pBase->device, pipeline_struct.pipeline, nullptr);
   }
}

BfPipelineHandler::BfPipelineHandler() {}
BfPipelineHandler::~BfPipelineHandler() {}

BfEvent
BfPipelineHandler::__readShaderFile(
    std::vector< char >& data, const fs::path& path
)
{
   std::ifstream file(
       path,
       std::ios::ate |      // Read from the end
           std::ios::binary // Read file as binary (avoid text transformations)
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
    VkShaderModule& module, VkDevice device, const std::vector< char >& data
)
{
   VkShaderModuleCreateInfo createInfo{};
   createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
   createInfo.codeSize = data.size();
   createInfo.pCode = reinterpret_cast< const uint32_t* >(data.data());

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
    std::vector< VkShaderModule >& modules,
    std::vector< VkPipelineShaderStageCreateInfo >& out
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

   static std::map< std::string, VkShaderStageFlagBits > stage_bits = {
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

   std::vector< std::string > file_names;

   for (const auto& entry : std::filesystem::directory_iterator(path))
   {
      if (entry.path().extension().string() != ".spv")
         continue;
      file_names.push_back(entry.path().filename().string());
   }

   modules.clear();
   modules.reserve(file_names.size());

   for (size_t i = 0; i < file_names.size(); ++i)
   {
      std::vector< char > code;
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

void
BfPipelineHandler::__create(
    BfPipelineInterface* builder,
    BfPipelineType type,
    BfPipelineLayoutType layout_type,
    const fs::path& shaders_path,
    const VkDevice& device,
    const VkRenderPass& render_pass
)
{
   auto [iPipelineStruct, insertSuccess] = __pipelines.insert({type, {}});
   if (!insertSuccess)
   {
      throw std::runtime_error("Cant insert new pipeline to pipeline-storage");
   }

   // LOADING SHADERS;
   std::vector< VkShaderModule > modules;
   std::vector< VkPipelineShaderStageCreateInfo > infos;

   __createShaderCreateInfos(device, shaders_path, modules, infos);

   auto VertexInputState = builder->genVertexInputState();
   auto DynamicState = builder->genDynamicState();
   auto ViewportState = builder->genViewportState();
   auto RasterizationState = builder->genRasterizationState();
   auto MultisampleState = builder->genMultisampleState();
   auto DepthStencilState = builder->genDepthStencilState();
   auto ColorBlendState = builder->genColorBlendAttachmentState();
   auto InputAssemblyState = builder->genInputAssemblyState();

   // Bind pipeline-layout ( BINDINGS to group of descriptor sets
   // (descriptor-set layout) ) This new pipeline will have access inside
   // shaders to number of variables which were declared inside descriptor
   // set layouts which was grouped as pipeline-layout
   iPipelineStruct->second.pLayout = &__pipeline_layouts[layout_type];

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
   pipelineInfo.layout = *iPipelineStruct->second.pLayout;
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

   m_bindingDescription = BfVertex3::getBindingDescription();
   m_attributeDescriptions = BfVertex3::getAttributeDescriptions();

   vertexInputState.vertexBindingDescriptionCount = 1;
   vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(
      m_attributeDescriptions.size()
   );
   vertexInputState.pVertexBindingDescriptions = &m_bindingDescription;
   vertexInputState.pVertexAttributeDescriptions = m_attributeDescriptions.data();
   // clang-format on
   return vertexInputState;
}

BfPipelineInterface::D_t
BfPipelineInterfaceStd::genDynamicState()
{
   m_dynamicStates = {
       VK_DYNAMIC_STATE_VIEWPORT,
       VK_DYNAMIC_STATE_SCISSOR,
       // VK_DYNAMIC_STATE_BLEND_CONSTANTS // Было ранее включено
   };

   VkPipelineDynamicStateCreateInfo dynamicState{};

   // clang-format off
   dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
   dynamicState.dynamicStateCount = static_cast<uint32_t>(m_dynamicStates.size());
   dynamicState.pDynamicStates = m_dynamicStates.data();
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
   rasterizationState.lineWidth = 2.0f;
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
   colorBlendAttachment.blendEnable = VK_TRUE;
   colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;   // Optional
   colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;  // Optional
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

   m_colorBlendAttachments = {colorBlendAttachment, idBlendAttachment};

   VkPipelineColorBlendStateCreateInfo colorBlending{};
   colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
   colorBlending.logicOpEnable = VK_FALSE;
   colorBlending.logicOp = VK_LOGIC_OP_COPY;  // Optional
   colorBlending.attachmentCount = m_colorBlendAttachments.size();
   colorBlending.pAttachments = m_colorBlendAttachments.data();
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
       VK_COMPARE_OP_LESS; // bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
   info.depthBoundsTestEnable = VK_FALSE;
   // info.minDepthBounds = -10.0f; // Optional
   // info.maxDepthBounds = 10.0f; // Optional
   info.stencilTestEnable = VK_FALSE;

   return info;
}

BfPipelineInterface::L_t
BfPipelineInterfaceStd::genLayout(const VkDevice& device)
{
   // clang-format off
   
   m_pushConstantRange = { 
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      .offset = 0,
      .size = sizeof(BfViewPC)
   };

   auto& man = base::desc::own::BfDescriptorPipelineDefault::manager();
   m_descriptorSetLayouts = man.getAllLayouts();
   VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
   pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
   pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());
   pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.data();
   pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
   pipelineLayoutCreateInfo.pPushConstantRanges = &m_pushConstantRange;
   // clang-format on
   return pipelineLayoutCreateInfo;
}
