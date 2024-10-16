#ifndef BF_DRAW_OBJECT_DEFINE_TYPE_H
#define BF_DRAW_OBJECT_DEFINE_TYPE_H

#include <stdexcept>
#include <string>
#include <utility>

// BfCurves3
#define BF_DRAW_OBJ_TYPE_NULL 0x00
#define BF_DRAW_OBJ_TYPE_PLANE 0x01
#define BF_DRAW_OBJ_TYPE_SINGLE_LINE 0x02
#define BF_DRAW_OBJ_TYPE_BEZIER_CURVE 0x03
#define BF_DRAW_OBJ_TYPE_CIRCLE 0x04
#define BF_DRAW_OBJ_TYPE_TRIANGLE 0x05

// bfBladeSection
#define BF_DRAW_LAYER_TYPE_BLADE_SECTION 0xA1
#define BF_DRAW_LAYER_TYPE_BLADE_BASE 0xA2
#define BF_DRAW_LAYER_TYPE_BEZIER_FRAME 0xA3

static std::string bfGetStrNameDrawObjType(uint32_t type)
{
   switch (type)
   {
      // BfCurves3
      case BF_DRAW_OBJ_TYPE_NULL:
         return "Underfined";
      case BF_DRAW_OBJ_TYPE_PLANE:
         return "Plane";
      case BF_DRAW_OBJ_TYPE_SINGLE_LINE:
         return "SingleLine";
      case BF_DRAW_OBJ_TYPE_BEZIER_CURVE:
         return "BezierCurve";
      case BF_DRAW_OBJ_TYPE_CIRCLE:
         return "Circle";
      case BF_DRAW_OBJ_TYPE_TRIANGLE:
         return "Triangle";

      // bfBladeSection
      case BF_DRAW_LAYER_TYPE_BLADE_SECTION:
         return "BladeSection";
      case BF_DRAW_LAYER_TYPE_BLADE_BASE:
         return "BladeBase";
      case BF_DRAW_LAYER_TYPE_BEZIER_FRAME:
         return "BezierCurveFrame";
      default:
         throw std::runtime_error("Invalid BfDrawObject type");
   }

}

#endif  // !BF_DRAW_OBJECT_DEFINE_TYPE_H
