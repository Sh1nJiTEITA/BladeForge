#ifndef BF_CURVES4_H
#define BF_CURVES4_H

#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vector_relational.hpp>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "bfCamera.h"
#include "bfDrawObject2.h"
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

      m_vertices = {
          {m_first.get().pos, this->color(), m_first.get().normals},
          {m_second.get().pos, this->color(), m_second.get().normals},

      };
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

   virtual std::shared_ptr< BfDrawObjectBase > clone() const override
   {
      auto cloned = std::make_shared< BfCircleCenter< T > >(m_center, m_radius);
      cloned->copy(*this);
      return cloned;
   }

   virtual void copy(const BfDrawObjectBase& obj) override
   {
      BfDrawObject::copy(obj);
      const auto& casted = static_cast< const BfCircleCenter< T >& >(obj);
      m_radius = casted.m_radius;
      m_center = casted.m_center;
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

   virtual std::shared_ptr< BfDrawObjectBase > clone() const override
   {
      auto cloned = std::make_shared< BfCircle2Vertices >(m_center, m_other);
      cloned->copy(*this);
      return cloned;
   }

   virtual void copy(const BfDrawObjectBase& obj) override
   {
      BfDrawObject::copy(obj);
      const auto& casted = static_cast< const BfCircle2Vertices& >(obj);
      m_other = casted.m_other;
      m_center = casted.m_center;
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

   virtual std::shared_ptr<BfDrawObjectBase> clone() const override
   {
      auto cloned = std::make_shared<BfCircleCenterFilled>(m_center, m_radius);
      cloned->copy(*this);
      return cloned;
   }

   virtual void copy(const BfDrawObjectBase& obj) override
   {
      BfDrawObject::copy(obj);
      const auto& casted = static_cast<const BfCircleCenterFilled&>(obj);
      m_radius = casted.m_radius;
      m_center = casted.m_center;
   }

protected:
   float m_radius;
   BfVertex3Uni m_center;
};

// template <typename T, math::IsBfVertex3Variation<T> = true>
class BfHandle : public BfCircleCenterFilled
{
public:
   template <typename T>
   BfHandle(T&& center, float radius)
       : BfCircleCenterFilled(std::forward<T>(center), radius)
   {
   }

   virtual void copy(const BfDrawObjectBase& obj) override
   {
      BfCircleCenterFilled::copy(obj);
      const auto& casted = static_cast<const BfHandle&>(obj);
      m_initialMousePos = casted.m_initialMousePos;
      m_initialCenterPos = casted.m_initialCenterPos;
   }

   virtual void processDragging() override
   {
      bool is_dragging =
          ((this->isHovered || m_isDraggingStarted) &&
           ImGui::IsMouseDown(ImGuiMouseButton_Left));

      auto inst = BfCamera::m_pInstance;
      if (is_dragging && inst->m_mode == BfCameraMode_Ortho)
      {
         // Calculate the initial offset between the object's center and the
         // mouse position

         if (!m_isDraggingStarted)
         {
            // Store the initial positions when the dragging starts
            m_initialMousePos = BfCamera::instance()->mouseWorldCoordinates();
            m_initialCenterPos = this->center().pos;
            m_isDraggingStarted = true;
         }

         // Calculate the difference (offset) between the initial mouse position
         // and the center of the object
         glm::vec3 mouse_offset =
             BfCamera::instance()->mouseWorldCoordinates() - m_initialMousePos;

         // Update the object’s center position based on this offset, keeping it
         // relative
         this->center().pos = m_initialCenterPos + mouse_offset;

         // You can make your copy if necessary, or just continue with the
         // camera update
         // this->make();
         m_isChanged = true;

         this->root()->make();
         this->root()->control().updateBuffer(true);
      }
      else
      {
         m_isDraggingStarted = false;
      }
   }


   virtual void make() override { 
      BfCircleCenterFilled::make();
      m_isChanged = false;
   }

   std::shared_ptr<BfDrawObjectBase> clone() const override
   {
      auto cloned = std::make_shared<BfHandle>(this->m_center, this->m_radius);
      cloned->copy(*this);
      return cloned;
   }

   bool isChanged() const noexcept { return m_isChanged; }

   void resetPos() { 
      m_initialCenterPos = m_center.pos();
   }

private:
   glm::vec3 m_initialMousePos;
   glm::vec3 m_initialCenterPos;
   bool m_isDraggingStarted = false;
   bool m_isChanged = false;
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
             "Bezier curve 2", BF_PIPELINE(BfPipelineType_Lines), 400
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
         handles_layer->add(std::make_shared<curves::BfHandle>(v.getp(), 0.01f));
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
         auto handle = std::static_pointer_cast<curves::BfHandle>(handles_layer->children()[i]);
         handle->resetPos();
      }
      this->push_back(*new_vertices.rbegin());

      handles_layer->add(std::make_shared< curves::BfHandle >(this->rbegin()->getp(), 0.01f));
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
         auto handle = std::static_pointer_cast<curves::BfHandle>(handles_layer->children()[i]);
         handle->resetPos();
      }

      auto curve = std::static_pointer_cast< curves::BfBezierN >(m_children[0]);
      auto new_pointers = this->_genControlVerticesPointers();
      curve->assign(new_pointers.begin(), new_pointers.end());
      toggleBoundHandles(oldstate);
   }

   bool toggleHandle(size_t i, int status = -1){ 
      auto handles = std::static_pointer_cast<obj::BfDrawLayer>(m_children[1]);
      auto handle = std::static_pointer_cast<curves::BfHandle>(handles->children()[i]);
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
      auto lHandle = std::make_shared< curves::BfHandle >(
         m_left.getp(), 0.01f
      );
      this->add(lHandle);
      auto rHandle = std::make_shared< curves::BfHandle >(
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

   std::shared_ptr< curves::BfHandle > leftHandle()
   {
      return std::static_pointer_cast< curves::BfHandle >(m_children[0]);
   }

   std::shared_ptr< curves::BfHandle > rightHandle()
   {
      return std::static_pointer_cast< curves::BfHandle >(m_children[1]);
   }

   std::shared_ptr< curves::BfSingleLine > line()
   {
      return std::static_pointer_cast< curves::BfSingleLine >(m_children[2]);
   }

   // clang-format off
   virtual bool isChanged() const noexcept { 
      return std::static_pointer_cast<curves::BfHandle>(m_children[0])->isChanged() ||
             std::static_pointer_cast<curves::BfHandle>(m_children[1])->isChanged();
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

      auto center_handle = std::make_shared<curves::BfHandle>(
         m_center.getp(),
         0.01f
      );
      this->add(center_handle);

      auto r_handle = std::make_shared<curves::BfHandle>(
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
   std::shared_ptr< BfHandle > centerHandle() {
      return std::static_pointer_cast< BfHandle >(m_children[1]);
   }

   std::shared_ptr< BfHandle > otherHandle() {
      return std::static_pointer_cast< BfHandle >(m_children[2]);
   }

   

   virtual bool isChanged() const noexcept { 
      return std::static_pointer_cast<curves::BfHandle>(m_children[1])->isChanged() ||
             std::static_pointer_cast<curves::BfHandle>(m_children[2])->isChanged();
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
      auto handle = std::make_shared<curves::BfHandle>(
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

   std::shared_ptr<curves::BfHandle> handle()  
   {
      return std::static_pointer_cast<curves::BfHandle>(m_children[1]);
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
         auto circle = std::make_shared<BfCircleCenterWH>(
            center, m_radius.get()
         );
         this->add(circle);
      }
      else { 
         throw std::runtime_error("Can't lock BfBezierN ...");
      }
   }

   std::shared_ptr< BfCircleCenterWH > circle() { 
      return std::static_pointer_cast<BfCircleCenterWH>(m_children[0]);
   }

   virtual void make() override { 
      auto c = circle();    
      auto initCurve = m_curve.lock();
      if (!initCurve) { throw std::runtime_error("Can't lock BfBezierN ..."); }
      auto casted = std::static_pointer_cast<BfBezierN>(initCurve);

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

      obj::BfDrawLayer::make();
   }

private:
   std::weak_ptr<BfDrawObjectBase> m_curve;

   BfVar<float> m_relativePos;
   BfVar<float> m_radius;
};



class BfBezierChain : public obj::BfDrawLayer { 
public:
   template< typename... Args>
   BfBezierChain(Args&& ...args) 
      : obj::BfDrawLayer{ "Bezier chain" }
   { 
      std::vector<BfObj>objs( std::forward<Args>(args)... ); 
      for (auto&& obj : objs) { 
         this->add(obj);
      }
   }
};











}; // namespace curves

} // namespace obj

#endif
