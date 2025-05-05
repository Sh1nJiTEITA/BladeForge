#include <bfCurves4.h>

#include <cmath>
#include <fmt/base.h>
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include <stdexcept>

#include "bfCamera.h"
#include "imgui.h"

namespace obj
{

namespace curves
{

BfVertex3Uni&
BfSingleLine::first()
{
   return m_first;
}
BfVertex3Uni&
BfSingleLine::second()
{
   return m_second;
}

const float
BfSingleLine::length() const
{
   return glm::distance(m_first.pos(), m_second.pos());
}

glm::vec3
BfSingleLine::directionFromStart() const
{
   return glm::normalize(m_second.pos() - m_first.pos());
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

BfVertex3&
BfCircle2Vertices::center()
{
   return m_center.get();
}

BfVertex3&
BfCircle2Vertices::other()
{
   return m_other.get();
}

const float
BfCircle2Vertices::radius() const noexcept
{
   return glm::distance(m_center.pos(), m_other.pos());
}

void
BfCircle2Vertices::make()
{
   m_indices.clear();
   m_vertices = std::move(
       math::calcCircleVertices(center(), radius(), m_discretization, m_color)
   );
   _genIndicesStandart();
}

float
BfCircleCenterFilled::radius() const noexcept
{
   return m_radius;
}
BfVertex3Uni&
BfCircleCenterFilled::center()
{
   return m_center;
}

void
BfCircleCenterFilled::make()
{
   m_indices.clear();
   m_vertices = std::move(
       math::calcCircleVertices(center(), m_radius, m_discretization, m_color)
   );
   m_indices.reserve(m_vertices.size());
   for (size_t i = 1; i < m_vertices.size() - 2; ++i)
   {
      m_indices.emplace_back(0);
      m_indices.emplace_back(i);
      m_indices.emplace_back(i + 1);
   }
}

void
BfHandleCircle::processDragging()
{
   BfHandleBehavior::processDragging();
   if (m_isPressed)
   {
      m_initialCenterPos = center().pos();
   }
   if (m_isDown)
   {
      center().pos() = m_initialCenterPos + delta3D();
      m_isChanged = true;
      this->root()->make();
      this->root()->control().updateBuffer(true);
   }
}

void
BfHandleCircle::make()
{
   BfCircleCenterFilled::make();
   m_isChanged = false;
}

bool
BfHandleCircle::isChanged() const noexcept
{
   return m_isChanged;
}
void
BfHandleCircle::resetPos()
{
   m_initialCenterPos = m_center.pos();
}

void
BfHandleRectangle::processDragging()
{
   BfHandleBehavior::processDragging();
   if (m_isPressed)
   {
      m_initialCenterPos = center().pos();
   }
   if (m_isDown)
   {
      center().pos() = m_initialCenterPos + delta3D();
      m_isChanged = true;
      this->root()->make();
      this->root()->control().updateBuffer(true);
   }
}

void
BfHandleRectangle::move(const glm::vec3& v)
{
   m_center.pos() = v;
   m_isChanged = true;
}

void
BfHandleRectangle::make()
{
   m_vertices.clear();
   m_indices.clear();
   auto& cp = m_center.pos();
   auto& cn = m_center.normals();
   float h = m_side;
   m_vertices = {
       BfVertex3{{cp.x - h, cp.y + h, cp.z}, color(), cn},
       BfVertex3{{cp.x + h, cp.y + h, cp.z}, color(), cn},
       BfVertex3{{cp.x + h, cp.y - h, cp.z}, color(), cn},
       BfVertex3{{cp.x - h, cp.y - h, cp.z}, color(), cn},
   };
   m_indices = {0, 1, 2, 2, 0, 3};
   m_isChanged = false;
}

BfVertex3Uni&
BfHandleRectangle::center()
{
   return m_center;
};

bool
BfHandleRectangle::isChanged() const noexcept
{
   return m_isChanged;
}
void
BfHandleRectangle::resetPos()
{
   m_initialCenterPos = m_center.pos();
}

glm::vec3
BfBezierN::calcNormal(float t) const
{
   return math::BfBezierBase::calcNormal(*this, t);
}

glm::vec3
BfBezierN::calcTangent(float t) const
{
   return math::BfBezierBase::calcTangent(*this, t);
}

glm::vec3
BfBezierN::calcDerivative(float t) const
{
   return math::BfBezierBase::calcDerivative(*this, t);
}

float
BfBezierN::length() const
{
   return math::BfBezierBase::length(*this);
}

BfVertex3
BfBezierN::calc(float t) const
{
   auto v = math::BfBezierBase::calc(*this, t);
   v.color = m_color;
   return v;
}

void
BfBezierN::make()
{
   m_vertices.clear();
   m_indices.clear();

   float t;
   m_vertices.reserve(m_discretization);
   for (int i = 0; i < m_discretization; i++)
   {
      t = static_cast< float >(i) / static_cast< float >(m_discretization - 1);
      auto v = this->calc(t);
      m_vertices.push_back(std::move(v));
   }
   m_indices.reserve(m_discretization);
   _genIndicesStandart();
}

void
BfBezierWH::elevateOrder()
{
   bool oldstate = toggleBoundHandles(true);
   auto handles_layer =
       std::static_pointer_cast< obj::BfDrawLayer >(m_children[1]);
   auto lines_layer =
       std::static_pointer_cast< obj::BfDrawLayer >(m_children[2]);
   auto new_vertices = math::BfBezierBase::elevateOrder(*this);

   for (size_t i = 0; i < new_vertices.size() - 1; ++i)
   {
      (*this)[i].pos() = new_vertices[i].pos();
      auto handle = std::static_pointer_cast< curves::BfHandleCircle >(
          handles_layer->children()[i]
      );
      handle->resetPos();
   }
   this->push_back(*new_vertices.rbegin());

   handles_layer->add(
       std::make_shared< curves::BfHandleCircle >(this->rbegin()->getp(), 0.01f)
   );
   lines_layer->add(
       std::make_shared< curves::BfSingleLine >(
           this->rbegin()->getp(),
           (this->rbegin() + 1)->getp()
       )
   );

   auto casted_curve =
       std::static_pointer_cast< curves::BfBezierN >(m_children[0]);
   auto new_pointers = this->_genControlVerticesPointers();
   casted_curve->assign(new_pointers.begin(), new_pointers.end());
   toggleBoundHandles(oldstate);
}

void
BfBezierWH::lowerateOrder()
{
   bool oldstate = toggleBoundHandles(true);
   auto handles_layer =
       std::static_pointer_cast< obj::BfDrawLayer >(m_children[1]);
   auto lines_layer =
       std::static_pointer_cast< obj::BfDrawLayer >(m_children[2]);
   auto new_vertices = math::BfBezierBase::lowerateOrder(*this);

   handles_layer->children().pop_back();
   lines_layer->children().pop_back();
   this->pop_back();

   for (size_t i = 0; i < new_vertices.size(); ++i)
   {
      (*this)[i].pos() = new_vertices[i].pos();
      auto handle = std::static_pointer_cast< curves::BfHandleCircle >(
          handles_layer->children()[i]
      );
      handle->resetPos();
   }

   auto curve = std::static_pointer_cast< curves::BfBezierN >(m_children[0]);
   auto new_pointers = this->_genControlVerticesPointers();
   curve->assign(new_pointers.begin(), new_pointers.end());
   toggleBoundHandles(oldstate);
}

bool
BfBezierWH::toggleHandle(size_t i, int status)
{
   auto handles = std::static_pointer_cast< obj::BfDrawLayer >(m_children[1]);
   auto handle =
       std::static_pointer_cast< curves::BfHandleCircle >(handles->children()[i]
       );
   return handle->toggleRender(status);
}

bool
BfBezierWH::toggleBoundHandles(int sts)
{
   bool f = toggleHandle(0, sts);
   bool s = toggleHandle(this->size() - 1, sts);
   return f && s;
}

std::shared_ptr< curves::BfBezierN >
BfBezierWH::curve()
{
   return std::static_pointer_cast< curves::BfBezierN >(m_children[0]);
}

std::vector< BfVertex3Uni >
BfBezierWH::_genControlVerticesPointers()
{
   std::vector< BfVertex3Uni > tmp;
   tmp.reserve(this->size());
   for (auto& it : *this)
   {
      tmp.push_back(BfVertex3Uni(it.getp()));
   }
   return tmp;
}

BfVertex3Uni&
BfSingleLineWH::left()
{
   return m_left;
}
BfVertex3Uni&
BfSingleLineWH::right()
{
   return m_right;
}

std::shared_ptr< curves::BfHandleCircle >
BfSingleLineWH::leftHandle()
{
   return std::static_pointer_cast< curves::BfHandleCircle >(m_children[0]);
}

std::shared_ptr< curves::BfHandleCircle >
BfSingleLineWH::rightHandle()
{
   return std::static_pointer_cast< curves::BfHandleCircle >(m_children[1]);
}

std::shared_ptr< curves::BfSingleLine >
BfSingleLineWH::line()
{
   return std::static_pointer_cast< curves::BfSingleLine >(m_children[2]);
}

// clang-format off
bool BfSingleLineWH::isChanged() const noexcept { 
   return std::static_pointer_cast<curves::BfHandleCircle>(m_children[0])->isChanged() ||
          std::static_pointer_cast<curves::BfHandleCircle>(m_children[1])->isChanged();
}
// clang-format on

float
BfCircleCenterWH::radius() const
{
   return glm::distance(m_other.pos(), m_center.pos());
}

void
BfCircleCenterWH::setRadius(float r)
{
   auto& dir = m_previusOtherHandleDirection;
   if (glm::all(glm::epsilonEqual(dir, glm::vec3{0.0f, 0.0f, 0.0f}, 0.001f)) ||
       glm::any(glm::isnan(dir)) || glm::any(glm::isinf(dir)))
   {
      dir = glm::vec3{0.0001f, 0.0f, 0.0f};
   }
   other().pos = center().pos + dir * r;
};

BfVertex3&
BfCircleCenterWH::center()
{
   return m_center.get();
}
BfVertex3&
BfCircleCenterWH::other()
{
   return m_other.get();
}

void
BfCircleCenterWH::setCenter(const BfVertex3& v)
{
   m_lastCenterPos = v.pos;
   m_center.get() = v;
}

void
BfCircleCenterWH::make()
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

const glm::vec3&
BfCircleCenterWH::previousDirection() const noexcept
{
   return m_previusOtherHandleDirection;
}

std::shared_ptr< BfHandleCircle >
BfCircleCenterWH::centerHandle()
{
   return std::static_pointer_cast< BfHandleCircle >(m_children[1]);
}

std::shared_ptr< BfHandleCircle >
BfCircleCenterWH::otherHandle()
{
   return std::static_pointer_cast< BfHandleCircle >(m_children[2]);
}

bool
BfCircleCenterWH::isChanged() const noexcept
{
   return std::static_pointer_cast< curves::BfHandleCircle >(m_children[1])
              ->isChanged() ||
          std::static_pointer_cast< curves::BfHandleCircle >(m_children[2])
              ->isChanged();
}

void
BfCircle2Lines::make()
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

BfVertex3
BfCircle2Lines::center()
{
   float alpha = curves::math::angleBetween3Vertices(m_begin, m_center, m_end);
   glm::vec3 direction_to_begin =
       glm::normalize(m_begin.pos() - m_center.pos());
   float distance_to_begin =
       m_radius.get() / glm::tan(glm::radians(alpha) * 0.5f);
   glm::vec3 vertex_to_center =
       m_center.pos() + direction_to_begin * distance_to_begin;
   glm::vec3 direction_to_center =
       (glm::rotate(glm::mat4(1.0f), -glm::radians(90.0f), m_center.normals()) *
        glm::vec4(direction_to_begin, 1.0f));
   glm::vec3 res = vertex_to_center + direction_to_center * m_radius.get();
   return BfVertex3(res, m_center.color(), m_center.normals());
}

void
BfTextureQuad::presentContextMenu()
{
   if (auto root = m_root.lock())
   {
      auto casted = std::static_pointer_cast< BfTexturePlane >(root);
      if (ImGui::Checkbox("Lock", &m_isLocked))
      {
         casted->tlHandle()->toggleRender(!m_isLocked);
         casted->trHandle()->toggleRender(!m_isLocked);
         casted->brHandle()->toggleRender(!m_isLocked);
         casted->blHandle()->toggleRender(!m_isLocked);
      }
      if (ImGui::Checkbox("Save ratio", &m_isRatio))
      {
         fmt::println("Changing ratio flag: {}", m_isRatio);
         this->root()->make();
         this->root()->control().updateBuffer(true);
      }
      ImGui::SliderFloat("Transparency", &m_transp, 0, 1);
      if (ImGui::DragFloat("Rotate", &m_rotateAngle, 1, 0, 360))
      {
         this->root()->make();
         this->root()->control().updateBuffer(true);
      }
      if (ImGui::Button("Move to center"))
      {
         casted->moveToCenter();
         this->root()->make();
         this->root()->control().updateBuffer(true);
      }
   }
}

void
BfCircle2LinesWH::make()
{
   _assignRoots();

   glm::vec3 handle_pos = handle()->center().pos();
   float distance_to_center_vertex = glm::distance(handle_pos, m_center.pos());
   float distance_to_line =
       curves::math::distanceToLine(handle_pos, m_begin, m_center);

   if (glm::all(glm::equal(m_begin.pos(), m_oldbegin)) &&
       glm::all(glm::equal(m_center.pos(), m_oldcenter)) &&
       glm::all(glm::equal(m_end.pos(), m_oldend)))
   {

      m_radius.get() = (distance_to_center_vertex * distance_to_line) /
                       (distance_to_center_vertex + distance_to_line);
   }

   // accurate handle pos
   float alpha = curves::math::angleBetween3Vertices(m_begin, m_center, m_end);
   glm::vec3 alpha_dir =
       glm::rotate(
           glm::mat4(1.0f),
           -glm::radians(alpha * 0.5f),
           m_center.normals()
       ) *
       glm::vec4(glm::normalize(m_center.pos() - m_begin.pos()), 1.0f);

   handle()->center().pos() = curves::math::closestPointOnLine(
       handle_pos,
       m_center,
       m_center.pos() + alpha_dir
   );

   // FIXME Ручка создается 2 раза тут и еще ниже
   for (auto child : m_children)
   {
      child->make();
   }
   glm::vec3 circle_center = circle()->center().pos;
   glm::vec3 direction_to_handle =
       glm::normalize(circle_center - m_center.pos());
   handle()->center().pos() =
       circle()->center().pos + direction_to_handle * m_radius.get();
   handle()->make();

   m_oldbegin = m_begin.pos();
   m_oldcenter = m_center.pos();
   m_oldend = m_end.pos();
}

BfVertex3
BfCircle2LinesWH::centerVertex()
{
   return circle()->center();
}

std::shared_ptr< curves::BfCircle2Lines >
BfCircle2LinesWH::circle()
{
   return std::static_pointer_cast< curves::BfCircle2Lines >(m_children[0]);
}

std::shared_ptr< curves::BfHandleCircle >
BfCircle2LinesWH::handle()
{
   return std::static_pointer_cast< curves::BfHandleCircle >(m_children[1]);
}

BfVar< float >&
BfCircle2LinesWH::radius() noexcept
{
   return m_radius;
}

/* BfCirclePackWH BEGIN */

std::shared_ptr< BfSingleLine >
BfCirclePackWH::centerTangentLine()
{
   return std::static_pointer_cast< BfSingleLine >(m_children[0]);
}

std::shared_ptr< BfCircleCenterWH >
BfCirclePackWH::circle()
{
   return std::static_pointer_cast< BfCircleCenterWH >(m_children[1]);
}

std::shared_ptr< BfSingleLine >
BfCirclePackWH::angleLine()
{
   return std::static_pointer_cast< BfSingleLine >(m_children[2]);
}

std::shared_ptr< BfHandleCircle >
BfCirclePackWH::firstAngleLineHandle()
{
   return std::static_pointer_cast< BfHandleCircle >(m_children[3]);
}

std::shared_ptr< BfHandleCircle >
BfCirclePackWH::secondAngleLineHandle()
{
   return std::static_pointer_cast< BfHandleCircle >(m_children[4]);
}

std::shared_ptr< BfBezierN >
BfCirclePackWH::bezierCurve()
{
   if (auto locked = m_curve.lock())
   {
      return locked;
   }
   else
   {
      throw std::runtime_error("Can't lock Bezier curve");
   }
}

std::pair< BfVertex3, BfVertex3 >
BfCirclePackWH::_calcTangentLineVertices()
{
   auto v = bezierCurve()->calc(m_relativePos.get());
   auto dir = glm::normalize(bezierCurve()->calcTangent(m_relativePos.get()));
   auto other = v.otherPos(v.pos + dir * m_radius.get());
   return {v, other};
}

void
BfCirclePackWH::_addUpdateTangentLine()
{
   auto [center_vert, tangent_vert] = _calcTangentLineVertices();
   if (m_children.size() != m_totalObjectCount)
   {
      auto line = addf< BfSingleLine >(center_vert, tangent_vert);
      line->color() = glm::vec3(0.0, 0.7, 0.2);
   }
   else
   {
      auto line = centerTangentLine();
      line->first().pos() = center_vert.pos;
      line->second().pos() = tangent_vert.pos;
   }
}

std::pair< BfVertex3, BfVertex3 >
BfCirclePackWH::_calcRadiusVertices()
{
   const auto tangent_line = centerTangentLine();
   const auto dir = tangent_line->directionFromStart();
   auto center = tangent_line->first().get();
   glm::vec3 new_dir = glm::rotate(
                           glm::mat4(1.0f),
                           glm::radians(this->m_angle.get()),
                           center.normals
                       ) *
                       glm::vec4(dir, 1.0f);
   return {
       center.otherPos(center.pos + new_dir * m_radius.get()),
       center.otherPos(center.pos - new_dir * m_radius.get()),
   };
}

std::pair< float, float >
BfCirclePackWH::_calcRadius()
{
   auto fline = firstAngleLineHandle();
   auto sline = secondAngleLineHandle();
   auto line = centerTangentLine();
   return {
       glm::distance(fline->center().pos(), line->first().pos()),
       glm::distance(sline->center().pos(), line->first().pos())
   };
}

void
BfCirclePackWH::_addUpdateCircle()
{
   if (m_children.size() != m_totalObjectCount)
   {
      auto line = centerTangentLine();
      this->addf< BfCircleCenter >(line->first().getp(), m_radius.getp());
      auto [aR, bR] = _calcRadiusVertices();
      auto angle_line = this->addf< BfSingleLine >(aR, bR);
      this->addf< BfHandleCircle >(angle_line->first().getp(), 0.01f);
      this->addf< BfHandleCircle >(angle_line->second().getp(), 0.01f);
   }
   else
   {
      auto [fR, sR] = _calcRadius();
      if (fR != sR)
      {
         if (firstAngleLineHandle()->isChanged())
         {
            m_radius.get() = fR;
         }
         else if (secondAngleLineHandle()->isChanged())
         {
            m_radius.get() = sR;
         }
      }

      auto [aR, bR] = _calcRadiusVertices();
      auto angle_line = angleLine();
      angle_line->first().pos() = aR.pos;
      angle_line->second().pos() = bR.pos;
   }
}

void
BfCirclePackWH::make()
{
   _addUpdateTangentLine();
   _addUpdateCircle();
   BfDrawLayer::make();
}

/* BfCirclePackWH END */

std::shared_ptr< BfCircleCenterWH >
BfCirclePack::circle()
{
   return std::static_pointer_cast< BfCircleCenterWH >(m_children[0]);
}

std::shared_ptr< BfSingleLine >
BfCirclePack::normalLine()
{
   return std::static_pointer_cast< BfSingleLine >(m_children[1]);
}

std::shared_ptr< BfSingleLine >
BfCirclePack::perpLineFirst()
{
   return std::static_pointer_cast< BfSingleLine >(m_children[2]);
}

std::shared_ptr< BfSingleLine >
BfCirclePack::perpLineSecond()
{
   return std::static_pointer_cast< BfSingleLine >(m_children[3]);
}

std::shared_ptr< BfBezierN >
BfCirclePack::boundCurve()
{
   auto initCurve = m_curve.lock();
   if (!initCurve)
   {
      throw std::runtime_error("Can't lock BfBezierN ...");
   }
   return std::static_pointer_cast< BfBezierN >(initCurve);
}

void
BfCirclePack::bindNext(std::weak_ptr< obj::BfDrawObjectBase > next) noexcept
{
   m_next = next;
}

void
BfCirclePack::bindPrevious(std::weak_ptr< obj::BfDrawObjectBase > pre) noexcept
{
   m_previous = pre;
}

bool
BfCirclePack::hasLeft()
{
   return !m_previous.expired();
}
bool
BfCirclePack::hasRight()
{
   return !m_next.expired();
}

std::shared_ptr< BfCirclePack >
BfCirclePack::next()
{
   auto snext = m_next.lock();
   if (!snext)
   {
      throw std::runtime_error("Cant lock next circle pack");
   }
   return std::static_pointer_cast< BfCirclePack >(snext);
}

std::shared_ptr< BfCirclePack >
BfCirclePack::previous()
{
   auto snext = m_next.lock();
   if (!snext)
   {
      throw std::runtime_error("Cant lock next circle pack");
   }
   return std::static_pointer_cast< BfCirclePack >(snext);
}

void
BfCirclePack::make()
{
   _premakeCircle();
   _premakeNormalLine();
   _premakeIntersection();
   obj::BfDrawLayer::make();
}

BfVar< float >&
BfCirclePack::relativePos() noexcept
{
   return m_relativePos;
}

BfVar< float >&
BfCirclePack::radius() noexcept
{
   return m_radius;
}

glm::vec3
BfCirclePack::_findPerpDirectionFirst()
{
   const auto current_normal_line = normalLine();
   const glm::vec3 from_start = current_normal_line->directionFromStart();
   const glm::vec3 normal_start = current_normal_line->first().normals();
   return glm::normalize(glm::cross(from_start, normal_start));
}
glm::vec3
BfCirclePack::_findPerpDirectionSecond()
{
   const auto current_normal_line = normalLine();
   const glm::vec3 from_end = current_normal_line->directionFromEnd();
   const glm::vec3 normal_end = current_normal_line->first().normals();
   return glm::normalize(glm::cross(from_end, normal_end));
}

void
BfCirclePack::_premakeIntersectionSecond()
{
   const auto current_normal_line = normalLine();
   auto current_perp_line = perpLineSecond();
   glm::vec3 current_perp_dir = _findPerpDirectionSecond();
   if (hasRight())
   {
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
   if (!hasLeft())
   {
      current_perp_line->first().pos() = current_normal_line->second().pos();
   }
   if (hasLeft() && !hasRight())
   {
      current_perp_line->second().pos() = current_normal_line->second().pos();
   }
}

void
BfCirclePack::_premakeIntersectionFirst()
{
   const auto current_normal_line = normalLine();
   auto current_perp_line = perpLineFirst();
   glm::vec3 current_perp_dir = _findPerpDirectionFirst();
   if (hasRight())
   {
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
   if (!hasLeft())
   {
      current_perp_line->first().pos() = current_normal_line->first().pos();
   }
   if (hasLeft() && !hasRight())
   {
      current_perp_line->second().pos() = current_normal_line->first().pos();
   }
}

void
BfCirclePack::_premakeIntersection()
{
   _premakeIntersectionFirst();
   _premakeIntersectionSecond();
}

void
BfCirclePack::_premakeNormalLine()
{
   auto curve = boundCurve();
   auto c = circle();
   auto line = normalLine();
   auto normal =
       curves::math::BfBezierBase::calcNormal(*curve, m_relativePos.get());
   line->first().pos() = c->center().pos + normal * m_radius.get();
   line->second().pos() = c->center().pos - normal * m_radius.get();
};

void
BfCirclePack::_premakeCircle()
{
   auto c = circle();
   auto casted = boundCurve();

   if (c->centerHandle()->isChanged())
   {
      auto info_pos = casted->calc(m_relativePos.get()).pos;
      auto actual_pos = c->center().pos;

      if (glm::any(glm::notEqual(info_pos, actual_pos)))
      {
         float close_t =
             curves::math::BfBezierBase::findClosest(*casted, c->center());
         m_relativePos.get() = close_t;
      }
   }
   c->center().pos = casted->calc(m_relativePos.get()).pos;
   if (c->otherHandle()->isChanged())
   {
      m_radius.get() = c->radius();
   }
   else
   {
      c->setRadius(m_radius.get());
   }
}

std::shared_ptr< obj::BfDrawLayer >
BfIOCirclePack::centerCircles()
{
   auto locked = m_centerCircles.lock();
   if (!locked)
      throw std::runtime_error("Cant lock central circles");
   return std::static_pointer_cast< obj::BfDrawLayer >(locked);
}

std::shared_ptr< BfSingleLineWH >
BfIOCirclePack::line()
{
   auto locked = m_line.lock();
   if (!locked)
      throw std::runtime_error("Cant lock IO line");
   return std::static_pointer_cast< BfSingleLineWH >(locked);
}

std::shared_ptr< BfCircle2LinesWH >
BfIOCirclePack::circle()
{
   auto locked = m_circle.lock();
   if (!locked)
      throw std::runtime_error("Cant lock IO circle");
   return std::static_pointer_cast< BfCircle2LinesWH >(locked);
}

std::shared_ptr< BfSingleLine >
BfIOCirclePack::normalLine()
{
   return std::static_pointer_cast< BfSingleLine >(m_children[0]);
}

std::shared_ptr< BfSingleLine >
BfIOCirclePack::firstLine()
{
   return std::static_pointer_cast< BfSingleLine >(m_children[1]);
}

std::shared_ptr< BfSingleLine >
BfIOCirclePack::secondLine()
{
   return std::static_pointer_cast< BfSingleLine >(m_children[2]);
}

std::shared_ptr< BfSingleLine >
BfIOCirclePack::firstLineIntr()
{
   return std::static_pointer_cast< BfSingleLine >(m_children[3]);
}

std::shared_ptr< BfSingleLine >
BfIOCirclePack::secondLineIntr()
{
   return std::static_pointer_cast< BfSingleLine >(m_children[4]);
}

void
BfIOCirclePack::make()
{
   auto c = circle();
   auto line = this->line();

   glm::vec3 normal_direction = glm::normalize(
       glm::cross(line->line()->directionFromStart(), line->left().normals())
   );

   auto nLine = normalLine();
   nLine->first().pos() =
       c->centerVertex().pos - normal_direction * c->radius().get();
   nLine->second().pos() =
       c->centerVertex().pos + normal_direction * c->radius().get();

   glm::vec3 fintr, sintr;
   _findIntersection(fintr, sintr);
   firstLine()->second().pos() = fintr;
   secondLine()->second().pos() = sintr;

   if (m_type == Inlet)
   {
      auto next = _nearCircle();
      firstLineIntr()->second() =
          BfVertex3Uni(next->perpLineFirst()->first().getp());
      secondLineIntr()->second() =
          BfVertex3Uni(next->perpLineSecond()->first().getp());
   }
   else if (m_type == Outlet)
   {
      auto prev = _nearCircle();
      firstLineIntr()->second() =
          BfVertex3Uni(prev->perpLineSecond()->second().getp());
      secondLineIntr()->second() =
          BfVertex3Uni(prev->perpLineFirst()->second().getp());
   }
   obj::BfDrawLayer::make();
}

void
BfIOCirclePack::_addInterLines()
{
   auto first = this->firstLine();
   auto second = this->secondLine();

   BfVertex3 fintrv = first->second();
   fintrv.pos =
       first->second().pos() + first->directionFromStart() * first->length();
   auto first_inter = std::make_shared< BfSingleLine >(
       BfVertex3Uni(first->second().getp()),
       std::move(fintrv)
   );
   this->add(first_inter);

   BfVertex3 sintrv = second->second();
   sintrv.pos =
       second->second().pos() + second->directionFromStart() * second->length();
   auto second_intr = std::make_shared< BfSingleLine >(
       BfVertex3Uni(second->second().getp()),
       std::move(sintrv)
   );
   this->add(second_intr);
}

void
BfIOCirclePack::_addPerpLines()
{
   auto nLine = this->normalLine();
   auto line = this->line();
   auto next = this->_nearCircle();
   auto c = this->circle();
   auto first = next->perpLineFirst();
   auto second = next->perpLineSecond();

   BfVertex3 first_inter = nLine->first();
   first_inter.pos = nLine->first().pos() +
                     line->line()->directionFromEnd() * c->radius().get();

   auto first_line = std::make_shared< BfSingleLine >(
       BfVertex3Uni(nLine->first().getp()),
       std::move(first_inter)
   );
   this->add(first_line);

   BfVertex3 second_inter = nLine->first();
   second_inter.pos = nLine->second().pos() +
                      line->line()->directionFromEnd() * c->radius().get();

   auto second_line = std::make_shared< BfSingleLine >(
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
std::shared_ptr< BfCirclePack >
BfIOCirclePack::_nearCircle()
{
   auto nLine = this->normalLine();
   auto cc = this->centerCircles();
   auto line = this->line();
   if (m_type == Inlet)
   {
      auto it_next = std::min_element(
          cc->children().begin(),
          cc->children().end(),
          [](const auto& lhs, const auto& rhs) {
             auto l = std::static_pointer_cast< BfCirclePack >(lhs);
             auto r = std::static_pointer_cast< BfCirclePack >(rhs);
             return l->relativePos().get() < r->relativePos().get();
          }
      );
      if (it_next == cc->children().end())
      {
         throw std::runtime_error("Cant find most left circle pack");
      }
      return std::static_pointer_cast< BfCirclePack >(*it_next);
   }
   else if (m_type == Outlet)
   {
      auto it_next = std::max_element(
          cc->children().begin(),
          cc->children().end(),
          [](const auto& lhs, const auto& rhs) {
             auto l = std::static_pointer_cast< BfCirclePack >(lhs);
             auto r = std::static_pointer_cast< BfCirclePack >(rhs);
             return l->relativePos().get() < r->relativePos().get();
          }
      );
      if (it_next == cc->children().end())
      {
         throw std::runtime_error("Cant find right left circle pack");
      }
      return std::static_pointer_cast< BfCirclePack >(*it_next);
   }
   else
   {
      throw std::runtime_error("Unhandled BfIOcirclePack");
   }
}

void
BfIOCirclePack::_findIntersection(glm::vec3& fintr, glm::vec3& sintr)
{
   auto nLine = this->normalLine();
   auto cc = this->centerCircles();
   auto line = this->line();

   if (m_type == Inlet)
   {
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
   }
   else if (m_type == Outlet)
   {
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

std::shared_ptr< BfIOCirclePack >
BfBezierChain::inletPack()
{
   auto locked = m_ipack.lock();
   if (!locked)
      throw std::runtime_error("Cant lock inlet pack");
   return locked;
}

std::shared_ptr< obj::BfDrawLayer >
BfBezierChain::centerPacks()
{
   auto locked = m_centerPacks.lock();
   if (!locked)
      throw std::runtime_error("Cant lock center packs");
   return locked;
}

std::shared_ptr< BfIOCirclePack >
BfBezierChain::outletPack()
{
   auto locked = m_opack.lock();
   if (!locked)
      throw std::runtime_error("Cant lock outlet pack");
   return locked;
}

std::shared_ptr< obj::BfDrawLayer >
BfBezierChain::chain()
{
   return std::static_pointer_cast< obj::BfDrawLayer >(m_children[0]);
}

void
BfBezierChain::make()
{
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

std::vector< BfVertex3Uni >
BfBezierChain::_controlPoints()
{
   auto ipack = inletPack();
   auto opack = outletPack();
   auto cpacks = centerPacks();

   using pack_t = std::shared_ptr< curves::BfCirclePack >;
   std::vector< pack_t > packs;
   std::transform(
       cpacks->children().begin(),
       cpacks->children().end(),
       std::back_inserter(packs),
       [](std::shared_ptr< obj::BfDrawObjectBase >& o) {
          return std::static_pointer_cast< curves::BfCirclePack >(o);
       }
   );

   std::sort(
       packs.begin(),
       packs.end(),
       [](const pack_t& lhs, const pack_t& rhs) {
          return lhs->relativePos().get() < rhs->relativePos().get();
       }
   );

   std::vector< BfVertex3Uni > vertices;
   vertices.reserve(2 + opack->children().size());

   if (m_type == Front)
   {
      vertices.push_back(BfVertex3Uni(ipack->firstLine()->first().getp()));
      vertices.push_back(BfVertex3Uni(ipack->firstLine()->second().getp()));
      for (auto& cp : packs)
      {
         auto pack = std::static_pointer_cast< BfCirclePack >(cp);
         vertices.push_back(BfVertex3Uni(pack->normalLine()->first().getp()));
         vertices.push_back(BfVertex3Uni(pack->perpLineFirst()->second().getp())
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
         vertices.push_back(BfVertex3Uni(pack->normalLine()->second().getp()));
         vertices.push_back(BfVertex3Uni(pack->perpLineSecond()->second().getp()
         ));
      }
      vertices.pop_back();
      vertices.push_back(BfVertex3Uni(opack->firstLine()->second().getp()));
      vertices.push_back(BfVertex3Uni(opack->firstLine()->first().getp()));
   }
   return vertices;
}

} // namespace curves
} // namespace obj
//
