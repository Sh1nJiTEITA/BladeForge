#ifndef BF_DRAW_OBJECT_DEFINE_TYPE_H
#define BF_DRAW_OBJECT_DEFINE_TYPE_H

#include <stdexcept>
#include <string>

// BfCurves3
#define BF_DRAW_OBJ_TYPE_NULL 0x00
#define BF_DRAW_OBJ_TYPE_PLANE 0x01
#define BF_DRAW_OBJ_TYPE_SINGLE_LINE 0x02
#define BF_DRAW_OBJ_TYPE_BEZIER_CURVE 0x03
#define BF_DRAW_OBJ_TYPE_BEZIER_CURVE_WITH_HANDLES 0x031
#define BF_DRAW_OBJ_TYPE_CIRCLE 0x04
#define BF_DRAW_OBJ_TYPE_TRIANGLE 0x05
#define BF_DRAW_OBJ_TYPE_TUBE 0x06
#define BF_DRAW_OBJ_TYPE_DOUBLE_TUBE 0x07
#define BF_DRAW_OBJ_TYPE_CONE 0x08

// bfBladeSection
#define BF_DRAW_LAYER_TYPE_BLADE_SECTION 0xA1
#define BF_DRAW_LAYER_TYPE_BLADE_BASE 0xA2
#define BF_DRAW_LAYER_TYPE_BEZIER_FRAME 0xA3

// BfAxis
#define BF_DRAW_LAYER_TYPE_ARROW 0xB1
#define BF_DRAW_LAYER_TYPE_AXIS_X 0xB2
#define BF_DRAW_LAYER_TYPE_AXIS_Y 0xB3
#define BF_DRAW_LAYER_TYPE_AXIS_Z 0xB4
#define BF_DRAW_LAYER_TYPE_AXIS_SYSTEM 0xB5

static std::string
bfGetStrNameDrawObjType(unsigned int type)
{
   // clang-format off
   switch (type)
   {
      // BfCurves3
      case BF_DRAW_OBJ_TYPE_NULL: return "Underfined";
      case BF_DRAW_OBJ_TYPE_PLANE: return "Plane";
      case BF_DRAW_OBJ_TYPE_SINGLE_LINE: return "SingleLine";
      case BF_DRAW_OBJ_TYPE_BEZIER_CURVE: return "BezierCurve";
      case BF_DRAW_OBJ_TYPE_CIRCLE: return "Circle";
      case BF_DRAW_OBJ_TYPE_TRIANGLE: return "Triangle";
      case BF_DRAW_OBJ_TYPE_TUBE: return "Tube";
      case BF_DRAW_OBJ_TYPE_DOUBLE_TUBE: return "DoubleTube";
      case BF_DRAW_OBJ_TYPE_CONE: return "Cone";
      case BF_DRAW_OBJ_TYPE_BEZIER_CURVE_WITH_HANDLES: return "Bezier curve with handles";

      // bfBladeSection
      case BF_DRAW_LAYER_TYPE_BLADE_SECTION: return "BladeSection";
      case BF_DRAW_LAYER_TYPE_BLADE_BASE: return "BladeBase";
      case BF_DRAW_LAYER_TYPE_BEZIER_FRAME: return "BezierCurveFrame";

      // BfAxis
      case BF_DRAW_LAYER_TYPE_ARROW : return "Arrow";
      case BF_DRAW_LAYER_TYPE_AXIS_X: return "X-Axis";
      case BF_DRAW_LAYER_TYPE_AXIS_Y: return "Y-Axis";
      case BF_DRAW_LAYER_TYPE_AXIS_Z: return "Z-Axis";
      case BF_DRAW_LAYER_TYPE_AXIS_SYSTEM : return "AxisSystem";


      default: throw std::runtime_error("Invalid BfDrawObject type");
   }
   // clang-format on
}

#endif  // !BF_DRAW_OBJECT_DEFINE_TYPE_H
