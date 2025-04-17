#include "bfBladeSection2.h"

#include "bfCurves4.h"
#include "bfObjectMath.h"
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vector_relational.hpp>
#include <memory>

namespace obj
{
namespace section
{

bool
BfBladeSection::_isChordChanged()
{
   auto oChord = _part< BfBladeSectionEnum::Chord, curves::BfSingleLineWH >();
   return !oChord->left().get().equal(m_lastChordL) ||
          !oChord->right().get().equal(m_lastChordR);
}

float
BfBladeSection::_equivalentInletAngle()
{
   // return 90.0f - this->m_info.get().inletAngle;
   return this->m_info.get().inletAngle;
}
float
BfBladeSection::_equivalentOutletAngle()
{
   // return 90.0f - this->m_info.get().outletAngle;
   return this->m_info.get().outletAngle;
}

glm::vec3
BfBladeSection::_ioIntersection()
{
   auto inletCircle =
       _part< BfBladeSectionEnum::InletCircle, curves::BfCircle2LinesWH >();
   auto outletCircle =
       _part< BfBladeSectionEnum::OutletCircle, curves::BfCircle2LinesWH >();
   auto oChord = _part< BfBladeSectionEnum::Chord, curves::BfSingleLineWH >();

   auto iCenter = inletCircle->circle()->center();
   auto oCenter = outletCircle->circle()->center();

   glm::vec3 angle_dir = oChord->line()->directionFromStart();
   glm::vec3 inlet_dir = glm::rotate(
                             glm::mat4(1.0f),
                             glm::radians(_equivalentInletAngle()),
                             iCenter.normals
                         ) *
                         glm::vec4(angle_dir, 1.0f);

   glm::vec3 outlet_dir = glm::rotate(
                              glm::mat4(1.0f),
                              -glm::radians(_equivalentOutletAngle()),
                              oCenter.normals
                          ) *
                          glm::vec4(angle_dir, 1.0f);

   return curves::math::findLinesIntersection(
       iCenter,
       iCenter.pos + inlet_dir,
       oCenter,
       oCenter.pos + outlet_dir,
       BF_MATH_FIND_LINES_INTERSECTION_ANY
   );
}

void
BfBladeSection::_createChord()
{
   // clang-format off
   auto& g = m_info;
   auto oChord = _addPartForward<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>(
      BfVertex3{
         glm::vec3{ 0.0f, 0.0f, 0.0f }, 
         glm::vec3{ 0.5f, 0.5f, 0.1f },
         glm::vec3{ 0.0f, 0.0f, 1.0f }
      },
      BfVertex3{
         glm::vec3{ g.get().chord, 0.0f, 0.0f }, 
         glm::vec3{ 0.5f, 0.5f, 0.1f },
         glm::vec3{ 0.0f, 0.0f, 1.0f }
      }
   );
   m_lastChordL = oChord->left().get();
   m_lastChordR = oChord->right().get();

   auto oChordLeft = _addPartForward<BfBladeSectionEnum::_ChordLeftBorder, curves::BfSingleLineWH>( 
      _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>()->left().getp(),
      BfVertex3{
         glm::vec3{ oChord->left().pos().x, g.get().chord, 0.0f }, 
         glm::vec3{ 0.5f, 0.5f, 0.1f },
         glm::vec3{ 0.0f, 0.0f, 1.0f }
      }
   );

   auto oChordRight = _addPartForward<BfBladeSectionEnum::_ChordRightBorder, curves::BfSingleLineWH>( 
      _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>()->right().getp(),
      BfVertex3{
         glm::vec3{ oChord->right().pos().x, g.get().chord, 0.0f }, 
         glm::vec3{ 0.5f, 0.5f, 0.1f },
         glm::vec3{ 0.0f, 0.0f, 1.0f }
      }
   );
   // clang-format on
}

void
BfBladeSection::_processChord()
{
   // clang-format off
   auto oChord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();
   auto oChordL = _part<BfBladeSectionEnum::_ChordLeftBorder, curves::BfSingleLineWH>();
   auto oChordR = _part<BfBladeSectionEnum::_ChordRightBorder, curves::BfSingleLineWH>();

   m_info.get().chord = oChord->line()->length();

   if (_isChordChanged()) 
   { 
      auto line = oChord->line();
      auto up = glm::normalize(-glm::cross(line->directionFromStart(),
                                           line->first().normals));

      oChordL->right().pos() = oChord->left().pos() + up * m_info.get().chord;
      oChordR->right().pos() = oChord->right().pos() + up * m_info.get().chord;
   }
   // clang-format on
}

void
BfBladeSection::_createCircleEdges()
{
   // clang-format off
   auto& g = m_info;

   auto oChord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();
   auto oChordL = _part<BfBladeSectionEnum::_ChordLeftBorder, curves::BfSingleLineWH>();
   auto oChordR = _part<BfBladeSectionEnum::_ChordRightBorder, curves::BfSingleLineWH>();

   auto inletCircle = _addPartForward<BfBladeSectionEnum::InletCircle, curves::BfCircle2LinesWH>(
      oChordL->right().getp(),
      oChord->left().getp(),
      oChord->right().getp(),
      &g.get().inletRadius 
   );

   auto outletCircle = _addPartForward<BfBladeSectionEnum::OutletCircle, curves::BfCircle2LinesWH>(
      oChord->left().getp(),
      oChord->right().getp(),
      oChordR->right().getp(),
      &g.get().outletRadius 
   );

   // clang-format on
}

void
BfBladeSection::_processCircleEdges()
{
   // clang-format off
   // auto& g = m_info;
   // auto chord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();

   // clang-format off
}



void BfBladeSection::_createAverageInitialCurve() 
{ 
   auto inletCircle = _part<BfBladeSectionEnum::InletCircle, curves::BfCircle2LinesWH>();
   auto outletCircle = _part<BfBladeSectionEnum::OutletCircle, curves::BfCircle2LinesWH>();   
   auto oChord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();
   
   _addPartForward<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>(
      BfVertex3Uni(inletCircle->circle()->center()),
      BfVertex3Uni(BfVertex3(
         _ioIntersection(),
         inletCircle->circle()->center().color,
         inletCircle->circle()->center().normals
      )),
      BfVertex3Uni(outletCircle->circle()->center())
   );
}

void BfBladeSection::_processAverageInitialCurve() { 
   auto inletCircle = _part<BfBladeSectionEnum::InletCircle, curves::BfCircle2LinesWH>();
   auto outletCircle = _part<BfBladeSectionEnum::OutletCircle, curves::BfCircle2LinesWH>();
   auto initCurve = _part<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>();
   initCurve->at(0).pos() = inletCircle->centerVertex().pos;
   initCurve->at(1).pos() = _ioIntersection();
   initCurve->at(2).pos() = outletCircle->centerVertex().pos;
}

void BfBladeSection::_createCenterCircles() {
   auto initCurve = _part<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>();
   auto circleLayer = _addPartForward<BfBladeSectionEnum::CenterCircles, obj::BfDrawLayer>(
      "Center circles layer"
   );
   for (auto&& circleInfo : m_info.get().centerCircles) { 
      auto circle = std::make_shared<curves::BfCircleCenterWH>(
         initCurve->curve()->calc(circleInfo.relativePos),
         circleInfo.radius
      );
      circleLayer->add(circle);
   }
}

void BfBladeSection::_processCenterCircles() { 
   auto initCurve = _part<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>();
   if (initCurve->curve()->vertices().empty()) return;
   auto circleLayer = _part<BfBladeSectionEnum::CenterCircles, obj::BfDrawLayer>();
   for (size_t i = 0; i < m_info.get().centerCircles.size(); ++i) { 
      auto circle = std::static_pointer_cast<curves::BfCircleCenterWH>(circleLayer->children()[i]);
      if (circle->isChanged()) { 
         auto info_pos = initCurve->curve()->calc(m_info.get().centerCircles[i].relativePos).pos;
         auto actual_pos = circle->center().pos;
         if (glm::any(glm::notEqual(info_pos, actual_pos))) { 
            float close_t = curves::math::BfBezierBase::findClosest(*initCurve, circle->center());
            m_info.get().centerCircles[i].relativePos = close_t;
         }
      }
      circle->center().pos = curves::math::BfBezierBase::calc(
         *initCurve, m_info.get().centerCircles[i].relativePos
      ).pos;
      m_info.get().centerCircles[i].radius = circle->radius();
   }
}

void BfBladeSection::_createCCLines() 
{ 
   auto initCurve = _part<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>();
   const auto& CC = _part<BfBladeSectionEnum::CenterCircles, obj::BfDrawLayer>()->children();

   auto CCLinesLayer = _addPartForward<BfBladeSectionEnum::CenterCirclesLines, obj::BfDrawLayer>(
      "Center circle lines layer"
   );
   for (size_t i = 0; i < m_info.get().centerCircles.size(); ++i) { 
      float t = m_info.get().centerCircles[i].relativePos;
      float r = m_info.get().centerCircles[i].radius;
      auto normal = curves::math::BfBezierBase::calcNormal(*initCurve, t);
      auto circle = std::static_pointer_cast<curves::BfCircleCenterWH>(CC[i]);
      auto line = std::make_shared<curves::BfSingleLine>(
         BfVertex3{ 
            circle->center().pos + normal * r,
            glm::vec3(1.0f),
            circle->center().normals
         },
         BfVertex3{ 
            circle->center().pos - normal * r,
            glm::vec3(1.0f),
            circle->center().normals
         }
      );
      line->color() = glm::vec3(0.0f, 1.0f, .2f);
      CCLinesLayer->add(line);
   }
}

void BfBladeSection::_processCCLines() 
{ 
   auto initCurve = _part<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>();
   const auto& CC = _part<BfBladeSectionEnum::CenterCircles, obj::BfDrawLayer>()->children();
   const auto& CCL = _part<BfBladeSectionEnum::CenterCirclesLines, obj::BfDrawLayer>()->children();

   for (size_t i = 0; i < m_info.get().centerCircles.size(); ++i) { 
      float t = m_info.get().centerCircles[i].relativePos;
      float r = m_info.get().centerCircles[i].radius;
      auto normal = curves::math::BfBezierBase::calcNormal(*initCurve, t);
      auto circle = std::static_pointer_cast<curves::BfCircleCenterWH>(CC[i]);
      auto line = std::static_pointer_cast<curves::BfSingleLine>(CCL[i]);
      line->first().pos = circle->center().pos + normal * r;
      line->second().pos = circle->center().pos - normal * r;
   }
}

void BfBladeSection::_createFrontIntersectionLines() { 
   const auto& CCL = _part<BfBladeSectionEnum::CenterCirclesLines, obj::BfDrawLayer>()->children();
   auto IL = _addPartForward<BfBladeSectionEnum::IntersectionLines, obj::BfDrawLayer>("Intersection lines layer");

   for (auto& l : CCL) { 
      auto cline = std::static_pointer_cast<curves::BfSingleLine>(l);
      glm::vec3 dir = cline->directionFromStart();
      glm::vec3 normal = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), cline->first().normals) *
                         glm::vec4(dir, 1.0f);

      auto line = std::make_shared<curves::BfSingleLine>(
         BfVertex3( 
            cline->first().pos + normal,
            glm::vec3(1.0f),
            cline->first().normals
         ),
         BfVertex3( 
            cline->first().pos - normal,
            glm::vec3(1.0f),
            cline->first().normals
         )
      );
      line->color() = glm::vec3(0.2f, 0.5f, 0.44f);
      IL->add(line);
   }

   for (size_t i = 0; i < CCL.size() - 1; ++i) 
   { 
      auto left = std::static_pointer_cast<curves::BfSingleLine>(IL->children()[i]);
      auto right = std::static_pointer_cast<curves::BfSingleLine>(IL->children()[i + 1]);
      glm::vec3 intersection = curves::math::findLinesIntersection(
         left->first(), 
         left->second(),
         right->first(), 
         right->second(),
         BF_MATH_FIND_LINES_INTERSECTION_ANY
      );
      if (glm::any(glm::isnan(intersection))) { 
         continue; 
      }

      left->second().pos = intersection;
      right->first().pos = intersection;
   }
}

void BfBladeSection::_processFrontIntersectionLines() {  
   const auto& CCL = _part<BfBladeSectionEnum::CenterCirclesLines, obj::BfDrawLayer>()->children();
   const auto& IL = _part<BfBladeSectionEnum::IntersectionLines, obj::BfDrawLayer>()->children();

   for (size_t i = 0; i < CCL.size(); ++i) { 
      auto cline = std::static_pointer_cast<curves::BfSingleLine>(CCL[i]);
      auto line = std::static_pointer_cast<curves::BfSingleLine>(IL[i]);
      glm::vec3 dir = cline->directionFromStart();
      glm::vec3 normal = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), cline->first().normals) *
                         glm::vec4(dir, 1.0f);
      line->first().pos = cline->first().pos - normal;
      line->second().pos = cline->first().pos + normal;
   }
   for (size_t i = 0; i < CCL.size() - 1; ++i) 
   { 
      auto left = std::static_pointer_cast<curves::BfSingleLine>(IL[i]);
      auto right = std::static_pointer_cast<curves::BfSingleLine>(IL[i + 1]);
      glm::vec3 intersection = curves::math::findLinesIntersection(
         left->first(), 
         left->second(),
         right->first(), 
         right->second(),
         BF_MATH_FIND_LINES_INTERSECTION_ANY
      );
      if (glm::any(glm::isnan(intersection))) { 
         continue; 
      }

      left->second().pos = intersection;
      right->first().pos = intersection;
   }
}

void _createFrontCurves() { 

}
void _processFrontCurves() { 

}



};  // namespace section
};  // namespace obj
