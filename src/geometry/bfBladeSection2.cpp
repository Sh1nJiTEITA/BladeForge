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

   // clang-format on
}

void
BfBladeSection::_processCircleEdges()
{
   // clang-format off
   auto& g = m_info;
   auto chord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();
   // auto inletCircle = _part<BfBladeSectionEnum::InletCircle, curves::BfCircleCenterWH>();
   //
   // if (inletCircle->isChanged())
   // {
   //    // m_info.inletRadius = inletCircle->radius();
   //    // auto direction_between = glm::normalize(inletCircle->center().pos - inletCircle->other().pos);
   //    // std::cout << "dir: " << BfVertex3( direction_between  ) << "\n";
   //    // 
   //    // // auto direction = inletCircle->center().pos - glm::vec3{ g.inletRadius, g.inletRadius, 0.0f };
   //    //
   //    // // inletCircle->center().pos -= direction;
   //    // 
   //    // // inletCircle->other().pos = inletCircle->center().pos + direction_between * g.inletRadius;
   //    //
   //    //
   //    // std::cout << "center: " << inletCircle->center()  << "\n";
   //    // std::cout << "other: " <<  inletCircle->other() << "\n\n";
   // }

   // clang-format off
}

};  // namespace section
};  // namespace obj
