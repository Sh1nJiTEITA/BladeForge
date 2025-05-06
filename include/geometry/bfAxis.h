#ifndef BF_GEOMETRY_AXIS_H
#define BF_GEOMETRY_AXIS_H

#include <cstdint>

#include "bfCurves3.h"
#include "bfDrawObject.h"

/**
 * @class BfArrow3D
 * @brief Класс для генерации трехмерной стрелочки
 *
 */
class BfArrow3D : public BfDrawLayer
{
   std::shared_ptr< BfDoubleTube > __tail;
   std::shared_ptr< BfCone > __cone;

public:
   /**
    * @brief Дефолтный конструктор
    *
    * @param pointTo => точка на которую указывает стрелочка
    * @param tail => точка откуда начинается стрелочка
    * @param circleRadius => радиус конуса
    * @param tailRadius => радиус хвоста
    * @param relConeLength => относительная длина конуса стрелочки [0...1)
    */
   BfArrow3D(
       const BfVertex3& pointTo,
       const BfVertex3& tail,
       float circleRadius,
       float tailRadius,
       float relConeLength,
       VkPipeline* pipeline,
       const glm::vec3& coneColor = {1.f, 1.f, 1.f},
       const glm::vec3& tailColor = {1.f, 1.f, 1.f}
   );
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
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
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

typedef uint32_t BfAxis3DType;

enum BfAxis3DType_ : BfAxis3DType
{
   BfAxis3DType_X,
   BfAxis3DType_Y,
   BfAxis3DType_Z
};

class BfAxis3D : public BfArrow3D
{
   BfArrow3D __genArrow(BfAxis3DType type, VkPipeline* pipeline);

public:
   BfAxis3D(BfAxis3DType type, VkPipeline* pipeline);
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
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
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

class BfAxis3DPack : public BfDrawLayer
{
public:
   BfAxis3DPack(VkPipeline* pipeline);
};

#endif
