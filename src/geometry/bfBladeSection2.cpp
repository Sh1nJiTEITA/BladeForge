#include "bfBladeSection2.h"

#include "bfCurves4.h"
#include <glm/geometric.hpp>

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

float BfBladeSection::_equivalentInletAngle() { 
   return 90.0f - this->m_info.inletAngle;
}
float BfBladeSection::_equivalentOutletAngle() { 
   return 90.0f - this->m_info.outletAngle;
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
         glm::vec3{ g.chord, 0.0f, 0.0f }, 
         glm::vec3{ 0.5f, 0.5f, 0.1f },
         glm::vec3{ 0.0f, 0.0f, 1.0f }
      }
   );
   m_lastChordL = oChord->left().get();
   m_lastChordR = oChord->right().get();

   auto oChordLeft = _addPartForward<BfBladeSectionEnum::_ChordLeftBorder, curves::BfSingleLineWH>( 
      _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>()->left().getp(),
      BfVertex3{
         glm::vec3{ oChord->left().pos().x, g.chord, 0.0f }, 
         glm::vec3{ 0.5f, 0.5f, 0.1f },
         glm::vec3{ 0.0f, 0.0f, 1.0f }
      }
   );

   auto oChordRight = _addPartForward<BfBladeSectionEnum::_ChordRightBorder, curves::BfSingleLineWH>( 
      _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>()->right().getp(),
      BfVertex3{
         glm::vec3{ oChord->right().pos().x, g.chord, 0.0f }, 
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

   m_info.chord = oChord->line()->length();

   if (_isChordChanged()) 
   { 
      auto line = oChord->line();
      auto up = glm::normalize(-glm::cross(line->directionFromStart(),
                                           line->first().normals));

      oChordL->right().pos() = oChord->left().pos() + up * m_info.chord;
      oChordR->right().pos() = oChord->right().pos() + up * m_info.chord;
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
      &g.inletRadius 
   );

   auto outletCircle = _addPartForward<BfBladeSectionEnum::OutletCircle, curves::BfCircle2LinesWH>(
      oChord->left().getp(),
      oChord->right().getp(),
      oChordR->right().getp(),
      &g.outletRadius 
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

   auto iac = _addPartForward<BfBladeSectionEnum::AverageInitialCurve, curves::BfBezierWH>(
       
   );
   
}


};  // namespace section
};  // namespace obj
