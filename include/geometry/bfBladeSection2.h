#ifndef BF_NEW_BLADESECTION2_H
#define BF_NEW_BLADESECTION2_H

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
      // auto& g = m_info;
      auto oChord = _addPartForward<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>(
         BfVertex3{
            glm::vec3{ 0.0f, 0.0f, 0.0f }, 
            glm::vec3{ 0.5f, 0.5f, 0.1f },
            glm::vec3{ 0.0f, 0.0f, 1.0f }
         },
         BfVertex3{
            glm::vec3{ m_info.chord, 0.0f, 0.0f }, 
            glm::vec3{ 0.5f, 0.5f, 0.1f },
            glm::vec3{ 0.0f, 0.0f, 1.0f }
         }
      );
      // this->add(oChord);

      auto oChordLeft = _addPartForward<BfBladeSectionEnum::_ChordLeftBorder, curves::BfSingleLineWH>( 
         _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>()->left().getp(),
         // BfVertex3{
         //    glm::vec3{ 1.5f, 0.5f, 0.0f }, 
         //    glm::vec3{ 0.5f, 0.5f, 0.1f },
         //    glm::vec3{ 0.0f, 0.0f, 1.0f }
         // },
         BfVertex3{
            glm::vec3{ 0.5f, 0.5f, 0.0f }, 
            glm::vec3{ 0.5f, 0.5f, 0.1f },
            glm::vec3{ 0.0f, 0.0f, 1.0f }
         }
      );
      // this->add(oChordLeft);

      std::cout << oChordLeft->left().get() << "\n";
      std::cout << oChordLeft->right().get() << "\n";

      // clang-format on
   }

   virtual void make() override
   {
      _assignRoots();
      for (auto& child : m_children)
      {
         std::cout << "Makeing child " << child->id() << " inside section\n";
         child->make();
      }
   }

private:
   SectionCreateInfo m_info;
};

}  // namespace section
}  // namespace obj

#endif
