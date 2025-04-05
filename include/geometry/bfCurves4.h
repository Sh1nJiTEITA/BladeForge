#ifndef BF_CURVES4_H
#define BF_CURVES4_H

#include <type_traits>
#include <unordered_map>
#include <utility>

#include "bfDrawObject2.h"
#include "bfTypeManager.h"
#include "bfVertex2.hpp"

namespace curves
{

class BfSingleLine;

namespace math
{
#define BF_PI glm::pi<float>()
#define BF_MATH_ABS_ACCURACY 10e-5
#define BF_MATH_DEFAULT_DERIVATIVE_STEP 10e-5

template <typename T, typename U, typename B>
bool
isVerticesInPlain(T&& _f, U&& _s, B&& _t)
{
   return true;
}

template <typename T, typename U, typename B, typename A, typename... Args>
bool
isVerticesInPlain(T&& _f, U&& _s, B&& _t, A&& _np, Args&&... args)
{
   auto np = BfVertex3{std::forward<A>(_np)};
   auto f = BfVertex3{std::forward<T>(_f)};
   auto s = BfVertex3{std::forward<U>(_s)};
   auto t = BfVertex3{std::forward<B>(_t)};

   // clang-format off
   return (glm::abs(
               glm::dot(
                  glm::cross(s.pos - f.pos, 
                             t.pos - f.pos),
                  np.pos - f.pos
               )
          ) < BF_MATH_ABS_ACCURACY) && 
   isVerticesInPlain(std::forward<U>(_s), 
                     std::forward<B>(_t), 
                     std::forward<A>(_np), 
                     std::forward<Args>(args)...);
   // clang-format on
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
//

#define BF_MATH_FIND_LINES_INTERSECTION_BETWEEN_VERTICES 0x1
#define BF_MATH_FIND_LINES_INTERSECTION_ANY 0x2
glm::vec3 findLinesIntersection(
    const BfSingleLine& line1,
    const BfSingleLine& line2,
    int mode = BF_MATH_FIND_LINES_INTERSECTION_BETWEEN_VERTICES
);

};  // namespace math

class BfSingleLine : public obj::BfDrawObject
{
public:
   template <typename T, typename U>
   BfSingleLine(T&& fp, U&& sp)
       : obj::BfDrawObject{"Single line", BF_PIPELINE(BfPipelineType_Lines)}
       , m_first{std::forward<T>(fp)}
       , m_second{std::forward<U>(sp)}
   {
   }

   const BfVertex3& first() const;
   const BfVertex3& second() const;
   const float length() const;
   glm::vec3 directionFromStart() const;
   glm::vec3 directionFromEnd() const;
   void make() override;

   // clang-format on
private:
   BfVertex3 m_first;
   BfVertex3 m_second;
};

class BfCircleCenter : public obj::BfDrawObject
{
public:
   template <typename T>
   BfCircleCenter(T&& center, float radius)
       : obj::BfDrawObject{"Circle", BF_PIPELINE(BfPipelineType_Lines), 200}
       , m_center{std::forward<T>(center)}
       , m_radius{radius}
   {
   }

private:
   BfVertex3 m_center;
   float m_radius;
};

class Bfcircle3Vertices : public obj::BfDrawObject
{
public:
   template <typename T, typename U, typename B>
   Bfcircle3Vertices(T&& P_1, U&& P_2, B&& P_3)
       : obj::BfDrawObject{"Circle", BF_PIPELINE(BfPipelineType_Lines), 200}
       , m_first{std::forward<T>(P_1)}
       , m_second{std::forward<U>(P_2)}
       , m_third{std::forward<B>(P_3)}
   {
   }

   const BfVertex3& first() const;
   const BfVertex3& second() const;
   const BfVertex3& third() const;

   void make() override;

private:
   BfVertex3 m_first;
   BfVertex3 m_second;
   BfVertex3 m_third;
};

}  // namespace curves

#endif
