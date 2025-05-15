#include "bfBladeSection2.h"

#include "bfCirclePack.h"
#include "bfCurves4.h"
#include "bfDrawObject2.h"
#include "bfEdge.h"
#include "bfObjectMath.h"
#include <algorithm>
#include <bfChain.h>
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
#include <stdexcept>

namespace obj
{
namespace section
{
void
BfBladeSection::applyRenderToggle()
{
   for (uint32_t v = static_cast< uint32_t >(BfBladeSectionEnum::Chord);
        v < static_cast< uint32_t >(BfBladeSectionEnum::End);
        v <<= 1)
   {
      auto section = static_cast< BfBladeSectionEnum >(v);
      _toggleRender(
          section,
          m_info.get().renderBitSet & static_cast< uint32_t >(section)
      );
   }
};

void
BfBladeSection::toggleAllHandles(int sts)
{
   std::stack< std::shared_ptr< BfDrawObjectBase > > stack;
   for (auto ch : m_children)
   {
      stack.push(ch);
   }

   while (!stack.empty())
   {
      auto top = stack.top();
      stack.pop();
      for (auto ch : top->children())
      {
         stack.push(ch);
      }
      if (auto cast = std::dynamic_pointer_cast< curves::BfHandleCircle >(top))
      {
         top->toggleRender(sts);
      }
   }
}

std::vector< BfVertex3 >
BfBladeSection::genOutputShape()
{
   // clang-format off
   if (!_isPart< E::InletArc >()) {
      throw std::runtime_error("Cant gen output shape from blade section: No inlet arc"); 
   }
   if (!_isPart< E::OutletArc >()) {
      throw std::runtime_error("Cant gen output shape from blade section: No outlet arc");
   }
   if (!_isPart< E::Chain >()) {
      throw std::runtime_error("Cant gen output shape from blade section: No chain layer");
   }

   auto iarc = _part< E::InletArc, curves::BfArcCenter >();
   auto oarc = _part< E::OutletArc, curves::BfArcCenter >();
   auto chain = _part< E::Chain, curves::BfChain >();

   if (iarc->vertices().empty()) {
      fmt::println("No vertices in inlet arc");
      return {};
   }
   if (oarc->vertices().empty()) {
      fmt::println("No vertices in outlet arc");
   }

   size_t size = iarc->vertices().size() + oarc->vertices().size();
   for (auto back : chain->bezierCurveChain(curves::BfChain::Back))
   {
      if (back->vertices().empty())
      {
         fmt::println("No vertices in part of back chain");
      }
      size += back->vertices().size();
   }
   for (auto front : chain->bezierCurveChain(curves::BfChain::Front))
   {
      if (front->vertices().empty())
      {
         fmt::println("No vertices in part of front chain");
      }
      size += front->vertices().size();
   }
   // fmt::println("Total output vertices count={}", size);

   std::vector< BfVertex3 > v;
   v.reserve(size);
   size_t counter = 0;

   size = iarc->vertices().size();
   for (auto vert = iarc->vertices().begin(); vert != iarc->vertices().end(); ++vert)
   {
      const float t = static_cast< float >(counter) / size;
      glm::vec3 color{
          t,
          1 - t,
          0,
      };
      v.push_back(vert->otherColor(std::move(color)));
      counter++;
   }

   
   //
   
   //
   for (auto front : chain->bezierCurveChain(curves::BfChain::Front))
   {
      size = front->vertices().size();
      counter = 0;
      for (const auto& vert : front->vertices())
      {
         const float t = static_cast< float >(counter) / size;
         glm::vec3 color{
             t,
             1 - t,
             0,
         };
         v.push_back(vert.otherColor(std::move(color)));
         counter++;
      }
   }

   
   for (const auto& vert : oarc->vertices())
   {
      const float t = static_cast< float >(counter) / size;
      glm::vec3 color{
          t,
          1 - t,
          0,
      };
      v.push_back(vert.otherColor(std::move(color)));
      counter++;
   }

   auto back_chain = chain->bezierCurveChain(curves::BfChain::Back);
   for (auto back = back_chain.rbegin(); back != back_chain.rend(); ++back) {
      size = (*back)->vertices().size();
      counter = 0;
      for (auto vert = (*back)->vertices().rbegin(); vert != (*back)->vertices().rend(); ++vert)
      {
         const float t = static_cast< float >(counter) / size;
         glm::vec3 color{
             t,
             1 - t,
             0,
         };
         v.push_back(vert->otherColor(std::move(color)));
         counter++;
      }
   }

   // fmt::println("Done {}/{}", counter, size);

   return v;
}

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
   auto oChord = _addPartF<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>(
      BfVertex3{
         glm::vec3{ 0.0f, 0.0f, g.get().z }, 
         glm::vec3{ 0.5f, 0.5f, 0.1f },
         glm::vec3{ 0.0f, 0.0f, 1.0f }
      },
      BfVertex3{
         glm::vec3{ g.get().chord, 0.0f, g.get().z }, 
         glm::vec3{ 0.5f, 0.5f, 0.1f },
         glm::vec3{ 0.0f, 0.0f, 1.0f }
      }
   );
   // oChord->toggleRender(false);
   // oChord->toggleRender(false);
   m_lastChordL = oChord->left().get();
   m_lastChordR = oChord->right().get();

   auto oChordLeft = _addPartF<BfBladeSectionEnum::_ChordLeftBorder, curves::BfSingleLineWH>( 
      _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>()->left().getp(),
      BfVertex3{
         glm::vec3{ oChord->left().pos().x, g.get().chord, g.get().z }, 
         glm::vec3{ 0.5f, 0.5f, 0.1f },
         glm::vec3{ 0.0f, 0.0f, 1.0f }
      }
   );
   // oChordLeft->toggleRender(false);
   // oChordLeft->toggleRender(false);

   auto oChordRight = _addPartF<BfBladeSectionEnum::_ChordRightBorder, curves::BfSingleLineWH>( 
      _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>()->right().getp(),
      BfVertex3{
         glm::vec3{ oChord->right().pos().x, g.get().chord, g.get().z }, 
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

   if (oChord->left().pos().z != m_info.get().z) { 
      float z = m_info.get().z;
      oChord->line()->setZ(z);
      oChordL->line()->setZ(z);
      oChordR->line()->setZ(z);
   }


   if (oChord->leftHandle()->isChanged()) { 
      oChord->left().pos() = { 0.0f, 0.0f, m_info.get().z };
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

   auto inletCircle = _addPartF<BfBladeSectionEnum::InletCircle, curves::BfCircle2LinesWH>(
      oChordL->right().getp(),
      oChord->left().getp(),
      oChord->right().getp(),
      &g.get().inletRadius 
   );

   auto outletCircle = _addPartF<BfBladeSectionEnum::OutletCircle, curves::BfCircle2LinesWH>(
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
   auto iLine = _addPartF<BfBladeSectionEnum::InletDirection, curves::BfSingleLineWH>( 
      iFree,
      BfVertex3{  
         iFree.pos - _eqInletDirection() * m_info.get().chord * 0.2f,
         iFree.color, 
         iFree.normals
      }
   );

   auto oFree = OC->centerVertex();
   auto oLine = _addPartF<BfBladeSectionEnum::OutletDirection, curves::BfSingleLineWH>( 
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
   
   auto curve = _addPartF<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>(
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

   // FIXME: WORKS EVERY REMAKE
   float z = m_info.get().z;
   for (auto& v : *curve) { v.pos().z = z; }
}

void BfBladeSection::_createCenterCircles() { 
   auto circ_layer = _addPartF<BfBladeSectionEnum::CenterCircles, obj::BfDrawLayer>("Circles layer 2");
   for (auto& circ : m_info.get().centerCircles) { 
      circ_layer->addf<curves::BfCirclePackWH>(
         BfVar< float >(&circ.relativePos),
         BfVar< float >(&circ.radius),
         BfVar< float >(90.0f),
         BfVar< float >(90.0f),
         _part<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>()->curve()
         , curves::BfCirclePackWH::Flag::SeparateHandles// | curves::BfCirclePackWH::Flag::FixedAngle
      );
   }
   circ_layer->make();
}

void BfBladeSection::_processCenterCircles() {
   // clang-format on
   auto& c = m_info.get().centerCircles;
   auto circ_layer = _part< E::CenterCircles, obj::BfDrawLayer >();

   if (c.size() > circ_layer->children().size())
   {
      // NOTE:
      // If new circle was added somewhere -> add it to
      // circle layers, assuming that adding are done by
      // single element max per frame and pushed to the
      // back of create info circle list
      //
      circ_layer->addf< curves::BfCirclePackWH >(
          BfVar< float >(&c.back().relativePos),
          BfVar< float >(&c.back().radius),
          BfVar< float >(90.0f),
          BfVar< float >(90.0f),
          _part< E::AverageInitialCurve, curves::BfBezierWH >()->curve(),
          curves::BfCirclePackWH::Flag::SeparateHandles
      );
   }
   else
   {
      // NOTE:
      // If relative position from gui set to NAN -> erase it
      // from layers and from create info
      //
      std::erase_if(
          circ_layer->children(),
          [](const std::shared_ptr< BfDrawObjectBase >& p) {
             auto pack = std::static_pointer_cast< curves::BfCirclePackWH >(p);
             return std::isnan(pack->relativePos());
          }
      );

      std::erase_if(m_info.get().centerCircles, [](const CenterCircle& c) {
         return std::isnan(c.relativePos);
      });
   }

   c.sort([](const auto& lhs, const auto& rhs) {
      return std::less{}(lhs.relativePos, rhs.relativePos);
   });
}

void
BfBladeSection::_createIOCircles()
{
   _addPartF< E::InletEdge, curves::BfEdge >(
       BfVar< float >(90.f),
       BfVar< float >(90.f),
       _part< E::InletDirection, curves::BfSingleLineWH >(),
       _part< E::InletCircle, curves::BfCircle2LinesWH >(),
       curves::BfEdge::Type::Inlet
   );

   _addPartF< E::OutletEdge, curves::BfEdge >(
       BfVar< float >(90.f),
       BfVar< float >(90.f),
       _part< E::OutletDirection, curves::BfSingleLineWH >(),
       _part< E::OutletCircle, curves::BfCircle2LinesWH >(),
       curves::BfEdge::Type::Outlet
   );
}

void
BfBladeSection::_processIOCircles()
{
}

void
BfBladeSection::_createChain()
{
   auto chain = _addPartF< E::Chain, curves::BfChain >(
       _part< E::CenterCircles, obj::BfDrawLayer >(),
       _part< E::InletEdge, curves::BfEdge >(),
       _part< E::OutletEdge, curves::BfEdge >()
   );
}

void
BfBladeSection::_createIOArc()
{
}

void
BfBladeSection::_processIOArc()
{
   if (!_isPart< E::InletArc >())
   {
      auto iedge = _part< E::InletEdge, curves::BfEdge >();
      iedge->make();

      auto iarcv = iedge->arcVertices();
      _addPartF< E::InletArc, curves::BfArcCenter >(
          iarcv[0],
          iarcv[1],
          iarcv[2]
      );

      auto oedge = _part< E::OutletEdge, curves::BfEdge >();
      oedge->make();

      auto oarcv = oedge->arcVertices();
      _addPartF< E::OutletArc, curves::BfArcCenter >(
          oarcv[0],
          oarcv[1],
          oarcv[2]
      );
   }
   else
   {
      auto iedge = _part< E::InletEdge, curves::BfEdge >();
      auto iarcv = iedge->arcVertices();
      auto iarc = _part< E::InletArc, curves::BfArcCenter >();
      iarc->begin() = iarcv[0];
      iarc->middle() = iarcv[1];
      iarc->end() = iarcv[2];

      auto oedge = _part< E::OutletEdge, curves::BfEdge >();
      auto oarcv = oedge->arcVertices();
      auto oarc = _part< E::OutletArc, curves::BfArcCenter >();
      oarc->begin() = oarcv[0];
      oarc->middle() = oarcv[1];
      oarc->end() = oarcv[2];
   }
}

void
BfBladeSection::_processChain()
{
   auto chain = _part< E::Chain, curves::BfChain >();
   chain->z() = m_info.get().z;
   // chain->addUpdateLines();
   // chain->addUpdateLines();
}

void
BfBladeSection::_createOutputShape()
{
}

void
BfBladeSection::_processOutputShape()
{
   // clang-format off
   if (!_isPart< E::OutputShape >())
   {
      std::vector< BfVertex3 > v = genOutputShape();
      auto shape = _addPartF< E::OutputShape, curves::BfSectionOutputShape  >(
         std::move(v),
         BfVar< float >(&m_info.get().installAngle),
         BfVar< float >(&m_info.get().step)
      );
      outputShapeSegments() = v.size();
   }
   else
   {
      auto shape = _part< E::OutputShape, curves::BfSectionOutputShape >();
      std::vector< BfVertex3 > v = genOutputShape();
      if (std::isnan(outputShapeSegments()) || outputShapeSegments() == 0) { 
         outputShapeSegments() = v.size();
      }
      if (outputShapeSegments() == v.size()) { 
         shape->setVertices(std::move(v));
      } else { 
         shape->setVertices(curves::math::resampleCurve(v, outputShapeSegments()));
      }
   }
   // clang-format on
}

void
BfBladeSection::_processMassCenter()
{
   // clang-format off
   if (!_isPart< E::MassCenter >())
   {
      auto shape = _part< E::OutputShape, curves::BfSectionOutputShape >();
      auto chord = _part< E::Chord, curves::BfSingleLineWH >();
      const glm::vec3 mass_center = curves::math::centerOfMass(shape->vertices());
      auto obj = _addPartF< E::MassCenter, curves::BfCircleCenterFilled >(
          BfVertex3(mass_center, glm::vec3(0.0f, 0.0f, 0.8f), chord->line()->first().normals()),
          0.015f
      );
      obj->color() = glm::vec3(0.0f, 0.0f, 0.8f);
   }
   else
   {
      auto shape = _part< E::OutputShape, curves::BfSectionOutputShape >();
      const glm::vec3 mass_center = curves::math::centerOfMass(shape->vertices());
      auto center =_part< E::MassCenter, curves::BfCircleCenterFilled >();
      center->center().pos() = mass_center;
   }
   // clang-format on
}

void
BfBladeSection::_processTriangularShape()
{
   // clang-format off
   if (!_isPart< E::TriangularShape >())
   {
      auto l = _addPartF<E::TriangularShape, obj::BfDrawLayer>("Triangular shape");
      auto t = triangulate();
      l->children() = std::move(t);
   }
   else
   {
      auto l = _part<E::TriangularShape, obj::BfDrawLayer>();
      auto t = triangulate();
      if (t.size() == l->children().size()) { 
         for (size_t i = 0; i < t.size(); ++i)  {
            auto tr = std::static_pointer_cast<curves::BfTriangle>(l->children()[i]);
            auto ntr = std::static_pointer_cast< curves::BfTriangle >(t[i]);
            tr->first().pos() = ntr->first().pos();
            tr->second().pos() = ntr->second().pos();
            tr->third().pos() = ntr->third().pos();
         }
      }
      else { 
         l->children() = std::move(t);
      }
   }
   // clang-format on
}

void
BfBladeSection::viewOutputShapeOnly()
{
   // clang-format off
   //
   m_lastRenderBitSet = m_info.get().renderBitSet;
   m_info.get().renderBitSet = 0;
   m_info.get().renderBitSet |= static_cast< uint32_t >(BfBladeSectionEnum::OutputShape);
   applyRenderToggle();
   m_isOutputShapeWasEnabled = true;
}

void
BfBladeSection::viewFormattingShapeOnly()
{
   // clang-format off
   m_lastRenderBitSet = m_info.get().renderBitSet;
   m_info.get().renderBitSet &= ~static_cast< uint32_t >(BfBladeSectionEnum::OutputShape);
   
   applyRenderToggle();
}

void BfBladeSection::revertView() 
{ 
   if (m_isOutputShapeWasEnabled) { 
      m_info.get().renderBitSet = m_lastRenderBitSet;
      applyRenderToggle();
      m_isOutputShapeWasEnabled = false;
   }
}

// clang-format on
void
BfBladeSection::prerender(size_t viewport_index)
{
   switch (viewport_index)
   {
   case 0:
      viewFormattingShapeOnly();
      break;
   case 1:
      viewOutputShapeOnly();
      break;
   };
};

void
BfBladeSection::postrender(size_t viewport_index)
{
   switch (viewport_index)
   {
   case 0:
      revertView();
      break;
   case 1:
      revertView();
      break;
   };
};

std::vector< BfObj >
BfBladeSection::triangulate()
{
   // clang-format off
   std::vector< BfObj > o;

   auto iedge = _part< E::InletEdge, curves::BfEdge >();
   const BfVertex3 ic = iedge->circle()->centerVertex();
   auto iarc = _part< E::InletArc, curves::BfArcCenter >();
   auto& iarcv = iarc->vertices();
   
   // FIXME: Fix if entire section logic of creation will 
   // be refractored (multiple make initial calls)
   if (iarcv.empty()) return {};
   
   for (size_t i = 0; i < iarcv.size() - 1; ++i)
   {
      auto t = std::make_shared< curves::BfTriangle >(
          BfVertex3Uni(ic),
          BfVertex3Uni(iarcv[i]),
          BfVertex3Uni(iarcv[i + 1])
      );
      o.push_back(std::move(t));
   }

   auto oedge = _part< E::OutletEdge, curves::BfEdge >();
   const BfVertex3 oc = oedge->circle()->centerVertex();
   auto oarc = _part< E::OutletArc, curves::BfArcCenter >();
   auto& oarcv = oarc->vertices();
   
   if (iarcv.empty()) return {};
   
   // for (size_t i = 0; i < iarcv.size() - 1; ++i)
   // {
   //    auto t = std::make_shared< curves::BfTriangle >(
   //        BfVertex3Uni(oc),
   //        BfVertex3Uni(oarcv[i]),
   //        BfVertex3Uni(oarcv[i + 1])
   //    );
   //    o.push_back(std::move(t));
   // }

   auto chain = _part<E::Chain, curves::BfChain>();
   auto front = chain->bezierCurveChain(curves::BfChain::Front);
   auto back = chain->bezierCurveChain(curves::BfChain::Back);
   if (front.empty()) return o;
   
   assert(front.size() == back.size() && "Chain back & front number of Bezier curves are not the same");
   
   for (size_t i = 0; i < front.size(); ++i) { 
      auto& f = front[i]->vertices();
      auto& b = back[i]->vertices();
      
      assert(f.size() == b.size() && "Chain back & front ELEMENTS do not have the same amount of vertices");
      
      
      for (size_t j = 0 + (i == 0 ? 1 : 0); j < f.size() - 1; ++j) { 
         auto t = std::make_shared< curves::BfTriangle >(
            BfVertex3Uni(b[j]),
            BfVertex3Uni(f[j]),
            BfVertex3Uni(f[j+1])
         );
         o.push_back(std::move(t));   
         break;
      }

      for (size_t j = 1 + (i == 0 ? 1 : 0); j < f.size() - 1; ++j) { 
         auto t = std::make_shared< curves::BfTriangle >(
            BfVertex3Uni(f[j]),
            BfVertex3Uni(b[j]),
            BfVertex3Uni(b[j-1])
         );
         o.push_back(std::move(t));   
         break;
      }
   }

   const glm::vec3 cc { 0.2, 0.1, 0.78 };
   // CONNECTIONS
   auto& fverts = front[0]->vertices();
   {
      auto t = std::make_shared<curves::BfTriangle>(
          BfVertex3Uni(ic),
          BfVertex3Uni(*(front[0]->begin())),
          BfVertex3Uni(iarcv.back())
      );
      t->color() = cc;
      o.push_back(std::move(t));
   }
   {
      // auto t = std::make_shared<curves::BfTriangle>(
      //     BfVertex3Uni(ic),
      //     BfVertex3Uni(back.front()->front()),
      //     BfVertex3Uni(iarcv.front())
      // );
      // t->color() = cc;
      // o.push_back(std::move(t));
   }


   return o;
   // clang-format on
}

}; // namespace section
}; // namespace obj
