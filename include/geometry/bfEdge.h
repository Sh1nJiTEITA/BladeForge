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
   FirstTangentLine,
   SecondTangentLine,
};

class BfEdge : public obj::BfDrawLayerWithAccess< BfEdgePartEnum >,
               public obj::curves::BfChainElement
{
public:
   enum Type
   {
      Inlet,
      Outlet
   };

public:
   BfEdge(
       BfVar< float > firstAndle,
       BfVar< float > secondAngle,
       std::weak_ptr< BfSingleLineWH > dir,
       std::weak_ptr< BfCircle2LinesWH > circle,
       Type type
   )
       : BfDrawLayerWithAccess("Blade edge")
       , m_direction{dir}
       , m_circle{circle}
       , m_angleFirst{firstAndle}
       , m_angleSecond{secondAngle}
       , m_type{type}
   {
   }

   void make() override;

public:
   auto circle() -> std::shared_ptr< BfCircle2LinesWH >;
   auto directionLine() -> std::shared_ptr< BfSingleLineWH >;

   // clang-format off
   auto backConnections() -> std::pair< BfVertex3Uni, BfVertex3Uni > override;
   auto frontConnections() -> std::pair< BfVertex3Uni, BfVertex3Uni > override;
   auto relativePos() -> float override;
   auto frontBackDirection() -> std::pair< glm::vec3, glm::vec3 > override;
   auto frontBackTangentVertices() -> std::pair< const BfVertex3Uni, const BfVertex3Uni > override;

   auto arcVertices() -> std::array< BfVertex3Uni, 3 >;

   // clang-format on

private:
   auto _calcRadiusVertices() -> std::pair< BfVertex3, BfVertex3 >;
   void _addUpdateAngleLines();
   void _updateRadius();
   void _addUpdateTangentLines();

private:
   Type m_type;
   BfVar< float > m_angleFirst;
   BfVar< float > m_angleSecond;
   std::weak_ptr< BfSingleLineWH > m_direction;
   std::weak_ptr< BfCircle2LinesWH > m_circle;
};

} // namespace curves
} // namespace obj

#endif
