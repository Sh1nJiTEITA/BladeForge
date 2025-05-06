#ifndef BF_CURVES4_H
#define BF_CURVES4_H

#include <algorithm>
#include <cmath>
#include <fmt/base.h>
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vector_relational.hpp>
#include <imgui.h>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "bfCamera.h"
#include "bfDescriptorStructs.h"
#include "bfDrawObject2.h"
#include "bfHandle.h"
#include "bfObjectMath.h"
#include "bfPipeline.h"
#include "bfVertex2.hpp"

namespace obj
{

namespace curves
{

class BfSingleLine : public obj::BfDrawObject
{
public:
   template < typename T, typename U >
   BfSingleLine(T&& fp, U&& sp)
       : obj::BfDrawObject{"Single line", BF_PIPELINE(BfPipelineType_Lines)}
       , m_first{std::forward< T >(fp)}
       , m_second{std::forward< U >(sp)}
   {
   }

   auto first() -> BfVertex3Uni&;
   auto second() -> BfVertex3Uni&;
   auto length() const -> const float;
   auto directionFromStart() const -> glm::vec3;
   auto directionFromEnd() const -> glm::vec3;
   void setPos(const glm::vec3& f, const glm::vec3& s);

   void make();

   // clang-format on
private:
   BfVertex3Uni m_first;
   BfVertex3Uni m_second;
};

class BfCircle2Vertices : public obj::BfDrawObject
{
public:
   template < typename T, typename U >
   BfCircle2Vertices(T&& center, U&& other)
       : obj::
             BfDrawObject{"Circle center", BF_PIPELINE(BfPipelineType_Lines), 200}
       , m_center{std::forward< T >(center)}
       , m_other{std::forward< U >(other)}
   {
   }

   BfVertex3& center();
   BfVertex3& other();

   const float radius() const noexcept;
   virtual void make() override;

private:
   BfVertex3Uni m_other;
   BfVertex3Uni m_center;
};

class BfCircleCenterFilled : public obj::BfDrawObject
{
public:
   // clang-format off
   template< typename U > 
   BfCircleCenterFilled(U&& center, float radius)
       : obj::BfDrawObject{"Circle center filled", 
                           BF_PIPELINE(BfPipelineType_Triangles), 200}
       , m_radius{radius}
       , m_center{std::forward<U>(center)}
   {
   }

   float radius() const noexcept;
   BfVertex3Uni& center();
   virtual void make() override;

protected:
   float m_radius;
   BfVertex3Uni m_center;
};

class BfHandleCircle : public BfCircleCenterFilled
                     , public virtual BfHandleBehavior
{
public:
   template <typename T>
   BfHandleCircle(T&& center, float radius)
       : BfCircleCenterFilled(std::forward<T>(center), radius)
   {
   }

   virtual void processDragging() override ;

   virtual void make() override;
   bool isChanged() const noexcept;
   void resetPos();

private:
   glm::vec3 m_initialMousePos;
   glm::vec3 m_initialCenterPos;
   bool m_isChanged = false;
};

class BfHandleRectangle : public BfDrawObject 
                        , public virtual BfHandleBehavior
{
public:
   template <typename T>
   BfHandleRectangle (T&& center, float side)
       : BfDrawObject ("Square handle", BF_PIPELINE(BfPipelineType_Triangles), 200)
       , m_center { std::forward<T>(center) }
       , m_side { side }
   {
   }

   virtual void processDragging() override;
   void move(const glm::vec3& v);
   virtual void make() override;
   BfVertex3Uni& center();
   bool isChanged() const noexcept;
   void resetPos();

private:
   glm::vec3 m_initialMousePos;
   glm::vec3 m_initialCenterPos;
   bool m_isChanged = false;
   BfVertex3Uni m_center;
   float m_side;
};



//
//
//
//
//
//
//
//
//
//
//
//
//
//

class BfBezierN : public obj::BfDrawObject, public std::vector<BfVertex3Uni>
{
public:
   template <typename... Args>
   BfBezierN(Args&&... args)
       : std::vector<BfVertex3Uni>{std::forward<Args>(args)...}
       , obj::BfDrawObject{
             "Bezier curve 2", BF_PIPELINE(BfPipelineType_Lines), 100
         }
   {
      if (this->size() < 3)
      {
         throw std::runtime_error(
             "Bezier curve with < 3 control points is not handled"
         );
      }
   }

   glm::vec3 calcNormal(float t) const;
   glm::vec3 calcTangent(float t) const;
   glm::vec3 calcDerivative(float t) const;
   float length() const ;
   BfVertex3 calc(float t) const;
   virtual void make() override;
};


class BfBezierWH : public obj::BfDrawLayer, public std::vector<BfVertex3Uni>
{
public:
   template <typename... Args>
   BfBezierWH(Args&&... args)
       : obj::BfDrawLayer("Bezier curve with handles")
       , std::vector<BfVertex3Uni>{std::forward<Args>(args)...}
   {
      // FIXME: СУПЕР ВРЕМЕННОЕ РЕШЕНИЕ, если при добавлении точек
      // внутренний вектор сделает ресайз -> все ручки, указывающие
      // на точки вектора уйдут в segmentation fault
      this->reserve(100);
      auto curve = std::make_shared<curves::BfBezierN>(
          _genControlVerticesPointers()
      );
      this->add(curve);

      auto handles_layer = std::make_shared<obj::BfDrawLayer>("Handles layer");
      this->add(handles_layer);


      for (auto& v : *curve) {
         handles_layer->add(std::make_shared<curves::BfHandleCircle>(v.getp(), 0.01f));
      }

      auto lines_layer = std::make_shared<obj::BfDrawLayer>("Lines layer");
      this->add(lines_layer);

      for (auto i = 0; i < this->size() - 1; ++i) { 
         
         auto line = std::make_shared<curves::BfSingleLine>(
            this->at(i).getp(),
            this->at(i + 1).getp()
         );

         line->color() = glm::vec3(1.0f, 1.0f, 1.0f);
         lines_layer->add(line);
      }
   }

   
   void elevateOrder();
   void lowerateOrder();
   bool toggleHandle(size_t i, int status = -1);
   bool toggleBoundHandles(int sts = -1);
   std::shared_ptr<curves::BfBezierN> curve();
   
private:
   std::vector<BfVertex3Uni> _genControlVerticesPointers();
};

class BfSingleLineWH : public obj::BfDrawLayer
{
public:
   template <typename T, typename U>
   BfSingleLineWH(T&& left, U&& right)
       : obj::BfDrawLayer("Single line with handles")
       , m_left{std::forward<T>(left)}
       , m_right{std::forward<U>(right)}
   {
      // clang-format off
      auto lHandle = std::make_shared< curves::BfHandleCircle >(
         m_left.getp(), 0.01f
      );
      this->add(lHandle);
      auto rHandle = std::make_shared< curves::BfHandleCircle >(
         m_right.getp(), 0.01f
      );
      this->add(rHandle);
      auto line = std::make_shared< curves::BfSingleLine >(
         m_left.getp(),
         m_right.getp()
      );
      this->add(line);

      // clang-format on
   }

   BfVertex3Uni& left();
   BfVertex3Uni& right();

   std::shared_ptr< curves::BfHandleCircle > leftHandle();
   std::shared_ptr< curves::BfHandleCircle > rightHandle();
   std::shared_ptr< curves::BfSingleLine > line();
   virtual bool isChanged() const noexcept;

private:
   BfVertex3Uni m_left;
   BfVertex3Uni m_right;
};

class BfCircleCenterWH : public obj::BfDrawLayer
{
public:
   template < typename T >
   BfCircleCenterWH(T&& init_center, float init_radius)
       : obj::BfDrawLayer("Circle with handles")
       , m_center{std::forward< T >(init_center)}
   {
      // clang-format off
      auto other_pos = m_center.pos() +
      glm::normalize(glm::vec3(-m_center.normals().z, 0.0f,
      m_center.normals().x)) * init_radius;

      m_other.pos() = other_pos;
      m_other.color() = m_center.color();
      m_other.normals() = m_center.normals();
      m_lastCenterPos = m_center.pos();

      auto circle = std::make_shared<curves::BfCircle2Vertices>(
          m_center.getp(),
          m_other.getp()
      );
      this->add(circle);

      auto center_handle = std::make_shared<curves::BfHandleCircle>(
         m_center.getp(),
         0.01f
      );
      this->add(center_handle);

      auto r_handle = std::make_shared<curves::BfHandleCircle>(
         m_other.getp(),
         0.01f
      );
      this->add(r_handle);

      m_previusOtherHandleDirection = glm::normalize(other().pos - center().pos);
      // clang-format on
   }

   float radius() const;
   void setRadius(float r);
   BfVertex3& center();
   BfVertex3& other();
   void setCenter(const BfVertex3& v);

   virtual void make() override;
   const glm::vec3& previousDirection() const noexcept;

   std::shared_ptr< BfHandleCircle > centerHandle();
   std::shared_ptr< BfHandleCircle > otherHandle();

   virtual bool isChanged() const noexcept;
   // clang-format on

private:
   BfVertex3Uni m_other;
   BfVertex3Uni m_center;
   glm::vec3 m_previusOtherHandleDirection;
   glm::vec3 m_lastCenterPos;
};

class BfCircle2Lines : public obj::BfDrawObject
{
public:
   template < typename V1, typename V2, typename V3, typename R >
   BfCircle2Lines(V1&& begin, V2&& center, V3&& end, R&& radius)
       : obj::BfDrawObject(
             "Circle 2 lines", BF_PIPELINE(BfPipelineType_Lines), 400
         )
       , m_begin{std::forward< V1 >(begin)}
       , m_center{std::forward< V2 >(center)}
       , m_end{std::forward< V3 >(end)}
       , m_radius{std::forward< R >(radius)}
   {
   }

   virtual void make() override;
   BfVertex3 center();

private:
   BfVertex3Uni m_begin;
   BfVertex3Uni m_center;
   BfVertex3Uni m_end;
   BfVar< float > m_radius;
};

class BfCircle2LinesWH : public obj::BfDrawLayer
{
public:
   template < typename V1, typename V2, typename V3, typename R >
   BfCircle2LinesWH(V1&& begin, V2&& center, V3&& end, R&& radius)
       : obj::BfDrawLayer("Circle 2 lines with handles")
       , m_begin{std::forward< V1 >(begin)}
       , m_center{std::forward< V2 >(center)}
       , m_end{std::forward< V3 >(end)}
       , m_radius{std::forward< R >(radius)}
   {
      auto circle = std::make_shared< curves::BfCircle2Lines >(
          m_begin.getp(),
          m_center.getp(),
          m_end.getp(),
          m_radius.getp()
      );
      this->add(circle);

      glm::vec3 circle_center = circle->center().pos;
      glm::vec3 direction_to_handle =
          glm::normalize(circle_center - m_center.pos());
      auto handle = std::make_shared< curves::BfHandleCircle >(
          BfVertex3(
              circle_center + direction_to_handle * m_radius.get(),
              m_center.color(),
              m_center.normals()
          ),
          0.01f
      );
      m_lastHandlePos = handle->center().pos();
      this->add(handle);
   }

   virtual void make() override;
   BfVertex3 centerVertex();
   std::shared_ptr< curves::BfCircle2Lines > circle();
   std::shared_ptr< curves::BfHandleCircle > handle();
   BfVar< float >& radius() noexcept;

private:
   glm::vec3 m_lastHandlePos;

   glm::vec3 m_oldbegin;
   glm::vec3 m_oldcenter;
   glm::vec3 m_oldend;

   BfVertex3Uni m_begin;
   BfVertex3Uni m_center;
   BfVertex3Uni m_end;
   BfVar< float > m_radius;
};

class BfCircleCenter : public obj::BfDrawObject
{
public:
   template < typename T, typename U >
   BfCircleCenter(T&& center, U&& radius)
       : obj::BfDrawObject(
             "Circle center new", BF_PIPELINE(BfPipelineType_Lines), 100
         )
       , m_center{std::forward< T >(center)}
       , m_radius{std::forward< U >(radius)}
   {
   }

   void make() override
   {
      m_indices.clear();
      m_vertices = std::move(
          math::calcCircleVertices(
              m_center.get(),
              m_radius.get(),
              m_discretization,
              m_color
          )
      );
      _genIndicesStandart();
   };

private:
   BfVertex3Uni m_center;
   BfVar< float > m_radius;
};

class BfCirclePack : public obj::BfDrawLayer
{
public:
   template < typename T >
   BfCirclePack(T&& t, T&& R, std::weak_ptr< BfDrawObjectBase > curve)
       : obj::BfDrawLayer("Circle pack")
       , m_relativePos{std::forward< T >(t)}
       , m_radius{std::forward< T >(R)}
       , m_curve{curve}
   {
      if (auto locked = m_curve.lock())
      {
         // TODO: ADD CHECK
         auto curve = std::static_pointer_cast< BfBezierN >(locked);
         auto center = curve->calc(m_relativePos.get());
         auto circle =
             std::make_shared< BfCircleCenterWH >(center, m_radius.get());
         this->add(circle);

         auto normal = curves::math::BfBezierBase::calcNormal(
             *curve,
             m_relativePos.get()
         );
         auto line = std::make_shared< curves::BfSingleLine >(
             BfVertex3{
                 circle->center().pos + normal * m_radius.get(),
                 glm::vec3(1.0f, 0.0f, 0.0f),
                 circle->center().normals
             },
             BfVertex3{
                 circle->center().pos - normal * m_radius.get(),
                 glm::vec3(0.0f, 1.0f, 0.0f),
                 circle->center().normals
             }
         );
         line->color() = glm::vec3(std::nanf(""));
         this->add(line);

         glm::vec3 perpDirFirst = _findPerpDirectionFirst();
         auto perpLineFirst = std::make_shared< curves::BfSingleLine >(
             BfVertex3{
                 line->first().pos() + perpDirFirst * m_radius.get(),
                 glm::vec3(1.0f),
                 circle->center().normals
             },
             BfVertex3{
                 line->first().pos() - perpDirFirst * m_radius.get(),
                 glm::vec3(1.0f),
                 circle->center().normals
             }
         );
         perpLineFirst->color() = glm::vec3{0.23, 0.45, 0.1};
         this->add(perpLineFirst);

         glm::vec3 perpDirSecond = _findPerpDirectionSecond();
         auto perpLineSecond = std::make_shared< curves::BfSingleLine >(
             BfVertex3{
                 line->second().pos() + perpDirSecond * m_radius.get(),
                 glm::vec3(1.0f),
                 circle->center().normals
             },
             BfVertex3{
                 line->second().pos() - perpDirSecond * m_radius.get(),
                 glm::vec3(1.0f),
                 circle->center().normals
             }
         );
         perpLineSecond->color() = glm::vec3{0.23, 0.45, 0.1};
         this->add(perpLineSecond);
      }
      else
      {
         throw std::runtime_error("Can't lock BfBezierN ...");
      }
   }

   std::shared_ptr< BfCircleCenterWH > circle();

   std::shared_ptr< BfSingleLine > normalLine();
   std::shared_ptr< BfSingleLine > perpLineFirst();
   std::shared_ptr< BfSingleLine > perpLineSecond();

   std::shared_ptr< BfBezierN > boundCurve();

   void bindNext(std::weak_ptr< obj::BfDrawObjectBase > next) noexcept;
   void bindPrevious(std::weak_ptr< obj::BfDrawObjectBase > pre) noexcept;

   bool hasLeft();
   bool hasRight();

   std::shared_ptr< BfCirclePack > next();
   std::shared_ptr< BfCirclePack > previous();

   virtual void make() override;

   BfVar< float >& relativePos() noexcept;
   BfVar< float >& radius() noexcept;

private:
   glm::vec3 _findPerpDirectionFirst();
   glm::vec3 _findPerpDirectionSecond();

   void _premakeIntersectionSecond();
   void _premakeIntersectionFirst();

   void _premakeIntersection();
   void _premakeNormalLine();

   void _premakeCircle();

private:
   std::weak_ptr< BfDrawObjectBase > m_previous;
   std::weak_ptr< BfDrawObjectBase > m_next;
   std::weak_ptr< BfDrawObjectBase > m_curve;

   BfVar< float > m_relativePos;
   BfVar< float > m_radius;
};

/**
 * @class BfIOCirclePack
 * @brief NOT CLEAN IMPLEMENTATION
 * TODO: REFRACTOR ALL
 *
 */
class BfIOCirclePack : public obj::BfDrawLayer
{
public:
   enum Type
   {
      Inlet,
      Outlet
   };

   BfIOCirclePack(
       std::weak_ptr< BfCircle2LinesWH > circle,
       std::weak_ptr< BfSingleLineWH > ioline,
       std::weak_ptr< obj::BfDrawLayer > center_circles,
       Type type
   )
       : obj::BfDrawLayer("IO circle")
       , m_circle{circle}
       , m_line{ioline}
       , m_centerCircles{center_circles}
       , m_type{type}
   {
      auto c = circle.lock();
      auto line = ioline.lock();
      auto cc = center_circles.lock();
      if (!c || !line || !cc)
      {
         throw std::runtime_error("Cant lock circle or line or center circles");
      }

      glm::vec3 normal_direction = glm::normalize(
          glm::cross(line->line()->directionFromStart(), line->left().normals())
      );

      // First
      BfVertex3 nV1 = c->centerVertex();
      nV1.pos = c->centerVertex().pos;
      nV1.color = glm::vec3(1.0f, 0.0f, 0.0f);

      // Second
      BfVertex3 nV2 = c->centerVertex();
      nV2.pos = c->centerVertex().pos + normal_direction * c->radius().get();
      nV2.color = glm::vec3(0.0f, 1.0f, 0.0f);

      if (m_type == Inlet)
      {
         auto nLine =
             std::make_shared< BfSingleLine >(std::move(nV1), std::move(nV2));
         nLine->color() = glm::vec3(std::nanf(""));
         this->add(nLine);
      }
      else if (m_type == Outlet)
      {
         auto nLine =
             std::make_shared< BfSingleLine >(std::move(nV2), std::move(nV1));
         nLine->color() = glm::vec3(std::nanf(""));
         this->add(nLine);
      }
      _addPerpLines();
      _addInterLines();
   }

   /**
    * @brief Returns IO line, specified by inlet/outlet angle
    *
    * @return line object
    */
   std::shared_ptr< obj::BfDrawLayer > centerCircles();

   /**
    * @brief Returns IO line, specified by inlet/outlet angle
    *
    * @return line object
    */
   std::shared_ptr< BfSingleLineWH > line();

   /**
    * @brief Returns IO circle (circular edge)
    *
    * @return object 2 lines object
    */
   std::shared_ptr< BfCircle2LinesWH > circle();

   /**
    * @brief Returns normal line, perpendicular to IO line
    *
    * @return line object
    */
   std::shared_ptr< BfSingleLine > normalLine();

   /**
    * @brief Returns parallel to normal line first line
    * (relative to first/second vertex of normal line)
    *
    * This line::first() -> is same is normalLine()::first()
    *
    * @return line object
    */
   std::shared_ptr< BfSingleLine > firstLine();

   /**
    * @brief Returns parallel to nornal line second line
    * (relative to first/second vertex of normal line)
    *
    * This line::first() -> is same is normalLine()::second()
    *
    * @return
    */
   std::shared_ptr< BfSingleLine > secondLine();

   /**
    * @brief Returns parallel to normal line first line
    * (relative to first/second vertex of normal line)
    *
    * This line::first() -> is same is normalLine()::first()
    *
    * @return line object
    */
   std::shared_ptr< BfSingleLine > firstLineIntr();

   /**
    * @brief Returns parallel to nornal line second line
    * (relative to first/second vertex of normal line)
    *
    * This line::first() -> is same is normalLine()::second()
    *
    * @return
    */
   std::shared_ptr< BfSingleLine > secondLineIntr();

   virtual void make() override;

private:
   void _addInterLines();
   void _addPerpLines();

   /**
    * @brief Finds near circle from center circles by this object type.
    * If type is INLET -> returns first central circle or circle with lowest
    * relative pos (t) -> 0.0f
    * If typy is OUTLET -> returns last central circle or circle with highest
    * relative pos (t) -> 1.0f
    */
   std::shared_ptr< BfCirclePack > _nearCircle();

   void _findIntersection(glm::vec3& fintr, glm::vec3& sintr);

private:
   Type m_type;
   std::weak_ptr< BfCircle2LinesWH > m_circle;
   std::weak_ptr< BfSingleLineWH > m_line;
   std::weak_ptr< obj::BfDrawLayer > m_centerCircles;
};

class BfBezierChain : public obj::BfDrawLayer
{
public:
   enum Type
   {
      Front,
      Back
   };

   BfBezierChain(
       Type type,
       std::weak_ptr< BfIOCirclePack > inlet_pack,
       std::weak_ptr< obj::BfDrawLayer > center_packs,
       std::weak_ptr< BfIOCirclePack > outlet_pack
   )
       : obj::BfDrawLayer{"Bezier chain"}
       , m_type{type}
       , m_ipack{inlet_pack}
       , m_centerPacks{center_packs}
       , m_opack{outlet_pack}
   {
      auto v = _controlPoints();

      auto bezier_chain = std::make_shared< BfDrawLayer >("Bezier chain");
      for (size_t i = 0; i < v.size() - 2; i += 2)
      {
         auto bez = std::make_shared< BfBezierN >(
             std::vector< BfVertex3Uni >{v[i], v[i + 1], v[i + 2]}
         );
         if (m_type == Front)
         {
            bez->color() = glm::vec3(1.0f, 0.0f, 0.0f);
         }
         else if (m_type == Back)
         {
            bez->color() = glm::vec3(0.0f, 1.0f, 0.0f);
         }
         bezier_chain->add(bez);
      }
      this->add(bezier_chain);
   }

   std::shared_ptr< BfIOCirclePack > inletPack();
   std::shared_ptr< obj::BfDrawLayer > centerPacks();
   std::shared_ptr< BfIOCirclePack > outletPack();
   std::shared_ptr< obj::BfDrawLayer > chain();
   void make() override;

private:
   std::vector< BfVertex3Uni > _controlPoints();

private:
   Type m_type;
   std::weak_ptr< BfIOCirclePack > m_ipack;
   std::weak_ptr< obj::BfDrawLayer > m_centerPacks;
   std::weak_ptr< BfIOCirclePack > m_opack;
};

class BfQuad : public obj::BfDrawObject
{
public:
   template < typename P1, typename P2, typename P3, typename P4 >
   BfQuad(P1&& tl, P2&& tr, P3&& br, P4&& bl)
       : obj::BfDrawObject(
             "Texture plane", BF_PIPELINE(BfPipelineType_LoadedImage), 10
         )
       , m_tl{std::forward< P1 >(tl)}
       , m_tr{std::forward< P2 >(tr)}
       , m_br{std::forward< P3 >(br)}
       , m_bl{std::forward< P4 >(bl)}
   {
   }

   void make() override
   {
      m_vertices.clear();
      m_indices.clear();
      m_vertices = {m_tl.get(), m_tr.get(), m_br.get(), m_bl.get()};
      m_indices = {0, 1, 2, 2, 0, 3};
   }

   BfVertex3Uni& tl() { return m_tl; }
   BfVertex3Uni& tr() { return m_tr; }
   BfVertex3Uni& br() { return m_br; }
   BfVertex3Uni& bl() { return m_bl; }

protected:
   BfVertex3Uni m_tl;
   BfVertex3Uni m_tr;
   BfVertex3Uni m_br;
   BfVertex3Uni m_bl;
};

class BfTextureQuad : public BfQuad
{
public:
   template < typename P1, typename P2, typename P3, typename P4 >
   BfTextureQuad(P1&& tl, P2&& tr, P3&& br, P4&& bl)
       : BfQuad{
             std::forward< P1 >(tl),
             std::forward< P2 >(tr),
             std::forward< P3 >(br),
             std::forward< P4 >(bl)
         }
   {
   }

   void presentContextMenu() override;
   bool isLocked() noexcept { return m_isLocked; }
   bool isRatio() noexcept { return m_isRatio; }

   void make() override
   {
      m_vertices.clear();
      m_indices.clear();

      glm::mat4 r = glm::rotate(
          glm::mat4(1.0f),
          glm::radians(m_rotateAngle),
          m_bl.normals()
      );
      glm::vec3 center = (m_tl.pos() + m_br.pos()) * 0.5f;

      auto rotate = [&](BfVertex3Uni& v) -> BfVertex3 {
         glm::vec4 local = glm::vec4(v.pos() - center, 1.0f);
         glm::vec3 rotated = glm::vec3(r * local) + center;
         return BfVertex3{rotated, v.color(), v.normals()};
      };

      m_vertices = {rotate(m_tl), rotate(m_tr), rotate(m_br), rotate(m_bl)};
      m_indices = {0, 1, 2, 2, 0, 3};
   }

   virtual BfObjectData _objectData() override
   {
      return {
          .model_matrix = m_modelMatrix,
          .select_color = glm::vec3(m_transp, 0.5f, 0.0f),
          .index = static_cast< uint32_t >(m_isLocked ? 1 : 0),
          .id = id(),
          .line_thickness = 0.00025f
      };
   }

private:
   float m_rotateAngle = 0.0f;
   float m_transp = 0.0f;
   bool m_isLocked = false;
   bool m_isRatio = false;
};

class BfTexturePlane : public obj::BfDrawLayer
{
public:
   BfTexturePlane(float w, float h)
       : obj::BfDrawLayer{"Texture plane"}
   {
      const auto n = glm::vec3{0.f, 0.f, 1.f};
      auto quad = std::make_shared< BfTextureQuad >(
          BfVertex3{glm::vec3(0, h, -.1f), glm::vec3(0.0f, 0.0f, 0.0f), n},
          BfVertex3{glm::vec3(w, h, -.1f), glm::vec3(1.0f, 0.0f, 0.0f), n},
          BfVertex3{glm::vec3(w, 0, -.1f), glm::vec3(1.0f, 1.0f, 0.0f), n},
          BfVertex3{glm::vec3(0, 0, -.1f), glm::vec3(0.0f, 1.0f, 0.0f), n}
      );
      this->add(std::make_shared< BfHandleCircle >(quad->tl().getp(), 0.01f));
      this->add(std::make_shared< BfHandleCircle >(quad->tr().getp(), 0.01f));
      this->add(std::make_shared< BfHandleCircle >(quad->br().getp(), 0.01f));
      this->add(
          std::make_shared< BfHandleRectangle >(quad->bl().getp(), 0.01f)
      );
      this->add(quad);
   }

   std::shared_ptr< BfHandleCircle > tlHandle()
   {
      return std::static_pointer_cast< BfHandleCircle >(m_children[0]);
   }
   std::shared_ptr< BfHandleCircle > trHandle()
   {
      return std::static_pointer_cast< BfHandleCircle >(m_children[1]);
   }
   std::shared_ptr< BfHandleCircle > brHandle()
   {
      return std::static_pointer_cast< BfHandleCircle >(m_children[2]);
   }
   std::shared_ptr< BfHandleRectangle > blHandle()
   {
      return std::static_pointer_cast< BfHandleRectangle >(m_children[3]);
   }
   std::shared_ptr< BfTextureQuad > quad()
   {
      return std::static_pointer_cast< BfTextureQuad >(m_children[4]);
   }

   void moveToCenter()
   {
      blHandle()->move({0, 0, blHandle()->center().pos().z});
   }

   void make() override
   {
      _assignRoots();

      auto bl = blHandle();
      auto br = brHandle();
      auto tr = trHandle();
      auto tl = tlHandle();
      bool isRatio = quad()->isRatio();
      bool isRatioChanged = isRatio != m_isRatioOld;

      const float ratio =
          base::desc::own::BfDescriptorPipelineDefault::getTextureDescriptor()
              .ratio();

      glm::vec3 blPos = bl->center().pos();

      if (bl->isChanged() || isRatioChanged)
      {
         auto dir = blPos - m_oldbl;
         br->center().pos() += dir;
         tr->center().pos() += dir;
         tl->center().pos() += dir;
      }

      if (br->isChanged())
      {
         glm::vec3 newBr = br->center().pos();

         if (!isRatio)
         {
            br->center().pos().y = blPos.y;
            tr->center().pos().x = br->center().pos().x;
         }
         else
         {
            br->center().pos().y = bl->center().pos().y;
            br->center().pos().x = tr->center().pos().x;
         }
      }

      if (tr->isChanged() || isRatioChanged)
      {
         glm::vec3 newTr = tr->center().pos();

         if (isRatio)
         {
            float width = newTr.x - blPos.x;
            float height = width / ratio;

            tr->center().pos().y = blPos.y + height;
            br->center().pos() =
                glm::vec3(newTr.x, blPos.y, br->center().pos().z);
            tl->center().pos() =
                glm::vec3(blPos.x, blPos.y + height, tl->center().pos().z);
         }
         else
         {
            br->center().pos().x = tr->center().pos().x;
            tl->center().pos().y = tr->center().pos().y;
         }
      }

      if (tl->isChanged() || isRatioChanged)
      {
         glm::vec3 newTl = tl->center().pos();

         if (!isRatio)
         {
            tl->center().pos().x = blPos.x;
            tr->center().pos().y = tl->center().pos().y;
         }
         else
         {
            tl->center().pos().y = tr->center().pos().y;
            tl->center().pos().x = bl->center().pos().x;
         }
      }

      obj::BfDrawLayer::make();

      m_oldtl = tl->center().pos();
      m_oldtr = tr->center().pos();
      m_oldbr = br->center().pos();
      m_oldbl = bl->center().pos();

      m_isRatioOld = isRatio;
   }

private:
   bool m_isRatioOld;

   glm::vec3 m_oldtl;
   glm::vec3 m_oldtr;
   glm::vec3 m_oldbr;
   glm::vec3 m_oldbl;
};

}; // namespace curves

} // namespace obj

#endif
