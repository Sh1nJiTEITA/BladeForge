#include <bfCurves4.h>

namespace curves
{

namespace math
{

glm::vec3
findLinesIntersection(
    const BfSingleLine& line1, const BfSingleLine& line2, int mode
)
{
}

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

void
Bfcircle3Vertices::make()
{
}

}  // namespace curves
