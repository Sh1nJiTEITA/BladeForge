#pragma once
#include "bfCurves4.h"
#include "bfDrawObject2.h"
#include <array>
#ifndef BF_BLADE_CHAIN_H
#define BF_BLADE_CHAIN_H

#include "bfCirclePack.h"
#include "bfEdge.h"

namespace obj
{
namespace curves
{

enum BfChainPartEnum
{
   BackLinesLayer,
   FrontLinesLayer,
};

class BfChain : public obj::BfDrawLayerWithAccess< BfChainPartEnum >
{
public:
   enum ChainType
   {
      Back,
      Front
   };

public:
   BfChain(
       std::weak_ptr< BfDrawLayer > cc,
       std::weak_ptr< BfEdge > ic,
       std::weak_ptr< BfEdge > oc
   )
       : obj::BfDrawLayerWithAccess< E >("Chain")
       , m_centerCircles(cc)
       , m_inletCircle(ic)
       , m_outletCircle(oc)
   {
   }

   void make() override;

public:
   auto centerCircles() -> std::shared_ptr< BfDrawLayer >;
   auto inletCircle() -> std::shared_ptr< BfEdge >;
   auto outletCircle() -> std::shared_ptr< BfEdge >;

private:
   auto _addUpdateLines();
   void _updateList();
   void _addUpdateChains();

   auto _getBezierVert(
       ChainType type,
       std::weak_ptr< BfChainElement > left,
       std::weak_ptr< BfChainElement > right
   ) -> std::array< BfVertex3Uni, 3 >;

   auto _findIntersection(
       const glm::vec3& left_tangent,
       const glm::vec3& left_direction,
       const glm::vec3& right_tangent,
       const glm::vec3& right_direction
   ) -> glm::vec3;

private:
   std::weak_ptr< BfDrawLayer > m_centerCircles;
   std::weak_ptr< BfEdge > m_inletCircle;
   std::weak_ptr< BfEdge > m_outletCircle;

   std::list< std::weak_ptr< BfChainElement > > m_list;
};

} // namespace curves
} // namespace obj

#endif
