#ifndef BF_DRAW_OBJECT_MANAGER_H
#define BF_DRAW_OBJECT_MANAGER_H

#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <stdexcept>
#include <unordered_map>

#include "bfDescriptorStructs.h"
#include "bfDrawObject2.h"
#include "bfTypeManager.h"

namespace obj
{

class BfDrawManager
{
   BfDrawManager() {}

public:
   enum RootType
   {
      MAIN,
      IMAGE_LOAD
   };

   static BfDrawManager& inst()
   {
      static BfDrawManager m;
      return m;
   }

   static void setHovered(BfOID id) { inst().m_hoveredID = id; }
   static BfOID getHovered() { return inst().m_hoveredID; }

   // void createDefault() { auto main = }

   template < RootType Type >
   void addRoot()
   {
      auto root = std::make_shared< obj::BfDrawRootLayer >(3000, 50);
      auto [_, sts] = m_rootObjects.insert({Type, root});
      if (!sts)
      {
         throw std::runtime_error("Cant add layer, layer with RootType exists");
      }
   }

   template < RootType Type >
   void add(BfObj obj)
   {
      if (!obj->control().isBuffer())
      {
         throw std::runtime_error(
             "Input obj to DrawManager is not a allocated buffer"
         );
      }
      auto [_, sts] = m_rootObjects.insert({Type, obj});
      if (!sts)
      {
         throw std::runtime_error("Cant add layer, layer with RootType exists");
      }
   }

   template < RootType Type >
   std::shared_ptr< obj::BfDrawRootLayer > get()
   {
      return std::static_pointer_cast< obj::BfDrawRootLayer >(
          m_rootObjects[Type]
      );
   }

   static BfObj findObjectById(BfOID id)
   {
      // clang-format off
      auto findInLayer = [](BfObj l, BfOID id) -> std::optional< BfObj > {
         auto& ch = l->children();
         auto b = std::find_if(ch.begin(), ch.end(), [&id](const BfObj& o) {
            return o->id() == id;
         });
         if (b == ch.end()) return {};
         return *b;
      };
      std::stack< BfObj > stack;
      
      for (auto& [type, l] : inst().m_rootObjects) {
         auto found = findInLayer(l, id);
         if (found.has_value()) return found.value();
         std::for_each(l->children().begin(),
                       l->children().end(),
                       [&stack](const BfObj& o) { 
                           stack.push(o);
                       });
      }
      
      while (!stack.empty()) {
         auto l = stack.top();
         stack.pop();
         auto found = findInLayer(l, id);
         if (found.has_value()) return found.value();
         std::for_each(l->children().begin(),
                       l->children().end(),
                       [&stack](const BfObj& o) { 
                           stack.push(o);
                       });
      }
      
      throw std::runtime_error("No object found");
   }

   void mapModels(size_t frame_index)
   {
      // clang-format off
      auto& man = base::desc::own::BfDescriptorPipelineDefault::manager();
      auto& desc = man.get<base::desc::own::BfDescriptorModel>(
         base::desc::own::SetType::Global, 0
      );
      
      void* ndata = desc.map();
      {
         size_t offset = 0;
         for (const auto& [type, l] : m_rootObjects) 
         { 
            l->control().mapModel(frame_index, offset, ndata, nullptr);
         }
      }
      desc.unmap();

      // clang-format on
   }

   void draw(VkCommandBuffer command_buffer, size_t viewport_index)
   {
      size_t obj_data_offset = 0;

      if (m_prerenderVec.size() > 1000)
      {
         throw std::runtime_error("Prerender vector is overfulled > 1000");
      }
      if (m_postrenderVec.size() > 1000)
      {
         throw std::runtime_error("Postrender vector is overfulled > 1000");
      }

      for (const auto& [type, l] : m_rootObjects)
      {
         size_t vertex_offset = 0;
         size_t index_offset = 0;

         l->control().draw(
             command_buffer,
             viewport_index,
             nullptr,
             &obj_data_offset,
             &index_offset,
             &vertex_offset,
             l->isRender(),
             m_hoveredID
         );
      }
   }

   void kill()
   {
      for (auto& [type, l] : m_rootObjects)
      {
         l.reset();
      }
      m_rootObjects.clear();
   }

   void pushPrerenderFunc(BfRenderObjFunc func)
   {
      m_prerenderVec.push_back(std::move(func));
   }

   void popPrerenderFunc(size_t count)
   {
      for (size_t i = 0; i < count; i++)
         m_prerenderVec.pop_back();
   }

   void pushPostrenderFunc(BfRenderObjFunc func)
   {
      m_postrenderVec.push_back(std::move(func));
   }

   void popPostrenderFunc(size_t count)
   {
      for (size_t i = 0; i < count; i++)
         m_postrenderVec.pop_back();
   }

private:
   uint32_t m_hoveredID;
   std::unordered_map< RootType, BfObj > m_rootObjects;
   std::vector< BfRenderObjFunc > m_prerenderVec;
   std::vector< BfRenderObjFunc > m_postrenderVec;
};

}; // namespace obj

#endif
