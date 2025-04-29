#include "bfDescriptor.h"
#include "bfVariative.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace base
{
namespace desc
{

BfSampler::BfSampler()
{
   VkSamplerCreateInfo samplerInfo{};
   samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
   samplerInfo.magFilter = VK_FILTER_LINEAR;
   samplerInfo.minFilter = VK_FILTER_LINEAR;

   samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
   samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
   samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

   samplerInfo.anisotropyEnable = VK_TRUE;
   samplerInfo.maxAnisotropy =
       base::g::phdeviceprop().limits.maxSamplerAnisotropy;

   samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
   samplerInfo.unnormalizedCoordinates = VK_FALSE;

   samplerInfo.compareEnable = VK_FALSE;
   samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

   samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
   samplerInfo.mipLodBias = 0.0f;
   samplerInfo.minLod = 0.0f;
   samplerInfo.maxLod = 0.0f;

   if (vkCreateSampler(base::g::device(), &samplerInfo, nullptr, &m_sampler) !=
       VK_SUCCESS)
   {
      throw std::runtime_error("failed to create texture sampler!");
   }
}

BfSampler::~BfSampler()
{
   vkDestroySampler(base::g::device(), m_sampler, nullptr);
}

/* DESCRIPTOR POOL */

BfDescriptorPool::BfDescriptorPool(
    const std::vector< VkDescriptorPoolSize >& sz
)
{
   VkDescriptorPoolCreateInfo poolInfo{
       .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
       .pNext = nullptr,
       .flags = 0,
       .maxSets = 10,
       .poolSizeCount = static_cast< uint32_t >(sz.size()),
       .pPoolSizes = sz.data()
   };
   if (vkCreateDescriptorPool(base::g::device(), &poolInfo, nullptr, &m_pool) !=
       VK_SUCCESS)
   {
      throw std::runtime_error("Cant create descriptor pool!");
   }
}

BfDescriptorPool::~BfDescriptorPool()
{
   vkDestroyDescriptorPool(base::g::device(), m_pool, nullptr);
}

/* DESCRIPTOR */

void
BfDescriptor::createBuffer()
{
   throw std::runtime_error(
       "base::desc::BfDescriptor::createBuffer must be implemented"
   );
}

void
BfDescriptor::createImage()
{
   throw std::runtime_error(
       "base::desc::BfDescriptor::createImage must be implemented"
   );
}

void
BfDescriptor::createView()
{
   throw std::runtime_error(
       "base::desc::BfDescriptor::createView must be implemented"
   );
}

VkDescriptorSetLayoutBinding
BfDescriptor::layoutBinding()
{
   return VkDescriptorSetLayoutBinding{
       .binding = binding,
       .descriptorType = vkType,
       .descriptorCount = 1,
       .stageFlags = vkShaderStages,
       .pImmutableSamplers = nullptr
   };
}

VkDescriptorBufferInfo
BfDescriptor::bufferInfo()
{
   if (!m_buffer)
   {
      return VkDescriptorBufferInfo{};
   }
   return VkDescriptorBufferInfo{
       .buffer = m_buffer->handle(),
       .offset = 0,
       .range = m_buffer->size()
   };
}

VkDescriptorImageInfo
BfDescriptor::imageInfo()
{
   if (!m_image)
   {
      return VkDescriptorImageInfo{};
   }
   return VkDescriptorImageInfo{
       // .sampler = m_sampler->handle(),
       .sampler = base::g::sampler(),
       .imageView = m_view->handle(),
       .imageLayout = m_image->layout()
   };
}

/* DESCRIPTOR SET */

BfDescriptorSet::BfDescriptorSet(
    std::vector< std::unique_ptr< BfDescriptor > >&& desc
)
    : m_desc{std::move(desc)}
{
   std::vector< VkDescriptorSetLayoutBinding > bindings;
   std::transform(
       m_desc.begin(),
       m_desc.end(),
       std::back_inserter(bindings),
       [](const auto& d) { return d->layoutBinding(); }
   );

   VkDescriptorSetLayoutCreateInfo l{
       .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
       .pNext = nullptr,
       .flags = 0,
       .bindingCount = static_cast< uint32_t >(bindings.size()),
       .pBindings = bindings.data()
   };

   if (vkCreateDescriptorSetLayout(base::g::device(), &l, nullptr, &m_layout) !=
       VK_SUCCESS)
   {
      throw std::runtime_error("Cant create Descriptor set");
   }
}

BfDescriptorSet::~BfDescriptorSet()
{
   vkDestroyDescriptorSetLayout(base::g::device(), m_layout, nullptr);
   m_desc.clear();
}

void
BfDescriptorSet::allocate(const VkDescriptorPool& pool)
{
   m_sets.clear();
   m_sets.resize(base::g::frames());
   auto frames = base::g::frames();
   for (int i = 0; i < frames; ++i)
   {
      VkDescriptorSetAllocateInfo info{
          .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
          .pNext = nullptr,
          .descriptorPool = pool,
          .descriptorSetCount = 1,
          .pSetLayouts = &m_layout
      };
      if (vkAllocateDescriptorSets(base::g::device(), &info, &m_sets[i]) !=
          VK_SUCCESS)
      {
         throw std::runtime_error("Cant allocate descriptor set");
      }
      fmt::println("Allocated set for frame={}", i);
   }
}

std::vector< VkWriteDescriptorSet >
BfDescriptorSet::write(
    int frame,
    std::list< VkDescriptorBufferInfo >& binfos,
    std::list< VkDescriptorImageInfo >& iinfos
)
{
   std::vector< VkWriteDescriptorSet > writes;
   auto& set = m_sets[frame];
   for (auto& desc : m_desc)
   {
      auto& buffer_info = *binfos.insert(binfos.end(), desc->bufferInfo());
      auto& image_info = *iinfos.insert(iinfos.end(), desc->imageInfo());
      VkWriteDescriptorSet w{
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .pNext = nullptr,
          .dstSet = set,
          .dstBinding = desc->binding,
          .descriptorCount = 1,
          .descriptorType = desc->vkType,
          .pImageInfo = desc->hasImage ? &image_info : nullptr,
          .pBufferInfo = desc->hasBuffer ? &buffer_info : nullptr,
      };
      writes.push_back(std::move(w));
   }
   return writes;
}

/* DESCRIPTOR TEXTURE */
void
BfDescriptorTexture::reload(const std::filesystem::path& newPath)
{
   m_path = newPath;
   m_buffer.reset();
   m_image.reset();
   m_view.reset();
   createBuffer();
   createImage();
   createView();
}

void
BfDescriptorTexture::createBuffer()
{
   if (!std::filesystem::exists(std::filesystem::absolute(m_path)))
   {
      std::string msg = fmt::format(
          "Cant load image... path={} does not exist",
          m_path.string()
      );
      throw std::runtime_error(msg.c_str());
   }

   stbi_uc* pixels = stbi_load(
       std::filesystem::absolute(m_path).c_str(),
       &m_texWidth,
       &m_texHeight,
       &m_texChannels,
       STBI_rgb_alpha
   );
   VkDeviceSize imageSize = m_texWidth * m_texHeight * 4;

   if (!pixels)
   {
      throw std::runtime_error("failed to load texture image!");
   }

   m_buffer = std::make_unique< obj::BfBuffer >(
       static_cast< size_t >(imageSize),
       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
       VMA_MEMORY_USAGE_AUTO,
       VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
           VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
   );

   void* data = m_buffer->map();
   {
      memcpy(data, pixels, static_cast< size_t >(imageSize));
   }
   m_buffer->unmap();
   stbi_image_free(pixels);
}

void
BfDescriptorTexture::createImage()
{
   m_image = std::make_unique< obj::BfImage >(
       m_texWidth,
       m_texHeight,
       VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
       VMA_MEMORY_USAGE_AUTO
   );
   transitionImageLayout(
       VK_IMAGE_LAYOUT_UNDEFINED,
       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
   );

   copyBufferToImage();

   transitionImageLayout(
       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
   );
}
void
BfDescriptorTexture::createView()
{
   VkImageViewCreateInfo viewInfo{};
   viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
   viewInfo.image = m_image->handle();
   viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
   viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
   viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
   viewInfo.subresourceRange.baseMipLevel = 0;
   viewInfo.subresourceRange.levelCount = 1;
   viewInfo.subresourceRange.baseArrayLayer = 0;
   viewInfo.subresourceRange.layerCount = 1;
   m_view = std::make_unique< obj::BfImageView >(viewInfo);
}

void
BfDescriptorTexture::transitionImageLayout(
    VkImageLayout oldl, VkImageLayout newl
)
{
   // clang-format on
   VkCommandBuffer cb = base::g::beginSingleTimeCommands();

   VkImageMemoryBarrier barrier{};
   barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
   barrier.oldLayout = oldl;
   barrier.newLayout = newl;

   barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
   barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

   barrier.image = m_image->handle();
   barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
   barrier.subresourceRange.baseMipLevel = 0;
   barrier.subresourceRange.levelCount = 1;
   barrier.subresourceRange.baseArrayLayer = 0;
   barrier.subresourceRange.layerCount = 1;
   barrier.srcAccessMask = 0; // TODO
   barrier.dstAccessMask = 0; // TODO

   VkPipelineStageFlags sourceStage;
   VkPipelineStageFlags destinationStage;

   if (oldl == VK_IMAGE_LAYOUT_UNDEFINED &&
       newl == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
   {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      m_image->layout() = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
   }
   else if (oldl == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            newl == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
   {
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      m_image->layout() = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
   }
   else
   {
      throw std::invalid_argument("unsupported layout transition!");
   }

   vkCmdPipelineBarrier(
       cb,
       sourceStage,
       destinationStage,
       0,
       0,
       nullptr,
       0,
       nullptr,
       1,
       &barrier
   );
   base::g::endSingleTimeCommands(cb);
};

void
BfDescriptorTexture::copyBufferToImage()
{
   VkCommandBuffer cb = base::g::beginSingleTimeCommands();
   {

      VkBufferImageCopy region{};
      region.bufferOffset = 0;
      region.bufferRowLength = 0;
      region.bufferImageHeight = 0;
      region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      region.imageSubresource.mipLevel = 0;
      region.imageSubresource.baseArrayLayer = 0;
      region.imageSubresource.layerCount = 1;
      region.imageOffset = {0, 0, 0};
      region.imageExtent = {
          static_cast< uint32_t >(m_image->width()),
          static_cast< uint32_t >(m_image->height()),
          1
      };

      vkCmdCopyBufferToImage(
          cb,
          m_buffer->handle(),
          m_image->handle(),
          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
          1,
          &region
      );
   }
   base::g::endSingleTimeCommands(cb);
}

} // namespace desc
} // namespace base
