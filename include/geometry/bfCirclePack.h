#pragma once
#ifndef BF_CIRCLE_PACK_H
#define BF_CIRCLE_PACK_H

#include "bfCurves4.h"
#include "bfDrawObject2.h"

namespace obj
{
namespace curves
{

enum class BfCirclePackWHEnum
{
   CenterTangentLine = 0,
   Circle,
   CenterHandle,
   FirstAngleLine,
   SecondAngleLine,
   FirstHandle,
   SecondHandle,
   FirstTangentLine,
   SecondTangentLine
};

class BfCirclePackWH : public obj::BfDrawLayerWithAccess< BfCirclePackWHEnum >
{
public:
   using FlagType = uint32_t;
   enum Flag : FlagType
   {
      FixedAngle = 1 << 1,
      SeparateHandles = 1 << 2
   };

public: // ASSIGNMENT
   /**
    * @brief
    *
    * @tparam T BfVar<float> only
    * @param t Relative pos [0 .. 1]
    * @param R Radius of circle
    * @param angle Angle to rotate from tangent of Bezier curve point
    * @param curve Pointer to Bezier curve to be located on
    * @param flags Class flags, CANT be SeparateHandles
    * @return Pack of draw objects with handles
    */
   BfCirclePackWH(
       BfVar< float > t,
       BfVar< float > R,
       BfVar< float > angle,
       std::weak_ptr< BfBezierN > curve,
       FlagType flags = 0
   )
       : obj::BfDrawLayerWithAccess< E >("Circle pack")
       , m_relativePos{t}
       , m_radius{R}
       , m_angleFirst{angle}
       , m_angleSecond{angle}
       , m_curve{curve}
       , m_flags{flags}
   {
      assert(
          flags & SeparateHandles &&
          "Wrong type. Flags cant contain SeparateHandles and be specified "
          "only single angle"
      );
   }

   BfCirclePackWH(
       BfVar< float > t,
       BfVar< float > R,
       BfVar< float > angle_first,
       BfVar< float > angle_second,
       std::weak_ptr< BfBezierN > curve,
       FlagType flags = 0
   )
       : obj::BfDrawLayerWithAccess< E >("Circle pack")
       , m_relativePos{t}
       , m_radius{R}
       , m_angleFirst{angle_first}
       , m_angleSecond{angle_second}
       , m_curve{curve}
       , m_flags{flags}
   {
   }

   void make() override;

public:
   std::shared_ptr< BfBezierN > bezierCurve();

private:
   auto _calcTangentLineVertices() -> std::pair< BfVertex3, BfVertex3 >;
   void _addUpdateTangentLine();
   auto _calcRadiusVertices() -> std::pair< BfVertex3, BfVertex3 >;
   auto _calcRadius() -> std::pair< float, float >;
   void _updateCircleCenter();
   void _updateRadius();
   void _addUpdateCircle();
   void _addUpdateTangentLines();
   auto _calcTangentDirections() -> std::pair< glm::vec3, glm::vec3 >;

private:
   BfVar< float > m_radius;
   BfVar< float > m_relativePos;
   BfVar< float > m_angleFirst;
   BfVar< float > m_angleSecond;
   std::weak_ptr< BfBezierN > m_curve;
   FlagType m_flags;
};

} // namespace curves
} // namespace obj

#endif
