#include "bfCirclePack.h"

/* BfCirclePackWH BEGIN */
namespace obj
{
namespace curves
{

std::shared_ptr< BfBezierN >
BfCirclePackWH::bezierCurve()
{
   if (auto locked = m_curve.lock())
   {
      return locked;
   }
   else
   {
      throw std::runtime_error("Can't lock Bezier curve");
   }
}

std::pair< BfVertex3, BfVertex3 >
BfCirclePackWH::_calcTangentLineVertices()
{
   auto v = bezierCurve()->calc(m_relativePos.get());
   auto dir = glm::normalize(bezierCurve()->calcTangent(m_relativePos.get()));
   auto other = v.otherPos(v.pos + dir * m_radius.get());
   return {v, other};
}

void
BfCirclePackWH::_addUpdateTangentLine()
{
   if (!_isPart< E::CenterTangentLine >())
   {
      auto [center_vert, tangent_vert] = _calcTangentLineVertices();
      auto line = _addPartF< E::CenterTangentLine, BfSingleLine >(
          center_vert,
          tangent_vert
      );
      line->color() = glm::vec3(0.0, 0.7, 0.2);
   }
   else
   {
      _updateCircleCenter();
      auto [center_vert, tangent_vert] = _calcTangentLineVertices();
      auto line = _part< E::CenterTangentLine, BfSingleLine >();
      line->first().pos() = center_vert.pos;
      line->second().pos() = tangent_vert.pos;
   }
}

std::pair< BfVertex3, BfVertex3 >
BfCirclePackWH::_calcRadiusVertices()
{
   const auto tangent_line = _part< E::CenterTangentLine, BfSingleLine >();
   const auto dir = tangent_line->directionFromStart();
   auto center = tangent_line->first().get();

   const auto fangle = glm::radians(this->m_angleFirst.get());
   const auto fmtx = glm::rotate(glm::mat4(1.0f), fangle, center.normals);
   const glm::vec3 fdir = fmtx * glm::vec4(dir, 1.0f);

   if (m_flags & SeparateHandles)
   {
      const auto sangle = glm::radians(this->m_angleSecond.get());
      const auto smtx = glm::rotate(glm::mat4(1.0f), -sangle, center.normals);
      const glm::vec3 sdir = smtx * glm::vec4(dir, 1.0f);

      return {
          center.otherPos(center.pos + fdir * m_radius.get()),
          center.otherPos(center.pos + sdir * m_radius.get()),
      };
   }
   else
   {
      return {
          center.otherPos(center.pos + fdir * m_radius.get()),
          center.otherPos(center.pos - fdir * m_radius.get()),
      };
   }
}

std::pair< float, float >
BfCirclePackWH::_calcRadius()
{
   auto line = _part< E::CenterTangentLine, BfSingleLine >();

   auto fline = _part< E::FirstHandle, BfHandleCircle >();
   const auto fdis = glm::distance(fline->center().pos(), line->first().pos());

   auto sline = _part< E::SecondHandle, BfHandleCircle >();
   const auto sdis = glm::distance(sline->center().pos(), line->first().pos());

   return {fdis, sdis};
}

void
BfCirclePackWH::_updateCircleCenter()
{
   auto center_handle = _part< E::CenterHandle, BfHandleCircle >();
   if (center_handle->isChanged())
   {
      auto bez = bezierCurve();
      const auto info_vert = bez->calc(m_relativePos.get());
      const auto actual_vert = center_handle->center().get();

      if (glm::any(glm::notEqual(info_vert.pos, actual_vert.pos)))
      {
         float close_t = math::BfBezierBase::findClosest(*bez, actual_vert);
         fmt::println("({}) {} -> {}", close_t, info_vert.pos, actual_vert.pos);
         m_relativePos.get() = close_t;
      }
   }
}

void
BfCirclePackWH::_updateRadius()
{
   auto [fR, sR] = _calcRadius();
   if (fR != sR)
   {
      auto tangent_line = _part< E::CenterTangentLine, BfSingleLine >();
      auto first_handle = _part< E::FirstHandle, BfHandleCircle >();
      auto second_handle = _part< E::SecondHandle, BfHandleCircle >();

      if (first_handle->isChanged())
      {
         m_radius.get() = fR;
         if (m_flags & FixedAngle)
            return;

         m_angleFirst.get() = math::angleBetween3Vertices(
             tangent_line->second(),
             tangent_line->first(),
             first_handle->center()
         );
      }
      else if (second_handle->isChanged())
      {
         m_radius.get() = sR;
         if (m_flags & FixedAngle)
            return;

         const float angle = math::angleBetween3Vertices(
             tangent_line->second(),
             tangent_line->first(),
             second_handle->center()
         );

         if (m_flags & SeparateHandles)
            m_angleSecond.get() = angle;
         else
            m_angleFirst.get() = 180.f - angle;
      }
   }
}

void
BfCirclePackWH::_addUpdateCircle()
{
   // clang-format off
   if (!_isPart< E::Circle >())
   {
      auto center_line = _part< E::CenterTangentLine, BfSingleLine >();
      BfVertex3Uni& center_vert = center_line->first();
      
      _addPartF< E::Circle, BfCircleCenter >( center_vert.getp(), m_radius.getp());
      _addPartF< E::CenterHandle, BfHandleCircle >( center_vert.getp(), 0.01f);

      const auto [aR, bR] = _calcRadiusVertices();
      auto fangle_line = _addPartF< E::FirstAngleLine, BfSingleLine >(center_vert.getp(), aR);
      auto sangle_line = _addPartF< E::SecondAngleLine, BfSingleLine >(center_vert.getp(), bR);

      _addPartF< E::FirstHandle, BfHandleCircle >( fangle_line->second().getp(), 0.01f);
      _addPartF< E::SecondHandle, BfHandleCircle >( sangle_line->second().getp(), 0.01f);
   }
   else
   {
      _updateRadius();
      auto [aR, bR] = _calcRadiusVertices();

      auto fangle_line = _part< E::FirstAngleLine, BfSingleLine >();
      auto sangle_line = _part< E::SecondAngleLine, BfSingleLine >();

      fangle_line->second().pos() = aR.pos;
      sangle_line->second().pos() = bR.pos;
   }
   // clang-format on
}

void
BfCirclePackWH::_addUpdateTangentLines()
{
   auto fangle_line = _part< E::FirstAngleLine, BfSingleLine >();
   auto sangle_line = _part< E::SecondAngleLine, BfSingleLine >();
   auto [fdir, sdir] = _calcTangentDirections();

   BfVertex3* fvert = fangle_line->second().getp();
   BfVertex3* svert = sangle_line->second().getp();

   const glm::vec3 fposb = fvert->pos + fdir * m_radius.get();
   const glm::vec3 fpost = fvert->pos - fdir * m_radius.get();
   const glm::vec3 sposb = svert->pos + sdir * m_radius.get();
   const glm::vec3 spost = svert->pos - sdir * m_radius.get();

   if (!_isPart< E::FirstTangentLine >())
   {
      const BfVertex3 ftangentb = fvert->otherPos(fposb);
      const BfVertex3 ftangentt = fvert->otherPos(fpost);
      _addPartF< E::FirstTangentLine, BfSingleLine >(ftangentb, ftangentt);

      const BfVertex3 stangentb = fvert->otherPos(sposb);
      const BfVertex3 stangentt = fvert->otherPos(spost);
      _addPartF< E::SecondTangentLine, BfSingleLine >(stangentb, stangentt);
   }
   else
   {
      _part< E::FirstTangentLine, BfSingleLine >()->setPos(fposb, fpost);
      _part< E::SecondTangentLine, BfSingleLine >()->setPos(sposb, spost);
   }
}

std::pair< glm::vec3, glm::vec3 >
BfCirclePackWH::_calcTangentDirections()
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

   return {ftangent, stangent};
}

void
BfCirclePackWH::make()
{
   _addUpdateTangentLine();
   _addUpdateCircle();
   _addUpdateTangentLines();
   BfDrawLayer::make();
}

} // namespace curves
} // namespace obj

/* BfCirclePackWH END */
