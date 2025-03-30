#ifndef BF_DRAW_OBJECT_MANAGER_H
#define BF_DRAW_OBJECT_MANAGER_H

#include <stdexcept>

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

   void mapModels(size_t frame_index)
   {
      // clang-format off
      void* data;
      m_descriptor->map_descriptor(BfDescriptorModelMtxUsage, frame_index, &data);
      {
         size_t offset = 0;
         for (const auto& obj : m_rootObjects) 
         { 
            obj->control().mapModel(frame_index, offset, data);
         }
      }
      m_descriptor->unmap_descriptor(BfDescriptorModelMtxUsage, frame_index);

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
             obj_data_offset,
             index_offset,
             vertex_offset
         );
      }
   }

private:
   BfDescriptor* m_descriptor;
   std::vector<BfObj> m_rootObjects;
};

};  // namespace obj

#endif
