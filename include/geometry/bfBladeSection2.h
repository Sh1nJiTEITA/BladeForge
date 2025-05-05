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

   uint32_t renderBitSet = INT32_MAX;
};

// clang-format off
enum class BfBladeSectionEnum : uint32_t
{
   Chord                = 1 << 1,
   _ChordLeftBorder     = 1 << 2,
   _ChordRightBorder    = 1 << 3,
   InletCircle          = 1 << 4,
   OutletCircle         = 1 << 5,
   InletDirection	= 1 << 6,
   OutletDirection	= 1 << 7,
   AverageInitialCurve	= 1 << 8,
   CenterCircles	= 1 << 9,
   InletPack	        = 1 << 10,
   OutletPack	        = 1 << 11,
   FrontCurveChain	= 1 << 12,
   BackCurveChain	= 1 << 13,
   
   CenterCircles2	= 1 << 14,

   End                  = 1 << 15
};
// clang-format on

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
      _createCenterCircles2();
      // FRONTIER 

      
      
      // _createCenterCircles();
      // _createIOCircles();
      // _createFrontCurves();
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
         // applyRenderToggle();
      }
      postmake();    
   }

   virtual void premake() { 
      // applyRenderToggle();
      _processChord(); 
      _processCircleEdges();
      _processIOAngles();
      _processAverageInitialCurve();
      // FRONTIER 

      // _processCenterCircles();
      // _processCCLines();
      // _processFrontIntersectionLines();
   }

   virtual void postmake() 
   { 
      auto oChord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();
      m_lastChordL = oChord->left().get();
      m_lastChordR = oChord->right().get();
   }

   

   // struct ToggleProxy { 
   //    ToggleProxy(BfBladeSection& bs) : m_bs{ bs } {}
   //    
   //    bool toggleBack(int sts = -1) { return m_bs._toggleRender<BfBladeSectionEnum::BackCurveChain>(); }
   //    bool toggleFront(int sts = -1) { return m_bs._toggleRender<BfBladeSectionEnum::FrontCurveChain>(); }
   //
   // private:
   //    BfBladeSection& m_bs;
   // };
   //
   // ToggleProxy toggle() { return ToggleProxy(*this); };

private:

   bool _isChordChanged();
   float _eqInletAngle();
   float _eqOutletAngle();
   glm::vec3 _eqInletDirection(); 
   glm::vec3 _eqOutletDirection(); 
   glm::vec3 _ioIntersection();

private:

   void applyRenderToggle() { 
      for (uint32_t v = static_cast<uint32_t>(BfBladeSectionEnum::Chord);
           v < static_cast<uint32_t>(BfBladeSectionEnum::End);
           v <<= 1) 
      {
          auto section = static_cast<BfBladeSectionEnum>(v);
          _toggleRender(section, m_info.get().renderBitSet & static_cast<uint32_t>(section));
      }
   };

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

   void _createCenterCircles2();
   void _processCenterCircles2();

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
