#include "bfBladeSection2.h"

#include "bfCurves4.h"
#include "bfObjectMath.h"
#include <algorithm>
#include <cmath>
#include <fmt/base.h>
#include <fmt/ranges.h>
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vector_relational.hpp>
#include <iterator>
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
BfBladeSection::_eqInletAngle()
{
   auto& g = this->m_info.get();
   return g.isEqMode ? 180 - g.inletAngle - g.installAngle : g.inletAngle;
}
float
BfBladeSection::_eqOutletAngle()
{
   auto& g = this->m_info.get();
   return g.isEqMode ? g.installAngle - g.outletAngle : g.outletAngle;
}

glm::vec3
BfBladeSection::_eqInletDirection()
{
   auto chord = _part< BfBladeSectionEnum::Chord, curves::BfSingleLineWH >();
   glm::vec3 direction = chord->line()->directionFromStart();
   return glm::rotate(
              glm::mat4(1.0f),
              glm::radians(this->_eqInletAngle()),
              chord->line()->first().normals()
          ) *
          glm::vec4(direction, 1.0f);
}

glm::vec3
BfBladeSection::_eqOutletDirection()
{
   auto chord = _part< BfBladeSectionEnum::Chord, curves::BfSingleLineWH >();
   glm::vec3 direction = chord->line()->directionFromStart();
   return glm::rotate(
              glm::mat4(1.0f),
              glm::radians(180.0f - this->_eqOutletAngle()),
              chord->line()->first().normals()
          ) *
          glm::vec4(direction, 1.0f);
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
                             glm::radians(_eqInletAngle()),
                             iCenter.normals
                         ) *
                         glm::vec4(angle_dir, 1.0f);

   glm::vec3 outlet_dir = glm::rotate(
                              glm::mat4(1.0f),
                              -glm::radians(_eqOutletAngle()),
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

   if (oChord->leftHandle()->isChanged()) { 
      oChord->left().pos() = { 0.0f, 0.0f, 0.0f };
   }
   if (oChord->rightHandle()->isChanged()) { 
      oChord->right().pos().y = oChord->left().pos().y;
   }

   if (_isChordChanged()) 
   { 
      auto line = oChord->line();
      auto up = glm::normalize(-glm::cross(line->directionFromStart(),
                                           line->first().normals()));

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


void BfBladeSection::_createIOAngles() { 
   auto IC = _part<BfBladeSectionEnum::InletCircle, curves::BfCircle2LinesWH>();
   auto OC = _part<BfBladeSectionEnum::OutletCircle, curves::BfCircle2LinesWH>();

   auto iFree = IC->centerVertex();
   auto iLine = _addPartForward<BfBladeSectionEnum::InletDirection, curves::BfSingleLineWH>( 
      iFree,
      BfVertex3{  
         iFree.pos - _eqInletDirection() * m_info.get().chord * 0.2f,
         iFree.color, 
         iFree.normals
      }
   );

   auto oFree = OC->centerVertex();
   auto oLine = _addPartForward<BfBladeSectionEnum::OutletDirection, curves::BfSingleLineWH>( 
      oFree,
      BfVertex3{  
         oFree.pos - _eqOutletDirection() * m_info.get().chord * 0.2f,
         oFree.color, 
         oFree.normals
      }
   );
}

void BfBladeSection::_processIOAngles() 
{ 
   auto chord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();

   auto iLine = _part<BfBladeSectionEnum::InletDirection, curves::BfSingleLineWH>();
   auto IC = _part<BfBladeSectionEnum::InletCircle, curves::BfCircle2LinesWH>();
   if (iLine->rightHandle()->isChanged()) { 
      glm::vec3 cDir = chord->line()->directionFromStart();
      glm::vec3 iDir = iLine->line()->directionFromEnd();
      m_info.get().inletAngle = glm::degrees(curves::math::angle(
         std::move(cDir), 
         std::move(iDir)
      ));
   }
   iLine->line()->first().pos() = IC->centerVertex().pos;
   iLine->line()->second().pos() = IC->centerVertex().pos - _eqInletDirection() * m_info.get().chord * 0.2f;

   auto oLine = _part<BfBladeSectionEnum::OutletDirection, curves::BfSingleLineWH>();
   auto OC = _part<BfBladeSectionEnum::OutletCircle, curves::BfCircle2LinesWH>();
   if (oLine->rightHandle()->isChanged()) { 
      glm::vec3 cDir = chord->line()->directionFromStart();
      glm::vec3 oDir = oLine->line()->directionFromEnd();
      float angle = glm::degrees(curves::math::angle(
         std::move(cDir), 
         std::move(oDir)
      ));
      m_info.get().outletAngle = 180 - angle;
   }
   oLine->line()->first().pos() = OC->centerVertex().pos;
   oLine->line()->second().pos() = OC->centerVertex().pos - _eqOutletDirection() * m_info.get().chord * 0.2f;
}


void BfBladeSection::_createAverageInitialCurve() 
{ 
   auto inletCircle = _part<BfBladeSectionEnum::InletCircle, curves::BfCircle2LinesWH>();
   auto outletCircle = _part<BfBladeSectionEnum::OutletCircle, curves::BfCircle2LinesWH>();   
   auto oChord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();
   
   auto curve = _addPartForward<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>(
      BfVertex3Uni(inletCircle->circle()->center()),
      BfVertex3Uni(BfVertex3(
         _ioIntersection(),
         inletCircle->circle()->center().color,
         inletCircle->circle()->center().normals
      )),
      BfVertex3Uni(outletCircle->circle()->center())
   );
   for (size_t i = 2; i < m_info.get().initialBezierCurveOrder; ++i) { 
      curve->elevateOrder();
   }
   auto& inlet_vert = *(curve->begin() + 1);
   auto& outlet_vert = *(curve->rbegin() + 1);
   
   auto chord_line = oChord->line();

   inlet_vert.pos() = curves::math::closestPointOnLine(
      inlet_vert, 
      chord_line->first().pos() + inletCircle->centerVertex().pos,
      _eqInletDirection()
   );
   outlet_vert.pos() = curves::math::closestPointOnLine(
      outlet_vert, 
      chord_line->second().pos() + outletCircle->centerVertex().pos, 
      _eqOutletDirection()
   );
}

void BfBladeSection::_processAverageInitialCurve() { 
   auto inletCircle = _part<BfBladeSectionEnum::InletCircle, curves::BfCircle2LinesWH>();
   auto outletCircle = _part<BfBladeSectionEnum::OutletCircle, curves::BfCircle2LinesWH>();
   auto curve = _part<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>();
   
   const int current_curve_order = curve->size() - 1;
   const int needed_curve_order = m_info.get().initialBezierCurveOrder;

   if (current_curve_order < needed_curve_order) { 
      for (int i = current_curve_order; i < needed_curve_order; ++i) { 
         fmt::println("Elevating initial curve order {} -> {}", i, needed_curve_order);
         curve->elevateOrder();
      }
   }
   else if (current_curve_order > needed_curve_order) { 
      for (int i = current_curve_order; i > needed_curve_order; --i) { 
         fmt::println("Lowering initial curve order {} -> {}", i, needed_curve_order);
         curve->lowerateOrder();
      }
   }
   auto& inlet_vert = *(curve->begin() + 1);
   auto& outlet_vert = *(curve->rbegin() + 1);

   glm::vec3 iCenter = inletCircle->centerVertex().pos;
   inlet_vert.pos() = curves::math::closestPointOnLine(inlet_vert, iCenter, iCenter + _eqInletDirection());
   curve->begin()->pos() = iCenter;

   glm::vec3 oCenter = outletCircle->centerVertex().pos;
   outlet_vert.pos() = curves::math::closestPointOnLine(outlet_vert, oCenter, oCenter + _eqOutletDirection());
   curve->rbegin()->pos() = oCenter;
}

void BfBladeSection::_createCenterCircles() {
   auto initCurve = _part<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>();
   auto circleLayer = _addPartForward<BfBladeSectionEnum::CenterCircles, obj::BfDrawLayer>(
      "Center circles layer"
   );
   std::shared_ptr< curves::BfCirclePack > last { nullptr };
   for (auto& circleInfo : m_info.get().centerCircles) { 
      auto circle_pack = std::make_shared<curves::BfCirclePack>(
         BfVar<float>(&circleInfo.relativePos),
         BfVar<float>(&circleInfo.radius), 
         initCurve->curve()->weak_from_this()
      );
      circleLayer->add(circle_pack);
      if (last.get()) { 
         last->bindNext(circle_pack);
         circle_pack->bindPrevious(last);
      }
      last = circle_pack;
   }
}

void BfBladeSection::_processCenterCircles() { 
   auto initCurve = _part<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>();
   auto circleLayer = _part<BfBladeSectionEnum::CenterCircles, obj::BfDrawLayer>();
   using pack_t = std::shared_ptr< curves::BfCirclePack >;

   const size_t current_circles_count = circleLayer->children().size();
   const size_t needed_circles_count = m_info.get().centerCircles.size();
   
   if (current_circles_count < needed_circles_count) { 
      for (size_t i = current_circles_count; i < needed_circles_count; ++i) { 
         auto& info = m_info.get().centerCircles.at(i); 
         auto circle_pack = std::make_shared<curves::BfCirclePack>(
            BfVar<float>(&info.relativePos),
            BfVar<float>(&info.radius), 
            initCurve->curve()->weak_from_this()
         );
         circle_pack->make();
         circleLayer->add(circle_pack);   
      }
   } else { 
      circleLayer->children().erase(
         std::remove_if(circleLayer->children().begin(),
                        circleLayer->children().end(),
                        [](const std::shared_ptr<obj::BfDrawObjectBase>& p) { 
                           auto casted = std::static_pointer_cast<curves::BfCirclePack>(p);
                           return std::isnan(casted->radius().get()) && std::isnan(casted->relativePos().get());
                        }), 
         circleLayer->children().end()
      );
      m_info.get().centerCircles.erase(
         std::remove_if(m_info.get().centerCircles.begin(),
                        m_info.get().centerCircles.end(),
                        [](const section::CenterCircle& circle) { 
                           return std::isnan(circle.radius) && std::isnan(circle.relativePos);
                        }), 
         m_info.get().centerCircles.end()
      );
   }

   std::vector<pack_t> packs ;
   std::transform(circleLayer->children().begin(), 
                  circleLayer->children().end(), 
                  std::back_inserter(packs),
                  [](std::shared_ptr< obj::BfDrawObjectBase >& o) { 
                     return std::static_pointer_cast<curves::BfCirclePack>(o);                  
                  });
   
   std::sort(packs.begin(), packs.end(), [](const pack_t& lhs, const pack_t& rhs) { 
      return lhs->relativePos().get() < rhs->relativePos().get();
   });
   
   packs.front()->bindPrevious(std::weak_ptr<obj::BfDrawObjectBase>{});
   packs.back()->bindNext(std::weak_ptr<obj::BfDrawObjectBase>{});
   
   pack_t prev{};
   for (auto& pack : packs) { 
      if (prev) { 
         pack->bindPrevious(prev);
         prev->bindNext(pack);
      }
      prev = pack;
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
   auto CC = _part<BfBladeSectionEnum::CenterCircles, obj::BfDrawLayer>();
   auto CCL = _part<BfBladeSectionEnum::CenterCirclesLines, obj::BfDrawLayer>();

   const size_t current_lines_count = CCL->children().size();
   const size_t needed_lines_count = CC->children().size();

   if (current_lines_count < needed_lines_count) { 
      for (size_t i = current_lines_count; i < needed_lines_count; ++i) { 
         auto circle = std::static_pointer_cast<curves::BfCircleCenterWH>(CC->children()[i]);
         float t = m_info.get().centerCircles[i].relativePos;
         float r = m_info.get().centerCircles[i].radius;
         auto normal = curves::math::BfBezierBase::calcNormal(*initCurve, t);
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
      }
   }


   for (size_t i = 0; i < m_info.get().centerCircles.size(); ++i) { 
      float t = m_info.get().centerCircles[i].relativePos;
      float r = m_info.get().centerCircles[i].radius;
      auto normal = curves::math::BfBezierBase::calcNormal(*initCurve, t);
      auto circle = std::static_pointer_cast<curves::BfCircleCenterWH>(CC->children()[i]);
      auto line = std::static_pointer_cast<curves::BfSingleLine>(CCL->children()[i]);
      line->first().pos() = circle->center().pos + normal * r;
      line->second().pos() = circle->center().pos - normal * r;
   }
}

void BfBladeSection::_createFrontIntersectionLines() { 
   const auto& CCL = _part<BfBladeSectionEnum::CenterCirclesLines, obj::BfDrawLayer>()->children();
   auto IL = _addPartForward<BfBladeSectionEnum::IntersectionLines, obj::BfDrawLayer>("Intersection lines layer");

   for (auto& l : CCL) { 
      auto cline = std::static_pointer_cast<curves::BfSingleLine>(l);
      glm::vec3 dir = cline->directionFromStart();
      glm::vec3 normal = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), cline->first().normals()) *
                         glm::vec4(dir, 1.0f);

      auto line = std::make_shared<curves::BfSingleLine>(
         BfVertex3( 
            cline->first().pos() + normal,
            glm::vec3(1.0f),
            cline->first().normals()
         ),
         BfVertex3( 
            cline->first().pos() - normal,
            glm::vec3(1.0f),
            cline->first().normals()
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

      left->second().pos() = intersection;
      right->first().pos() = intersection;
   }
}

void BfBladeSection::_processFrontIntersectionLines() {  
   const auto& CCL = _part<BfBladeSectionEnum::CenterCirclesLines, obj::BfDrawLayer>()->children();
   const auto& IL = _part<BfBladeSectionEnum::IntersectionLines, obj::BfDrawLayer>()->children();

   for (size_t i = 0; i < CCL.size(); ++i) { 
      auto cline = std::static_pointer_cast<curves::BfSingleLine>(CCL[i]);
      auto line = std::static_pointer_cast<curves::BfSingleLine>(IL[i]);
      glm::vec3 dir = cline->directionFromStart();
      glm::vec3 normal = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), cline->first().normals()) *
                         glm::vec4(dir, 1.0f);
      line->first().pos() = cline->first().pos() - normal;
      line->second().pos() = cline->first().pos() + normal;
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

      left->second().pos() = intersection;
      right->first().pos() = intersection;
   }
}

void BfBladeSection::_createFrontCurves() { 
   const auto& CC = _part<BfBladeSectionEnum::CenterCircles, obj::BfDrawLayer>()->children();
   const auto& CCL = _part<BfBladeSectionEnum::CenterCirclesLines, obj::BfDrawLayer>()->children();
   const auto& IL = _part<BfBladeSectionEnum::IntersectionLines, obj::BfDrawLayer>()->children();
   // assert( CC.size() == CCL.size() == IL.size() );   

   auto back_layer = _addPartForward<BfBladeSectionEnum::Back, obj::BfDrawLayer>("Back layer");

   for (size_t i = 0; i < CC.size() - 1; ++i) { 
      auto normal_left = std::static_pointer_cast<curves::BfSingleLine>(CCL[i]);
      auto normal_right = std::static_pointer_cast<curves::BfSingleLine>(CCL[i + 1]);
      auto tangent = std::static_pointer_cast<curves::BfSingleLine>(IL[i]);
      auto bezier = std::make_shared<curves::BfBezierN>(
         BfVertex3Uni(normal_left->first().getp()),
         BfVertex3Uni(tangent->second().getp()),
         BfVertex3Uni(normal_right->first().getp())
      );
      bezier->color() = glm::vec3(1.0f, 0.1f, 0.1f);
      this->add(bezier);
   }
}

void BfBladeSection::_processFrontCurves() { 

}



};  // namespace section
};  // namespace obj
