#ifndef BF_DRAW_OBJECT_MANAGER_H
#define BF_DRAW_OBJECT_MANAGER_H

#include <stdexcept>

#include "BfDescriptorStructs.h"
#include "bfDescriptor.h"
#include "bfDrawObject2.h"

namespace obj
{

class BfDrawManager
{
   BfDrawManager() {}

public:
   static BfDrawManager& inst()
   {
      static BfDrawManager m;
      return m;
   }

   void bindDescriptor(BfDescriptor* descriptor) { m_descriptor = descriptor; }

   void setHovered(uint32_t id) { m_hoveredid = id; }
   uint32_t getHovered() noexcept { return m_hoveredid; }

   void add(BfObj obj)
   {
      if (!obj->control().isBuffer())
      {
         throw std::runtime_error(
             "Input obj to DrawManager is not a allocated buffer"
         );
      }

      m_rootObjects.push_back(obj);
   }

   // std::shared_ptr< obj::BfDrawObjectBase > find(BfOId id) {}

   void mapModels(size_t frame_index)
   {
      // clang-format off
      auto& man = base::desc::own::BfDescriptorPipelineDefault::manager();
      auto& desc = man.get<base::desc::own::BfDescriptorModel>(
         base::desc::own::SetType::Global, 0
      );
      
      void* ndata = desc.map();
      void* data;
      m_descriptor->map_descriptor(BfDescriptorModelMtxUsage, frame_index, &data);
      {
         size_t offset = 0;
         for (const auto& obj : m_rootObjects) 
         { 
            obj->control().mapModel(frame_index, offset, data);
         }
         offset = 0;
         for (const auto& obj : m_rootObjects) 
         { 
            obj->control().mapModel(frame_index, offset, ndata);
         }
      }
      m_descriptor->unmap_descriptor(BfDescriptorModelMtxUsage, frame_index);
      desc.unmap();

      // clang-format on
   }

   void draw(VkCommandBuffer command_buffer)
   {
      size_t obj_data_offset = 0;

      for (const auto& obj : m_rootObjects)
      {
         size_t vertex_offset = 0;
         size_t index_offset = 0;

         obj->control().draw(
             command_buffer,
             &obj_data_offset,
             &index_offset,
             &vertex_offset,
             m_hoveredid
         );
      }
   }

   void kill()
   {
      for (auto l : m_rootObjects)
      {
         l.reset();
      }
      m_rootObjects.clear();
   }

private:
   uint32_t m_hoveredid;
   BfDescriptor* m_descriptor;
   std::vector< BfObj > m_rootObjects;
};

}; // namespace obj

#endif
