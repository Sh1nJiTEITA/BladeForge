#ifndef BF_NEW_BLADESECTION2_H
#define BF_NEW_BLADESECTION2_H

#include <glm/geometric.hpp>
#include <glm/vector_relational.hpp>
#include <memory>

#include "bfCurves4.h"
#include "bfDrawObject2.h"

namespace obj
{
namespace section
{

struct SectionCreateInfo
{
   float chord = 1.0f;
   float inletAngle = 30.f;
   float outletAngle = 20.f;
   float inletRadius = 0.2f;
   float outletRadius = 0.05f;
};

enum class BfBladeSectionEnum : uint32_t
{
   Chord = 0,
   _ChordLeftBorder = 1,
   _ChordRightBorder = 2
};

class BfBladeSection : public obj::BfDrawLayerWithAccess<BfBladeSectionEnum>
{
public:
   using V = BfVertex3;

   // clang-format off
   template <typename T>
   BfBladeSection(T&& info)
       : obj::BfDrawLayerWithAccess<BfBladeSectionEnum>("Blade section")
       , m_info{std::forward<T>(info)}
   {
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


   }

   virtual void make() override
   {
      _assignRoots();

      premake(); 
      {
         for (auto& child : m_children)
         {
            child->make();
         }
      }
      postmake();    
   }

   virtual void premake() { 
      auto oChord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();
      auto oChordL = _part<BfBladeSectionEnum::_ChordLeftBorder, curves::BfSingleLineWH>();
      auto oChordR = _part<BfBladeSectionEnum::_ChordRightBorder, curves::BfSingleLineWH>();

      m_info.chord = oChord->line()->length();

      if (!oChord->left().get().equal(m_lastChordL) ||
          !oChord->right().get().equal(m_lastChordR)) 
      { 
         auto line = oChord->line();
         auto up = glm::normalize(-glm::cross(line->directionFromStart(),
                                              line->first().normals));

         oChordL->right().pos() = oChord->left().pos() + up * m_info.chord;
         oChordR->right().pos() = oChord->right().pos() + up * m_info.chord;
      }
   }

   virtual void postmake() 
   { 
      auto oChord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();
      m_lastChordL = oChord->left().get();
      m_lastChordR = oChord->right().get();
   }

private:
   BfVertex3 m_lastChordL; 
   BfVertex3 m_lastChordR; 

   SectionCreateInfo m_info;
};

}  // namespace section
}  // namespace obj

#endif
