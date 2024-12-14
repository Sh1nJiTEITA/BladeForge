#ifndef BF_GEOMETRY_AXIS_H
#define BF_GEOMETRY_AXIS_H

#include "bfDrawObject.h"

class BfArrow3D : BfDrawLayer
{
public:
   BfArrow3D(
       const BfVertex3& pointTo, const BfVertex3& tail, float circleRadius
   );
};

#endif
