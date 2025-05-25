#ifndef BF_NEW_BLADESECTION2_H
#define BF_NEW_BLADESECTION2_H

#include <cmath>
#include <cstdint>
#include <fmt/base.h>
#include <glm/geometric.hpp>
#include <glm/vector_relational.hpp>
#include <memory>
#include <optional>

#include "bfBladeParts.h"
#include "bfChain.h"
#include "bfCirclePack.h"
#include "bfCurves4.h"
#include "bfDrawObject2.h"
#include "bfEdge.h"
#include "bfViewport.h"

namespace obj
{
namespace section
{

struct CenterCircle
{
   float relativePos;
   float radius;
   float backVertexAngle = 90.f;
   float frontVertexAngle = 90.f;
};

struct ImageData
{
   std::optional< fs::path > imagePath;
   float width = 0.5f;
   float height = 0.5f;
   float rotateAngle = 0.0f;
   float transparency = 0.0f;
   BfVertex3 tl;
   BfVertex3 tr;
   BfVertex3 br;
   BfVertex3 bl;
};

struct SectionCreateInfo
{
   //! Default ctor for initial section view
   //! FIXME: Change initial values to achieve more real section view
   SectionCreateInfo();

   //! Z position of section plane (assuming that plane lays in xOy plane)
   //! Can be in range of [0..1]
   float z;

   //! Chord - line between most far points of blade section
   float chord;

   //! Installation angle - angle to rotate from minus y-axis
   //! If this angle is 90.f degrees -> no rotation relative
   //! to formatting view
   float installAngle;

   //! Useless (for now) var to show absolute distance between
   //! 2 sections which are flow channel when combined
   float step;

   //! ...
   bool isEqMode;
   float inletAngle;
   float outletAngle;

   //! Radius of inlet/outlet (io) edges of section
   float inletRadius;
   float outletRadius;

   //! List of center circles to form chain
   std::list< CenterCircle > centerCircles;
   //! Order of initial curve (this curves is not the average
   //! curve between front/back curve chains!)
   int initialBezierCurveOrder;

   uint32_t renderBitSet;

   BfVertex3 vertChordLeft;
   BfVertex3 vertChordRight;

   //! Left & right chord borders (free vertices)
   BfVertex3 vertLeftChordBorder;
   BfVertex3 vertRightChordBorder;

   //! Control vertices of "average" "skelet" initial curve
   std::vector< BfVertex3 > initialCurveControlVertices;

   //! Back & front angles for inlet / outlet circle
   //! Which may one day be part of central circles
   float inletBackVertexAngle;
   float inletFrontVertexAngle;
   float outletBackVertexAngle;
   float outletFrontVertexAngle;

   ImageData imageData;
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
   InletEdge            = 1 << 10,
   OutletEdge           = 1 << 11,
   Chain                = 1 << 12,
   InletArc             = 1 << 13,
   OutletArc            = 1 << 14,
   OutputShape          = 1 << 15,
   MassCenter           = 1 << 16,
   TriangularShape      = 1 << 17,
   GeometricCenter      = 1 << 18,
   // TexturePlane         = 1 << 19,
   End                  = 1 << 19
};
// clang-format on

class BfBladeSection : public obj::BfDrawLayerWithAccess< BfBladeSectionEnum >
{
public:
   using V = BfVertex3;

   // clang-format off
   template <typename T>
   BfBladeSection(T&& info)
       : obj::BfDrawLayerWithAccess<BfBladeSectionEnum>("Blade section", BUFFER_LAYER)
       , m_info{std::forward<T>(info)}
   {

      m_info.get().renderBitSet &= ~static_cast< uint32_t >(BfBladeSectionEnum::TriangularShape);
      _createChord(); 
      _createCircleEdges();
      _createIOAngles();
      _createAverageInitialCurve();
      _createCenterCircles();
      _createIOCircles();
      _createChain();
      _createIOArc();
      _createTexturePlane();
   }

   virtual void make() override
   {
      _assignRoots();
      if (!m_children.empty()) premake(); 
      {
         int i = 0;
         for (auto& child : m_children) {
            child->make();
         }
      }
      postmake();    
   }

   virtual void premake() { 
      // applyRenderToggle();
      _processChord(); 
      _processCircleEdges();
      _processIOAngles();
      _processAverageInitialCurve();
      _processCenterCircles();
      _processChain();
   }

   virtual void postmake() 
   { 
      auto oChord = _part<BfBladeSectionEnum::Chord, curves::BfSingleLineWH>();
      m_lastChordL = oChord->left().get();
      m_lastChordR = oChord->right().get();

      _processIOArc();
      _processOutputShape();
      _processMassCenter();
      _processTriangularShape();
      _processGeometricCenter();
   }

public: // EXPORT

   BfVar< SectionCreateInfo >& info() { return m_info; }
   
   auto inletCircle() { return _part<E::InletEdge, curves::BfEdge>(); }
   auto outletCircle() { return _part<E::OutletEdge, curves::BfEdge>(); }
   auto chain() { return _part<E::Chain, curves::BfChain>(); }
   auto applyRenderToggle() -> void;
   auto toggleAllHandles(int sts = -1) -> void;
   auto genOutputShape() -> std::vector<BfVertex3>;
   auto outputShapeSegments() -> int& { return m_outputShapeSegments; }

   auto viewNone() -> void; 
   auto viewOutputShapeOnly(bool init = false) -> void; 
   auto viewFormattingShapeOnly(bool init = false) -> void; 
   auto viewRevert() -> void; 
   


   virtual void prerender(uint32_t elem) override;
   virtual void postrender(uint32_t elem) override;

   auto triangulate() -> std::vector< BfObj >;
   auto outputShape() -> std::shared_ptr< curves::BfSectionOutputShape >;

private:

   auto _isChordChanged() -> bool;
   auto _eqInletAngle() -> float;
   auto _eqOutletAngle() -> float;
   auto _eqInletDirection() -> glm::vec3; 
   auto _eqOutletDirection() -> glm::vec3; 
   auto _ioIntersection() -> glm::vec3;
   auto _calcGeometricCenter() -> glm::vec3;

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

   /**
    * @brief Создание цепочек (поверхностей спинки / корыта)
    */
   void _createChain();
   void _processChain();
   
   /** @} */ // End of MathFunctions group
   void _createIOArc();
   void _processIOArc();

   void _createOutputShape();
   void _processOutputShape();

   void _processMassCenter();
   void _processTriangularShape();
   void _processGeometricCenter();

   void _createTexturePlane();
   void _processTexturePlane();

private:
   int m_outputShapeSegments = std::nanf("");

   BfVertex3 m_lastChordL; 
   BfVertex3 m_lastChordR; 
   
   
   bool m_isRenderLast = false;
   uint32_t m_lastRenderBitSet = UINT32_MAX;

   BfVar<SectionCreateInfo> m_info;

};


}  // namespace section
}  // namespace obj


#endif
