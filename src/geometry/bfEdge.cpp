#include "bfEdge.h"
#include <stdexcept>

namespace obj
{
namespace curves
{
std::shared_ptr< BfCircle2LinesWH >
BfEdge::circle()
{
   if (auto locked = m_circle.lock())
   {
      return locked;
   }
   else
   {
      throw std::runtime_error("Cant lock outside bound IO blade circle");
   }
}

std::shared_ptr< BfSingleLineWH >
BfEdge::directionLine()
{
   if (auto locked = m_direction.lock())
   {
      return locked;
   }
   else
   {
      throw std::runtime_error("Cant lock direction line");
   }
}

std::pair< BfVertex3, BfVertex3 >
BfEdge::_calcRadiusVertices()
{
   const auto tangent_line = directionLine()->line();
   const auto dir = tangent_line->directionFromStart();
   auto center = tangent_line->first().get();

   const auto fangle = glm::radians(this->m_angleFirst.get());
   const auto fmtx = glm::rotate(glm::mat4(1.0f), fangle, center.normals);
   const glm::vec3 fdir = fmtx * glm::vec4(dir, 1.0f);

   const auto sangle = glm::radians(this->m_angleSecond.get());
   const auto smtx = glm::rotate(glm::mat4(1.0f), -sangle, center.normals);
   const glm::vec3 sdir = smtx * glm::vec4(dir, 1.0f);

   const float R = circle()->radius().get();
   return {
       center.otherPos(center.pos + fdir * R),
       center.otherPos(center.pos + sdir * R)
   };
}

void
BfEdge::_addUpdateAngleLines()
{
   if (!_isPart< E::FirstAngleLine >())
   {
      // clang-format off
      auto cvert = this->directionLine()->line()->first();
      const auto [aR, bR] = _calcRadiusVertices();
      auto fangle_line = _addPartF< E::FirstAngleLine, BfSingleLine >(cvert.getp(), aR);
      auto sangle_line = _addPartF< E::SecondAngleLine, BfSingleLine >(cvert.getp(), bR);
      _addPartF< E::FirstHandle, BfHandleCircle >( fangle_line->second().getp(), 0.01f);
      _addPartF< E::SecondHandle, BfHandleCircle >( sangle_line->second().getp(), 0.01f);
      // clang-format on
   }
   else
   {
      _updateRadius();
      const auto [aR, bR] = _calcRadiusVertices();
      _part< E::FirstAngleLine, BfSingleLine >()->second().get() = aR;
      _part< E::SecondAngleLine, BfSingleLine >()->second().get() = bR;
   }
}

void
BfEdge::_updateRadius()
{
   auto tangent_line = directionLine()->line();
   auto& radius = circle()->radius();
   auto first_handle = _part< E::FirstHandle, BfHandleCircle >();
   auto second_handle = _part< E::SecondHandle, BfHandleCircle >();

   if (first_handle->isChanged())
   {
      m_angleFirst.get() = math::angleBetween3Vertices(
          tangent_line->second(),
          tangent_line->first(),
          first_handle->center()
      );
   }
   else if (second_handle->isChanged())
   {
      const float angle = math::angleBetween3Vertices(
          tangent_line->second(),
          tangent_line->first(),
          second_handle->center()
      );

      m_angleSecond.get() = angle;
   }
}

void
BfEdge::make()
{
   _addUpdateAngleLines();
   BfDrawLayer::make();
}

} // namespace curves

} // namespace obj
