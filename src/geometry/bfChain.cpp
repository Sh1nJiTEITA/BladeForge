#include "bfChain.h"
#include "bfCurves4.h"
#include "bfDrawObject2.h"
#include "bfObjectMath.h"
#include <algorithm>
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
      return llock->relativePos() > llock->relativePos();
   });

   m_list.push_front(m_inletCircle);
   m_list.push_back(m_outletCircle);
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

auto
BfChain::_addUpdateLines()
{
   _updateList();
   if (!_isPart< E::BackLinesLayer >())
   {
      _addPartF< E::BackLinesLayer, obj::BfDrawLayer >("Back lines layer");
      _addPartF< E::FrontLinesLayer, obj::BfDrawLayer >("Front lines layer");
   }
   else
   {
      auto back = _part< E::BackLinesLayer, obj::BfDrawLayer >();
      auto front = _part< E::FrontLinesLayer, obj::BfDrawLayer >();

      if (back->children().size() != m_list.size())
      {
         back->children().clear();

         auto lit = m_list.begin();
         auto rit = std::next(lit);

         const auto v = inletCircle()->circle()->centerVertex();
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

            const glm::vec3 front_intersection = _findIntersection(
                lfront_tang.pos(),
                lfront_dir,
                rfront_tang.pos(),
                rfront_dir
            );

            const glm::vec3 back_intersection = _findIntersection(
                lback_tang.pos(),
                lback_dir,
                rback_tang.pos(),
                rback_dir
            );

            // fmt::println("{} : {}", front_intersection, back_intersection);
            // fmt::println("{} : {}", lfront_dir, rfront_dir);

            auto lback_con = l->backConnections();
            lback_con.second.pos() = back_intersection;
            auto rback_con = r->backConnections();
            rback_con.first.pos() = back_intersection;

            auto lfront_con = l->frontConnections();
            lfront_con.second.pos() = front_intersection;
            auto rfront_con = r->frontConnections();
            rfront_con.first.pos() = front_intersection;

            // back->addf< BfHandleCircle >(v.otherPos(bintr), 0.01f);

            lit = std::next(lit);
            rit = std::next(rit);
         }
      }
   }
}

void
BfChain::make()
{
   _addUpdateLines();
   BfDrawLayer::make();
}

} // namespace curves

} // namespace obj
