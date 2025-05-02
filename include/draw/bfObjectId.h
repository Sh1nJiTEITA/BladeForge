#ifndef BF_DRAW_OBJECT_2_H
#define BF_DRAW_OBJECT_2_H

#include "bfTypeManager.h"
#include "fmt/core.h"

namespace obj
{

class BfObjectId
{
   BfOID m_value;
   BfOID m_type;

   static BfOID m_counter;

public:
   explicit BfObjectId(BfOTypeName typeName)
       : m_type{obj::BfTypeManager::inst().regType(typeName)}
       , m_value{m_counter++}
   {
      obj::BfTypeManager::inst().regId(m_type, m_value);
   }

   ~BfObjectId()
   {
      auto& inst = obj::BfTypeManager::inst();
      fmt::print(
          "Deleting object with id {} and typename {}\n",
          this->id(),
          inst.getTypeNameById(this->id())
      );
      inst.remId(id());
   }

   explicit BfObjectId(const BfObjectId& other)
       : m_type{other.m_type}, m_value{other.m_value}
   {
   }

   explicit BfObjectId(BfObjectId&& other)
       : m_type{other.m_type}, m_value{other.m_value}
   {
   }

   BfOTypeName typeName() const
   {
      return BfTypeManager::inst().getTypeNameByTypeId(m_type);
   }
   BfOID type() const { return m_type; }
   BfOID id() const { return m_value; }
};

inline BfOID BfObjectId::m_counter = 0;
}; // namespace obj

#endif
