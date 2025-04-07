#include <bfCurves4.h>

#include <cmath>
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <memory>
#include <stdexcept>

#include "bfCamera.h"
#include "imgui.h"

namespace obj
{

namespace curves
{

namespace math
{

}  // namespace math

const BfVertex3&
BfSingleLine::first() const
{
   return m_first;
}
const BfVertex3&
BfSingleLine::second() const
{
   return m_second;
}
const float
BfSingleLine::length() const
{
   return glm::distance(m_first.pos, m_second.pos);
}
glm::vec3
BfSingleLine::directionFromStart() const
{
   return glm::normalize(m_second.pos - m_first.pos);
}
glm::vec3
BfSingleLine::directionFromEnd() const
{
   return -directionFromStart();
}

void
BfSingleLine::make()
{
   m_vertices.clear();
   m_indices.clear();

   m_vertices = {m_first, m_second};
   _genIndicesStandart();
};

void
BfCircleCenter::make()
{
   m_indices.clear();
   m_vertices = std::move(
       math::calcCircleVertices(m_center, m_radius, m_discretization, m_color)
   );
   _genIndicesStandart();
}

const BfVertex3&
BfCircleCenter::center() const
{
   return m_center;
}
const float
BfCircleCenter::radius() const
{
   return m_radius;
}

const BfVertex3&
Bfcircle3Vertices::first() const
{
   return m_first;
}
const BfVertex3&
Bfcircle3Vertices::second() const
{
   return m_second;
}
const BfVertex3&
Bfcircle3Vertices::third() const
{
   return m_third;
}

const float
Bfcircle3Vertices::radius() const
{
   if (std::isnan(m_radius))
   {
      throw std::runtime_error(
          "Tring to get circle radius but its Nan."
          " Call make() method before this function"
      );
   }
   return m_radius;
}

const BfVertex3&
Bfcircle3Vertices::center() const
{
   if (glm::any(glm::isnan(m_center.pos)) ||
       glm::any(glm::isnan(m_center.color)) ||
       glm::any(glm::isnan(m_center.normals)))
   {
      throw std::runtime_error(
          "Tring to get circle center but some of its commponents is Nan."
          " Call make() method before this function"
      );
   }
   return m_center;
}

void
Bfcircle3Vertices::make()
{
   m_vertices.clear();
   m_indices.clear();

   BfSingleLine l_12(m_first, m_second);
   BfSingleLine l_23(m_second, m_third);
   BfSingleLine l_31(m_third, m_first);

   glm::vec3 ave_12 = (m_first.pos + m_second.pos) * 0.5f;
   glm::vec3 ave_23 = (m_second.pos + m_third.pos) * 0.5f;
   glm::vec3 ave_31 = (m_third.pos + m_first.pos) * 0.5f;

   glm::vec3 v_12 = l_12.directionFromStart();
   glm::vec3 v_23 = l_23.directionFromStart();
   glm::vec3 v_31 = l_31.directionFromStart();

   glm::vec3 n_12 = glm::cross(glm::cross(v_12, v_23), v_12);
   glm::vec3 n_23 = glm::cross(glm::cross(v_23, v_12), v_23);
   glm::vec3 n_31 = glm::cross(glm::cross(v_31, v_23), v_31);

   BfSingleLine per_l_12(ave_12, ave_12 + n_12);
   BfSingleLine per_l_23(ave_23, ave_23 + n_23);
   BfSingleLine per_l_31(ave_31, ave_31 + n_31);

   BfVertex3 center;
   center.pos = obj::curves::math::findLinesIntersection(
       ave_12,
       ave_12 + n_12,
       ave_23,
       ave_23 + n_23,
       BF_MATH_FIND_LINES_INTERSECTION_ANY
   );

   // TODO
   if (glm::isnan(center.pos.x) || glm::isnan(center.pos.y) ||
       glm::isnan(center.pos.z))
   {
      if (m_first.equal(m_second))
      {
         m_center = BfVertex3(
             {m_first.pos.x + m_second.pos.x * 0.5f,
              m_first.pos.y + m_second.pos.y * 0.5f,
              m_first.pos.z + m_second.pos.z * 0.5f},

             m_first.color,
             m_first.normals
         );
         m_radius = glm::distance(m_first.pos, m_second.pos) * 0.5f;
      }
      else if (m_second.equal(m_third))
      {
         m_center = BfVertex3(
             {m_third.pos.x + m_second.pos.x * 0.5f,
              m_third.pos.y + m_second.pos.y * 0.5f,
              m_third.pos.z + m_second.pos.z * 0.5f},

             m_third.color,
             m_third.normals
         );
         m_radius = glm::distance(m_second.pos, m_third.pos) * 0.5f;
      }
      else if (m_third.equal(m_first))
      {
         m_center = BfVertex3(
             {m_first.pos.x + m_third.pos.x * 0.5f,
              m_first.pos.y + m_third.pos.y * 0.5f,
              m_first.pos.z + m_third.pos.z * 0.5f},

             m_first.color,
             m_first.normals
         );
         m_radius = glm::distance(m_third.pos, m_first.pos) * 0.5f;
      }
      return;
   }

   float rad_1 = glm::distance(center.pos, m_first.pos);
   float rad_2 = glm::distance(center.pos, m_first.pos);
   float rad_3 = glm::distance(center.pos, m_first.pos);

   if (!CHECK_FLOAT_EQUALITY(rad_1, rad_2) ||
       !CHECK_FLOAT_EQUALITY(rad_2, rad_3) ||
       !CHECK_FLOAT_EQUALITY(rad_3, rad_1))
   {
      throw std::runtime_error("Circle was'n made -> different radious");
   }

   center.normals = math::calcPlaneCoeffs(m_first, m_second, m_third);

   m_radius = rad_1;
   m_center = center;
   m_vertices = std::move(
       math::calcCircleVertices(m_center, m_radius, m_discretization, m_color)
   );
   _genIndicesStandart();
}

float
BfCircleCenterFilled::radius() const noexcept
{
   return m_radius;
}

const BfVertex3&
BfCircleCenterFilled::center() const noexcept
{
   return m_center;
}

void
BfCircleCenterFilled::make()
{
   m_indices.clear();
   m_vertices = std::move(
       math::calcCircleVertices(m_center, m_radius, m_discretization, m_color)
   );
   m_indices.reserve(m_vertices.size());
   for (size_t i = 1; i < m_vertices.size() - 2; ++i)
   {
      m_indices.emplace_back(0);
      m_indices.emplace_back(i);
      m_indices.emplace_back(i + 1);
   }
}

std::shared_ptr<BfDrawObjectBase>
BfCircleCenterFilled::clone() const
{
   auto cloned = std::make_shared<BfCircleCenterFilled>(m_center, m_radius);
   cloned->copy(*this);
   return cloned;
}

void
BfHandle::processDragging()
{
   bool is_dragging = (isHovered && ImGui::IsMouseDown(ImGuiMouseButton_Left));

   auto inst = BfCamera::m_pInstance;
   if (is_dragging && inst->m_mode == BfCameraMode_Ortho)
   {
      // Calculate the initial offset between the object's center and the mouse
      // position

      if (!m_isDraggingStarted)
      {
         // Store the initial positions when the dragging starts
         m_initialMousePos = BfCamera::instance()->mouseWorldCoordinates();
         m_initialCenterPos = m_center.pos;
         m_isDraggingStarted = true;
      }

      // Calculate the difference (offset) between the initial mouse position
      // and the center of the object
      glm::vec3 mouse_offset =
          BfCamera::instance()->mouseWorldCoordinates() - m_initialMousePos;

      // Update the object’s center position based on this offset, keeping it
      // relative
      m_center.pos = m_initialCenterPos + mouse_offset;

      // You can make your copy if necessary, or just continue with the camera
      // update
      make();
      root()->control().updateBuffer();
   }
   else
   {
      m_isDraggingStarted = false;
   }
}

std::shared_ptr<BfDrawObjectBase>
BfHandle::clone() const
{
   auto cloned = std::make_shared<BfHandle>(m_center, m_radius);
   cloned->copy(*this);
   return cloned;
}

glm::vec3
BfBezier::calcNormal(float t) const
{
   auto casted = static_cast<std::vector<BfVertex3>>(*this);
   return math::BfBezierBase::calcNormal(casted, t);
}

glm::vec3
BfBezier::calcTangent(float t) const
{
   auto casted = static_cast<std::vector<BfVertex3>>(*this);
   return math::BfBezierBase::calcTangent(casted, t);
}

glm::vec3
BfBezier::calcDerivative(float t) const
{
   auto casted = static_cast<std::vector<BfVertex3>>(*this);
   return math::BfBezierBase::calcDerivative(casted, t);
}

float
BfBezier::length() const
{
   auto casted = static_cast<std::vector<BfVertex3>>(*this);
   return math::BfBezierBase::length(casted);
}

BfVertex3
BfBezier::calc(float t) const
{
   auto casted = static_cast<std::vector<BfVertex3>>(*this);
   auto v = math::BfBezierBase::calc(casted, t);
   v.color = m_color;
   return v;
}

void
BfBezier::make()
{
   m_vertices.clear();
   m_indices.clear();

   float t;
   m_vertices.reserve(m_discretization);
   for (int i = 0; i < m_discretization; i++)
   {
      t = static_cast<float>(i) / static_cast<float>(m_discretization - 1);
      m_vertices.push_back(this->calc(t));
   }
   m_indices.reserve(m_discretization);
   _genIndicesStandart();
}

}  // namespace curves
}  // namespace obj
//
