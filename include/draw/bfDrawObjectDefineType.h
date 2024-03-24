#ifndef BF_DRAW_OBJECT_DEFINE_TYPE_H
#define BF_DRAW_OBJECT_DEFINE_TYPE_H

#include <string>
#include <stdexcept>

// BfCurves3
#define BF_DRAW_OBJ_TYPE_NULL 0x00
#define BF_DRAW_OBJ_TYPE_PLANE 0x01
#define BF_DRAW_OBJ_TYPE_SINGLE_LINE 0x02
#define BF_DRAW_OBJ_TYPE_BEZIER_CURVE 0x03
#define BF_DRAW_OBJ_TYPE_CIRCLE 0x04

static std::string bfGetStrNameDrawObjType(uint32_t type) {
	switch (type) {
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
		default:
			throw std::runtime_error("Invalid BfDrawObject type");
	}
}


#endif // !BF_DRAW_OBJECT_DEFINE_TYPE_H
