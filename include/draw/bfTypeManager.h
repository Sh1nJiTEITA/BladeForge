#ifndef BF_DRAW_TYPA_MANAGER_H
#define BF_DRAW_TYPA_MANAGER_H

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <cstdint>

namespace obj
{

using BfOId = uint32_t;
using BfOType = uint32_t;
using BfOTypeName = const char*;

namespace bi = boost::bimaps;

class BfTypeManager
{
   // clang-format off
   bi::bimap<
       bi::unordered_set_of<BfOType>,
       bi::unordered_set_of<BfOTypeName>
   > m_types;
   // clang-format on

   std::unordered_map<BfOId, BfOType> m_id;

   BfTypeManager() {};

protected:
   static BfOType m_counter;
   static void _reset() { m_counter = 0; }

public:
   static BfTypeManager& inst()
   {
      static BfTypeManager m;
      return m;
   };

   BfOType regType(BfOTypeName name)
   {
      // CHECK IF id registered
      auto found_pair = m_types.right.find(name);
      if (found_pair != m_types.right.end())
      {
         return found_pair->second;
      }
      // ADD NEW
      auto [it, success] = m_types.insert({m_counter, name});
      if (success)
      {
         return m_counter++;
      }
      else
      {
         abort();
      }
   };

   void regId(BfOType type, BfOId id) { m_id[id] = type; }
   void remId(BfOId id) { m_id.erase(id); }

   BfOTypeName getTypeNameByTypeId(BfOType id) { return m_types.left.at(id); }
   BfOTypeName getTypeNameById(BfOId id)
   {
      return m_types.left.at(m_id.at(id));
   }
};
inline BfOType BfTypeManager::m_counter = 0;

};  // namespace obj

#endif
