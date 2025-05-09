#include "bfChain.h"
#include "bfCurves4.h"
#include "bfDrawObject2.h"
#include "bfObjectMath.h"
#include <algorithm>
#include <array>
#include <fmt/base.h>
#include <glm/ext/vector_float3.hpp>
#include <iterator>
#include <memory>
#include <ranges>
#include <stdexcept>

namespace obj
{
namespace curves
{

std::shared_ptr< BfDrawLayer >
BfChain::centerCircles()
{
   if (auto locked = m_centerCircles.lock())
   {
      return locked;
   }
   else
   {
      throw std::runtime_error("Cant lock ceneter circles");
   }
}

std::shared_ptr< BfEdge >
BfChain::inletCircle()
{
   if (auto locked = m_inletCircle.lock())
   {
      return locked;
   }
   else
   {
      throw std::runtime_error("Cant lock inlet circle");
   }
}

std::shared_ptr< BfEdge >
BfChain::outletCircle()
{
   if (auto locked = m_outletCircle.lock())
   {
      return locked;
   }
   else
   {
      throw std::runtime_error("Cant lock outlet circle");
   }
}
std::vector< std::shared_ptr< BfBezierN > >
BfChain::bezierCurveChain(ChainType type)
{
   // clang-format off
   BfChainPartEnum part;
   switch (type) { 
      case Back: part = BfChainPartEnum::BackLinesLayer; break;
      case Front: part = BfChainPartEnum::FrontLinesLayer; break;
   }

   auto l = _part< obj::BfDrawLayer >(part);
   std::vector< std::shared_ptr< BfBezierN > > bez;
   std::transform(l->children().begin(), 
                  l->children().end(),
                  std::back_inserter(bez),
                  [](const auto& base) {
                     return std::static_pointer_cast< BfBezierN >(base);
                  });
   // clang-format o
   return bez;
}

template < typename T >
std::shared_ptr< T >
upgrade(const std::weak_ptr< T >& weak)
{
   if (auto locked = weak.lock())
   {
      return locked;
   }
   else
   {
      throw std::runtime_error("Cant lock some element");
   }
}

void
BfChain::_updateList()
{
   m_list.clear();
   auto center_circles = m_centerCircles.lock();
   if (!center_circles)
   {
      throw std::runtime_error("Cant lock layer of center circles");
   }
   auto center_range = std::views::all(center_circles->children());

   using raw_obj = std::weak_ptr< BfDrawObjectBase >;
   using trf_obj = std::weak_ptr< BfChainElement >;

   std::transform(
       center_circles->children().begin(),
       center_circles->children().end(),
       std::back_inserter(m_list),
       [](const raw_obj& base) {
          auto locked = base.lock();
          if (!locked)
          {
             throw std::runtime_error("Cant lock circle");
          }
          auto pack = std::dynamic_pointer_cast< BfCirclePackWH >(locked);
          if (pack)
          {
             return pack;
          }
          else
          {
             throw std::runtime_error("Cant cast pack to pack type");
          }
       }
   );
   m_list.sort([](const trf_obj& lhs, const trf_obj& rhs) {
      auto llock = lhs.lock();
      auto rlock = rhs.lock();
      if (!llock || !rlock)
      {
         throw std::runtime_error("Cant lock lhs || rhs");
      }
      return llock->relativePos() < rlock->relativePos();
   });

   m_list.push_front(m_inletCircle);
   m_list.push_back(m_outletCircle);
}

void
BfChain::_addUpdateChains()
{
   if (!_isPart< E::BackLinesLayer >())
   {
      _addPartF< E::BackLinesLayer, obj::BfDrawLayer >("Back Bezier chain");
      _addPartF< E::FrontLinesLayer, obj::BfDrawLayer >("Front Bezier chain");
   }
   else
   {
      auto back_layer = _part< E::BackLinesLayer, obj::BfDrawLayer >();
      auto front_layer = _part< E::FrontLinesLayer, obj::BfDrawLayer >();

      bool isReadding = back_layer->children().size() != m_list.size() - 1;
      if (isReadding)
      {
         back_layer->children().clear();
         front_layer->children().clear();
      }

      auto lit = m_list.begin();
      auto rit = std::next(lit);

      while (rit != m_list.end())
      {
         auto back_v = _getBezierVert(ChainType::Back, *lit, *rit);
         auto front_v = _getBezierVert(ChainType::Front, *lit, *rit);

         if (isReadding)
         {
            front_layer->addf< BfBezierN >(front_v[0], front_v[1], front_v[2]);
            back_layer->addf< BfBezierN >(back_v[0], back_v[1], back_v[2]);
         }
         else
         {
            const size_t bez_index = std::distance(m_list.begin(), lit);

            auto front_bez_b = front_layer->children().at(bez_index);
            auto front_bez = std::static_pointer_cast< BfBezierN >(front_bez_b);
            front_bez->assign(front_v.begin(), front_v.end());

            auto back_bez_b = back_layer->children().at(bez_index);
            auto back_bez = std::static_pointer_cast< BfBezierN >(back_bez_b);
            back_bez->assign(back_v.begin(), back_v.end());
         }

         lit = std::next(lit);
         rit = std::next(rit);
      }
   }
}

glm::vec3
BfChain::_findIntersection(
    const glm::vec3& left_tangent,
    const glm::vec3& left_direction,
    const glm::vec3& right_tangent,
    const glm::vec3& right_direction
)
{
   return curves::math::findLinesIntersection(
       left_tangent,
       left_tangent + left_direction,
       right_tangent,
       right_tangent + right_direction,
       BF_MATH_FIND_LINES_INTERSECTION_ANY
   );
}

void
BfChain::addUpdateLines()
{
   _updateList();

   auto lit = m_list.begin();
   auto rit = std::next(lit);

   fmt::println("-------------------------------");
   while (rit != m_list.end())
   {
      auto l = upgrade(*lit);
      auto r = upgrade(*rit);

      // clang-format off
      const auto& [lfront_tang, lback_tang] = l->frontBackTangentVertices();
      const auto& [lfront_dir, lback_dir] = l->frontBackDirection();
      const auto& [rfront_tang, rback_tang] = r->frontBackTangentVertices();
      const auto& [rfront_dir, rback_dir] = r->frontBackDirection();
      // clang-format on

      auto z = this->z();
      // auto z = -1.f;

      glm::vec3 front_intersection = _findIntersection(
          // lfront_tang.pos(),
          lfront_tang.get().otherZ(z),
          lfront_dir,
          // rfront_tang.pos(),
          rfront_tang.get().otherZ(z),
          rfront_dir
      );
      front_intersection.z = z;

      glm::vec3 back_intersection = _findIntersection(
          lback_tang.get().otherZ(z),
          // lback_tang.pos(),
          lback_dir,
          rback_tang.get().otherZ(z),
          // rback_tang.pos(),
          rback_dir
      );
      back_intersection.z = z;

      // BACK
      auto lback_con = l->backConnections();
      lback_con.second.pos() = back_intersection;
      auto rback_con = r->backConnections();
      rback_con.first.pos() = back_intersection;

      fmt::println(
          "z={}|tf={}, \t tb={}, \t f={} \t b={}",
          z,
          lfront_tang.get().pos,
          lback_tang.get().pos,
          // lfront_dir,
          // lback_dir,
          front_intersection,
          back_intersection
      );

      // FRONT
      auto lfront_con = l->frontConnections();
      lfront_con.second.pos() = front_intersection;
      auto rfront_con = r->frontConnections();
      rfront_con.first.pos() = front_intersection;

      lit = std::next(lit);
      rit = std::next(rit);
   }
   // }
}

std::array< BfVertex3Uni, 3 >
BfChain::_getBezierVert(
    ChainType type,
    std::weak_ptr< BfChainElement > left,
    std::weak_ptr< BfChainElement > right
)
{
   auto l = upgrade(left);
   auto r = upgrade(right);

   auto [L_TAN_FRONT, L_TAN_BACK] = l->frontBackTangentVertices();
   auto [R_TAN_FRONT, R_TAN_BACK] = r->frontBackTangentVertices();
   auto INTR_FRONT = l->frontConnections().second;
   auto INTR_BACK = l->backConnections().second;
   if (type == ChainType::Front)
   {
      return {
          BfVertex3Uni(L_TAN_FRONT),
          BfVertex3Uni(INTR_FRONT),
          BfVertex3Uni(R_TAN_FRONT)
      };
   }
   else
   {
      return {
          BfVertex3Uni(L_TAN_BACK),
          BfVertex3Uni(INTR_BACK),
          BfVertex3Uni(R_TAN_BACK)
      };
   }
}

void
BfChain::make()
{
   addUpdateLines();
   _addUpdateChains();
   BfDrawLayer::make();
}

} // namespace curves

} // namespace obj
