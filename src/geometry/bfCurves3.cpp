#include "bfCurves3.h"

#include <cfloat>
#include <cmath>
#include <functional>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include <stdexcept>

#include "bfAllocator.h"
#include "bfDrawObject.h"
#include "bfDrawObjectDefineType.h"
#include "bfPipeline.h"

BfPlane::BfPlane(std::vector< BfVertex3 > d_vertices)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_PLANE)
{
   __dvertices = d_vertices;
}

void
BfPlane::createVertices()
{
   switch (__depMode)
   { // clang-format off
      case BfDrawObjDependencies_Mode_Ptr: {
         __vertices.reserve(__pdvertices.size());
         for (const auto& dvert : __pdvertices)
         {
            __vertices.emplace_back(*dvert);
         }
      }
      case BfDrawObjDependencies_Mode_No: {
         __vertices.reserve(__dvertices.size());
         for (const auto& dvert : __dvertices)
         {
            __vertices.emplace_back(dvert);
         }
      }
      default:
         throw std::runtime_error("Unexpected dependency mode in BfPlane");
   } // clang-format on
}

void
BfPlane::createIndices()
{
   __indices.reserve(__vertices.size() * 2);
   for (size_t i = 0; i < __vertices.size(); ++i)
   {
      __indices.emplace_back(i);
   }
}

BfSingleLine::BfSingleLine()
    : BfSingleLine{
          {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
          {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
      }
{
}

BfSingleLine::BfSingleLine(const BfVertex3& fp, const BfVertex3& sp)
    : BfDrawObj{{fp, sp}, BF_DRAW_OBJ_TYPE_SINGLE_LINE}
{
   __dvertices.reserve(2);
   __vertices.reserve(2);
   __indices.reserve(2);
}

BfSingleLine::BfSingleLine(BfVertex3* fp, BfVertex3* sp)
{
   __depMode = BfDrawObjDependencies_Mode_Ptr;
   __pdvertices.reserve(2);
   __pdvertices.push_back(fp);
   __pdvertices.push_back(sp);

   __vertices.reserve(2);
   __indices.reserve(2);
}

BfSingleLine::BfSingleLine(const glm::vec3& fp, const glm::vec3& sp)
    : BfSingleLine(
          {fp, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
          {sp, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
      )
{
}

float
BfSingleLine::get_k(int proj)
{
   glm::vec3 f{this->get_first().pos};
   glm::vec3 s{this->get_second().pos};

   switch (proj)
   {
   case BF_SINGLE_LINE_PROJ_XY:
      return (s.y - f.y) / (s.x - f.x);
   case BF_SINGLE_LINE_PROJ_XZ:
      return (s.z - f.z) / (s.x - f.x);
   case BF_SINGLE_LINE_PROJ_YX:
      return (s.x - f.x) / (s.y - f.y);
   case BF_SINGLE_LINE_PROJ_YZ:
      return (s.z - f.z) / (s.y - f.y);
   case BF_SINGLE_LINE_PROJ_ZX:
      return (s.x - f.x) / (s.z - f.z);
   case BF_SINGLE_LINE_PROJ_ZY:
      return (s.y - f.y) / (s.z - f.z);
   default:
      throw std::runtime_error("BfSingleLine::get_k - invalid proj");
   }
}

float
BfSingleLine::get_k_perpendicular(int proj)
{
   return -1.0f / this->get_k(proj);
}

float
BfSingleLine::get_b(int proj)
{
   glm::vec3 f{this->get_first().pos};
   glm::vec3 s{this->get_second().pos};

   switch (proj)
   {
   case BF_SINGLE_LINE_PROJ_XY:
      return f.y - this->get_k(proj) * f.x;
   case BF_SINGLE_LINE_PROJ_XZ:
      return f.z - this->get_k(proj) * f.x;
   case BF_SINGLE_LINE_PROJ_YX:
      return f.x - this->get_k(proj) * f.y;
   case BF_SINGLE_LINE_PROJ_YZ:
      return f.z - this->get_k(proj) * f.y;
   case BF_SINGLE_LINE_PROJ_ZX:
      return f.x - this->get_k(proj) * f.z;
   case BF_SINGLE_LINE_PROJ_ZY:
      return f.y - this->get_k(proj) * f.z;
   default:
      throw std::runtime_error("BfSingleLine::get_b - invalid proj");
   }
}

float
BfSingleLine::get_b_perpendicular(glm::vec3 f, int proj)
{
   float k_per{this->get_k_perpendicular(proj)};

   switch (proj)
   {
   case BF_SINGLE_LINE_PROJ_XY:
      return f.y - k_per * f.x;
   case BF_SINGLE_LINE_PROJ_XZ:
      return f.z - k_per * f.x;
   case BF_SINGLE_LINE_PROJ_YX:
      return f.x - k_per * f.y;
   case BF_SINGLE_LINE_PROJ_YZ:
      return f.z - k_per * f.y;
   case BF_SINGLE_LINE_PROJ_ZX:
      return f.x - k_per * f.z;
   case BF_SINGLE_LINE_PROJ_ZY:
      return f.y - k_per * f.z;
   default:
      throw std::runtime_error(
          "BfSingleLine::get_b_perpenduclar - invalid proj"
      );
   }
}

float
BfSingleLine::get_single_proj(float ival, int proj)
{
   return this->get_k(proj) * ival + this->get_b(proj);
}

float
BfSingleLine::get_length()
{
   return glm::distance(get_first().pos, get_second().pos);
}

const BfVertex3&
BfSingleLine::get_first() const
{
   switch (__depMode)
   { // clang-format off
      case BfDrawObjDependencies_Mode_Ptr: return *__pdvertices[0];
      case BfDrawObjDependencies_Mode_No: return __dvertices[0];
      default: throw std::runtime_error("Unexpected dependency mode in BfSingleLine");
   } // clang-format on
}

const BfVertex3&
BfSingleLine::get_second() const
{
   switch (__depMode)
   { // clang-format off
      case BfDrawObjDependencies_Mode_Ptr: return *__pdvertices[1];
      case BfDrawObjDependencies_Mode_No: return __dvertices[1];
      default: throw std::runtime_error("Unexpected dependency mode in BfSingleLine");
   } // clang-format on
}

glm::vec3
BfSingleLine::get_direction_from_start() const
{
   return glm::normalize(this->get_second().pos - this->get_first().pos);
}

glm::vec3
BfSingleLine::get_direction_from_end() const
{
   return -this->get_direction_from_start();
}

void
BfSingleLine::createVertices()
{
   if (this->is_ok())
      throw std::runtime_error("BfSingleLine::createVertices abort");

   __vertices.clear();
   __vertices.emplace_back(get_first());
   __vertices.at(0).color = __main_color;
   __vertices.emplace_back(get_second());
   __vertices.at(1).color = __main_color;
}

BfTriangle::BfTriangle(
    const BfVertex3& P_1, const BfVertex3& P_2, const BfVertex3& P_3
)
    : BfDrawObj({{P_1, P_2, P_3}}, BF_DRAW_OBJ_TYPE_TRIANGLE)
{
   __vertices.reserve(4);
}

const BfVertex3&
BfTriangle::get_first() const
{
   return __dvertices[0];
}
const BfVertex3&
BfTriangle::get_second() const
{
   return __dvertices[1];
}
const BfVertex3&
BfTriangle::get_third() const
{
   return __dvertices[2];
}

float
BfTriangle::get_area() const
{
   glm::vec3 u = get_second().pos - get_first().pos;
   glm::vec3 v = get_third().pos - get_first().pos;
   return glm::length(glm::cross(u, v)) / 2.0f;
}

BfVertex3
BfTriangle::get_center() const
{
   return {
       (get_first().pos + get_second().pos + get_third().pos) / 3.0f,
       get_first().color,
       bfMathGetNormal(get_first().pos, get_second().pos, get_third().pos)
   };
}

void
BfTriangle::createVertices()
{
   for (auto& it : __dvertices)
   {
      __vertices.emplace_back(it);
   }
   __vertices.emplace_back(__dvertices[0]);
}

glm::vec3
bfMathFindLinesIntersection(
    const BfSingleLine& line1, const BfSingleLine& line2, int mode
)
{
   if (bfMathIsVerticesInPlain(
           {line1.get_first(),
            line1.get_second(),
            line2.get_first(),
            line2.get_second()}
       ))
   {
      // a1.x + b1.x * t1 = a2.x + b2.x * t2
      // a1.y + b1.y * t1 = a2.y + b2.y * t2
      glm::vec3 a1 = line1.get_first().pos;
      glm::vec3 b1 = line1.get_direction_from_start();

      glm::vec3 a2 = line2.get_first().pos;
      glm::vec3 b2 = line2.get_direction_from_start();

      float frac = b1.x * b2.y - b1.y * b2.x;

      float t1 =
          (a1.x * b2.y - a1.y * b2.x - a2.x * b2.y + a2.y * b2.x) / (-frac);
      float t2 =
          (a1.x * b1.y - a1.y * b1.x - a2.x * b1.y + a2.y * b1.x) / (-frac);

      // if lines are parallel
      if (glm::isnan(t1) || glm::isnan(t2))
      {
         return glm::vec3(std::nan(""), std::nan(""), std::nan(""));
      }
      if (mode == BF_MATH_FIND_LINES_INTERSECTION_BETWEEN_VERTICES)
      {
         if ((t1 >= 0.0f) && (t1 <= 1.0f) && (t2 >= 0.0f) && (t2 <= 1.0f))
         {
            return a1 + b1 * t1;
         }
         else
         {
            return glm::vec3(std::nan(""), std::nan(""), std::nan(""));
         }
      }
      else if (mode == BF_MATH_FIND_LINES_INTERSECTION_ANY)
      {
         return a1 + b1 * t1;
      }
      else
      {
         throw std::runtime_error(
             "Invalid bfMathFindLinesIntersection mode inputed"
         );
      }
   }
   else
   {
      return glm::vec3(std::nan(""), std::nan(""), std::nan(""));
   }
}

size_t
bfMathGetFactorial(size_t n)
{
   const static std::map< uint32_t, uint32_t > __factorial{
       {1, 1},
       {2, 2},
       {3, 6},
       {4, 24},
       {5, 120},
       {6, 720},
       {7, 5040},
       {8, 40320},
       {9, 362880},
       {10, 3628800}
   };

   if (n < 1)
      return 1;
   else if (n > 10)
      throw std::runtime_error("Factorial can't be calculated: n > 10");
   else
   {
      return __factorial.at(n);
   }
}

size_t
bfMathGetBinomialCoefficient(size_t n, size_t k)
{
   return bfMathGetFactorial(n) / bfMathGetFactorial(k) /
          bfMathGetFactorial(n - k);
}

glm::vec3
bfMathGetNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
   glm::vec3 _v1{p2 - p1};
   glm::vec3 _v2{p3 - p1};
   glm::vec3 normal = glm::cross(_v1, _v2);

   if (normal == glm::vec3(0.0f))
      return normal;
   else
      return normal / glm::length(normal);
}

glm::vec4
bfMathGetPlaneCoeffs(const glm::vec3& f, const glm::vec3& s, const glm::vec3& t)
{
   glm::mat3 xd = {f.y, f.z, 1, s.y, s.z, 1, t.y, t.z, 1};

   glm::mat3 yd = {f.x, f.z, 1, s.x, s.z, 1, t.x, t.z, 1};

   glm::mat3 zd = {f.x, f.y, 1, s.x, s.y, 1, t.x, t.y, 1};

   auto direction = glm::vec3(
       glm::determinant(xd),
       -glm::determinant(yd),
       glm::determinant(zd)
   );

   glm::mat3 D = {f.x, f.y, f.z, s.x, s.y, s.z, t.x, t.y, t.z};

   return {direction, -glm::determinant(D)};
}

std::array< glm::vec3, 3 >
bfMathGetPlaneOrths(glm::vec4 plane)
{
   std::array< glm::vec3, 3 > orths;
   orths[0] = {plane.x, plane.y, plane.z}; // normal
   orths[1] = glm::normalize(glm::cross(orths[0], glm::vec3(0.0f, 0.0f, 1.0f)));
   orths[2] =
       glm::normalize(glm::cross(glm::cross(orths[0], orths[1]), orths[1]));
   return orths;
}

std::array< glm::vec3, 2 >
bfMathGetOrthsByNormal(glm::vec3 normal)
{
   std::array< glm::vec3, 2 > orths;
   orths[0] = glm::normalize(glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
   orths[1] =
       glm::normalize(glm::cross(glm::cross(orths[0], normal), orths[0]));
   return orths;
}

bool
bfMathIsVertexInPlain(const glm::vec4& plane, const glm::vec3& p)
{
   if (plane.x * p.x + plane.y * p.y + plane.z * p.z + plane.w <=
       BF_MATH_ABS_ACCURACY)
      return true;
   else
      return false;
}

bool
bfMathIsVertexInPlain(
    const glm::vec3& np,
    const glm::vec3& f,
    const glm::vec3& s,
    const glm::vec3& t
)
{
   glm::vec3 fs = s - f;
   glm::vec3 ft = t - f;
   glm::vec3 fn = np - f;

   float volume = glm::dot(glm::cross(fs, ft), fn);

   if (glm::abs(volume) < BF_MATH_ABS_ACCURACY)
      return true;
   else
      return false;
}

bool
bfMathIsVerticesInPlain(
    const std::vector< BfVertex3 >& np,
    const glm::vec3& f,
    const glm::vec3& s,
    const glm::vec3& t
)
{
   bool decision = true;
   for (const auto& vert : np)
   {
      decision *= bfMathIsVertexInPlain(vert.pos, f, s, t);
   }

   return decision;
}

bool
bfMathFindLinesIntersection(
    glm::vec3& intersection,
    const BfSingleLine& line1,
    const BfSingleLine& line2
)
{
   glm::vec3 P1_first{line1.get_first().pos};
   glm::vec3 P1_second{line1.get_second().pos};
   glm::vec3 P2_first{line2.get_first().pos};
   glm::vec3 P2_second{line2.get_second().pos};

   if (bfMathIsVerticesInPlain({P1_first, P1_second, P2_first, P2_second}))
   {
      glm::vec3 a1 = P1_first;
      glm::vec3 b1 = line1.get_direction_from_start();

      glm::vec3 a2 = P2_first;
      glm::vec3 b2 = line2.get_direction_from_start();

      float frac = b1.x * b2.y - b1.y * b2.x;

      float t1 =
          (a1.x * b2.y - a1.y * b2.x - a2.x * b2.y + a2.y * b2.x) / (-frac);
      float t2 =
          (a1.x * b1.y - a1.y * b1.x - a2.x * b1.y + a2.y * b1.x) / (-frac);

      // if lines are parallel
      if (glm::isnan(t1) || glm::isnan(t2))
      {
         return false;
      }

      if ((t1 >= 0.0f) && (t1 <= 1.0f) && (t2 >= 0.0f) && (t2 <= 1.0f))
      {
         intersection = a1 + b1 * t1;
         return true;
      }
      else
      {
         return false;
      }
   }
   else
   {
      return false;
   }
}

glm::vec3
bfMathFindLinesIntersection(cVec3& P0, cVec3& P1, cVec3 Q0, cVec3 Q1)
{
   bfAssert(bfMathIsVerticesInPlain({P0, P1, Q0, Q1}));

   glm::vec3 dP = P1 - P0;
   glm::vec3 dQ = Q1 - Q0;

   float denom = dP.x * dQ.y - dP.y * dQ.x;
   bfAssert(denom != 0.0f);
   float t = ((Q0.x - P0.x) * dQ.y - (Q0.y - P0.y) * dQ.x) / denom;
   return P0 + t * dP;
}

bool
bfMathIsVerticesInPlain(const std::vector< BfVertex3 >& np)
{
   if (np.size() <= 3)
      return true;

   bool decision = true;
   for (size_t i = 3; i < np.size(); ++i)
   {
      decision *=
          bfMathIsVertexInPlain(np[i].pos, np[0].pos, np[1].pos, np[2].pos);
   }

   return decision;
}

bool
bfMathIsVerticesInPlain(const std::vector< glm::vec3 >& np)
{
   std::vector< BfVertex3 > _np;
   _np.reserve(np.size());
   for (const auto& it : np)
   {
      _np.emplace_back(it);
   }
   return bfMathIsVerticesInPlain(_np);
}

bool
bfMathIsVerticesInPlain(std::initializer_list< glm::vec3 > np)
{
   std::vector< glm::vec3 > _np;
   _np.reserve(np.size());
   for (const auto& it : np)
   {
      _np.emplace_back(it);
   }

   return bfMathIsVerticesInPlain(_np);
}

bool
bfMathIsSingleLinesInPlain(const BfSingleLine& L1, const BfSingleLine& L2)
{
   return bfMathIsVerticesInPlain(
       {L1.get_first(), L1.get_second(), L2.get_first(), L2.get_second()}
   );
}

float
bfMathGetBezierCurveLength(BfBezierCurve* curve)
{
   if (curve->__vertices.empty())
      throw std::runtime_error("BfBezierCurve vertices vector are empty");

   if (curve->__dvertices.empty())
      throw std::runtime_error("BfBezierCurve dvertices vector are empty");

   float len = 0;

   for (size_t i = 0; i < curve->__vertices.size() - 1; ++i)
   {
      len +=
          glm::length(curve->__vertices[i + 1].pos - curve->__vertices[i].pos);
   }

   return len;
}

std::vector< glm::vec3 >
bfMathGetBezierCurveLengthDerivative(BfBezierCurve* curve)
{
   std::vector< glm::vec3 > d;
   d.reserve(curve->dVertices().size());

   for (size_t i = 0; i < curve->dVertices().size(); ++i)
   {
      glm::vec3 axe_derivative{0.0f};
      for (size_t j = 0; j < 2; ++j)
      {
         std::vector< BfVertex3 > right_dvert = curve->dVertices();
         right_dvert[i].pos[j] += BF_MATH_ABS_ACCURACY;
         BfBezierCurve right_curve{
             curve->n(),
             curve->dVertices().size(),
             right_dvert
         };
         right_curve.createVertices();
         float right = bfMathGetBezierCurveLength(&right_curve);

         std::vector< BfVertex3 > left_dvert = curve->dVertices();
         left_dvert[i].pos[j] -= BF_MATH_ABS_ACCURACY;
         BfBezierCurve left_curve{
             curve->n(),
             curve->dVertices().size(),
             left_dvert
         };
         left_curve.createVertices();
         float left = bfMathGetBezierCurveLength(&left_curve);

         axe_derivative[j] = (right - left) / 2.0f / BF_MATH_ABS_ACCURACY;
      }
      d.emplace_back(axe_derivative);
   }
   return d;
}

std::vector< BfCircle >
bfMathGetInscribedCircles(
    size_t m, const BfSingleLine& L1, const BfSingleLine& L2, float radius
)
{
   glm::vec3 line_intersection =
       bfMathFindLinesIntersection(L1, L2, BF_MATH_FIND_LINES_INTERSECTION_ANY);

   glm::vec3 L1_dir = L1.get_direction_from_start();
   glm::vec3 L2_dir = L2.get_direction_from_start();

   float angle_1 = glm::degrees(glm::angle(L1_dir, L2_dir));
   float angle_2 = 180.0f - angle_1;

   glm::vec3 normal = bfMathGetNormal(
       L1.get_first().pos,
       L1.get_second().pos,
       L2.get_first().pos
   );
   if (normal == glm::vec3(0.0f))
   {
      normal = bfMathGetNormal(
          L1.get_first().pos,
          L2.get_second().pos,
          L2.get_first().pos
      );
   }

   L1_dir = glm::vec3(
       glm::rotate(glm::mat4(1.0f), -glm::radians(angle_1 / 2.0f), normal) *
       glm::vec4(L1_dir, 1.0f)
   );
   L2_dir = glm::vec3(
       glm::rotate(glm::mat4(1.0f), -glm::radians(angle_2 / 2.0f), normal) *
       glm::vec4(L2_dir, 1.0f)
   );

   auto c1 = line_intersection + glm::normalize(L1_dir) * radius /
                                     glm::sin(glm::radians(angle_1) / 2.0f);
   auto c2 = line_intersection + -glm::normalize(L1_dir) * radius /
                                     glm::sin(glm::radians(angle_1) / 2.0f);
   auto c3 = line_intersection + glm::normalize(L2_dir) * radius /
                                     glm::sin(glm::radians(angle_2) / 2.0f);
   auto c4 = line_intersection + -glm::normalize(L2_dir) * radius /
                                     glm::sin(glm::radians(angle_2) / 2.0f);

   return {
       {m, {c1, {1.0f, 1.0f, 1.0f}, normal}, radius},
       {m, {c2, {1.0f, 1.0f, 1.0f}, normal}, radius},
       {m, {c3, {1.0f, 1.0f, 1.0f}, normal}, radius},
       {m, {c4, {1.0f, 1.0f, 1.0f}, normal}, radius},
   };
}

float
bfMathGetDistanceToLine(const BfSingleLine& L, BfVertex3 P)
{
   /*glm::vec3 lineVector = L.get_second().pos - L.get_first().pos;

   glm::vec3 pointToLinePointVector = L.get_first().pos - P.pos;

   float projectionLength = glm::dot(pointToLinePointVector, lineVector) /
   glm::dot(lineVector, lineVector); glm::vec3 projection = projectionLength *
   lineVector;

   glm::vec3 distanceVector = pointToLinePointVector - projection;

   return glm::length(distanceVector);*/

   /*glm::vec3 lineVector = L.get_second().pos - L.get_first().pos;
   glm::vec3 pointToLineVector = P.pos - L.get_first().pos;

   float t = glm::dot(pointToLineVector, lineVector) / glm::dot(lineVector,
   lineVector);

   glm::vec3 projectionPoint = L.get_first().pos + t * lineVector;

   return glm::distance(projectionPoint, P.pos);*/

   // glm::vec3 normal =
   //     bfMathGetNormal(L.get_first().pos, L.get_second().pos, P.pos);
   // glm::vec3 dir_to_line =
   //     glm::normalize(glm::cross(L.get_direction_from_start(), normal));
   //
   // BfSingleLine line(P, P.pos + dir_to_line);
   //
   // glm::vec3 intersection = bfMathFindLinesIntersection(
   //     L,
   //     line,
   //     BF_MATH_FIND_LINES_INTERSECTION_ANY
   // );
   //
   // return glm::distance(intersection, P.pos);
   return 0;
}

std::vector< BfVertex3 >
bfMathStickObjVertices(
    std::initializer_list< std::shared_ptr< BfDrawObj > > objs
)
{
   size_t total_size = 0;
   for (auto& obj : objs)
   {
      if (obj->vertices().size() == 0)
         abort();
      total_size += obj->vertices().size();
   }
   std::vector< BfVertex3 > out;
   out.reserve(total_size);

   for (auto& obj : objs)
   {
      out.insert(out.end(), obj->vertices().begin(), obj->vertices().end());
   }

   return out;
}

std::vector< std::shared_ptr< BfTriangle > >
bfMathGenerateTriangleField(std::vector< BfVertex3 > v)
{
   // auto get_triangle_count = [](size_t current) {
   //	/*if (current <= 5)
   //		return (size_t)0;*/
   //	// even
   //	if (current % 2 == 0)
   //		return (size_t)(current / 2.0f);
   //	// odd
   //	else
   //		return (size_t)((current) / 2.0f) - 1;
   // };

   // std::vector<std::shared_ptr<BfTriangle>> out;
   // out.reserve(get_triangle_count(v.size()) * 2);

   // for (size_t i = 0; i < get_triangle_count(v.size()) -1; i++) {
   //	auto t = std::make_shared<BfTriangle>(
   //		v[0 + i * 2],
   //		v[1 + i * 2],
   //		v[2 + i * 2]
   //	);
   //	out.emplace_back(t);
   //	if (i == get_triangle_count(v.size()) - 2) {
   //		int a = 10;
   //	}
   // }

   // return out;

   //// ���������� ������� ������ ��� ���������� Triangle
   // std::vector<double> points;
   // std::vector<int> segments;

   // for (const auto& vertex : v) {
   //	points.push_back(static_cast<double>(vertex.pos.x));
   //	points.push_back(static_cast<double>(vertex.pos.y));
   //	points.push_back(static_cast<double>(vertex.pos.z));
   // }

   //// ����� ��� ������������
   // char options[] = "p";

   //// ������� ��������� triangulateio ��� ������� ������ � �����������
   // struct triangulateio in, out;

   //// �������������� ��������� triangulateio
   // memset(&in, 0, sizeof(struct triangulateio));
   // memset(&out, 0, sizeof(struct triangulateio));

   //// ��������� ��������� in �������
   // in.numberofpoints = points.size() / 3;
   // in.pointlist = &points[0];

   //// ��������� ������������
   // triangulate(options, &in, &out, NULL);

   //// ���������� ������������ �������� � ��������� out
   // std::vector<std::array<glm::vec3, 3>> triangles;
   // for (int i = 0; i < out.numberoftriangles; ++i) {
   //	std::array<glm::vec3, 3> triangle;
   //	for (int j = 0; j < 3; ++j) {
   //		int vertexIndex = out.trianglelist[i * 3 + j];
   //		triangle[j] = glm::vec3(
   //			static_cast<float>(out.pointlist[vertexIndex * 2]),
   //			static_cast<float>(out.pointlist[vertexIndex * 2 + 1]),
   //			0.0f // ����� ��������������, ��� ������������ ���������
   // � ��������� XY
   //		);
   //	}
   //	triangles.push_back(triangle);
   // }

   //// ����������� ������, ���������� ��� �����������
   // free(out.pointlist);
   // free(out.trianglelist);

   return std::vector< std::shared_ptr< BfTriangle > >();
}

glm::vec3
bfMathFindMassCenter(std::vector< BfVertex3 > v)
{
   return glm::vec3(0.0f);
}

std::vector< float >
bfMathGetRelativeSplineArgument(const std::vector< glm::vec3 >& v)
{
   size_t n = v.size() - 1;

   std::vector< float > t;
   t.reserve(v.size());
   t.emplace_back(0.0f);

   float total = 0.0f;
   for (size_t i = 0; i < n; i++)
   {
      float chord = glm::distance(v[i], v[i + 1]);
      total += chord;
   }

   float acc_len = 0.0f;
   for (size_t i = 0; i < n; i++)
   {
      float chord = glm::distance(v[i], v[i + 1]);
      acc_len += chord;
      t.emplace_back(acc_len / total);
   }

   return t;
}

std::vector< glm::vec2 >
bfMathSplineFit(const std::vector< float >& x, const std::vector< float >& y)
{
   if (x.size() != y.size())
      abort();
   std::vector< SplineLib::Vec2f > p;
   std::vector< glm::vec2 > p2;
   p.reserve(x.size());
   p2.reserve(x.size());
   for (size_t i = 0; i < x.size(); i++)
   {
      p.emplace_back(SplineLib::Vec2f(x[i], y[i]));
      p2.emplace_back(glm::vec2{x[i], y[i]});
   }

   SplineLib::cSpline2 splines[x.size()];

   // int numSplines = SplineLib::SplinesFromPoints(p.size(), p.data(), p.size()
   // + 1, splines);

   int numSplines =
       SplineLib::SplinesFromPoints(p.size(), p.data(), p.size() + 1, splines);

   std::vector< glm::vec2 > out;

   for (int i = 0; i < numSplines; i++)
   {
      for (float t = 0.0f; t <= 1.0f; t += 0.01)
      {
         SplineLib::Vec2f p = SplineLib::Position(splines[i], t);
         out.emplace_back(glm::vec2{p.x, p.y});
      }
   }
   return out;
}

std::vector< SplineLib::cSpline3 >
bfMathSplineFitExternal3D(const std::vector< BfVertex3 >& v)
{
   std::vector< SplineLib::Vec3f > spl_df_v;
   spl_df_v.reserve(v.size());

   for (auto vert = v.begin(); vert != v.end(); ++vert)
   {
      spl_df_v.emplace_back(
          SplineLib::Vec3f(vert->pos.x, vert->pos.y, vert->pos.z)
      );
   }

   SplineLib::cSpline3 splines[v.size()];

   size_t splines_count = SplineLib::SplinesFromPoints(
       v.size(),
       spl_df_v.data(),
       v.size() + 1,
       splines
   );

   std::vector< SplineLib::cSpline3 > o;
   o.reserve(splines_count);

   for (size_t i = 0; i < splines_count; ++i)
   {
      o.emplace_back(splines[i]);
   }
   return o;
}

std::array< glm::vec3, 4 >
bfMathFindTangentLines(const BfCircle& _c1, const BfCircle& _c2)
{
   glm::vec3 c1 = _c1.get_center().pos;
   glm::vec3 c2 = _c2.get_center().pos;
   float r1 = _c1.get_radius();
   float r2 = _c2.get_radius();

   glm::vec3 d_vec = c2 - c1;
   bfAssert(d_vec.z == c1.z == c1.z);

   float theta = glm::acos((r1 - r2) / glm::length(d_vec));

   glm::vec3 u = glm::normalize(d_vec);
   glm::vec3 v = glm::cross(u, _c2.get_center().normals);

   return {
       c1 + r1 * (glm::cos(theta) * u + glm::sin(theta) * v),
       c2 + r2 * (glm::cos(theta) * u + glm::sin(theta) * v),
       c1 + r1 * (glm::cos(theta) * u - glm::sin(theta) * v),
       c2 + r2 * (glm::cos(theta) * u - glm::sin(theta) * v),
   };
}

std::array< glm::vec3, 4 >
bfMathFindTangentLinesDiscrete(const BfCircle& c1, const BfCircle& c2)
{
   std::array< glm::vec3, 4 > vert{bfMathFindTangentLines(c1, c2)};
   std::array< glm::vec3, 4 > out{
       glm::vec3{FLT_MAX},
       glm::vec3{FLT_MAX},
       glm::vec3{FLT_MAX},
       glm::vec3{FLT_MAX}
   };

   for (size_t i = 0; i < 4; ++i)
   {
      float minDist1 = FLT_MAX;
      float minDist2 = FLT_MAX;

      for (const auto& v : c1.vertices())
      {
         float dist = glm::distance(v.pos, vert[i]);
         if (dist < minDist1)
         {
            minDist1 = dist;
            out[i] = v.pos;
         }
      }
      for (const auto& v : c2.vertices())
      {
         float dist = glm::distance(v.pos, vert[i]);
         if (dist < minDist2)
         {
            minDist2 = dist;
            out[i] = v.pos;
         }
      }
   }

   return out;
}

bool
bfMathFindBezierNormals(
    BfBezierCurve* curve, float t, float r, std::array< BfVertex3, 2 >& vert
)
{
   try
   { // clang-format off
      auto v = curve->calcBfV3(t);
      auto norm = curve->calcNormal(t);
      vert[0] = {v.pos + r * norm.pos, v.color, v.normals};
      vert[1] = {v.pos - r * norm.pos, v.color, v.normals};
   } // clang-format on
   catch (...)
   {
      return false;
   }
   return true;
}

bool
bfMathCreateBezier2From2Lines(
    const BfSingleLine& l1, const BfSingleLine& l2, BfBezierCurve& c
)
{ // clang-format off
   //
   if (!bfMathIsSingleLinesInPlain(l1, l2)) { 
      return false;
   }

   if (c.n() != 2) { 
      return false; 

   }

   auto back_intersection = bfMathFindLinesIntersection(l1, l2, BF_MATH_FIND_LINES_INTERSECTION_ANY);

   if (std::isnan(back_intersection.x) || 
       std::isnan(back_intersection.y) || 
       std::isnan(back_intersection.z)) { 
      return false;
   }

   BfVertex3 average_l1{
       (l1.dVertices().at(0).pos + l1.dVertices().at(1).pos) * 0.5f,
        l1.dVertices().at(0).color,
        l1.dVertices().at(0).normals,
   };
   BfVertex3 average_l2{
       (l2.dVertices().at(0).pos + l2.dVertices().at(1).pos) * 0.5f,
        l2.dVertices().at(0).color,
        l2.dVertices().at(0).normals,
   };

   c.dVertices().clear();
   c.dVertices().emplace_back(std::move(average_l1));
   c.dVertices().emplace_back(BfVertex3{back_intersection, l2.dVertices().at(0).color, l2.dVertices().at(0).normals});
   c.dVertices().emplace_back(std::move(average_l2));

   return true;

} // clang-format on

glm::mat4
bfOrtho(float right, float left, float bot, float top, float far, float near)
{
   glm::mat4 m(1.0f);
   // m[0][0] = 2.0f / (right - left);
   // m[1][1] = 2.0f / (top - bot);
   // m[2][2] = -2.0f / (far - near);
   // m[0][3] = -(right + left) / (right - left);
   // m[1][3] = -(top + bot) / (top - bot);
   // m[2][3] = -(far + near) / (far - near);
   // m[3][3] = 1;
   return m;
}

//
// std::vector<BfVertex3> bfMathSplineFitExternal3D(
// 	const std::vector<BfVertex3>& v
// ) {
//
//
//
// }

// std::vector<glm::vec3> BfMathSplineFit(const std::vector<glm::vec3>&
// controlPoints, int numPoints) { 	std::vector<glm::vec3> splinePoints;
// for
//(float t = 0.0f; t <= inputPoints.size() - 3; t += 0.1f) { glm::vec3
// splinePoint = glm::catmullRom(
// inputPoints[static_cast<int>(t)],
// inputPoints[static_cast<int>(t) + 1],
//			inputPoints[static_cast<int>(t) + 2],
//			inputPoints[static_cast<int>(t) + 3],
//			t - static_cast<int>(t)
//		);
//		splinePoints.push_back(splinePoint);
//	}
//
//	return splinePoints;
// }

BfBezierCurve::BfBezierCurve(size_t in_n, size_t in_m)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_BEZIER_CURVE)
    , __n{in_n}
    , __out_vertices_count{in_m}
{
}

BfBezierCurve::BfBezierCurve(
    size_t in_n, size_t in_m, std::vector< BfVertex3 >&& dvert
)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_BEZIER_CURVE)
    , __n{in_n}
    , __out_vertices_count{in_m}
{
   // Input bezier data is incorrect: vec.size() != in_n
   bfAssert(in_n + 1 == dvert.size());
   // Not all vertices in one plane of Bezier curve
   bfAssert(bfMathIsVerticesInPlain(dvert));

   __dvertices = std::move(dvert);
}

BfBezierCurve::BfBezierCurve(
    size_t in_n, size_t in_m, const std::vector< BfVertex3 >& dvert
)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_BEZIER_CURVE)
    , __n{in_n}
    , __out_vertices_count{in_m}
{
   // Input bezier data is incorrect: vec.size() != in_n
   bfAssert(in_n + 1 == dvert.size());
   // Not all vertices in one plane of Bezier curve
   bfAssert(bfMathIsVerticesInPlain(dvert));

   __dvertices = dvert;
}

BfBezierCurve::BfBezierCurve(BfBezierCurve&& ncurve) noexcept
    : BfDrawObj(BF_DRAW_OBJ_TYPE_BEZIER_CURVE)
    , __n{ncurve.__n}
    , __out_vertices_count{ncurve.__out_vertices_count}
{
   this->__dvertices = std::move(ncurve.__dvertices);
   this->__main_color = ncurve.__main_color;
   this->__pPipeline = ncurve.__pPipeline;
}

BfBezierCurve::BfBezierCurve(const BfBezierCurve& ncurve)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_BEZIER_CURVE)
    , __n{ncurve.__n}
    , __out_vertices_count{ncurve.__out_vertices_count}
{
   this->__dvertices = ncurve.__dvertices;
   this->__main_color = ncurve.__main_color;
   this->__pPipeline = ncurve.__pPipeline;
}

const size_t
BfBezierCurve::n() const noexcept
{
   return __n;
}

glm::vec3
BfBezierCurve::calcV3(float t) const
{
   glm::vec3 _v{0.0f, 0.0f, 0.0f};
   for (size_t i = 0; i <= __n; i++)
   {
      _v += static_cast< float >(
                bfMathGetBinomialCoefficient(__n, i) *
                std::pow(1 - t, __n - i) * std::pow(t, i)
            ) *
            __dvertices[i].pos;
   }
   return _v;
}

BfVertex3
BfBezierCurve::calcBfV3(float t) const
{
   BfVertex3 _v{};
   _v.pos = this->calcV3(t);
   _v.color = __main_color;
   _v.normals = bfMathGetNormal(
       __dvertices[0].pos,
       __dvertices[1].pos,
       __dvertices[2].pos
   );

   return _v;
}

BfBezierCurve
BfBezierCurve::get_derivative()
{
   uint32_t k = __n - 1;
   std::vector< BfVertex3 > new_define_vertices;
   new_define_vertices.reserve(k + 1);

   for (int i = 0; i <= k; i++)
   {
      new_define_vertices.emplace_back(
          static_cast< float >(__n) *
              (__dvertices[i + 1].pos - __dvertices[i].pos),
          this->__main_color,
          __dvertices[0].normals
      );
   }

   BfBezierCurve curve(k, this->__out_vertices_count, new_define_vertices);

   return curve;
}

void
BfBezierCurve::extend(BfBezierCurve* curve)
{
   __dvertices.reserve(__dvertices.size() + curve->__dvertices.size());
   size_t lastIndex = __dvertices.size() - 1 >= 0 ? __dvertices.size() - 1 : 0;
   for (int j = 0; j < curve->__dvertices.size(); ++j)
   {
      // TODO: Fix normals / color assignment
      __dvertices.push_back(
          {__dvertices[lastIndex].pos +
               0.5f * (curve->__dvertices[j].pos - __dvertices[lastIndex].pos),
           __dvertices[lastIndex].color,
           __dvertices[lastIndex].normals}
      );
   }
}

BfVertex3
BfBezierCurve::calcTangent(float t) const
{
   // return get_single_derivative_1_analyt_bfv3(t);
   return get_single_derivative_1_numeric_v3(t);
}

BfVertex3
BfBezierCurve::calcNormal(float t) const
{
   auto v = calcBfV3(t);
   auto norm = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), v.normals) *
               glm::vec4(glm::normalize(calcTangent(t).pos), 1.0f);
   return BfVertex3{norm, v.color, v.normals};
}

glm::vec3
BfBezierCurve::get_single_derivative_1_analyt_v3(float t) const
{
   int k = __n - 1;
   glm::vec3 _v{0.0f};
   for (int i = 0; i <= k; i++)
   {
      _v += (float)(bfMathGetBinomialCoefficient(k, i) *
                    std::pow(1 - t, k - i) * std::pow(t, i) * __n) *
            (__dvertices[i + 1].pos - __dvertices[i].pos);
   }
   return _v;
}

BfVertex3
BfBezierCurve::get_single_derivative_1_analyt_bfv3(float t) const
{
   BfVertex3 _v{};
   _v.pos = this->get_single_derivative_1_analyt_v3(t);
   _v.color = __main_color;
   _v.normals = bfMathGetNormal(
       __dvertices[0].pos,
       __dvertices[1].pos,
       __dvertices[2].pos
   );
   return _v;
}

glm::vec3
BfBezierCurve::get_single_derivative_1_numeric_v3(float t, float step) const
{
   glm::vec3 left = this->calcV3(t - step);
   glm::vec3 right = this->calcV3(t + step);

   return (right - left) / 2.0f / step;
}

glm::vec3
BfBezierCurve::get_single_derivative_2_analyt_v3(float t) const
{
   int k = __n - 2;
   glm::vec3 _v{0.0f};
   for (int i = 0; i <= k; i++)
   {
      _v += (float)((bfMathGetBinomialCoefficient(k, i) *
                     std::pow(1 - t, k - i) * std::pow(t, i) * __n) *
                    ((__n - 1))) *
            (__dvertices[i].pos - 2.0f * __dvertices[i + 1].pos +
             __dvertices[i + 2].pos);
   }
   return _v;
}

BfVertex3
BfBezierCurve::get_single_derivative_2_analyt_bfv3(float t) const
{
   BfVertex3 _v{};
   _v.pos = this->get_single_derivative_2_analyt_v3(t);
   _v.color = __main_color;
   _v.normals = bfMathGetNormal(
       __dvertices[0].pos,
       __dvertices[1].pos,
       __dvertices[2].pos
   );
   return _v;
}

void
BfBezierCurve::createVertices()
{
   __vertices.clear();

   float t;
   for (int i = 0; i < __out_vertices_count; i++)
   {
      t = static_cast< float >(i) /
          static_cast< float >(__out_vertices_count - 1);
      __vertices.emplace_back(this->calcBfV3(t));
   }
}

BfCircle::BfCircle(size_t m, const BfVertex3& center, float radius)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_CIRCLE)
    , __out_vertices_count{m}
    , __radius{radius}
    , __define_type{BF_CIRCLE_DEFINE_TYPE_CENTER_RADIUS}
{
   __dvertices = {center};
   __vertices.reserve(__out_vertices_count);
   __indices.reserve(__out_vertices_count * 2);
}

BfCircle::BfCircle(size_t m, BfVertex3* center, float radius)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_CIRCLE)
    , __radius{radius}
    , __out_vertices_count{m}
    , __define_type{BF_CIRCLE_DEFINE_TYPE_CENTER_RADIUS}
{
   __depMode = BfDrawObjDependencies_Mode_Ptr;
   __pdvertices = {center};
   __vertices.reserve(__out_vertices_count * 3);
   __indices.reserve(__out_vertices_count * 3);
}

BfCircle::BfCircle(
    size_t m, const BfVertex3& P_1, const BfVertex3& P_2, const BfVertex3& P_3
)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_CIRCLE)
    , __out_vertices_count{m}
    , __define_type{BF_CIRCLE_DEFINE_TYPE_3_VERTICES}
{
   BfSingleLine l_12(P_1, P_2);
   BfSingleLine l_23(P_2, P_3);
   BfSingleLine l_31(P_3, P_1);

   glm::vec3 ave_12 = (P_1.pos + P_2.pos) / 2.0f;
   glm::vec3 ave_23 = (P_2.pos + P_3.pos) / 2.0f;
   glm::vec3 ave_31 = (P_3.pos + P_1.pos) / 2.0f;

   glm::vec3 v_12 = l_12.get_direction_from_start();
   glm::vec3 v_23 = l_23.get_direction_from_start();
   glm::vec3 v_31 = l_31.get_direction_from_start();

   glm::vec3 n_12 = glm::cross(glm::cross(v_12, v_23), v_12);
   glm::vec3 n_23 = glm::cross(glm::cross(v_23, v_12), v_23);
   glm::vec3 n_31 = glm::cross(glm::cross(v_31, v_23), v_31);

   BfSingleLine per_l_12(ave_12, ave_12 + n_12);
   BfSingleLine per_l_23(ave_23, ave_23 + n_23);
   BfSingleLine per_l_31(ave_31, ave_31 + n_31);

   BfVertex3 center;
   center.pos = bfMathFindLinesIntersection(
       per_l_12,
       per_l_23,
       BF_MATH_FIND_LINES_INTERSECTION_ANY
   );
   // TODO
   if (glm::isnan(center.pos.x) || glm::isnan(center.pos.y) ||
       glm::isnan(center.pos.z))
   {
      // throw std::runtime_error("Circle define points are invalid");
      if (P_1.equal(P_2))
      {
         center = BfVertex3(
             {P_1.pos.x + P_2.pos.x / 2.0f,
              P_1.pos.y + P_2.pos.y / 2.0f,
              P_1.pos.z + P_2.pos.z / 2.0f},

             P_1.color,
             P_1.normals
         );
         __radius = glm::distance(P_1.pos, P_2.pos) / 2.0f;
         __dvertices = {center, P_1, P_2, P_3};
      }
      else if (P_2.equal(P_3))
      {
         center = BfVertex3(
             {P_3.pos.x + P_2.pos.x / 2.0f,
              P_3.pos.y + P_2.pos.y / 2.0f,
              P_3.pos.z + P_2.pos.z / 2.0f},

             P_3.color,
             P_3.normals
         );
         __radius = glm::distance(P_2.pos, P_3.pos) / 2.0f;
         __dvertices = {center, P_1, P_2, P_3};
      }
      else if (P_3.equal(P_1))
      {
         center = BfVertex3(
             {P_1.pos.x + P_3.pos.x / 2.0f,
              P_1.pos.y + P_3.pos.y / 2.0f,
              P_1.pos.z + P_3.pos.z / 2.0f},

             P_1.color,
             P_1.normals
         );
         __radius = glm::distance(P_3.pos, P_1.pos) / 2.0f;
         __dvertices = {center, P_1, P_2, P_3};
      }
      return;
   }

   float rad_1 = glm::distance(center.pos, P_1.pos);
   float rad_2 = glm::distance(center.pos, P_1.pos);
   float rad_3 = glm::distance(center.pos, P_1.pos);

   // if (!CHECK_FLOAT_EQUALITY(rad_1, rad_2) ||
   //     !CHECK_FLOAT_EQUALITY(rad_2, rad_3) ||
   //     !CHECK_FLOAT_EQUALITY(rad_3, rad_1))
   // {
   //    throw std::runtime_error("Circle was'n made -> different radious");
   // }

   center.normals = bfMathGetPlaneCoeffs(P_1.pos, P_2.pos, P_3.pos);

   __radius = rad_1;
   __dvertices = {center, P_1, P_2, P_3};
}

const BfVertex3&
BfCircle::get_center() const
{
   switch (__depMode)
   {
   case BfDrawObjDependencies_Mode_No: {
      if (!__dvertices.size())
         throw std::runtime_error("No addded center vertex inside dvertices");
      return __dvertices[0];
   }
   case BfDrawObjDependencies_Mode_Ptr: {
      if (!__pdvertices.size())
         throw std::runtime_error("No addded center vertex inside dvertices");
      return *__pdvertices[0];
   }
   }

   return __dvertices[0];
}

BfVertex3&
BfCircle::get_center()
{
   switch (__depMode)
   {
   case BfDrawObjDependencies_Mode_No: {
      if (!__dvertices.size())
         throw std::runtime_error("No addded center vertex inside dvertices");
      return __dvertices[0];
   }
   case BfDrawObjDependencies_Mode_Ptr: {
      if (!__pdvertices.size())
         throw std::runtime_error("No addded center vertex inside dvertices");
      return *__pdvertices[0];
   }
   }

   return __dvertices[0];
}

const BfVertex3&
BfCircle::get_first() const noexcept
{
   return __dvertices[1];
}
const BfVertex3&
BfCircle::get_second() const noexcept
{
   return __dvertices[2];
}
const BfVertex3&
BfCircle::get_third() const noexcept
{
   return __dvertices[3];
}

const float
BfCircle::get_radius() const noexcept
{
   return __radius;
}

std::array< BfVertex3, 2 >
BfCircle::get_tangent_vert(const BfVertex3& P) const
{
   glm::vec4 plane{
       this->get_center().normals,
       -(this->get_center().normals.x * P.pos.x +
         this->get_center().normals.y * P.pos.y +
         this->get_center().normals.z * P.pos.z)
   };

   if (!bfMathIsVertexInPlain(plane, P.pos))
   {
      throw std::runtime_error(
          "BfCircle::get_tangent_vert - point not in the same plane as "
          "circle"
      );
   }

   BfSingleLine dist{P, this->get_center()};
   float distance = glm::distance(dist.get_first().pos, dist.get_second().pos);

   glm::vec3 dir = dist.get_direction_from_start();

   glm::vec3 c1 = glm::rotate(
                      glm::mat4(1.0f),
                      glm::acos(this->get_radius() / distance),
                      this->get_center().normals
                  ) *
                  glm::vec4(dir, 1.0f);

   glm::vec3 c2 = glm::rotate(
                      glm::mat4(1.0f),
                      -glm::acos(this->get_radius() / distance),
                      this->get_center().normals
                  ) *
                  glm::vec4(dir, 1.0f);

   return {
       BfVertex3{
           {this->get_center().pos - glm::normalize(c1) * __radius},
           {1.0f, 1.0f, 1.0f},
           this->get_center().normals
       },
       BfVertex3{
           {this->get_center().pos - glm::normalize(c2) * __radius},
           {1.0f, 1.0f, 1.0f},
           this->get_center().normals
       }
   };
}

void
BfCircle::createVertices()
{
   if (!__vertices.empty())
      __vertices.clear();

   glm::vec3 orth_1;
   glm::vec3 orth_2;
   glm::vec3 normal = this->get_center().normals;

   if (std::abs(normal.x) > std::abs(normal.y))
   {
      orth_1 = glm::normalize(glm::vec3(-normal.z, 0.0f, normal.x));
   }
   else
   {
      orth_1 = glm::normalize(glm::vec3(0.0f, normal.z, -normal.y));
   }
   orth_2 = glm::normalize(glm::cross(normal, orth_1));

   float theta;
   glm::vec3 center = this->get_center().pos;
   for (size_t i = 0; i < __out_vertices_count + 1; ++i)
   {
      theta = 2 * BF_PI * i / __out_vertices_count;
      __vertices.emplace_back(
          center + __radius * cosf(theta) * orth_1 +
              __radius * sinf(theta) * orth_2,

          this->__main_color,
          normal
      );
   }
}
//
//
//
//
// BfCircleFilled
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
//
//
//
//
//
BfCircleFilled::BfCircleFilled(size_t m, const BfVertex3& center, float radius)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_CIRCLE_FILLED)
    , m_radius(radius)
    , m_outVerticesCount(m)
{
   __depMode = BfDrawObjDependencies_Mode_No;
   __dvertices.push_back(center);
   __vertices.push_back(center);
   __vertices.reserve(m * 3);
   __indices.reserve(m * 3);
}

BfCircleFilled::BfCircleFilled(size_t m, BfVertex3* center, float radius)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_CIRCLE_FILLED)
    , m_radius(radius)
    , m_outVerticesCount(m)
{
   __depMode = BfDrawObjDependencies_Mode_Ptr;
   __pdvertices.push_back(center);
   __vertices.reserve(m * 3);
   __indices.reserve(m * 3);
}

float
BfCircleFilled::radius() const noexcept
{
   return m_radius;
}

const glm::vec3&
BfCircleFilled::tangent() const noexcept
{
   return _center().normals;
}

void
BfCircleFilled::createVertices()
{
   if (!__vertices.empty())
      __vertices.clear();

   glm::vec3 orth_1;
   glm::vec3 orth_2;
   glm::vec3 normal = _center().normals;

   if (std::abs(normal.x) > std::abs(normal.y))
   {
      orth_1 = glm::normalize(glm::vec3(-normal.z, 0.0f, normal.x));
   }
   else
   {
      orth_1 = glm::normalize(glm::vec3(0.0f, normal.z, -normal.y));
   }
   orth_2 = glm::normalize(glm::cross(normal, orth_1));

   float theta;
   glm::vec3 center = _center().pos;
   for (size_t i = 0; i < m_outVerticesCount + 1; ++i)
   { // clang-format off
      theta = 2 * BF_PI * i / m_outVerticesCount;
      __vertices.emplace_back(
          center + m_radius * cosf(theta) * orth_1
                 + m_radius * sinf(theta) * orth_2,
          this->__main_color,
          normal
      );
   } // clang-format on
}

void
BfCircleFilled::createIndices()
{
   if (!__indices.empty())
      __indices.clear();
   for (size_t i = 1; i < __vertices.size() - 2; ++i)
   {
      __indices.emplace_back(0);
      __indices.emplace_back(i);
      __indices.emplace_back(i + 1);
   }
}

const BfVertex3&
BfCircleFilled::_center() const
{
   switch (__depMode)
   {
   case BfDrawObjDependencies_Mode_No: {
      if (!__dvertices.size())
         throw std::runtime_error("No addded center vertex inside dvertices");
      return __dvertices[0];
   }
   case BfDrawObjDependencies_Mode_Ptr: {
      if (!__pdvertices.size())
         throw std::runtime_error("No addded center vertex inside dvertices");
      return *__pdvertices[0];
   }
   }

   return __dvertices[0];
}

BfVertex3&
BfCircleFilled::_center()
{
   switch (__depMode)
   {
   case BfDrawObjDependencies_Mode_No: {
      if (!__dvertices.size())
         throw std::runtime_error("No addded center vertex inside dvertices");
      return __dvertices[0];
   }
   case BfDrawObjDependencies_Mode_Ptr: {
      if (!__pdvertices.size())
         throw std::runtime_error("No addded center vertex inside dvertices");
      return *__pdvertices[0];
   }
   }

   return __dvertices[0];
}

BfCircleFilledWithHandles::BfCircleFilledWithHandles(
    size_t m, const BfVertex3& center, float radius
)
{
   auto circle = std::make_shared< BfCircle >(m, center, radius);
   circle->createVertices();
   circle->createIndices();
   add_l(circle);
   circle->bind_pipeline(
       BfPipelineHandler::instance()->getPipeline(BfPipelineType_Lines)
   );

   auto center_handle = std::make_shared< BfCircleFilled >(
       m,
       &circle->dVertices().at(0),
       BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS
   );
   center_handle->createVertices();
   center_handle->createIndices();
   center_handle->bind_pipeline(
       BfPipelineHandler::instance()->getPipeline(BfPipelineType_Triangles)
   );
   add_l(center_handle);

   m_rHandleVert = circle->vertices().at(0);
   auto r_handle = std::make_shared< BfCircleFilled >(
       m,
       &m_rHandleVert,
       BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS
   );
   r_handle->createVertices();
   r_handle->createIndices();
   r_handle->bind_pipeline(
       BfPipelineHandler::instance()->getPipeline(BfPipelineType_Triangles)
   );
   add_l(r_handle);

   createVertices();
}
BfCircleFilledWithHandles::BfCircleFilledWithHandles(
    size_t m, BfVertex3* center, float radius
)
{
   auto circle = std::make_shared< BfCircle >(m, center, radius);
   circle->createVertices();
   circle->createIndices();
   add_l(circle);
   circle->bind_pipeline(
       BfPipelineHandler::instance()->getPipeline(BfPipelineType_Lines)
   );

   auto center_handle = std::make_shared< BfCircleFilled >(
       m,
       center,
       BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS
   );
   center_handle->createVertices();
   center_handle->createIndices();
   center_handle->bind_pipeline(
       BfPipelineHandler::instance()->getPipeline(BfPipelineType_Triangles)
   );
   add_l(center_handle);

   m_rHandleVert = circle->vertices().at(0);
   auto r_handle = std::make_shared< BfCircleFilled >(
       m,
       &m_rHandleVert,
       BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS
   );
   r_handle->createVertices();
   r_handle->createIndices();
   r_handle->bind_pipeline(
       BfPipelineHandler::instance()->getPipeline(BfPipelineType_Triangles)
   );
   add_l(r_handle);

   createVertices();
}

std::shared_ptr< BfCircle >
BfCircleFilledWithHandles::circle() noexcept
{
   return std::dynamic_pointer_cast< BfCircle >(this->get_object_by_index(0));
}

std::shared_ptr< BfCircleFilled >
BfCircleFilledWithHandles::centerHandle() noexcept
{
   return std::dynamic_pointer_cast< BfCircleFilled >(
       this->get_object_by_index(1)
   );
}

std::shared_ptr< BfCircleFilled >
BfCircleFilledWithHandles::radiusHandle() noexcept
{
   return std::dynamic_pointer_cast< BfCircleFilled >(
       this->get_object_by_index(2)
   );
}

void
BfCircleFilledWithHandles::createVertices()
{
   for (size_t i = 0; i < this->get_obj_count(); ++i)
   {
      auto obj = this->get_object_by_index(i);
      obj->createVertices();
      obj->createIndices();
   }
   this->update_buffer();
}

// void
// BfCircleFilledWithHandles::update()
// {
//    BfDrawLayer::update();
// }

//
//
//
//
// /BfCircleFilled
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
//
//
//
//
//

BfArc::BfArc(
    size_t m, const BfVertex3& P_1, const BfVertex3& P_2, const BfVertex3& P_3
)
    : BfCircle(m, P_1, P_2, P_3)
{
}

void
BfArc::createVertices()
{
   if (!__vertices.empty())
      __vertices.clear();

   glm::vec3 center = this->get_center().pos;
   glm::vec3 firstPoint = this->get_first().pos;
   glm::vec3 secondPoint = this->get_second().pos;
   glm::vec3 thirdPoint = this->get_third().pos;

   glm::vec3 vecToFirst = firstPoint - center;
   glm::vec3 vecToSecond = secondPoint - center;
   glm::vec3 vecToThird = thirdPoint - center;

   float angle_12 = acos(
       glm::dot(vecToFirst, vecToSecond) /
       (glm::length(vecToFirst) * glm::length(vecToSecond))
   );

   float angle_23 = acos(
       glm::dot(vecToSecond, vecToThird) /
       (glm::length(vecToSecond) * glm::length(vecToThird))
   );

   float theta;
   // glm::vec3 center = this->get_center().pos;
   for (size_t i = 0; i <= __out_vertices_count; ++i)
   {
      float theta = (angle_12 + angle_23) * i / (__out_vertices_count);

      __vertices.emplace_back(
          center - glm::vec3(
                       glm::rotate(
                           glm::mat4(1.0f),
                           theta,
                           this->get_center().normals
                       ) *
                       glm::vec4((center - firstPoint), 1.0f)
                   ),

          this->__main_color,
          glm::vec3(0.0f, 0.0f, 0.0f)
      );
   }
}
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

BfCone::BfCone(
    size_t m, const BfVertex3& tip, const BfVertex3& center, float radius
)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_CONE)
    , __out_vertices_count{m}
    , __base_radius{radius}
{
   __dvertices.push_back(tip);
   __dvertices.push_back(center);
}

BfCone::BfCone(size_t m, const BfVertex3& tip, float height, float radius)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_CONE)
    , __out_vertices_count{m}
    , __base_radius{radius}
{
   __dvertices.push_back(tip);
   glm::vec3 normolized = glm::normalize(tip.normals);
   __dvertices.push_back({
       {
           tip.pos.x + height * normolized.x,
           tip.pos.y + height * normolized.y,
           tip.pos.z + height * normolized.z,
       },
       tip.color,
       tip.normals,
   });
}

const BfVertex3&
BfCone::centerVertex()
{
   return __dvertices.at(1);
}
const BfVertex3&
BfCone::tipVertex()
{
   return __dvertices.at(0);
}

void
BfCone::createVertices()
{
   if (!__vertices.empty())
      __vertices.clear();

   auto baseCircle = std::make_shared< BfCircle >(
       __out_vertices_count - 1,
       this->centerVertex(),
       __base_radius
   );
   baseCircle->createVertices();
   __vertices.reserve((__out_vertices_count - 1) * 3.0f);
   const glm::vec3& p2 = this->tipVertex().pos;
   for (size_t i = 0; i < baseCircle->vertices().size() - 1; ++i)
   {
      const glm::vec3& p1 = baseCircle->vertices()[i].pos;
      const glm::vec3& p3 = baseCircle->vertices()[i + 1].pos;
      const glm::vec3 normal = bfMathGetNormal(p1, p2, p3);
      __vertices.emplace_back(BfVertex3{p1, __main_color, normal});
      __vertices.emplace_back(BfVertex3{p2, __main_color, normal});
      __vertices.emplace_back(BfVertex3{p3, __main_color, normal});
   }
}

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

BfTube::BfTube(
    size_t m,
    const BfVertex3& begin,
    const BfVertex3& end,
    float radius_begin,
    float radius_end
)
    : __out_m{m}, __radius_begin{radius_begin}, __radius_end{radius_end}
{
   // ��������, ��� ������� ���������� � ���� ������� (��� ����� �����)
   bfAssert(glm::length2(glm::cross(begin.pos, end.pos)) < 1e-6f);
   __dvertices.push_back(begin);
   __dvertices.push_back(end);
}

const BfVertex3&
BfTube::beginCenterVertex()
{
   return __dvertices[0];
}
const BfVertex3&
BfTube::endCenterVertex()
{
   return __dvertices[1];
}

void
BfTube::createVertices()
{
   // clang-format off
   auto beginCircle = std::make_shared<BfCircle>(
      __out_m, 
      beginCenterVertex(), 
      __radius_begin
   );
   beginCircle->createVertices();
   auto endCircle = std::make_shared<BfCircle>(
      __out_m, 
      endCenterVertex(), 
      __radius_end
   );
   endCircle->createVertices();
   // clang-format on
   __vertices.reserve(__out_m * 4);
   bfAssert(beginCircle->vertices().size() == endCircle->vertices().size());
   for (size_t i = 0; i < beginCircle->vertices().size() - 1; ++i)
   {
      const glm::vec3& p1 = beginCircle->vertices()[i].pos;
      const glm::vec3& p2 = beginCircle->vertices()[i + 1].pos;
      const glm::vec3& p3 = endCircle->vertices()[i].pos;
      const glm::vec3& p4 = endCircle->vertices()[i + 1].pos;
      const glm::vec3 normal_1 = bfMathGetNormal(p1, p2, p3);
      const glm::vec3 normal_2 = bfMathGetNormal(p3, p4, p2);
      bfAssert(glm::length2(glm::cross(normal_1, normal_2)) < 1e-6f);
      __vertices.emplace_back(BfVertex3{p1, __main_color, normal_1});
      __vertices.emplace_back(BfVertex3{p2, __main_color, normal_1});
      __vertices.emplace_back(BfVertex3{p3, __main_color, normal_1});
      __vertices.emplace_back(BfVertex3{p4, __main_color, normal_1});
   }
}

void
BfTube::createIndices()
{
   if (!__indices.empty())
      __indices.clear();
   for (int i = 0; i < __out_m * 4 - 3; ++i)
   {
      __indices.emplace_back(i + 0);
      __indices.emplace_back(i + 1);
      __indices.emplace_back(i + 2);

      __indices.emplace_back(i + 1);
      __indices.emplace_back(i + 3);
      __indices.emplace_back(i + 2);
   }
}

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

BfDoubleTube::BfDoubleTube()
    : BfDrawObj(BF_DRAW_OBJ_TYPE_DOUBLE_TUBE)
{
}

BfDoubleTube::BfDoubleTube(
    size_t m,
    const BfVertex3& begin,
    const BfVertex3& end,
    float radius_out_begin,
    float radius_in_begin,
    float radius_out_end,
    float radius_in_end
)
    : BfDrawObj(BF_DRAW_OBJ_TYPE_DOUBLE_TUBE)
    , __out_m{m}
    , __radius_in_begin{radius_in_begin}
    , __radius_out_begin{radius_out_begin}
    , __radius_in_end{radius_in_end}
    , __radius_out_end{radius_out_end}
{
   __dvertices.push_back(begin);
   __dvertices.push_back(end);
}

const BfVertex3&
BfDoubleTube::beginCenterVertex()
{
   return __dvertices[0];
}
const BfVertex3&
BfDoubleTube::endCenterVertex()
{
   return __dvertices[1];
}

void
BfDoubleTube::createVertices()
{
   static auto mksc = [](auto m, auto v, auto r) {
      auto tmp = std::make_shared< BfCircle >(m, v, r);
      tmp->createVertices();
      return tmp;
   };
   auto beginInCircle = mksc(__out_m, beginCenterVertex(), __radius_in_begin);
   auto beginOutCircle = mksc(__out_m, beginCenterVertex(), __radius_out_begin);
   auto endInCircle = mksc(__out_m, endCenterVertex(), __radius_in_end);
   auto endOutCircle = mksc(__out_m, endCenterVertex(), __radius_out_end);
   __vertices.reserve(__out_m * 2);
   for (size_t i = 0; i < __out_m; i += 2)
   // for (size_t i = 0; i < 1; i += 2)
   {
      const glm::vec3& p1 = beginInCircle->vertices()[i].pos;
      const glm::vec3& p2 = beginOutCircle->vertices()[i].pos;
      const glm::vec3& p3 = endInCircle->vertices()[i].pos;
      const glm::vec3& p4 = endOutCircle->vertices()[i].pos;

      const glm::vec3& p1_ = beginInCircle->vertices()[i + 1].pos;
      const glm::vec3& p2_ = beginOutCircle->vertices()[i + 1].pos;
      const glm::vec3& p3_ = endInCircle->vertices()[i + 1].pos;
      const glm::vec3& p4_ = endOutCircle->vertices()[i + 1].pos;
      // clang-format off
      const glm::vec3 toCenterNormal = glm::normalize(bfMathGetNormal(p1, p3, p3_));
      // std::cout << STRVEC3(toCenterNormal) << "\n";
#if defined(NDEBUG)
      const glm::vec3 toCetnerNormal_ = glm::normalize(bfMathGetNormal(p3_, p3, p1));
      bfAssert(glm::length2(glm::cross(toCenterNormal_, toCenterNormal)) < 1e-6f);
#endif
      const glm::vec3 fromCenterNormal = glm::normalize(bfMathGetNormal(p2, p4, p4_));
      // std::cout << STRVEC3(fromCenterNormal ) << "\n";
#if defined(NDEBUG)
      const glm::vec3 fromCenterNormal_ = glm::normalize(bfMathGetNormal(p4_, p2_, p2));
      bfAssert(glm::length2(glm::cross(fromCenterNormal_, fromCenterNormal)) < 1e-6f);
#endif
      const glm::vec3 backwordsCenterNormal = glm::normalize(bfMathGetNormal(p1, p2, p2_));
      // std::cout << STRVEC3(backwordsCenterNormal ) << "\n";
#if defined(NDEBUG)
      const glm::vec3 backwordsCetnerNormal_ = glm::normalize(bfMathGetNormal(p2_, p1_, p1));
      bfAssert(glm::length2(glm::cross(backwordsCenterNormal_, backwordsCenterNormal)) < 1e-6f);
#endif
      const glm::vec3 towardsCenterNormal = glm::normalize(bfMathGetNormal(p3, p4, p4_));
      // std::cout << STRVEC3(towardsCenterNormal ) << "\n";
#if defined(NDEBUG)
      const glm::vec3 towardsCenterNormal_ = glm::normalize(bfMathGetNormal(p4_, p3_, p3));
      bfAssert(glm::length2(glm::cross(towardsCenterNormal_, towardsCenterNormal)) < 1e-6f);
#endif
      __vertices.emplace_back(BfVertex3{p1, __main_color, toCenterNormal});            //   | 0
      __vertices.emplace_back(BfVertex3{p1, __main_color, backwordsCenterNormal});     // 0' | 1
      __vertices.emplace_back(BfVertex3{p2, __main_color, fromCenterNormal});          // 1  | 2
      __vertices.emplace_back(BfVertex3{p2, __main_color, backwordsCenterNormal});     // 1' | 3
      //
      __vertices.emplace_back(BfVertex3{p3, __main_color, toCenterNormal});            // 2  | 4
      __vertices.emplace_back(BfVertex3{p3, __main_color, towardsCenterNormal});       // 2' | 5
      __vertices.emplace_back(BfVertex3{p4, __main_color, fromCenterNormal});          // 3  | 6
      __vertices.emplace_back(BfVertex3{p4, __main_color, towardsCenterNormal});       // 3' | 7

      __vertices.emplace_back(BfVertex3{p1_, __main_color, toCenterNormal});           // 0_ | 8
      __vertices.emplace_back(BfVertex3{p1_, __main_color, backwordsCenterNormal});    // 0_'| 9
      __vertices.emplace_back(BfVertex3{p2_, __main_color, fromCenterNormal});         // 1_ | 10
      __vertices.emplace_back(BfVertex3{p2_, __main_color, backwordsCenterNormal});    // 1_'| 11
      //
      __vertices.emplace_back(BfVertex3{p3_, __main_color, toCenterNormal});           // 2_ | 12
      __vertices.emplace_back(BfVertex3{p3_, __main_color, towardsCenterNormal});      // 2_'| 13
      __vertices.emplace_back(BfVertex3{p4_, __main_color, fromCenterNormal});         // 3_ | 14
      __vertices.emplace_back(BfVertex3{p4_, __main_color, towardsCenterNormal});      // 3_'| 15
         // clang-format on
   }
}

void
BfDoubleTube::createIndices()
{
   if (!__indices.empty())
      __indices.clear();
   for (size_t i = 0; i < __vertices.size() - 8; i += 8)
   {
      // backwords
      __indices.emplace_back(i + 1);
      __indices.emplace_back(i + 3);
      __indices.emplace_back(i + 10);
      __indices.emplace_back(i + 10);
      __indices.emplace_back(i + 9);
      __indices.emplace_back(i + 1);

      // towards
      __indices.emplace_back(i + 5);
      __indices.emplace_back(i + 7);
      __indices.emplace_back(i + 15);
      __indices.emplace_back(i + 15);
      __indices.emplace_back(i + 13);
      __indices.emplace_back(i + 5);

      // to center
      __indices.emplace_back(i + 0);
      __indices.emplace_back(i + 4);
      __indices.emplace_back(i + 8);
      __indices.emplace_back(i + 8);
      __indices.emplace_back(i + 12);
      __indices.emplace_back(i + 4);

      // from center
      __indices.emplace_back(i + 2);
      __indices.emplace_back(i + 6);
      __indices.emplace_back(i + 10);
      __indices.emplace_back(i + 10);
      __indices.emplace_back(i + 6);
      __indices.emplace_back(i + 14);
   }
}

//
//
//
//
//
//
//
//
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
//
//
//
//
//
//
//

BfBezierCurveWithHandles::BfBezierCurveWithHandles(
    size_t in_m, std::vector< BfVertex3 >&& dvert
)
    : BfDrawLayer(
          BfAllocator::get(),
          sizeof(BfVertex3),
          2000,
          20,
          true,
          BF_DRAW_OBJ_TYPE_BEZIER_CURVE_WITH_HANDLES
      )
{
   bfAssert(dvert.size() > 1);
   auto curve = std::make_shared< BfBezierCurve >(
       dvert.size() - 1,
       in_m,
       std::move(dvert)
   );
   this->add_l(curve);
   curve->bind_pipeline(
       BfPipelineHandler::instance()->getPipeline(BfPipelineType_Lines)
   );

   for (size_t i = 0; i < curve->dVertices().size(); i++)
   {
      auto handle = std::make_shared< BfCircleFilled >(
          20,
          &curve->dVertices()[i],
          BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS
      );
      handle->set_color({0.3, 0.5, 0.1});
      handle->bind_pipeline(
          BfPipelineHandler::instance()->getPipeline(BfPipelineType_Triangles)
      );
      this->add_l(handle);
   }
   //
   for (size_t i = 1; i < curve->dVertices().size(); i++)
   {
      auto line = std::make_shared< BfSingleLine >(
          &curve->dVertices()[i],
          &curve->dVertices()[i - 1]
      );
      line->set_color({0.3, 0.5, 0.1});
      line->bind_pipeline(
          BfPipelineHandler::instance()->getPipeline(BfPipelineType_Lines)
      );
      this->add_l(line);
   }

   generate_draw_data();
   update_buffer();
}

BfBezierCurveWithHandles::BfBezierCurveWithHandles(
    size_t in_m, const std::vector< BfVertex3 >& dvert
)
{
   bfAssert(dvert.size() > 1);
   auto curve =
       std::make_shared< BfBezierCurve >(dvert.size() - 1, in_m, dvert);
   this->add_l(curve);

   for (size_t i = 0; i < dvert.size(); i++)
   {
      auto handle = std::make_shared< BfCircleFilled >(
          20,
          curve->dVertices()[i],
          BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS
      );
      handle->bind_pipeline(
          BfPipelineHandler::instance()->getPipeline(BfPipelineType_Triangles)
      );
      this->add_l(handle);
   }

   for (size_t i = 1; i < curve->dVertices().size(); i++)
   {
      auto line = std::make_shared< BfSingleLine >(
          curve->dVertices()[i],
          curve->dVertices()[i - 1]
      );
      line->bind_pipeline(
          BfPipelineHandler::instance()->getPipeline(BfPipelineType_Lines)
      );
      this->add_l(line);
   }

   update_buffer();
}

std::shared_ptr< BfBezierCurve >
BfBezierCurveWithHandles::curve()
{
   return std::dynamic_pointer_cast< BfBezierCurve >(get_object_by_index(0));
}

//
//
//
//
//
//
//
//
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
//
//
//
//
//
//
//

BfBezierCurveFrame::BfBezierCurveFrame(
    std::shared_ptr< BfBezierCurve > curve,
    VmaAllocator allocator,
    VkPipeline lines_pipeline,
    VkPipeline triangle_pipeline
)
    : BfDrawLayer(
          allocator,
          sizeof(BfVertex3),
          50,
          20,
          true,
          BF_DRAW_LAYER_TYPE_BEZIER_FRAME
      )
    , __curve{curve}
    , __lines_pipeline{lines_pipeline}
    , __triangle_pipeline{triangle_pipeline}

{
   for (size_t i = 0; i < curve->dVertices().size(); i++)
   {
      auto handle = std::make_shared< BfCircleFilled >(
          20,
          curve->dVertices()[i],
          BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS
      );
      handle->bind_pipeline(&__triangle_pipeline);
      this->add_l(handle);
   }

   for (size_t i = 1; i < curve->dVertices().size(); i++)
   {
      auto line = std::make_shared< BfSingleLine >(
          curve->dVertices()[i],
          curve->dVertices()[i - 1]
      );
      line->bind_pipeline(&__lines_pipeline);
      this->add_l(line);
   }

   this->generate_draw_data();
   this->update_buffer();
}

std::shared_ptr< BfBezierCurve >
BfBezierCurveFrame::attachedCurve() noexcept
{
   return __curve;
}

void
BfBezierCurveFrame::remake()
{
   for (auto& dvert : __curve->dVertices())
   {
   }

   BfDrawLayer::remake();
}

void
BfBezierCurveFrame::remake(
    std::shared_ptr< BfBezierCurve > curve, glm::vec3 col
)
{
   // this->del_all();
   // TODO: Add check for existing objects (if bezier changed its order)
   for (size_t i = 0; i < __curve->dVertices().size(); i++)
   {
      // auto handle = std::make_shared<BfCircleFilled>(
      //     20,
      //     __curve->dVertices()[i],
      //     BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS
      // );
      // handle->bind_pipeline(&__triangle_pipeline);
      // this->add_l(handle);
   }

   for (size_t i = 1; i < __curve->dVertices().size(); i++)
   {
      auto line = std::make_shared< BfSingleLine >(
          __curve->dVertices()[i],
          __curve->dVertices()[i - 1]
      );
      line->bind_pipeline(&__lines_pipeline);
      this->add_l(line);
   }

   this->generate_draw_data();
   // this->update_buffer();
}

BfCubicSplineCurve::BfCubicSplineCurve(
    size_t out_vertices_count, std::vector< BfVertex3 >& dp
)
    : BfDrawObj{dp}, __out_vertices_count{out_vertices_count}
{
}

BfCubicSplineCurve::BfCubicSplineCurve(
    size_t out_vertices_count, std::vector< glm::vec3 >& dp
)
    : __out_vertices_count{out_vertices_count}
{
   __dvertices.reserve(dp.size());
   for (auto& it : dp)
   {
      __dvertices.emplace_back(
          BfVertex3{it, {1.0, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
      );
   }
}

void
BfCubicSplineCurve::createVertices()
{
   auto calculateSplinePoint = [](const std::vector< glm::vec3 >& a,
                                  const std::vector< glm::vec3 >& b,
                                  const std::vector< glm::vec3 >& c,
                                  const std::vector< glm::vec3 >& d,
                                  float t) {
      size_t n = a.size();
      size_t segment = 0;

      for (size_t i = 1; i < n; ++i)
      {
         if (t < static_cast< float >(i) / static_cast< float >(n - 1))
         {
            segment = i - 1;
            break;
         }
      }

      float tSegment =
          (t - static_cast< float >(segment) / static_cast< float >(n - 1)) *
          (n - 1);

      glm::vec3 splinePoint = a[segment] + b[segment] * tSegment +
                              c[segment] * tSegment * tSegment +
                              d[segment] * tSegment * tSegment * tSegment;

      return splinePoint;
   };

   size_t n = __dvertices.size() - 1;

   /*float total = 0.0f;
   for (size_t i = 0; i < n - 1; i++) {
           float chord = glm::distance(__dvertices[i].pos, __dvertices[i +
   1].pos); total += chord;
   }


   float acc_len = 0.0f;
   for (size_t i = 0; i < n - 1; i++) {
           float chord = glm::distance(__dvertices[i].pos, __dvertices[i +
   1].pos); acc_len += chord; t.emplace_back(acc_len / total);
   }*/

   std::vector< glm::vec3 > a;
   a.reserve(n + 1);
   std::vector< glm::vec3 > t;
   t.reserve(n + 1);

   for (auto& it : __dvertices)
   {
      a.emplace_back(it.pos);
      t.emplace_back(it.pos);
   }

   std::vector< glm::vec3 > b(n);
   std::vector< glm::vec3 > d(n);
   std::vector< glm::vec3 > h(n);

   for (size_t i = 0; i <= n - 1; i++)
   {
      h[i] = t[i + 1] - t[i];
   }

   std::vector< glm::vec3 > alpha(n);
   for (size_t i = 1; i <= n - 1; i++)
   {
      alpha[i] =
          3.0f / h[i] * (a[i + 1] - a[i]) - 3.0f / h[i - 1] * (a[i] - a[i - 1]);
   }

   std::vector< glm::vec3 > c(n + 1);
   std::vector< glm::vec3 > l(n + 1);
   std::vector< glm::vec3 > mu(n + 1);
   std::vector< glm::vec3 > z(n + 1);

   l[0] = glm::vec3(1.0f);

   for (size_t i = 1; i <= n - 1; i++)
   {
      l[i] = 2.0f * (t[i + 1] - t[i - 1]) - h[i - 1] * mu[i - 1];
      mu[i] = h[i] / l[i];
      z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
   }

   l[n] = glm::vec3(1.0f);

   for (size_t j = n - 1; j > 0; j--)
   {
      c[j] = z[j] - mu[j] * c[j + 1];
      b[j] =
          (a[j + 1] - a[j]) / h[j] - (h[j] * (c[j + 1] + 2.0f * c[j])) / 3.0f;
      d[j] = (c[j + 1] - c[j]) / 3.0f / h[j];
   }

   if (!__vertices.empty())
   {
      __vertices.clear();
   }
   __vertices.reserve(__out_vertices_count);

   for (size_t i = 0; i < __out_vertices_count - 1; i++)
   {
      float t = static_cast< float >(i) /
                static_cast< float >(__out_vertices_count - 1);
      glm::vec3 splinePoint = calculateSplinePoint(a, b, c, d, t);
      __vertices.emplace_back(
          BfVertex3{splinePoint, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
      );
   }
}
