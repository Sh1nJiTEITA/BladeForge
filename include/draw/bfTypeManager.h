#ifndef BF_DRAW_TYPA_MANAGER_H
#define BF_DRAW_TYPA_MANAGER_H

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <cstdint>

namespace obj
{

using BfOID = uint32_t;
using BfOType = uint32_t;
using BfOTypeName = const char*;

namespace bi = boost::bimaps;

class BfTypeManager
{
   using types_map = bi::bimap<
       bi::unordered_set_of< BfOType >,
       bi::unordered_set_of< BfOTypeName > >;
   using id_map = std::unordered_map< BfOID, BfOType >;

   // clang-format off
   types_map m_types;
   // clang-format on

   id_map m_id;

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

   const types_map& types() { return m_types; }
   const id_map& ids() { return m_id; }

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

   void regId(BfOType type, BfOID id) { m_id[id] = type; }
   void remId(BfOID id) { m_id.erase(id); }

   BfOTypeName getTypeNameByTypeId(BfOType id) { return m_types.left.at(id); }
   BfOTypeName getTypeNameById(BfOID id)
   {
      return m_types.left.at(m_id.at(id));
   }
};
inline BfOType BfTypeManager::m_counter = 0;

}; // namespace obj

#endif
