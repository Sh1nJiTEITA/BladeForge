#ifndef BF_NEW_BLADESECTION2_H
#define BF_NEW_BLADESECTION2_H

#include <fmt/base.h>
#include <glm/geometric.hpp>
#include <glm/vector_relational.hpp>

#include "bfCurves4.h"
#include "bfDrawObject2.h"

namespace obj
{
namespace section
{

struct CenterCircle
{
   float relativePos;
   float radius;
};

struct SectionCreateInfo
{
   float chord = 1.0f;
   float installAngle = 80.0f;

   bool isEqMode = false;
   float inletAngle = 180.0f - 66.0f - 30.f;
   float outletAngle = 66.0f - 20.f;

   float inletRadius = 0.043f;
   float outletRadius = 0.017f;

   std::list< CenterCircle > centerCircles = {
       {0.214f, 0.063f}, {0.459f, 0.082f}, {0.853f, 0.025f}
   };
   int initialBezierCurveOrder = 5;
};

enum class BfBladeSectionEnum : uint32_t
{
   Chord = 0,
   _ChordLeftBorder,
   _ChordRightBorder,

   InletCircle,
   OutletCircle,

   InletDirection,
   OutletDirection,

   AverageInitialCurve,
   CenterCircles,

   InletPack,
   OutletPack,

   FrontCurveChain,
   BackCurveChain,
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
      _createIOAngles();
      _createAverageInitialCurve();
      _createCenterCircles();
      _createIOCircles();
      _createFrontCurves();
      // _createCCLines();
      // _createFrontIntersectionLines();
      // _createFrontCurves();
   }

   virtual void make() override
   {
      _assignRoots();
      
      if (!m_children.empty()) premake(); 
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
      _processIOAngles();
      _processAverageInitialCurve();
      _processCenterCircles();
      // _processCCLines();
      // _processFrontIntersectionLines();
   }

   virtual void postmake() 
   { 
      auto oChord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();
      m_lastChordL = oChord->left().get();
      m_lastChordR = oChord->right().get();
   }
   
   void toggleBack(int sts = -1) { 
      // const auto& IL = _part<BfBladeSectionEnum::IntersectionLines, obj::BfDrawLayer>()->children();
      // for (auto& child : IL ) { 
      //    child->toggleRender(sts);
      // }
   }

private:

   bool _isChordChanged();
   float _eqInletAngle();
   float _eqOutletAngle();
   glm::vec3 _eqInletDirection(); 
   glm::vec3 _eqOutletDirection(); 
   glm::vec3 _ioIntersection();

private:
   /** 
    * @defgroup Группа построения и конроля частей 2Д сечения
    * @{
    * 
    */

   /**
    * @brief Создание хорды
    */
   void _createChord();
   void _processChord();

   /**
    * @brief Создание входных и выходных окружностей (кромок)
    */
   void _createCircleEdges();
   void _processCircleEdges();

   /**
    * @brief Создание направляющих входных и выходных углов
    */
   void _createIOAngles();
   void _processIOAngles();

   /**
    * @brief Создание средней линии
    */
   void _createAverageInitialCurve();
   void _processAverageInitialCurve();

   /**
    * @brief Создание пакетов направляющих окружностей
    */
   void _createCenterCircles();
   void _processCenterCircles();

   /**
    * @brief Создание пакетов входных и выходных кромок
    */
   void _createIOCircles();
   void _processIOCircles();

   void _createFrontCurves();
   void _processFrontCurves();
   
   /** @} */ // End of MathFunctions group

private:
   BfVertex3 m_lastChordL; 
   BfVertex3 m_lastChordR; 

   BfVar<SectionCreateInfo> m_info;
};

}  // namespace section
}  // namespace obj

#endif
