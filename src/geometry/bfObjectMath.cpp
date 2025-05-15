#include "bfObjectMath.h"

namespace obj
{
namespace curves
{
namespace math
{

std::vector< BfVertex3 >
calcCircleVertices(
    const BfVertex3& center,
    float radius,
    uint32_t m_discretization,
    const glm::vec3& color
)
{
   glm::vec3 orth_1;
   glm::vec3 orth_2;
   glm::vec3 normal = center.normals;

   if (std::abs(normal.x) > std::abs(normal.y))
   {
      orth_1 = glm::normalize(glm::vec3(-normal.z, 0.0f, normal.x));
   }
   else
   {
      orth_1 = glm::normalize(glm::vec3(0.0f, normal.z, -normal.y));
   }
   orth_2 = glm::normalize(glm::cross(normal, orth_1));

   std::vector< BfVertex3 > v;
   v.reserve(m_discretization);

   for (size_t i = 0; i < m_discretization + 1; ++i)
   {
      float theta = 2 * glm::pi< float >() * i / m_discretization;
      v.emplace_back(
          center.pos + radius * cosf(theta) * orth_1 +
              radius * sinf(theta) * orth_2,
          color,
          normal
      );
   }
   return v;
}

std::vector< BfVertex3 >
calcArcVertices(
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    uint32_t segments,
    const glm::vec3& color
)
{
   std::vector< BfVertex3 > arc;
   if (segments < 2)
      return arc;

   // Compute plane normal
   glm::vec3 v1 = p1 - p0;
   glm::vec3 v2 = p2 - p0;
   glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

   // Calculate circle center from three points
   glm::vec3 u = p1 - p0;
   glm::vec3 v = p2 - p0;

   float a = glm::length(u);
   float b = glm::length(p2 - p1);
   float c = glm::length(v);

   glm::vec3 mid_ab = 0.5f * (p0 + p1);
   glm::vec3 mid_ac = 0.5f * (p0 + p2);

   glm::vec3 dir_ab = glm::normalize(glm::cross(normal, u));
   glm::vec3 dir_ac = glm::normalize(glm::cross(normal, v));

   // Solve intersection of two planes (mid_ab + t*dir_ab == mid_ac + s*dir_ac)
   glm::vec3 n = glm::cross(dir_ab, dir_ac);
   float denom = glm::dot(n, n);
   if (denom < 1e-6f)
      return arc; // degenerate

   glm::mat2 A{
       glm::dot(dir_ab, dir_ab),
       glm::dot(dir_ab, dir_ac),
       glm::dot(dir_ab, dir_ac),
       glm::dot(dir_ac, dir_ac)
   };

   glm::vec2 rhs{
       glm::dot(mid_ac - mid_ab, dir_ab),
       glm::dot(mid_ac - mid_ab, dir_ac)
   };

   float det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
   if (std::abs(det) < 1e-6f)
      return arc;

   float t = (rhs[0] * A[1][1] - rhs[1] * A[0][1]) / det;

   glm::vec3 center = mid_ab + t * dir_ab;
   float radius = glm::length(p0 - center);

   // Create orthonormal basis in the arc plane
   glm::vec3 x_axis = glm::normalize(p0 - center);
   glm::vec3 y_axis = glm::normalize(glm::cross(normal, x_axis));

   // Compute angles
   float angle0 = 0.0f;
   float angle1 =
       std::atan2(glm::dot(p2 - center, y_axis), glm::dot(p2 - center, x_axis));

   // Normalize angle direction
   if (angle1 < angle0)
      angle1 += 2 * glm::pi< float >();

   // Generate vertices
   for (uint32_t i = 0; i <= segments; ++i)
   {
      float t = static_cast< float >(i) / static_cast< float >(segments);
      float theta = angle0 + t * (angle1 - angle0);
      float ct = static_cast< float >(std::cos(theta));
      float st = static_cast< float >(std::sin(theta));
      glm::vec3 pos = center + radius * (ct * x_axis + st * y_axis);
      arc.emplace_back(pos, color, normal);
   }

   return arc;
}

uint32_t
factorial(uint32_t n)
{
   const static std::map< uint32_t, uint32_t > m{
       {1, 1},
       {2, 2},
       {3, 6},
       {4, 24},
       {5, 120},
       {6, 720},
       {7, 5040},
       {8, 40320},
       {9, 362880},
       {10, 3628800},
       {11, 3991680},
       {12, 47900160},
       {13, 622702080},
       {14, 130767436800}
   };

   if (n < 1)
      return 1;
   else if (n > 10)
      throw std::runtime_error("Factorial can't be calculated: n > 10");
   else
   {
      return m.at(n);
   }
}

uint32_t
binomial(uint32_t n, uint32_t k)
{
   return math::factorial(n) / math::factorial(k) / math::factorial(n - k);
}

glm::vec3
centerOfMass(const std::vector< BfVertex3 >& points)
{
   glm::vec3 center(0.0f);
   if (points.empty())
      return center;

   for (const auto& p : points)
   {
      center += p.pos;
   }
   center /= static_cast< float >(points.size());
   return center;
};

xt::xarray< double >
bernsteinRow(int degree, double t)
{
   xt::xarray< double > row = xt::zeros< double >({degree + 1});
   for (int i = 0; i <= degree; ++i)
   {
      double binom = binomial(degree, i);
      row(i) = binom * std::pow(t, i) * std::pow(1 - t, degree - i);
   }
   return row;
}

std::vector< BfVertex3 >
resampleCurve(
    const std::vector< BfVertex3 >& originalVertices, size_t targetCount
)
{
   using glm::distance;
   if (originalVertices.size() < 2 || targetCount < 2)
      return originalVertices;

   // Step 1: Compute cumulative arc length
   std::vector< float > arcLengths;
   arcLengths.push_back(0.0f);
   for (size_t i = 1; i < originalVertices.size(); ++i)
   {
      float d =
          glm::distance(originalVertices[i - 1].pos, originalVertices[i].pos);
      arcLengths.push_back(arcLengths.back() + d);
   }

   float totalLength = arcLengths.back();

   // Step 2: Create target samples evenly spaced in arc length
   std::vector< BfVertex3 > result;
   result.reserve(targetCount);

   size_t j = 0;
   for (size_t i = 0; i < targetCount; ++i)
   {
      float targetDist = (totalLength * i) / (targetCount - 1);

      // Find segment [j, j+1] such that arcLengths[j] <= targetDist <
      // arcLengths[j+1]
      while (j + 1 < arcLengths.size() && arcLengths[j + 1] < targetDist)
         ++j;

      if (j + 1 == arcLengths.size())
      {
         result.push_back(originalVertices.back());
         continue;
      }

      float t =
          (targetDist - arcLengths[j]) / (arcLengths[j + 1] - arcLengths[j]);
      const auto& p0 = originalVertices[j];
      const auto& p1 = originalVertices[j + 1];
      result.push_back(p0.otherPos(glm::mix(p0.pos, p1.pos, t)));
   }

   return result;
}

std::vector< SplineLib::cSpline3 >
splineFitExternal3D(const std::vector< BfVertex3 >& v)
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

} // namespace math
} // namespace curves
} // namespace obj
