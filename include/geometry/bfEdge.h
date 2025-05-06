#pragma once
#ifndef BF_BLADE_EDGE_H
#define BF_BLADE_EDGE_H

#include "bfCurves4.h"
#include "bfDrawObject2.h"

namespace obj
{
namespace curves
{

enum BfEdgePartEnum
{
   FirstAngleLine,
   SecondAngleLine,
   FirstHandle,
   SecondHandle,
};

class BfEdge : public obj::BfDrawLayerWithAccess< BfEdgePartEnum >
{
public:
   BfEdge(
       BfVar< float > firstAndle,
       BfVar< float > secondAngle,
       std::weak_ptr< BfSingleLineWH > dir,
       std::weak_ptr< BfCircle2LinesWH > circle
   )
       : BfDrawLayerWithAccess("Blade edge")
       , m_direction{dir}
       , m_circle{circle}
       , m_angleFirst{firstAndle}
       , m_angleSecond{secondAngle}
   {
   }

   void make() override;

public:
   auto circle() -> std::shared_ptr< BfCircle2LinesWH >;
   auto directionLine() -> std::shared_ptr< BfSingleLineWH >;

private:
   auto _calcRadiusVertices() -> std::pair< BfVertex3, BfVertex3 >;
   void _addUpdateAngleLines();
   void _updateRadius();

private:
   BfVar< float > m_angleFirst;
   BfVar< float > m_angleSecond;
   std::weak_ptr< BfSingleLineWH > m_direction;
   std::weak_ptr< BfCircle2LinesWH > m_circle;
};

} // namespace curves
} // namespace obj

#endif
