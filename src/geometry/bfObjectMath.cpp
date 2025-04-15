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
       {10, 3628800}
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

} // namespace math
} // namespace curves
} // namespace obj
