#ifndef BF_CURVES4_H
#define BF_CURVES4_H

#include <cmath>
#include <glm/geometric.hpp>
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

class BfSingleLine : public obj::BfDrawObject
{
public:
   template <typename T, typename U>
   BfSingleLine(T&& fp, U&& sp)
       : obj::BfDrawObject{"Single line", BF_PIPELINE(BfPipelineType_Lines)}
       , m_first{std::forward<T>(fp)}
       , m_second{std::forward<U>(sp)}
   {
   }

   const BfVertex3& first() const;
   const BfVertex3& second() const;
   const float length() const;
   glm::vec3 directionFromStart() const;
   glm::vec3 directionFromEnd() const;
   void make() override;

   // clang-format on
private:
   BfVertex3 m_first;
   BfVertex3 m_second;
};

template <typename T, math::IsBfVertex3Variation<T> = true>
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
   BfVertex3& center()
   {
      using _T = std::decay_t<T>;
      if constexpr (std::is_pointer_v<_T>)
      {
         return *m_center;
      }
      else
      {
         m_center;
      }
   }

   virtual void make() override
   {
      m_indices.clear();
      m_vertices = std::move(math::calcCircleVertices(
          center(),
          m_radius,
          m_discretization,
          m_color
      ));
      _genIndicesStandart();
   }

   virtual std::shared_ptr<BfDrawObjectBase> clone() const override
   {
      auto cloned = std::make_shared<BfCircleCenter<T>>(m_center, m_radius);
      cloned->copy(*this);
      return cloned;
   }

   virtual void copy(const BfDrawObjectBase& obj) override
   {
      BfDrawObject::copy(obj);
      const auto& casted = static_cast<const BfCircleCenter<T>&>(obj);
      m_radius = casted.m_radius;
      m_center = casted.m_center;
   }

private:
   float m_radius;
   T m_center;
};

template <typename T, math::IsBfVertex3Variation<T> = true>
class BfCircle2Vertices : public obj::BfDrawObject
{
public:
   BfCircle2Vertices(T&& center, T&& other)
       : obj::
             BfDrawObject{"Circle center", BF_PIPELINE(BfPipelineType_Lines), 200}
       , m_center{std::move(center)}
       , m_other{std::move(other)}
   {
   }
   BfCircle2Vertices(const T& center, const T& other)
       : obj::
             BfDrawObject{"Circle center", BF_PIPELINE(BfPipelineType_Lines), 200}
       , m_center{center}
       , m_other{other}
   {
   }

   const float radius() const noexcept
   {
      using _T = std::decay_t<T>;
      if constexpr (std::is_pointer_v<_T>)
      {
         return glm::distance(m_center->pos, m_other->pos);
      }
      else
      {
         return glm::distance(m_center.pos, m_other.pos);
      }
   }
   BfVertex3& center()
   {
      using _T = std::decay_t<T>;
      if constexpr (std::is_pointer_v<_T>)
      {
         return *m_center;
      }
      else
      {
         m_center;
      }
   }
   BfVertex3& other()
   {
      using _T = std::decay_t<T>;
      if constexpr (std::is_pointer_v<_T>)
      {
         return *m_other;
      }
      else
      {
         m_other;
      }
   }

   virtual void make() override
   {
      m_indices.clear();
      m_vertices = std::move(math::calcCircleVertices(
          center(),
          radius(),
          m_discretization,
          m_color
      ));
      _genIndicesStandart();
   }

   virtual std::shared_ptr<BfDrawObjectBase> clone() const override
   {
      auto cloned = std::make_shared<BfCircle2Vertices<T>>(m_center, m_other);
      cloned->copy(*this);
      return cloned;
   }

   virtual void copy(const BfDrawObjectBase& obj) override
   {
      BfDrawObject::copy(obj);
      const auto& casted = static_cast<const BfCircle2Vertices&>(obj);
      m_other = casted.m_other;
      m_center = casted.m_center;
   }

private:
   T m_other;
   T m_center;
};

class Bfcircle3Vertices : public obj::BfDrawObject
{
public:
   template <typename T, typename U, typename B>
   Bfcircle3Vertices(T&& P_1, U&& P_2, B&& P_3)
       : obj::
             BfDrawObject{"Circle 3 vertices", BF_PIPELINE(BfPipelineType_Lines), 200}
       , m_first{std::forward<T>(P_1)}
       , m_second{std::forward<U>(P_2)}
       , m_third{std::forward<B>(P_3)}
       , m_radius{std::nanf("")}
       , m_center{BfVertex3::nan()}
   {
   }

   const BfVertex3& first() const;
   const BfVertex3& second() const;
   const BfVertex3& third() const;
   const float radius() const;
   const BfVertex3& center() const;

   void make() override;

private:
   float m_radius;
   BfVertex3 m_center;
   BfVertex3 m_first;
   BfVertex3 m_second;
   BfVertex3 m_third;
};

template <typename T, math::IsBfVertex3Variation<T> = true>
class BfCircleCenterFilled : public obj::BfDrawObject
{
public:
   // clang-format off
   // template< typename U > 
   BfCircleCenterFilled(T&& center, float radius)
       : obj::BfDrawObject{"Circle center filled", 
                           BF_PIPELINE(BfPipelineType_Triangles), 200}
       , m_radius{radius}
       , m_center{std::move(center)}
   {
   }

   BfCircleCenterFilled(const T& center, float radius)
       : obj::BfDrawObject{"Circle center filled", 
                           BF_PIPELINE(BfPipelineType_Triangles), 200}
       , m_radius{radius}
       , m_center{center}
   {
   }
   // clang-format on

   float radius() const noexcept { return m_radius; }
   BfVertex3& center()
   {
      using _T = std::decay_t<T>;
      if constexpr (std::is_pointer_v<_T>)
      {
         return *m_center;
      }
      else
      {
         m_center;
      }
   }

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
      auto cloned =
          std::make_shared<BfCircleCenterFilled<T>>(m_center, m_radius);
      cloned->copy(*this);
      return cloned;
   }

   virtual void copy(const BfDrawObjectBase& obj) override
   {
      BfDrawObject::copy(obj);
      const auto& casted = static_cast<const BfCircleCenterFilled<T>&>(obj);
      m_radius = casted.m_radius;
      m_center = casted.m_center;
   }

protected:
   float m_radius;
   T m_center;
};

template <typename T, math::IsBfVertex3Variation<T> = true>
class BfHandle : public BfCircleCenterFilled<T>
{
public:
   // template <typename U>
   // BfHandle(U&& center, float radius)
   //     : BfCircleCenterFilled<T>(std::forward<U>(center), radius)
   // {
   // }
   //
   BfHandle(T&& center, float radius)
       : BfCircleCenterFilled<T>(std::move(center), radius)
   {
   }

   BfHandle(const T& center, float radius)
       : BfCircleCenterFilled<T>(center, radius)
   {
   }

   // BfHandle(const BfHandle& o)
   //     : BfCircleCenterFilled(o.center(), o.radius())
   // {
   // }
   // virtual void processDragging() override;

   virtual void copy(const BfDrawObjectBase& obj) override
   {
      BfCircleCenterFilled<T>::copy(obj);
      const auto& casted = static_cast<const BfHandle<T>&>(obj);
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
         this->root()->make();
         this->root()->control().updateBuffer(true);
      }
      else
      {
         m_isDraggingStarted = false;
      }
   }

   std::shared_ptr<BfDrawObjectBase> clone() const override
   {
      auto cloned =
          std::make_shared<BfHandle<T>>(this->m_center, this->m_radius);
      cloned->copy(*this);
      return cloned;
   }

private:
   glm::vec3 m_initialMousePos;
   glm::vec3 m_initialCenterPos;
   bool m_isDraggingStarted = false;
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

template <typename T, math::IsBfVertex3Variation<T> = true>
class BfBezier2 : public obj::BfDrawObject, public std::vector<T>
{
public:
   template <typename... Args>
   BfBezier2(Args&&... args)
       : std::vector<T>{std::forward<Args>(args)...}
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

   // BfBezier2(std::vector<T>&& vec)
   //     : std::vector<T>{std::move(vec)}
   //     , obj::BfDrawObject{
   //           "Bezier curve 2", BF_PIPELINE(BfPipelineType_Lines), 400
   //       }
   // {
   //    if (this->size() < 3)
   //    {
   //       throw std::runtime_error(
   //           "Bezier curve with < 3 control points is not handled"
   //       );
   //    }
   // }
   //
   // BfBezier2(const std::vector<T>& vec)
   //     : std::vector<T>{vec}
   //     , obj::BfDrawObject{
   //           "Bezier curve 2", BF_PIPELINE(BfPipelineType_Lines), 400
   //       }
   // {
   //    if (this->size() < 3)
   //    {
   //       throw std::runtime_error(
   //           "Bezier curve with < 3 control points is not handled"
   //       );
   //    }
   // }

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

class BfBezier : public obj::BfDrawObject, public std::vector<BfVertex3>
{
public:
   template <typename... Args>
   BfBezier(Args&&... args)
       : std::vector<BfVertex3>{args...}
       , obj::BfDrawObject{
             "Bezier curve", BF_PIPELINE(BfPipelineType_Lines), 400
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
   float length() const;

   BfVertex3 calc(float t) const;
   virtual void make() override;
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
//
//
//

class BfBezierWithHandles : public obj::BfDrawLayer,
                            public std::vector<BfVertex3>
{
public:
   template <typename... Args>
   BfBezierWithHandles(Args&&... args)
       : obj::BfDrawLayer("Bezier curve with handles")
       , std::vector<BfVertex3>{std::forward<Args>(args)...}
   {
      auto curve = std::make_shared<curves::BfBezier2<BfVertex3*>>(
          _genControlVerticesPointers()
      );
      this->add(curve);
      for (const auto& v : *curve.get())
      {
         this->add(std::make_shared<curves::BfHandle<BfVertex3*>>(v, 0.01f));
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

private:
   std::vector<BfVertex3*> _genControlVerticesPointers()
   {
      std::vector<BfVertex3*> tmp;
      tmp.reserve(this->size());
      for (auto& it : *this)
      {
         tmp.push_back(&it);
      }
      return tmp;
   }
};

class BfCircleCenterWithHandles : public obj::BfDrawLayer
{
public:
   template <typename T>
   BfCircleCenterWithHandles(T&& init_center, float init_radius)
       : obj::BfDrawLayer("Circle with handles")
       , m_center{std::forward<T>(init_center)}
   // , m_other{
   //       m_center.pos + glm::normalize(glm::vec3(
   //                          -m_center.normals.z, 0.0f, m_center.normals.x
   //                      )) * init_radius,
   //       m_center.color,
   //       m_center.normals
   //   }
   {
      // clang-format off
      auto other_pos = m_center.pos + glm::normalize(glm::vec3(-m_center.normals.z, 0.0f, m_center.normals.x)) * init_radius;

      m_other.pos = other_pos;
      m_other.color = m_center.color;
      m_other.normals = m_center.normals;
      m_lastCenterPos = m_center.pos;

      std::cout << m_other << "\n";
      std::cout << m_center << "\n";

      auto circle = std::make_shared<curves::BfCircle2Vertices<BfVertex3*>>(
          &m_center,
          &m_other
      );
      this->add(circle);
    
      auto center_handle = std::make_shared<curves::BfHandle<BfVertex3*>>( 
         &m_center,
         0.01f
      );
      this->add(center_handle);
      
      auto r_handle = std::make_shared<curves::BfHandle<BfVertex3*>>( 
         &m_other,
         0.01f
      );
      this->add(r_handle);

      // clang-format on
   }

   virtual void make() override
   {
      _assignRoots();

      if (glm::any(glm::notEqual(m_center.pos, m_lastCenterPos)))
      {
         std::cout << "NOT!\n";
         m_other.pos += -m_lastCenterPos + m_center.pos;
         m_lastCenterPos = m_center.pos;
      }

      for (auto child : m_children)
      {
         child->make();
      }
   }

private:
   BfVertex3 m_other;
   BfVertex3 m_center;

   glm::vec3 m_lastCenterPos;
};

};  // namespace curves

}  // namespace obj

#endif
