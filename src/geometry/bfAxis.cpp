#include "bfAxis.h"

#include <memory>
#include <stdexcept>

#include "bfCurves3.h"
#include "bfDrawObject.h"
#include "bfLayerHandler.h"

BfArrow3D::BfArrow3D(
    const BfVertex3& pointTo,
    const BfVertex3& tail,
    float circleRadius,
    float tailRadius,
    float relConeLength,
    VkPipeline* pipeline,
    const glm::vec3& coneColor,
    const glm::vec3& tailColor
)
    : BfDrawLayer(
          *BfLayerHandler::instance()->allocator(),
          sizeof(BfVertex3),
          4000,
          2,
          true,
          BF_DRAW_LAYER_TYPE_ARROW
      )
{
   glm::vec3 backDirection = glm::normalize(pointTo.pos - tail.pos);
   float arrowLength = glm::length(pointTo.pos - tail.pos);
   BfVertex3 coneBase = {
       pointTo.pos - backDirection * relConeLength * arrowLength,
       pointTo.color,
       backDirection
   };
   { // CONE
      auto cone_ptr =
          std::make_shared< BfCone >(50, pointTo, coneBase, circleRadius);
      cone_ptr->bind_pipeline(pipeline);
      cone_ptr->set_color(coneColor);
      this->add_l(cone_ptr);
   }
   { // TAIL
      auto tail_prt = std::make_shared< BfDoubleTube >(
          51,
          coneBase,
          tail,
          tailRadius,
          tailRadius / 100.0f,
          tailRadius,
          tailRadius / 100.0f
      );
      tail_prt->bind_pipeline(pipeline);
      tail_prt->set_color(tailColor);
      this->add_l(tail_prt);
   }

   this->generate_draw_data();
   this->update_buffer();
}

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
//

BfArrow3D
BfAxis3D::__genArrow(BfAxis3DType type, VkPipeline* pipeline)
{
   // clang-format off
   switch (type)
   {
      case BfAxis3DType_X:
         return BfArrow3D(
             BfVertex3( {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}),
             BfVertex3( {0.0f, 0.0f, 0.0f}, {0.25f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}),
             0.011f,
             0.005f,
             0.25f,
             pipeline,
             glm::vec3{1.0f, 0.0f, 0.0f},
             glm::vec3{1.0f, 0.0f, 0.0f}
         );
      case BfAxis3DType_Y:
         return BfArrow3D(
             BfVertex3( {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}),
             BfVertex3( {0.0f, 0.0f, 0.0f}, {0.0f, 0.25f, 0.0f}, {0.0f, 1.0f, 0.0f}),
             0.011f,
             0.005f,
             0.25f,
             pipeline,
             glm::vec3{.0f, 1.0f, 0.0f},
             glm::vec3{.0f, 1.0f, 0.0f}
         );
      case BfAxis3DType_Z:
         return BfArrow3D(
             BfVertex3({0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {.0f, 0.0f, 1.0f}),
             BfVertex3( {0.0f, 0.0f, 0.0f}, {0.0f, 0.0, 0.25f}, {.0f, 0.0f, 1.0f}),
             0.011f,
             0.005f,
             0.25f,
             pipeline,
             glm::vec3{.0f, 0.0f, 1.0f},
             glm::vec3{.0f, 0.0f, 1.0f}
         );
      default:
         throw std::runtime_error("Underfined BfAxisType");
   }
   // clang-format on
}

BfAxis3D::BfAxis3D(BfAxis3DType type, VkPipeline* pipeline)
    : BfArrow3D(__genArrow(type, pipeline))
{
   // clang-format off
   switch (type)
   {
      case BfAxis3DType_X: id.change_type(BF_DRAW_LAYER_TYPE_AXIS_X); break;
      case BfAxis3DType_Y: id.change_type(BF_DRAW_LAYER_TYPE_AXIS_Y); break;
      case BfAxis3DType_Z: id.change_type(BF_DRAW_LAYER_TYPE_AXIS_Z); break;
      default: throw std::runtime_error("Underfined BfAxisType");
   }
   // clang-format on
}

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
//

BfAxis3DPack::BfAxis3DPack(VkPipeline* pipeline)
    : BfDrawLayer(
          *BfLayerHandler::instance()->allocator(),
          sizeof(BfVertex3),
          5000,
          3,
          true,
          BF_DRAW_LAYER_TYPE_AXIS_SYSTEM
      )
{
   // clang-format off
   this->add(std::make_shared<BfAxis3D>(BfAxis3DType_X, pipeline));
   this->add(std::make_shared<BfAxis3D>(BfAxis3DType_Y, pipeline));
   this->add(std::make_shared<BfAxis3D>(BfAxis3DType_Z, pipeline));
   // clang-format on
}
