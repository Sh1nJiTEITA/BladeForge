#ifndef BF_CURVES4_H
#define BF_CURVES4_H

#include <cmath>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include "bfDrawObject2.h"
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
   isVerticesInPlain(std::move(_s), 
                     std::move(_t), 
                     std::move(_np), 
                     std::forward<Args>(args)...);
   // clang-format on
}

template <typename T, typename U, typename B>
glm::vec4
calcPlaneCoeffs(T&& _f, U&& _s, B&& _t)
{
   auto f = BfVertex3{std::forward<T>(_f)}.pos;
   auto s = BfVertex3{std::forward<U>(_s)}.pos;
   auto t = BfVertex3{std::forward<B>(_t)}.pos;

   glm::mat3 xd = {f.y, f.z, 1, s.y, s.z, 1, t.y, t.z, 1};
   glm::mat3 yd = {f.x, f.z, 1, s.x, s.z, 1, t.x, t.z, 1};
   glm::mat3 zd = {f.x, f.y, 1, s.x, s.y, 1, t.x, t.y, 1};

   auto direction = glm::vec3(
       glm::determinant(xd),
       -glm::determinant(yd),
       glm::determinant(zd)
   );

   glm::mat3 D = {f.x, f.y, f.z, s.x, s.y, s.z, t.x, t.y, t.z};

   return {direction, -glm::determinant(D)};
}

std::vector<BfVertex3> calcCircleVertices(
    const BfVertex3& center,
    float radius,
    uint32_t m_discretization,
    const glm::vec3& color
);

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
    const curves::BfSingleLine& line1,
    const curves::BfSingleLine& line2,
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
       : obj::
             BfDrawObject{"Circle center", BF_PIPELINE(BfPipelineType_Lines), 200}
       , m_center{std::forward<T>(center)}
       , m_radius{radius}
   {
   }
   void make() override;
   const BfVertex3& center() const;
   const float radius() const;

private:
   BfVertex3 m_center;
   float m_radius;
};

class Bfcircle3Vertices : public obj::BfDrawObject
{
public:
   template <typename T, typename U, typename B>
   Bfcircle3Vertices(T&& P_1, U&& P_2, B&& P_3)
       : obj::
             BfDrawObject{"Circle 3 vertices", BF_PIPELINE(BfPipelineType_Lines), 200}
       , m_first{std::forward<T>(P_1)}
       , m_second{std::forward<U>(P_2)}
       , m_third{std::forward<B>(P_3)}
       , m_radius{std::nanf("")}
       , m_center{BfVertex3::nan()}
   {
   }

   const BfVertex3& first() const;
   const BfVertex3& second() const;
   const BfVertex3& third() const;
   const float radius() const;
   const BfVertex3& center() const;

   void make() override;

private:
   float m_radius;
   BfVertex3 m_center;
   BfVertex3 m_first;
   BfVertex3 m_second;
   BfVertex3 m_third;
};

// class BfCircleCenterFilled : public obj::BfDrawObject
// {
// public:
//    template <typename T, typename U, typename B>
//    BfCircleCenterFilled(T&& P_1, U&& P_2, B&& P_3)
//        : obj::
//              BfDrawObject{"Circle center filled",
//              BF_PIPELINE(BfPipelineType_Triangles), 200}
//        , m_first{std::forward<T>(P_1)}
//        , m_second{std::forward<U>(P_2)}
//        , m_third{std::forward<B>(P_3)}
//        , m_radius{std::nanf("")}
//        , m_center{BfVertex3::nan()}
//    {
//    }
//
//    float radius() const noexcept;
//    const glm::vec3& tangent() const noexcept;
//
//    virtual void make() override;
//
//    BfVertex3& center();
//
// private:
//    float m_radius;
//    BfVertex3 m_center;
// };

}  // namespace curves

#endif
