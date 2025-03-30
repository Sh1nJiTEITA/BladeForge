#ifndef BF_DRAW_OBJECT_2_H
#define BF_DRAW_OBJECT_2_H

#include "bfTypeManager.h"

namespace obj
{

class BfObjectId
{
   BfOId m_value;
   BfOId m_type;

   static BfOId m_counter;

public:
   explicit BfObjectId(BfOTypeName typeName)
       : m_type{obj::BfTypeManager::inst().regType(typeName)}
       , m_value{m_counter++}
   {
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
      return BfTypeManager::inst().getType(m_type);
   }
   BfOId type() const { return m_type; }
   BfOId id() const { return m_value; }
};

inline BfOId BfObjectId::m_counter = 0;
};  // namespace obj

#endif
