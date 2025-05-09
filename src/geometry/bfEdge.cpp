#include "bfEdge.h"
#include <limits>
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

std::pair< BfVertex3Uni, BfVertex3Uni >
BfEdge::backConnections()
{
   auto back = _part< E::SecondTangentLine, BfSingleLine >();
   if (m_type == Inlet)
   {
      return {
          BfVertex3Uni(back->first().getp()),
          BfVertex3Uni(back->second().getp())
      };
   }
   else
   {
      return {
          BfVertex3Uni(back->second().getp()),
          BfVertex3Uni(back->first().getp())
      };
   }
   // }
   // else
   // {
   //    auto front = _part< E::FirstTangentLine, BfSingleLine >();
   //    return {
   //        BfVertex3Uni(front->first().getp()),
   //        BfVertex3Uni(front->second().getp())
   //    };
   // }
}

std::pair< BfVertex3Uni, BfVertex3Uni >
BfEdge::frontConnections()
{
   auto front = _part< E::FirstTangentLine, BfSingleLine >();
   if (m_type == Inlet)
   {
      return {
          BfVertex3Uni(front->first().getp()),
          BfVertex3Uni(front->second().getp())
      };
   }
   else
   {
      return {
          BfVertex3Uni(front->second().getp()),
          BfVertex3Uni(front->first().getp())
      };
   }
}

float
BfEdge::relativePos()
{
   return m_type == Inlet ? -std::numeric_limits< float >::infinity()
                          : std::numeric_limits< float >::infinity();
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
   // FIXME: MAY BE ERASE?
   // const auto dir = (m_type == Inlet) ? tangent_line->directionFromStart()
   //                                    : tangent_line->directionFromEnd();
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
      auto [aR, bR] = _calcRadiusVertices();
      if (m_type != Inlet) {  
         std::swap(aR, bR);
      }
      auto fangle_line = _addPartF< E::FirstAngleLine, BfSingleLine >(cvert.getp(), aR);
      auto sangle_line = _addPartF< E::SecondAngleLine, BfSingleLine >(cvert.getp(), bR);
      _addPartF< E::FirstHandle, BfHandleCircle >( fangle_line->second().getp(), 0.01f);
      _addPartF< E::SecondHandle, BfHandleCircle >( sangle_line->second().getp(), 0.01f);
      fangle_line->color() = glm::vec3(0.8, 0.1, 0.05);
      sangle_line->color() = glm::vec3(0.05, 0.1, 0.08);
   }
   else {
      _updateRadius();
      auto [aR, bR] = _calcRadiusVertices();
      if (m_type != Inlet) std::swap(aR, bR);
      _part< E::FirstAngleLine, BfSingleLine >()->second().get() = aR;
      _part< E::SecondAngleLine, BfSingleLine >()->second().get() = bR;
      // clang-format on
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
      const float angle = math::angleBetween3Vertices(
          tangent_line->second(),
          tangent_line->first(),
          first_handle->center()
      );
      // m_angleFirst.get() = (m_type == Inlet) ? angle : 180.f - angle;
      if (m_type == Inlet)
         m_angleFirst.get() = angle;
      else
         m_angleSecond.get() = angle;
   }
   else if (second_handle->isChanged())
   {
      const float angle = math::angleBetween3Vertices(
          tangent_line->second(),
          tangent_line->first(),
          second_handle->center()
      );
      // m_angleSecond.get() = (m_type == Inlet) ? angle : 180.f - angle;
      if (m_type == Inlet)
         m_angleSecond.get() = angle;
      else
         m_angleFirst.get() = angle;
   }
}

// NOTE: Similar code as in BfCirclePack analog 1 : 1
// FIXME: Move copy code to somethere?
void
BfEdge::_addUpdateTangentLines()
{
   auto fangle_line = _part< E::FirstAngleLine, BfSingleLine >();
   auto sangle_line = _part< E::SecondAngleLine, BfSingleLine >();
   auto [fdir, sdir] = frontBackDirection();
   auto& radius = circle()->radius();

   BfVertex3* fvert = fangle_line->second().getp();
   BfVertex3* svert = sangle_line->second().getp();

   const glm::vec3 fposb = fvert->pos + fdir * radius.get();
   const glm::vec3 sposb = svert->pos + sdir * radius.get();

   if (!_isPart< E::FirstTangentLine >())
   {
      // clang-format off
      const BfVertex3 ftangentb = fvert->otherPos(fposb);
      auto f = _addPartF< E::FirstTangentLine, BfSingleLine >(fvert, ftangentb);
      f->color() = glm::vec3(0.8, 0.1, 0.05);

      const BfVertex3 stangentb = fvert->otherPos(sposb);
      auto s = _addPartF< E::SecondTangentLine, BfSingleLine >(svert, stangentb);
      s->color() = glm::vec3(0.05, 0.1, 0.8);
      // clang-format on
   }
   else
   {
      // change second vertices here
      // _part< E::FirstTangentLine, BfSingleLine >()->setPos(fvert->pos,
      // fposb); _part< E::SecondTangentLine, BfSingleLine
      // >()->setPos(svert->pos, sposb);
      _part< E::FirstTangentLine, BfSingleLine >()->first().pos() = fvert->pos;
      _part< E::SecondTangentLine, BfSingleLine >()->first().pos() = svert->pos;
   }
}

// NOTE: Similar code as in BfCirclePack analog 1 : 1
// FIXME: Move copy code to somethere?
std::pair< glm::vec3, glm::vec3 >
BfEdge::frontBackDirection()
{
   const float hpi = glm::half_pi< float >();

   auto fangline = _part< E::FirstAngleLine, BfSingleLine >();
   const glm::vec3 fdir = fangline->directionFromStart();
   const glm::vec3 fnor = fangline->first().normals();
   const glm::mat4 frot = glm::rotate(glm::mat4(1.0f), hpi, fnor);
   const glm::vec3 ftangent = frot * glm::vec4(fdir, 1.0f);

   auto sangline = _part< E::SecondAngleLine, BfSingleLine >();
   const glm::vec3 sdir = sangline->directionFromStart();
   const glm::vec3 snor = sangline->first().normals();
   const glm::mat4 srot = glm::rotate(glm::mat4(1.0f), -hpi, snor);
   const glm::vec3 stangent = srot * glm::vec4(sdir, 1.0f);

   return {
       ftangent * (m_type == Inlet ? 1.f : -1.f),
       stangent * (m_type == Inlet ? 1.f : -1.f)
   };
}
std::pair< const BfVertex3Uni, const BfVertex3Uni >
BfEdge::frontBackTangentVertices()
{
   return {
       BfVertex3Uni(
           _part< E::FirstAngleLine, BfSingleLine >()->second().getp()
       ),
       BfVertex3Uni(
           _part< E::SecondAngleLine, BfSingleLine >()->second().getp()
       ),
   };
}

std::array< BfVertex3Uni, 3 >
BfEdge::arcVertices()
{
   auto line = this->directionLine();

   glm::vec3 start;
   glm::vec3 end;

   if (m_type == Inlet)
   {
      start = backConnections().first.get();
      end = frontConnections().first.get();
   }
   else
   {
      start = frontConnections().second.get();
      end = backConnections().second.get();
   }

   glm::vec3 ref = line->line()->directionFromStart();
   BfVertex3 center = circle()->centerVertex();
   float R = circle()->radius().get();

   return {
       BfVertex3Uni{start},
       BfVertex3Uni{center.otherPos(center.pos + ref * R)},
       BfVertex3Uni{end}
   };
}

void
BfEdge::make()
{
   _addUpdateAngleLines();
   _addUpdateTangentLines();
   BfDrawLayer::make();
}

} // namespace curves

} // namespace obj
