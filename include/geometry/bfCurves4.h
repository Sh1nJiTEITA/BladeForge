#ifndef BF_CURVES4_H
#define BF_CURVES4_H

#include <utility>

#include "bfDrawObject2.h"

namespace curves
{

class BfSingleLine : public obj::BfDrawObject
{
public:
   // clang-format off
   template <typename T, typename U>
   BfSingleLine(T&& fp, U&& sp)
       : obj::BfDrawObject { "Single line", BF_PIPELINE(BfPipelineType_Lines) }
       , m_first{std::forward<T>(fp)}
       , m_second{std::forward<U>(sp)}
   {
   }
   // clang-format on

   const BfVertex3& first() const { return m_first; }
   const BfVertex3& second() const { return m_second; }

   void make() override
   {
      m_vertices = {m_first, m_second};
      _genIndicesStandart();
   };

private:
   BfVertex3 m_first;
   BfVertex3 m_second;
};

}  // namespace curves

#endif
