#ifndef BF_NEW_BLADESECTION2_H
#define BF_NEW_BLADESECTION2_H

#include <glm/geometric.hpp>
#include <glm/vector_relational.hpp>

#include "bfCurves4.h"
#include "bfDrawObject2.h"

namespace obj
{
namespace section
{

struct CenterCircle { 
   float relativePos;
   float radius;
};

struct SectionCreateInfo
{
   float chord = 1.0f;
   float installAngle = 80.0f;
   float inletAngle = 30.f;
   float outletAngle = 20.f;
   float inletRadius = 0.043f;
   float outletRadius = 0.017f;
   std::vector<CenterCircle> centerCircles = { { 0.214f, 0.063f }, { 0.459f, 0.082f }, { 0.853f, 0.025f }};
};

enum class BfBladeSectionEnum : uint32_t
{
   Chord = 0,
   _ChordLeftBorder,
   _ChordRightBorder,

   InletCircle,
   OutletCircle,

   AverageInitialCurve,
   CenterCircles,
   CenterCirclesLines,
   IntersectionLines,
};

class BfBladeSection : public obj::BfDrawLayerWithAccess< BfBladeSectionEnum >
{
public:
   using V = BfVertex3;

   // clang-format off
   template <typename T>
   BfBladeSection(T&& info)
       : obj::BfDrawLayerWithAccess<BfBladeSectionEnum>("Blade section")
       , m_info{std::forward<T>(info)}
   {
      _createChord(); 
      _createCircleEdges();
      _createAverageInitialCurve();
      _createCenterCircles();
      _createCCLines();
      _createFrontIntersectionLines();
   }

   virtual void make() override
   {
      _assignRoots();

      premake(); 
      {
         for (auto& child : m_children) {
            child->make();
         }
      }
      postmake();    
   }

   virtual void premake() { 
      _processChord(); 
      _processCircleEdges();
      _processAverageInitialCurve();
      _processCenterCircles();
      _processCCLines();
      _processFrontIntersectionLines();
   }

   virtual void postmake() 
   { 
      auto oChord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();
      m_lastChordL = oChord->left().get();
      m_lastChordR = oChord->right().get();
   }
private:
   bool _isChordChanged();
   float _equivalentInletAngle();
   float _equivalentOutletAngle();
   glm::vec3 _ioIntersection();

private:
   void _createChord();
   void _processChord();

   void _createCircleEdges();
   void _processCircleEdges();

   void _createAverageInitialCurve();
   void _processAverageInitialCurve();

   void _createCenterCircles();
   void _processCenterCircles();

   void _createCCLines();
   void _processCCLines();

   void _createFrontIntersectionLines();
   void _processFrontIntersectionLines();

   void _createFrontCurves();
   void _processFrontCurves();

private:
   BfVertex3 m_lastChordL; 
   BfVertex3 m_lastChordR; 

   BfVar<SectionCreateInfo> m_info;
};

}  // namespace section
}  // namespace obj

#endif
