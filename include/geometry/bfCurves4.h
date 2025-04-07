#ifndef BF_CURVES4_H
#define BF_CURVES4_H

#include <cmath>
#include <memory>
#include <stdexcept>
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

class BfCircleCenter : public obj::BfDrawObject
{
public:
   template <typename T>
   BfCircleCenter(T&& center, float radius)
       : obj::
             BfDrawObject{"Circle center", BF_PIPELINE(BfPipelineType_Lines), 200}
       , m_center{std::forward<T>(center)}
       , m_radius{radius}
   {
   }
   void make() override;
   const BfVertex3& center() const;
   const float radius() const;

private:
   BfVertex3 m_center;
   float m_radius;
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

class BfCircleCenterFilled : public obj::BfDrawObject
{
public:
   // clang-format off
   template <typename T>
   BfCircleCenterFilled(T&& center, float radius)
       : obj::BfDrawObject{"Circle center filled", 
                           BF_PIPELINE(BfPipelineType_Triangles), 200}
       , m_radius{radius}
       , m_center{center}
   {
   }
   // clang-format on

   float radius() const noexcept;
   const BfVertex3& center() const noexcept;
   virtual void make() override;

   virtual void copy(const BfDrawObjectBase& obj) override
   {
      BfDrawObject::copy(obj);
      const auto& casted = static_cast<const BfCircleCenterFilled&>(obj);
      m_radius = casted.m_radius;
      m_center = casted.m_center;
   }

   virtual std::shared_ptr<BfDrawObjectBase> clone() const override;

protected:
   float m_radius;
   BfVertex3 m_center;
};

class BfHandle : public BfCircleCenterFilled
{
public:
   template <typename T>
   BfHandle(T&& center, float radius)
       : BfCircleCenterFilled(std::forward<T>(center), radius)
   {
   }

   BfHandle(const BfHandle& o)
       : BfCircleCenterFilled(o.center(), o.radius())
   {
   }
   virtual void processDragging() override;

   virtual void copy(const BfDrawObjectBase& obj) override
   {
      BfCircleCenterFilled::copy(obj);
      const auto& casted = static_cast<const BfHandle&>(obj);
      m_initialMousePos = casted.m_initialMousePos;
      m_initialCenterPos = casted.m_initialCenterPos;
   }

   virtual std::shared_ptr<BfDrawObjectBase> clone() const override;

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
         std::cout << v << "\n";
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

class BfBezierWithHandles : public obj::BfDrawLayer
{
public:
   template <typename... Args>
   BfBezierWithHandles(Args&&... args)
       : obj::BfDrawLayer("Bezier curve with handles")
   {
      auto curve = std::make_shared<curves::BfBezier2<BfVertex3>>(args...);
      this->add(curve);
      for (const auto& v : *curve.get())
      {
         this->add(std::make_shared<curves::BfHandle>(v, 0.01f));
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
};

};  // namespace curves

}  // namespace obj

#endif
