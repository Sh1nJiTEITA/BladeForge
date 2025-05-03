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

template < typename T >
decltype(auto)
processT(T&& t)
{
   using _T = std::decay_t< T >;
   if constexpr (std::is_pointer_v< _T >)
   {
      return *std::forward< T >(t);
   }
   else
   {
      return std::forward< T >(t);
   }
}

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

   // BfSingleLine(const T& fp, const T& sp)
   //     : obj::BfDrawObject{"Single line", BF_PIPELINE(BfPipelineType_Lines)}
   //     , m_first{fp}
   //     , m_second{sp}
   // {
   // }

   BfVertex3Uni& first() { return m_first; }
   BfVertex3Uni& second() { return m_second; }
   // const BfVertex3& first() const { return m_first.get(); }
   // const BfVertex3& second() const { return m_second.get(); }

   const float length() const
   {
      return glm::distance(m_first.pos(), m_second.pos());
   }

   glm::vec3 directionFromStart() const
   {
      return glm::normalize(m_second.pos() - m_first.pos());
   }

   glm::vec3 directionFromEnd() const { return -directionFromStart(); }

   void make()
   {
      m_vertices.clear();
      m_indices.clear();

      if (glm::any(glm::isnan(this->color())))
      {
         m_vertices = {m_first.get(), m_second.get()};
      }
      else
      {
         m_vertices = {
             {m_first.get().pos, this->color(), m_first.get().normals},
             {m_second.get().pos, this->color(), m_second.get().normals},
         };
      }
      _genIndicesStandart();
   };

   // clang-format on
private:
   BfVertex3Uni m_first;
   BfVertex3Uni m_second;
};

template < typename T, math::IsBfVertex3Variation< T > = true >
class BfCircleCenter : public obj::BfDrawObject
{
public:
   BfCircleCenter(T&& center, float radius)
       : obj::
             BfDrawObject{"Circle center", BF_PIPELINE(BfPipelineType_Lines), 200}
       , m_center{std::move(center)}
       , m_radius{radius}
   {
   }

   BfCircleCenter(const T& center, float radius)
       : obj::
             BfDrawObject{"Circle center", BF_PIPELINE(BfPipelineType_Lines), 200}
       , m_center{center}
       , m_radius{radius}
   {
   }

   const float radius() const noexcept { return m_radius; }

   BfVertex3& center() { return processT(m_center); }
   const BfVertex3& center() const { return processT(m_center); }

   virtual void make() override
   {
      m_indices.clear();
      m_vertices = std::move(
          math::calcCircleVertices(
              center(),
              m_radius,
              m_discretization,
              m_color
          )
      );
      _genIndicesStandart();
   }

private:
   float m_radius;
   T m_center;
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

   BfVertex3& center() { return m_center.get(); }
   const BfVertex3& center() const { return m_center.get(); }

   BfVertex3& other() { return m_other.get(); }
   const BfVertex3& other() const { return m_other.get(); }

   const float radius() const noexcept
   {
      return glm::distance(center().pos, other().pos);
   }

   virtual void make() override
   {
      m_indices.clear();
      m_vertices = std::move(
          math::calcCircleVertices(
              center(),
              radius(),
              m_discretization,
              m_color
          )
      );
      _genIndicesStandart();
   }

private:
   BfVertex3Uni m_other;
   BfVertex3Uni m_center;
};

// class Bfcircle3Vertices : public obj::BfDrawObject
// {
// public:
//    template < typename T, typename U, typename B >
//    Bfcircle3Vertices(T&& P_1, U&& P_2, B&& P_3)
//        : obj::
//              BfDrawObject{"Circle 3 vertices",
//              BF_PIPELINE(BfPipelineType_Lines), 200}
//        , m_first{std::forward< T >(P_1)}
//        , m_second{std::forward< U >(P_2)}
//        , m_third{std::forward< B >(P_3)}
//        , m_radius{std::nanf("")}
//        , m_center{BfVertex3::nan()}
//    {
//    }
//
//    const BfVertex3& first() const;
//    const BfVertex3& second() const;
//    const BfVertex3& third() const;
//    const float radius() const;
//    const BfVertex3& center() const;
//
//    void make() override;
//
// private:
//    float m_radius;
//    BfVertex3 m_center;
//    BfVertex3 m_first;
//    BfVertex3 m_second;
//    BfVertex3 m_third;
// };

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

   float radius() const noexcept { return m_radius; }
   BfVertex3& center() { return m_center.get(); }
   BfVertex3* centerp() { return m_center.getp(); }
   

   virtual void make() override
   {
      m_indices.clear();
      m_vertices = std::move(math::calcCircleVertices(
          center(),
          m_radius,
          m_discretization,
          m_color
      ));
      m_indices.reserve(m_vertices.size());
      for (size_t i = 1; i < m_vertices.size() - 2; ++i)
      {
         m_indices.emplace_back(0);
         m_indices.emplace_back(i);
         m_indices.emplace_back(i + 1);
      }
   }

   

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

   virtual void processDragging() override {
      BfHandleBehavior::processDragging();
      if (m_isPressed) { 
         m_initialCenterPos = center().pos;
      }
      if (m_isDown) { 
         center().pos = m_initialCenterPos + delta3D();
         m_isChanged = true;
         this->root()->make();
         this->root()->control().updateBuffer(true);
      }
   }

   virtual void make() override { 
      BfCircleCenterFilled::make();
      m_isChanged = false;
   }

   bool isChanged() const noexcept { return m_isChanged; }
   void resetPos() { m_initialCenterPos = m_center.pos(); }

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

   virtual void processDragging() override {
      BfHandleBehavior::processDragging();
      if (m_isPressed) { 
         m_initialCenterPos = center().pos();
      }
      if (m_isDown) { 
         center().pos() = m_initialCenterPos + delta3D();
         m_isChanged = true;
         this->root()->make();
         this->root()->control().updateBuffer(true);
      }
   }
   
   void move(const glm::vec3& v) { 
      m_center.pos() = v;
      m_isChanged = true;
   }

   virtual void make() override { 
      m_vertices.clear();
      m_indices.clear();
      auto& cp = m_center.pos();
      auto& cn = m_center.normals();
      float h = m_side;
      m_vertices = { 
         BfVertex3{ { cp.x - h, cp.y + h, cp.z }, color(), cn },
         BfVertex3{ { cp.x + h, cp.y + h, cp.z }, color(), cn },
         BfVertex3{ { cp.x + h, cp.y - h, cp.z }, color(), cn },
         BfVertex3{ { cp.x - h, cp.y - h, cp.z }, color(), cn },
      };
      m_indices = { 0, 1, 2, 2, 0, 3 };
      m_isChanged = false;
   }
   
   BfVertex3Uni& center() { return m_center; };

   bool isChanged() const noexcept { return m_isChanged; }
   void resetPos() { m_initialCenterPos = m_center.pos(); }

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

   glm::vec3 calcNormal(float t) const
   {
      return math::BfBezierBase::calcNormal(*this, t);
   }

   glm::vec3 calcTangent(float t) const
   {
      return math::BfBezierBase::calcTangent(*this, t);
   }

   glm::vec3 calcDerivative(float t) const
   {
      return math::BfBezierBase::calcDerivative(*this, t);
   }

   float length() const { return math::BfBezierBase::length(*this); }

   BfVertex3 calc(float t) const
   {
      auto v = math::BfBezierBase::calc(*this, t);
      v.color = m_color;
      return v;
   }

   virtual void make() override
   {
      m_vertices.clear();
      m_indices.clear();

      float t;
      m_vertices.reserve(m_discretization);
      for (int i = 0; i < m_discretization; i++)
      {
         t = static_cast<float>(i) / static_cast<float>(m_discretization - 1);
         auto v = this->calc(t);
         m_vertices.push_back(std::move(v));
      }
      m_indices.reserve(m_discretization);
      _genIndicesStandart();
   }
};

// class BfBezier : public obj::BfDrawObject, public std::vector<BfVertex3Uni>
// {
// public:
//    template <typename... Args>
//    BfBezier(Args&&... args)
//        : std::vector<BfVertex3>{args...}
//        , obj::BfDrawObject{
//              "Bezier curve", BF_PIPELINE(BfPipelineType_Lines), 400
//          }
//    {
//       if (this->size() < 3)
//       {
//          throw std::runtime_error(
//              "Bezier curve with < 3 control points is not handled"
//          );
//       }
//    }
//
//    glm::vec3 calcNormal(float t) const;
//    glm::vec3 calcTangent(float t) const;
//    glm::vec3 calcDerivative(float t) const;
//    float length() const;
//
//    BfVertex3 calc(float t) const;
//    virtual void make() override;
// };
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
//
//
//

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


      for (auto& v : *curve)
      {
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

   virtual void make() override
   {
      _assignRoots();
      for (auto child : m_children)
      {
         child->make();
      }
   }
   
   void elevateOrder() { 
      bool oldstate = toggleBoundHandles(true);
      auto handles_layer = std::static_pointer_cast<obj::BfDrawLayer>(m_children[1]);
      auto lines_layer = std::static_pointer_cast<obj::BfDrawLayer>(m_children[2]);
      auto new_vertices = math::BfBezierBase::elevateOrder(*this);

      for (size_t i = 0; i < new_vertices.size() - 1; ++i) 
      { 
         (*this)[i].pos() = new_vertices[i].pos();
         auto handle = std::static_pointer_cast<curves::BfHandleCircle>(handles_layer->children()[i]);
         handle->resetPos();
      }
      this->push_back(*new_vertices.rbegin());

      handles_layer->add(std::make_shared< curves::BfHandleCircle >(this->rbegin()->getp(), 0.01f));
      lines_layer->add(std::make_shared< curves::BfSingleLine >(this->rbegin()->getp(), (this->rbegin() + 1)->getp()));

      auto casted_curve = std::static_pointer_cast< curves::BfBezierN >(m_children[0]);
      auto new_pointers = this->_genControlVerticesPointers();
      casted_curve->assign(new_pointers.begin(), new_pointers.end());
      toggleBoundHandles(oldstate);
   }
   
   void lowerateOrder() { 
      bool oldstate = toggleBoundHandles(true);
      auto handles_layer = std::static_pointer_cast<obj::BfDrawLayer>(m_children[1]);
      auto lines_layer = std::static_pointer_cast<obj::BfDrawLayer>(m_children[2]);
      auto new_vertices = math::BfBezierBase::lowerateOrder(*this);

      handles_layer->children().pop_back();
      lines_layer->children().pop_back();
      this->pop_back();

      for (size_t i = 0; i < new_vertices.size(); ++i) 
      { 
         (*this)[i].pos() = new_vertices[i].pos();
         auto handle = std::static_pointer_cast<curves::BfHandleCircle>(handles_layer->children()[i]);
         handle->resetPos();
      }

      auto curve = std::static_pointer_cast< curves::BfBezierN >(m_children[0]);
      auto new_pointers = this->_genControlVerticesPointers();
      curve->assign(new_pointers.begin(), new_pointers.end());
      toggleBoundHandles(oldstate);
   }

   bool toggleHandle(size_t i, int status = -1){ 
      auto handles = std::static_pointer_cast<obj::BfDrawLayer>(m_children[1]);
      auto handle = std::static_pointer_cast<curves::BfHandleCircle>(handles->children()[i]);
      return handle->toggleRender(status);
   }

   bool toggleBoundHandles(int sts = -1) { 
      bool f =  toggleHandle(0, sts);
      bool s = toggleHandle(this->size() - 1, sts);
      return f && s;
   }

   std::shared_ptr<curves::BfBezierN> curve() { 
      return std::static_pointer_cast<curves::BfBezierN>(m_children[0]);
   }
   
private:
   std::vector<BfVertex3Uni> _genControlVerticesPointers()
   {
      std::vector<BfVertex3Uni> tmp;
      tmp.reserve(this->size());
      for (auto& it : *this)
      {
         tmp.push_back(BfVertex3Uni(it.getp()));
      }
      return tmp;
   }
};

// template <typename T, math::IsBfVertex3Variation<T> = true>
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

   BfVertex3Uni& left() { return m_left; }
   const BfVertex3Uni& left() const { return m_left; }
   BfVertex3Uni& right() { return m_right; }
   const BfVertex3Uni& right() const { return m_right; }

   std::shared_ptr< curves::BfHandleCircle > leftHandle()
   {
      return std::static_pointer_cast< curves::BfHandleCircle >(m_children[0]);
   }

   std::shared_ptr< curves::BfHandleCircle > rightHandle()
   {
      return std::static_pointer_cast< curves::BfHandleCircle >(m_children[1]);
   }

   std::shared_ptr< curves::BfSingleLine > line()
   {
      return std::static_pointer_cast< curves::BfSingleLine >(m_children[2]);
   }

   // clang-format off
   virtual bool isChanged() const noexcept { 
      return std::static_pointer_cast<curves::BfHandleCircle>(m_children[0])->isChanged() ||
             std::static_pointer_cast<curves::BfHandleCircle>(m_children[1])->isChanged();
   }
   // clang-format on

   virtual void make() override
   {
      _assignRoots();
      for (auto& child : m_children)
      {
         child->make();
      }
   }

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

   float radius() const { return glm::distance(m_other.pos(), m_center.pos()); }

   void setRadius(float r)
   {
      auto& dir = m_previusOtherHandleDirection;
      if (glm::all(
              glm::epsilonEqual(dir, glm::vec3{0.0f, 0.0f, 0.0f}, 0.001f)
          ) ||
          glm::any(glm::isnan(dir)) || glm::any(glm::isinf(dir)))
      {
         dir = glm::vec3{0.0001f, 0.0f, 0.0f};
      }
      other().pos = center().pos + dir * r;
   };

   BfVertex3& center() { return m_center.get(); }
   const BfVertex3& center() const { return m_center.get(); }

   BfVertex3& other() { return m_other.get(); }
   const BfVertex3& other() const { return m_other.get(); }

   void setCenter(const BfVertex3& v)
   {
      m_lastCenterPos = v.pos;
      m_center.get() = v;
   }

   virtual void make() override
   {
      _assignRoots();

      // TODO: Вообще, строго говоря эта часть нужна, если окружность
      // используется вне blade-section
      //
      // if changed CENTER
      // if (!glm::all(glm::equal(m_center.pos(), m_lastCenterPos)))
      // {
      //    m_other.pos() += -m_lastCenterPos + m_center.pos();
      //    m_lastCenterPos = m_center.pos();
      // }

      // if (glm::any(glm::notEqual(m_center.pos(), m_lastCenterPos)))

      for (auto child : m_children)
      {
         child->make();
      }
      // clang-format off
      m_previusOtherHandleDirection = glm::normalize(other().pos - center().pos);
      // clang-format on
      // otherHandle()->debug().printVertices();
   }

   const glm::vec3& previousDirection() const noexcept
   {
      return m_previusOtherHandleDirection;
   }

   // clang-format off
   std::shared_ptr< BfHandleCircle > centerHandle() {
      return std::static_pointer_cast< BfHandleCircle >(m_children[1]);
   }

   std::shared_ptr< BfHandleCircle > otherHandle() {
      return std::static_pointer_cast< BfHandleCircle >(m_children[2]);
   }

   

   virtual bool isChanged() const noexcept { 
      return std::static_pointer_cast<curves::BfHandleCircle>(m_children[1])->isChanged() ||
             std::static_pointer_cast<curves::BfHandleCircle>(m_children[2])->isChanged();
   }
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

   // clang-format off
   virtual void make() override
   {
      m_indices.clear();
      m_vertices = std::move(
          math::calcCircleVertices(
              center(),
              m_radius.get(),
              m_discretization,
              m_color
          )
      );
      _genIndicesStandart();
   }
   // clang-format on

   // clang-format off
   BfVertex3 center() 
   { 
      float alpha = curves::math::angleBetween3Vertices(m_begin, m_center, m_end);
      glm::vec3 direction_to_begin = glm::normalize(m_begin.pos() - m_center.pos());
      float distance_to_begin = m_radius.get() / glm::tan(glm::radians(alpha) * 0.5f);
      glm::vec3 vertex_to_center = m_center.pos() + direction_to_begin * distance_to_begin;
      glm::vec3 direction_to_center = (
         glm::rotate(glm::mat4(1.0f), -glm::radians(90.0f), m_center.normals()) * glm::vec4(direction_to_begin, 1.0f)
      );
      glm::vec3 res = vertex_to_center + direction_to_center * m_radius.get();
      return BfVertex3(res, m_center.color(), m_center.normals());
   }

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
       : obj::BfDrawLayer( "Circle 2 lines with handles")
       , m_begin{std::forward< V1 >(begin)}
       , m_center{std::forward< V2 >(center)}
       , m_end{std::forward< V3 >(end)}
       , m_radius{std::forward< R >(radius)}
   {
      auto circle = std::make_shared<curves::BfCircle2Lines>(
         m_begin.getp(),
         m_center.getp(),
         m_end.getp(),
         m_radius.getp()
      );
      this->add(circle);
      
      glm::vec3 circle_center = circle->center().pos;
      glm::vec3 direction_to_handle = glm::normalize(circle_center - m_center.pos());
      auto handle = std::make_shared<curves::BfHandleCircle>(
         BfVertex3( 
            circle_center + direction_to_handle * m_radius.get(),
            m_center.color(),
            m_center.normals()
         ),
         0.01f
      );
      m_lastHandlePos = handle->center().pos;
      this->add(handle);
   }

   // clang-format off
   virtual void make() override
   {
      _assignRoots();

      glm::vec3 handle_pos = handle()->center().pos;
      float distance_to_center_vertex = glm::distance(handle_pos, m_center.pos());
      float distance_to_line = curves::math::distanceToLine(handle_pos, m_begin, m_center);

      if (glm::all(glm::equal(m_begin.pos(), m_oldbegin)) && 
          glm::all(glm::equal(m_center.pos(), m_oldcenter)) &&
          glm::all(glm::equal(m_end.pos(), m_oldend))) { 

         m_radius.get() = (distance_to_center_vertex * distance_to_line) / (distance_to_center_vertex + distance_to_line);
      }
      
      // accurate handle pos
      float alpha = curves::math::angleBetween3Vertices(m_begin, m_center, m_end);
      glm::vec3 alpha_dir = glm::rotate(glm::mat4(1.0f), -glm::radians(alpha * 0.5f), m_center.normals()) * 
                            glm::vec4(glm::normalize(m_center.pos() - m_begin.pos()), 1.0f);

      handle()->center().pos = curves::math::closestPointOnLine(handle_pos, m_center, m_center.pos() + alpha_dir);

      // FIXME Ручка создается 2 раза тут и еще ниже 
      for (auto child : m_children)
      {
         child->make();
      }
      glm::vec3 circle_center = circle()->center().pos;
      glm::vec3 direction_to_handle = glm::normalize(circle_center - m_center.pos());
      handle()->center().pos = circle()->center().pos + direction_to_handle * m_radius.get();
      handle()->make();

      m_oldbegin = m_begin.pos();
      m_oldcenter = m_center.pos();
      m_oldend = m_end.pos();

   }
   // clang-format on

   // clang-format off
   BfVertex3 centerVertex() 
   { 
      return circle()->center();
   }

   std::shared_ptr<curves::BfCircle2Lines> circle()  
   {
      return std::static_pointer_cast<curves::BfCircle2Lines>(m_children[0]);
   }

   std::shared_ptr<curves::BfHandleCircle> handle()  
   {
      return std::static_pointer_cast<curves::BfHandleCircle>(m_children[1]);
   }
   
   BfVar<float>& radius() noexcept { 
      return m_radius;
   }

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


// with guide lines
class BfBezierWGL : public obj::BfDrawLayer
{ 
public: 
   // template< typename V1, typename V2
   // BfBezierWGL 

};


class BfCirclePack : public obj::BfDrawLayer { 
public:
   template < typename T >
   BfCirclePack(T&& t, T&& R, std::weak_ptr<BfDrawObjectBase> curve)  
      : obj::BfDrawLayer("Circle pack")
      , m_relativePos{ std::forward<T>(t) }
      , m_radius{ std::forward<T>(R) }
      , m_curve{ curve } 
   { 
      if (auto locked = m_curve.lock()) { 
         // TODO: ADD CHECK
         auto curve = std::static_pointer_cast<BfBezierN>(locked);
         auto center = curve->calc(m_relativePos.get());
         auto circle = std::make_shared<BfCircleCenterWH>(center, m_radius.get());
         this->add(circle);

         auto normal = curves::math::BfBezierBase::calcNormal(*curve, m_relativePos.get());
         auto line = std::make_shared<curves::BfSingleLine>(
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
         line->color() = glm::vec3( std::nanf("") );
         this->add(line);
         
         glm::vec3 perpDirFirst = _findPerpDirectionFirst();
         auto perpLineFirst = std::make_shared<curves::BfSingleLine>(
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
         perpLineFirst->color() = glm::vec3{ 0.23, 0.45, 0.1 };
         this->add(perpLineFirst);

         glm::vec3 perpDirSecond = _findPerpDirectionSecond();
         auto perpLineSecond = std::make_shared<curves::BfSingleLine>(
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
         perpLineSecond->color() = glm::vec3{ 0.23, 0.45, 0.1 };
         this->add(perpLineSecond);
      }
      else { 
         throw std::runtime_error("Can't lock BfBezierN ...");
      }
   }

   std::shared_ptr< BfCircleCenterWH > circle() { 
      return std::static_pointer_cast<BfCircleCenterWH>(m_children[0]);
   }
   
   std::shared_ptr< BfSingleLine > normalLine() { 
      return std::static_pointer_cast<BfSingleLine>(m_children[1]);
   }

   std::shared_ptr< BfSingleLine > perpLineFirst() { 
      return std::static_pointer_cast<BfSingleLine>(m_children[2]);
   }

   std::shared_ptr< BfSingleLine > perpLineSecond() { 
      return std::static_pointer_cast<BfSingleLine>(m_children[3]);
   }

   std::shared_ptr< BfBezierN > boundCurve() { 
      auto initCurve = m_curve.lock();
      if (!initCurve) { throw std::runtime_error("Can't lock BfBezierN ..."); }
      return std::static_pointer_cast<BfBezierN>(initCurve);
   }

   void bindNext(std::weak_ptr<obj::BfDrawObjectBase> next) noexcept { 
      m_next = next;
   }

   void bindPrevious(std::weak_ptr<obj::BfDrawObjectBase> pre) noexcept { 
      m_previous = pre;
   }

   bool hasLeft() { return !m_previous.expired(); }
   bool hasRight() { return !m_next.expired(); }

   std::shared_ptr< BfCirclePack > next() { 
      auto snext = m_next.lock();
      if (!snext) { throw std::runtime_error("Cant lock next circle pack"); }
      return std::static_pointer_cast< BfCirclePack >(snext);
   }

   std::shared_ptr< BfCirclePack > previous() { 
      auto snext = m_next.lock();
      if (!snext) { throw std::runtime_error("Cant lock next circle pack"); }
      return std::static_pointer_cast< BfCirclePack >(snext);
   }

   virtual void make() override { 
      _premakeCircle();
      _premakeNormalLine();
      _premakeIntersection();
      obj::BfDrawLayer::make();
   }

   BfVar<float>& relativePos() noexcept { 
      return m_relativePos;
   }
   
   BfVar<float>& radius() noexcept { 
      return m_radius;
   }
   
private:
   glm::vec3 _findPerpDirectionFirst()  {
      const auto current_normal_line = normalLine();
      const glm::vec3 from_start = current_normal_line->directionFromStart();
      const glm::vec3 normal_start = current_normal_line->first().normals();
      return glm::normalize(glm::cross(from_start, normal_start)) ;
   }
   glm::vec3 _findPerpDirectionSecond()  {
      const auto current_normal_line = normalLine();
      const glm::vec3 from_end = current_normal_line->directionFromEnd();
      const glm::vec3 normal_end = current_normal_line->first().normals();
      return glm::normalize(glm::cross(from_end, normal_end)) ;
   }


   void _premakeIntersectionSecond() { 
      const auto current_normal_line = normalLine();
      auto current_perp_line = perpLineSecond();
      glm::vec3 current_perp_dir = _findPerpDirectionSecond();
      if (hasRight()) { 
         auto right = next();
         auto next_normal_line = right->normalLine();
         auto next_perp_dir = right->_findPerpDirectionSecond();
         glm::vec3 intersection = curves::math::findLinesIntersection(
            current_normal_line->second().pos() + current_perp_dir,
            current_normal_line->second().pos() - current_perp_dir,
            next_normal_line->second().pos() + next_perp_dir,
            next_normal_line->second().pos() - next_perp_dir,
            BF_MATH_FIND_LINES_INTERSECTION_ANY
         );   
         current_perp_line->second().pos() = intersection;
         right->perpLineSecond()->first().pos() = intersection;
      }
      if (!hasLeft()) { 
         current_perp_line->first().pos() = current_normal_line->second().pos();  
      }
      if (hasLeft() && !hasRight()) { 
         current_perp_line->second().pos() = current_normal_line->second().pos();  
      }
   }

   void _premakeIntersectionFirst() { 
      const auto current_normal_line = normalLine();
      auto current_perp_line = perpLineFirst();
      glm::vec3 current_perp_dir = _findPerpDirectionFirst();
      if (hasRight()) { 
         auto right = next();
         auto next_normal_line = right->normalLine();
         auto next_perp_dir = right->_findPerpDirectionFirst();
         glm::vec3 intersection = curves::math::findLinesIntersection(
            current_normal_line->first().pos() + current_perp_dir,
            current_normal_line->first().pos() - current_perp_dir,
            next_normal_line->first().pos() + next_perp_dir,
            next_normal_line->first().pos() - next_perp_dir,
            BF_MATH_FIND_LINES_INTERSECTION_ANY
         );   
         current_perp_line->second().pos() = intersection;
         right->perpLineFirst()->first().pos() = intersection;
      }
      if (!hasLeft()) { 
         current_perp_line->first().pos() = current_normal_line->first().pos();  
      }
      if (hasLeft() && !hasRight()) { 
         current_perp_line->second().pos() = current_normal_line->first().pos();  
      }
   }

   void _premakeIntersection() { 
      _premakeIntersectionFirst();
      _premakeIntersectionSecond();
   }

   void _premakeNormalLine() { 
      auto curve = boundCurve(); 
      auto c = circle();
      auto line = normalLine();
      auto normal = curves::math::BfBezierBase::calcNormal(*curve, m_relativePos.get());
      line->first().pos() = c->center().pos + normal * m_radius.get();
      line->second().pos() = c->center().pos - normal * m_radius.get();
   };

   void _premakeCircle() { 
      auto c = circle();    
      auto casted = boundCurve();

      if (c->centerHandle()->isChanged()) { 
         auto info_pos = casted->calc(m_relativePos.get()).pos;
         auto actual_pos = c->center().pos;

         if (glm::any(glm::notEqual(info_pos, actual_pos))) { 
            float close_t = curves::math::BfBezierBase::findClosest(*casted, c->center());
            m_relativePos.get() = close_t;
         }
      }
      c->center().pos = casted->calc(m_relativePos.get()).pos;
      if (c->otherHandle()->isChanged()) {
         m_radius.get() = c->radius();
      }
      else { 
         c->setRadius(m_radius.get());
      }
   }


private:
   std::weak_ptr<BfDrawObjectBase> m_previous;
   std::weak_ptr<BfDrawObjectBase> m_next;

   std::weak_ptr<BfDrawObjectBase> m_curve;

   BfVar<float> m_relativePos;
   BfVar<float> m_radius;
};

/**
 * @class BfIOCirclePack
 * @brief NOT CLEAN IMPLEMENTATION
 * TODO: REFRACTOR ALL
 *
 */
class BfIOCirclePack : public obj::BfDrawLayer { 
public: 
   enum Type { 
      Inlet,
      Outlet
   };

   BfIOCirclePack(std::weak_ptr<BfCircle2LinesWH> circle,
                  std::weak_ptr<BfSingleLineWH> ioline,
                  std::weak_ptr<obj::BfDrawLayer> center_circles,
                  Type type) 
      : obj::BfDrawLayer("IO circle")
      , m_circle{ circle }
      , m_line { ioline }
      , m_centerCircles { center_circles }
      , m_type { type } 
   {
      auto c = circle.lock();
      auto line = ioline.lock();
      auto cc = center_circles.lock();
      if (!c || !line || !cc) { throw std::runtime_error("Cant lock circle or line or center circles"); }

      glm::vec3 normal_direction = glm::normalize(glm::cross(
         line->line()->directionFromStart(), 
         line->left().normals()
      ));
   
      // First 
      BfVertex3 nV1 = c->centerVertex();
      nV1.pos =  c->centerVertex().pos;
      nV1.color = glm::vec3(1.0f, 0.0f, 0.0f);

      // Second
      BfVertex3 nV2 = c->centerVertex();
      nV2.pos =  c->centerVertex().pos + normal_direction * c->radius().get();
      nV2.color = glm::vec3(0.0f, 1.0f, 0.0f);

      if (m_type == Inlet) {
         auto nLine = std::make_shared<BfSingleLine>(std::move(nV1), std::move(nV2));
         nLine->color() = glm::vec3( std::nanf("") ); 
         this->add(nLine);
      } else if (m_type == Outlet) { 
         auto nLine = std::make_shared<BfSingleLine>(std::move(nV2), std::move(nV1));
         nLine->color() = glm::vec3( std::nanf("") ); 
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
   std::shared_ptr< obj::BfDrawLayer > centerCircles() { 
      auto locked = m_centerCircles.lock();
      if (!locked) throw std::runtime_error("Cant lock central circles");
      return std::static_pointer_cast< obj::BfDrawLayer >(locked); 
   }

   /**
    * @brief Returns IO line, specified by inlet/outlet angle
    *
    * @return line object
    */
   std::shared_ptr< BfSingleLineWH > line() { 
      auto locked = m_line.lock();
      if (!locked) throw std::runtime_error("Cant lock IO line");
      return std::static_pointer_cast<BfSingleLineWH>(locked); 
   }

   /**
    * @brief Returns IO circle (circular edge)
    *
    * @return object 2 lines object 
    */
   std::shared_ptr< BfCircle2LinesWH > circle() { 
      auto locked = m_circle.lock();
      if (!locked) throw std::runtime_error("Cant lock IO circle");
      return std::static_pointer_cast< BfCircle2LinesWH >(locked); 
   }

   /**
    * @brief Returns normal line, perpendicular to IO line
    *
    * @return line object
    */
   std::shared_ptr< BfSingleLine > normalLine() { 
      return std::static_pointer_cast<BfSingleLine>(m_children[0]); 
   }

   /**
    * @brief Returns parallel to normal line first line 
    * (relative to first/second vertex of normal line)
    * 
    * This line::first() -> is same is normalLine()::first()
    *
    * @return line object
    */
   std::shared_ptr< BfSingleLine > firstLine() { 
      return std::static_pointer_cast<BfSingleLine>(m_children[1]); 
   }

   /**
    * @brief Returns parallel to nornal line second line 
    * (relative to first/second vertex of normal line) 
    *
    * This line::first() -> is same is normalLine()::second()
    *
    * @return 
    */
   std::shared_ptr< BfSingleLine > secondLine() { 
      return std::static_pointer_cast<BfSingleLine>(m_children[2]); 
   }

   /**
    * @brief Returns parallel to normal line first line 
    * (relative to first/second vertex of normal line)
    * 
    * This line::first() -> is same is normalLine()::first()
    *
    * @return line object
    */
   std::shared_ptr< BfSingleLine > firstLineIntr() { 
      return std::static_pointer_cast<BfSingleLine>(m_children[3]); 
   }

   /**
    * @brief Returns parallel to nornal line second line 
    * (relative to first/second vertex of normal line) 
    *
    * This line::first() -> is same is normalLine()::second()
    *
    * @return 
    */
   std::shared_ptr< BfSingleLine > secondLineIntr() { 
      return std::static_pointer_cast<BfSingleLine>(m_children[4]); 
   }

   virtual void make() override { 
      auto c = circle();
      auto line = this->line();

      glm::vec3 normal_direction = glm::normalize(glm::cross(
         line->line()->directionFromStart(), 
         line->left().normals()
      ));

      auto nLine = normalLine();
      nLine->first().pos() = c->centerVertex().pos - normal_direction * c->radius().get();
      nLine->second().pos() = c->centerVertex().pos + normal_direction * c->radius().get();

      glm::vec3 fintr, sintr; _findIntersection(fintr, sintr);
      firstLine()->second().pos() = fintr;
      secondLine()->second().pos() = sintr;
      
      if (m_type == Inlet) { 
         auto next = _nearCircle();
         firstLineIntr()->second() = BfVertex3Uni(next->perpLineFirst()->first().getp());
         secondLineIntr()->second() = BfVertex3Uni(next->perpLineSecond()->first().getp());
      } else if (m_type == Outlet) { 
         auto prev = _nearCircle();
         firstLineIntr()->second() = BfVertex3Uni(prev->perpLineSecond()->second().getp());
         secondLineIntr()->second() = BfVertex3Uni(prev->perpLineFirst()->second().getp());
      }
      obj::BfDrawLayer::make();
   }

private:
   inline void _addInterLines() { 
      auto first = this->firstLine();
      auto second = this->secondLine();
   
      BfVertex3 fintrv = first->second();
      fintrv.pos = first->second().pos() + 
                   first->directionFromStart() * first->length();
      auto first_inter = std::make_shared<BfSingleLine>(
         BfVertex3Uni(first->second().getp()),
         std::move(fintrv)
      );
      this->add(first_inter);


      BfVertex3 sintrv = second->second();
      sintrv.pos = second->second().pos() + 
                   second->directionFromStart() * second->length();
      auto second_intr = std::make_shared<BfSingleLine>(
         BfVertex3Uni(second->second().getp()),
         std::move(sintrv)
      );
      this->add(second_intr);
   }

   inline void _addPerpLines() { 
      auto nLine = this->normalLine();
      auto line = this->line();
      auto next = this->_nearCircle();
      auto c = this->circle();
      auto first = next->perpLineFirst();
      auto second = next->perpLineSecond();

      BfVertex3 first_inter = nLine->first();
      first_inter.pos = nLine->first().pos() + 
                        line->line()->directionFromEnd() * c->radius().get();

      auto first_line = std::make_shared<BfSingleLine>(
         BfVertex3Uni(nLine->first().getp()),
         std::move(first_inter)
      );
      this->add(first_line);

      BfVertex3 second_inter = nLine->first();
      second_inter.pos = nLine->second().pos() + 
                         line->line()->directionFromEnd() * c->radius().get();

      auto second_line = std::make_shared<BfSingleLine>(
         BfVertex3Uni(nLine->second().getp()),
         std::move(second_inter)
      );
      this->add(second_line);
   }

   /**
    * @brief Finds near circle from center circles by this object type.
    * If type is INLET -> returns first central circle or circle with lowest 
    * relative pos (t) -> 0.0f
    * If typy is OUTLET -> returns last central circle or circle with highest 
    * relative pos (t) -> 1.0f
    */
   std::shared_ptr<BfCirclePack> _nearCircle() { 
      auto nLine = this->normalLine();
      auto cc = this->centerCircles();
      auto line = this->line();
      if (m_type == Inlet) { 
         auto it_next = std::min_element(cc->children().begin(),
                                         cc->children().end(),
                                         [](const auto& lhs, const auto& rhs) { 
                                             auto l = std::static_pointer_cast<BfCirclePack>(lhs);
                                             auto r = std::static_pointer_cast<BfCirclePack>(rhs);
                                             return l->relativePos().get() < r->relativePos().get();
                                         });
         if (it_next == cc->children().end()) { throw std::runtime_error("Cant find most left circle pack"); }
         return std::static_pointer_cast<BfCirclePack>(*it_next);
      }
      else if (m_type == Outlet) { 
         auto it_next = std::max_element(cc->children().begin(),
                                         cc->children().end(),
                                         [](const auto& lhs, const auto& rhs) { 
                                             auto l = std::static_pointer_cast<BfCirclePack>(lhs);
                                             auto r = std::static_pointer_cast<BfCirclePack>(rhs);
                                             return l->relativePos().get() < r->relativePos().get();
                                         });
         if (it_next == cc->children().end()) { throw std::runtime_error("Cant find right left circle pack"); }
         return std::static_pointer_cast<BfCirclePack>(*it_next);
      }
      else { 
         throw std::runtime_error("Unhandled BfIOcirclePack");
      }
   }

   void _findIntersection(glm::vec3& fintr, glm::vec3& sintr) { 
      auto nLine = this->normalLine();
      auto cc = this->centerCircles();
      auto line = this->line();

      if (m_type == Inlet) { 
         auto next = _nearCircle();
         auto first = next->perpLineFirst();
         fintr = curves::math::findLinesIntersection(
            first->first(),
            first->second(),
            nLine->first(),
            nLine->first().pos() + line->line()->directionFromStart(),
            BF_MATH_FIND_LINES_INTERSECTION_ANY
         );

         auto second = next->perpLineSecond();
         sintr = curves::math::findLinesIntersection(
            second->first(),
            second->second(),
            nLine->second(),
            nLine->second().pos() + line->line()->directionFromStart(),
            BF_MATH_FIND_LINES_INTERSECTION_ANY
         );

      } else if (m_type == Outlet) { 
         auto prev = _nearCircle();
         auto first = prev->perpLineSecond();
         fintr = curves::math::findLinesIntersection(
            first->first(),
            first->second(),
            nLine->first(),
            nLine->first().pos() + line->line()->directionFromStart(),
            BF_MATH_FIND_LINES_INTERSECTION_ANY
         );

         auto second = prev->perpLineFirst();
         sintr = curves::math::findLinesIntersection(
            second->first(),
            second->second(),
            nLine->second(),
            nLine->second().pos() + line->line()->directionFromStart(),
            BF_MATH_FIND_LINES_INTERSECTION_ANY
         );
      }
   }

private:
   Type m_type;
   std::weak_ptr<BfCircle2LinesWH> m_circle;
   std::weak_ptr<BfSingleLineWH> m_line;
   std::weak_ptr<obj::BfDrawLayer> m_centerCircles;
};



class BfBezierChain : public obj::BfDrawLayer { 
public:
   enum Type { Front, Back };

   BfBezierChain(
      Type type,
      std::weak_ptr< BfIOCirclePack > inlet_pack,
      std::weak_ptr< obj::BfDrawLayer > center_packs,
      std::weak_ptr< BfIOCirclePack > outlet_pack
   ) 
      : obj::BfDrawLayer{ "Bezier chain" }
      , m_type{ type }
      , m_ipack{ inlet_pack }
      , m_centerPacks { center_packs }
      , m_opack{ outlet_pack }
   { 
      auto v = _controlPoints();
      
      auto bezier_chain = std::make_shared<BfDrawLayer>("Bezier chain");
      for (size_t i = 0; i < v.size() - 2; i += 2) { 
         auto bez = std::make_shared<BfBezierN>(
            std::vector< BfVertex3Uni > { 
               v[i],
               v[i + 1],
               v[i + 2]
            }
         );
         if (m_type == Front) { 
               bez->color() = glm::vec3(1.0f, 0.0f, 0.0f);   
         } else if (m_type == Back) { 
               bez->color() = glm::vec3(0.0f, 1.0f, 0.0f);   
         }
         bezier_chain->add(bez);
      }
      this->add(bezier_chain);
   }

   std::shared_ptr< BfIOCirclePack > inletPack() { 
      auto locked = m_ipack.lock();
      if (!locked) throw std::runtime_error("Cant lock inlet pack");
      return locked;
   }

   std::shared_ptr< obj::BfDrawLayer > centerPacks() { 
      auto locked = m_centerPacks.lock();
      if (!locked) throw std::runtime_error("Cant lock center packs");
      return locked;
   }
 
   std::shared_ptr< BfIOCirclePack > outletPack() { 
      auto locked = m_opack.lock();
      if (!locked) throw std::runtime_error("Cant lock outlet pack");
      return locked;
   }

   std::shared_ptr< obj::BfDrawLayer  > chain() { 
      return std::static_pointer_cast< obj::BfDrawLayer >(m_children[0]);
   }
   
   void make() override {
      auto v = _controlPoints();

      // Check if we have enough points to form bezier segments.
      if (v.size() < 2)
      {
         return; // or handle the case where there aren't enough points.
      }

      auto bezier_chain = this->chain()->children().begin();

      // Loop through the control points in pairs.
      for (size_t i = 0; i < v.size() - 1; i += 2)
      {
         if (bezier_chain == this->chain()->children().end())
         {
            break; // Prevent accessing past the end of the collection.
         }

         // Ensure we are dereferencing a valid shared pointer.
         auto bez = std::static_pointer_cast< BfBezierN >(*bezier_chain);
         bez->at(0) = v[i]; 
         bez->at(1) = v[i + 1]; 
         bez->at(2) = v[i + 2]; 

         // Move to the next bezier in the chain.
         ++bezier_chain;
      }
      BfDrawLayer::make();
   };

private:
   std::vector< BfVertex3Uni > _controlPoints()
   {
      auto ipack = inletPack();
      auto opack = outletPack();
      auto cpacks = centerPacks();

      using pack_t = std::shared_ptr< curves::BfCirclePack >;
      std::vector<pack_t> packs;
      std::transform(cpacks->children().begin(), 
                     cpacks->children().end(), 
                     std::back_inserter(packs),
                     [](std::shared_ptr< obj::BfDrawObjectBase >& o) { 
                        return std::static_pointer_cast<curves::BfCirclePack>(o);                  
                     });
   
      std::sort(packs.begin(), 
                packs.end(), 
                [](const pack_t& lhs, const pack_t& rhs) { 
                   return lhs->relativePos().get() < rhs->relativePos().get();
                });

      std::vector< BfVertex3Uni > vertices;
      vertices.reserve(2 + opack->children().size());

      if (m_type == Front)
      {
         vertices.push_back(BfVertex3Uni(ipack->firstLine()->first().getp()));
         vertices.push_back(BfVertex3Uni(ipack->firstLine()->second().getp()));
         for (auto& cp : packs)
         {
            auto pack = std::static_pointer_cast< BfCirclePack >(cp);
            vertices.push_back(BfVertex3Uni(pack->normalLine()->first().getp())
            );
            vertices.push_back(
                BfVertex3Uni(pack->perpLineFirst()->second().getp())
            );
         }
         vertices.pop_back();
         vertices.push_back(BfVertex3Uni(opack->secondLine()->second().getp()));
         vertices.push_back(BfVertex3Uni(opack->secondLine()->first().getp()));
      }
      else if (m_type == Back)
      {
         vertices.push_back(BfVertex3Uni(ipack->secondLine()->first().getp()));
         vertices.push_back(BfVertex3Uni(ipack->secondLine()->second().getp()));
         for (auto& cp : packs)
         {
            auto pack = std::static_pointer_cast< BfCirclePack >(cp);
            vertices.push_back(BfVertex3Uni(pack->normalLine()->second().getp())
            );
            vertices.push_back(
                BfVertex3Uni(pack->perpLineSecond()->second().getp())
            );
         }
         vertices.pop_back();
         vertices.push_back(BfVertex3Uni(opack->firstLine()->second().getp()));
         vertices.push_back(BfVertex3Uni(opack->firstLine()->first().getp()));
      }
      return vertices;
   }

private:
   Type m_type;
   std::weak_ptr< BfIOCirclePack > m_ipack;
   std::weak_ptr< obj::BfDrawLayer > m_centerPacks;
   std::weak_ptr< BfIOCirclePack > m_opack;
};

class BfQuad : public obj::BfDrawObject { 
public:
   template < typename P1, typename P2, typename P3, typename P4> 
   BfQuad(P1&& tl, P2&& tr, P3&& br, P4&& bl) 
      : obj::BfDrawObject("Texture plane", BF_PIPELINE(BfPipelineType_LoadedImage), 10)
      , m_tl{ std::forward<P1>(tl) }
      , m_tr{ std::forward<P2>(tr) }
      , m_br{ std::forward<P3>(br) }
      , m_bl{ std::forward<P4>(bl) }
   { 
   }
   
   void make() override { 
      m_vertices.clear();
      m_indices.clear();
      m_vertices = { m_tl.get(), m_tr.get(), m_br.get(), m_bl.get() };
      m_indices = { 0, 1, 2, 2, 0, 3 };
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

class BfTextureQuad : public BfQuad { 
public:

   template < typename P1, typename P2, typename P3, typename P4> 
   BfTextureQuad(P1&& tl, P2&& tr, P3&& br, P4&& bl) 
      : BfQuad { std::forward<P1>(tl), std::forward<P2>(tr),
                 std::forward<P3>(br), std::forward<P4>(bl) }
   { 
   }

   void presentContextMenu() override;
   bool isLocked() noexcept { return m_isLocked; }
   bool isRatio() noexcept { return m_isRatio; }

   void make() override {
      m_vertices.clear();
      m_indices.clear();

      glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotateAngle), m_bl.normals());
      glm::vec3 center = (m_tl.pos() + m_br.pos()) * 0.5f;

      auto rotate = [&](BfVertex3Uni& v) -> BfVertex3 {
         glm::vec4 local = glm::vec4(v.pos() - center, 1.0f);
         glm::vec3 rotated = glm::vec3(r * local) + center;
         return BfVertex3{ rotated, v.color(), v.normals() };
      };

      m_vertices = {
         rotate(m_tl),
         rotate(m_tr),
         rotate(m_br),
         rotate(m_bl)
      };
      m_indices = { 0, 1, 2, 2, 0, 3 };
   }

   virtual BfObjectData _objectData() override { 
      return {
         .model_matrix = m_modelMatrix,
         .select_color = glm::vec3(m_transp, 0.5f, 0.0f) ,
         .index = static_cast<uint32_t>(m_isLocked ? 1 : 0),
         .id = id(),
         .line_thickness = 0.00025f
      };
   }

private:
   float m_rotateAngle = 0.0f;
   float m_transp = 1.0f;
   bool m_isLocked = false;
   bool m_isRatio = false;
};

class BfTexturePlane : public obj::BfDrawLayer 
{ 
public:
   BfTexturePlane(float w, float h) : obj::BfDrawLayer { "Texture plane" }
   {
      const auto n = glm::vec3{ 0.f , 0.f, 1.f };
      auto quad = std::make_shared< BfTextureQuad  >( 
         BfVertex3{ glm::vec3(0, h, -.1f), glm::vec3(0.0f, 0.0f, 0.0f), n },
         BfVertex3{ glm::vec3(w, h, -.1f), glm::vec3(1.0f, 0.0f, 0.0f), n },
         BfVertex3{ glm::vec3(w, 0, -.1f), glm::vec3(1.0f, 1.0f, 0.0f), n },
         BfVertex3{ glm::vec3(0, 0, -.1f), glm::vec3(0.0f, 1.0f, 0.0f), n }
      );
      this->add(std::make_shared< BfHandleCircle >(quad->tl().getp(),  0.01f));
      this->add(std::make_shared< BfHandleCircle >(quad->tr().getp(),  0.01f));
      this->add(std::make_shared< BfHandleCircle >(quad->br().getp(),  0.01f));
      this->add(std::make_shared< BfHandleRectangle >(quad->bl().getp(),  0.01f));
      this->add(quad);
   }

   std::shared_ptr< BfHandleCircle > tlHandle() { 
      return std::static_pointer_cast< BfHandleCircle >(m_children[0]); 
   }
   std::shared_ptr< BfHandleCircle > trHandle() { 
      return std::static_pointer_cast< BfHandleCircle >(m_children[1]); 
   }
   std::shared_ptr< BfHandleCircle > brHandle() { 
      return std::static_pointer_cast< BfHandleCircle >(m_children[2]); 
   }
   std::shared_ptr< BfHandleRectangle > blHandle() { 
      return std::static_pointer_cast< BfHandleRectangle >(m_children[3]); 
   }
   std::shared_ptr< BfTextureQuad > quad() { 
      return std::static_pointer_cast< BfTextureQuad >(m_children[4 ]); 
   }

   void moveToCenter() { 
      blHandle()->move({ 0, 0, blHandle()->center().pos().z });
   }

   void make() override {
      _assignRoots(); 

      auto bl = blHandle();
      auto br = brHandle();
      auto tr = trHandle();
      auto tl = tlHandle();
      bool isRatio = quad()->isRatio();
      bool isRatioChanged = isRatio != m_isRatioOld;

      const float ratio = base::desc::own::BfDescriptorPipelineDefault::getTextureDescriptor().ratio();

      glm::vec3 blPos = bl->center().pos();

      if (bl->isChanged() || isRatioChanged)
      {
         auto dir = blPos - m_oldbl;
         br->center().pos += dir;
         tr->center().pos += dir;
         tl->center().pos += dir;
      }

      if (br->isChanged())
      {
         glm::vec3 newBr = br->center().pos;

         if (!isRatio)
         {
            br->center().pos.y = blPos.y;
            tr->center().pos.x = br->center().pos.x;
         }
         else { 
            br->center().pos.y = bl->center().pos().y;
            br->center().pos.x = tr->center().pos.x;
         }
      }

      if (tr->isChanged() || isRatioChanged)
      {
         glm::vec3 newTr = tr->center().pos;

         if (isRatio)
         {
            float width = newTr.x - blPos.x;
            float height = width / ratio;

            tr->center().pos.y = blPos.y + height;
            br->center().pos = glm::vec3(newTr.x, blPos.y, br->center().pos.z);
            tl->center().pos =
                glm::vec3(blPos.x, blPos.y + height, tl->center().pos.z);
         }
         else
         {
            br->center().pos.x = tr->center().pos.x;
            tl->center().pos.y = tr->center().pos.y;
         }
      }

      if (tl->isChanged() || isRatioChanged)
      {
         glm::vec3 newTl = tl->center().pos;

         if (!isRatio) 
         {
            tl->center().pos.x = blPos.x;
            tr->center().pos.y = tl->center().pos.y;
         }
         else { 
            tl->center().pos.y = tr->center().pos.y;
            tl->center().pos.x = bl->center().pos().x;
         }
      }

      obj::BfDrawLayer::make();

      m_oldtl = tl->center().pos;
      m_oldtr = tr->center().pos;
      m_oldbr = br->center().pos;
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
