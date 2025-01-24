#ifndef BF_CURVES3_H
#define BF_CURVES3_H

// NOTE: Define this if BfCurves3_gui.cpp file is existing and need
// to override `GuiIntegration::update()` method
#define BF_CURVES3_GUI

#include <Splines.h>

#include <cfloat>

#include "bfDrawObject.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/vector_angle.hpp>
#include <iomanip>
#include <memory>

#include "bfDrawObjectDefineType.h"

#ifdef NDEBUG
#define bfAssert(cond) assert(cond)
#else
#define bfAssert(...)
#endif

class BfPlane;
class BfSingleLine;
class BfBezierCurve;
class BfCircle;
class BfTriangle;

#define BF_PI glm::pi<float>()
#define BF_MATH_ABS_ACCURACY 10e-5
#define BF_MATH_DEFAULT_DERIVATIVE_STEP 10e-5

#define BF_MATH_FIND_LINES_INTERSECTION_BETWEEN_VERTICES 0x1
#define BF_MATH_FIND_LINES_INTERSECTION_ANY 0x2

glm::vec3 bfMathFindLinesIntersection(
    const BfSingleLine& line1,
    const BfSingleLine& line2,
    int mode = BF_MATH_FIND_LINES_INTERSECTION_BETWEEN_VERTICES
);

// clang-format off
size_t bfMathGetFactorial(size_t n);
size_t bfMathGetBinomialCoefficient(size_t n, size_t k);

using cVec3 = const glm::vec3;
using cVec4 = const glm::vec4;

glm::vec3 bfMathGetNormal(cVec3& p1, cVec3& p2, cVec3& p3);
glm::vec4 bfMathGetPlaneCoeffs(cVec3& p1, cVec3& p2, cVec3& p3);
std::array<glm::vec3, 3> bfMathGetPlaneOrths(glm::vec4 plane);
std::array<glm::vec3, 2> bfMathGetOrthsByNormal(glm::vec3 normal);

bool bfMathIsVertexInPlain(cVec4& plane, cVec3& p);
bool bfMathIsVertexInPlain(cVec3& np, cVec3& f, cVec3& s, cVec3& t);
bool bfMathIsVerticesInPlain(const std::vector<BfVertex3>& np, cVec3& f, cVec3& s, cVec3& t);
bool bfMathFindLinesIntersection(glm::vec3& inter, const BfSingleLine& line1, const BfSingleLine& line2);
glm::vec3 bfMathFindLinesIntersection(cVec3& P1, cVec3& P2, cVec3 Q1, cVec3 Q2);
bool bfMathIsVerticesInPlain(const std::vector<BfVertex3>& np);
bool bfMathIsVerticesInPlain(const std::vector<glm::vec3>& np);
bool bfMathIsVerticesInPlain(std::initializer_list<glm::vec3> np);
bool bfMathIsSingleLinesInPlain(const BfSingleLine& L1, const BfSingleLine& L2);

float bfMathGetBezierCurveLength(BfBezierCurve* curve);
std::vector<glm::vec3> bfMathGetBezierCurveLengthDerivative(BfBezierCurve* curve);
std::vector<BfCircle> bfMathGetInscribedCircles(size_t m, const BfSingleLine& L1, const BfSingleLine& L2, float radius);
float bfMathGetDistanceToLine(const BfSingleLine& L, BfVertex3 P);
std::vector<BfVertex3> bfMathStickObjVertices(std::initializer_list<std::shared_ptr<BfDrawObj>> objs);
std::vector<std::shared_ptr<BfTriangle>> bfMathGenerateTriangleField( std::vector<BfVertex3> v);
glm::vec3 bfMathFindMassCenter(std::vector<BfVertex3> v);
std::vector<float> bfMathGetRelativeSplineArgument(const std::vector<glm::vec3>& v);
std::vector<glm::vec2> bfMathSplineFit(const std::vector<float>& x, const std::vector<float>& y);
std::vector<SplineLib::cSpline3> bfMathSplineFitExternal3D(const std::vector<BfVertex3>& v);

// Camera
glm::mat4 bfOrtho(float right, float left, float bot, float top, float far, float near);

// clang-format on

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

class BfPlane : public BfDrawObj
{
public:
   BfPlane(std::vector<BfVertex3> d_vertices);
   virtual void createVertices() override;
   virtual void createIndices() override;

private:
};

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

#define BF_SINGLE_LINE_PROJ_XY 0x1
#define BF_SINGLE_LINE_PROJ_XZ 0x2
#define BF_SINGLE_LINE_PROJ_YZ 0x3
// #define BF_SINGLE_LINE_PROJ_YX BF_SINGLE_LINE_PROJ_XY
// #define BF_SINGLE_LINE_PROJ_ZX BF_SINGLE_LINE_PROJ_XZ
// #define BF_SINGLE_LINE_PROJ_ZY BF_SINGLE_LINE_PROJ_YZ
#define BF_SINGLE_LINE_PROJ_YX 0x4
#define BF_SINGLE_LINE_PROJ_ZX 0x5
#define BF_SINGLE_LINE_PROJ_ZY 0x6

class BfSingleLine : public BfDrawObj
{
public:
   BfSingleLine();
   BfSingleLine(const BfVertex3& fp, const BfVertex3& sp);

   BfSingleLine(const glm::vec3& fp, const glm::vec3& sp);

   float get_k(int proj = BF_SINGLE_LINE_PROJ_XY);
   float get_k_perpendicular(int proj = BF_SINGLE_LINE_PROJ_XY);
   float get_b(int proj = BF_SINGLE_LINE_PROJ_XY);
   float get_b_perpendicular(glm::vec3 f, int proj = BF_SINGLE_LINE_PROJ_XY);

   float get_single_proj(float ival, int proj = BF_SINGLE_LINE_PROJ_XY);

   float get_length();

   const BfVertex3& get_first() const;
   const BfVertex3& get_second() const;

   glm::vec3 get_direction_from_start() const;
   glm::vec3 get_direction_from_end() const;

   virtual void createVertices() override;
};

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

class BfTriangle : public BfDrawObj
{
public:
   BfTriangle(const BfVertex3& P_1, const BfVertex3& P_2, const BfVertex3& P_3);

   const BfVertex3& get_first() const;
   const BfVertex3& get_second() const;
   const BfVertex3& get_third() const;

   float get_area() const;
   BfVertex3 get_center() const;

   virtual void createVertices() override;
};

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

#define BF_CIRCLE_DEFINE_TYPE_CENTER_RADIUS 0x1
#define BF_CIRCLE_DEFINE_TYPE_3_VERTICES 0x2

class BfCircle : public BfDrawObj
{
protected:
   float __radius;
   size_t __out_vertices_count;

   int __define_type;

public:
   BfCircle(size_t m, const BfVertex3& center, float radius);
   BfCircle(
       size_t m,
       const BfVertex3& P_1,
       const BfVertex3& P_2,
       const BfVertex3& P_3
   );

   const BfVertex3& get_center() const noexcept;
   const BfVertex3& get_first() const noexcept;
   const BfVertex3& get_second() const noexcept;
   const BfVertex3& get_third() const noexcept;

   const float get_radius() const noexcept;

   std::array<BfVertex3, 2> get_tangent_vert(const BfVertex3& P) const;

   virtual void createVertices() override;
};

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

class BfCircleFilled : public BfDrawObj
{
   float m_radius;
   size_t m_outVerticesCount;

public:
   BfCircleFilled(size_t m, const BfVertex3& center, float radius);

   float radius() const noexcept;
   const glm::vec3& tangent() const noexcept;

   virtual void createVertices() override;
   virtual void createIndices() override;

#if defined(BF_CURVES3_GUI)
   virtual void update() override;
#endif

private:
   const BfVertex3& _center() const;
};

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

class BfArc : public BfCircle
{
public:
   BfArc(
       size_t m,
       const BfVertex3& P_1,
       const BfVertex3& P_2,
       const BfVertex3& P_3
   );

   std::shared_ptr<BfBezierCurve> toBezier();
   // TODO: Make
   virtual void createVertices() override;
};

/**
 * @class BfCone
 * @brief Создает конус
 *
 */
class BfCone : public BfDrawObj
{
   size_t __out_vertices_count;
   float __base_radius;

public:
   /**
    * @brief Конструктор через 2 точки и радиус
    *
    * @param m      - количество граней
    * @param tip    - координата вершины конуса
    * @param center - координата центра основания
    * @param radius - радиус основания конуса
    *
    * @note Следует верно указывать нормали внутри 'tip', 'center',
    *       так как без них координаты не сгенерируются
    */
   BfCone(
       size_t m, const BfVertex3& tip, const BfVertex3& center, float radius
   );

   /**
    * @brief Конструктор через точку, нормаль и радиус
    *
    * @param m
    * @param tip
    * @param height
    * @param radius
    */
   BfCone(size_t m, const BfVertex3& tip, float height, float radius);

   const BfVertex3& centerVertex();
   const BfVertex3& tipVertex();

   virtual void createVertices() override;
};

#define STRVEC3(VEC) VEC.x << ", " << VEC.y << ", " << VEC.z

/**
 * @class BfTube
 * @brief Создает объект трубы без внутреннией стенки и без граней между
 * стенками соответственнo
 *
 */
class BfTube : public BfDrawObj
{
   size_t __out_m;
   float __radius_begin;
   float __radius_end;

public:
   BfTube(
       size_t m,
       const BfVertex3& begin,
       const BfVertex3& end,
       float radius_begin,
       float radius_end
   );

   const BfVertex3& beginCenterVertex();
   const BfVertex3& endCenterVertex();

   virtual void createVertices() override;
   virtual void createIndices() override;
};

/**
 * @class BfDoubleTube
 * @brief Создает одинарный объект двойной трубы
 *
 */
class BfDoubleTube : public BfDrawObj
{
   size_t __out_m;
   float __radius_in_begin;
   float __radius_out_begin;
   float __radius_in_end;
   float __radius_out_end;

public:
   /**
    * @brief Генерирует трубу с 'm' - количеством граней
    *
    * @param m - количество граней, должно быть нечетным
    *  TODO: добавить поддержку четного количества
    *
    * @param begin - начальная точка центра
    * @param end - конечная точка центра
    * @param radius_out_begin - внешний радиус начала
    * @param radius_in_begin - внутренний радиус начала
    * @param radius_out_end - внешний радиус конца
    * @param radius_in_end - внешний радиус конца
    */
   BfDoubleTube();
   BfDoubleTube(
       size_t m,
       const BfVertex3& begin,
       const BfVertex3& end,
       float radius_out_begin,
       float radius_in_begin,
       float radius_out_end,
       float radius_in_end
   );

   const BfVertex3& beginCenterVertex();
   const BfVertex3& endCenterVertex();

   virtual void createVertices() override;
   virtual void createIndices() override;
};

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

#define BF_BEZIER_CURVE_VERT_COUNT 70

class BfBezierCurve : public BfDrawObj
{
   const size_t __n;
   size_t __out_vertices_count;

public:
   BfBezierCurve();
   BfBezierCurve(size_t in_n, size_t in_m, std::vector<BfVertex3>&& dvert);
   BfBezierCurve(size_t in_n, size_t in_m, const std::vector<BfVertex3>& dvert);
   BfBezierCurve(BfBezierCurve&& ncurve) noexcept;

   const size_t n() const noexcept;

   glm::vec3 calcV3(float t) const;
   BfVertex3 calcBfV3(float t) const;
   BfBezierCurve get_derivative();
   void extend(BfBezierCurve* curve);

   // clang-format off
   glm::vec3 get_single_derivative_1_analyt_v3(float t) const;
   BfVertex3 get_single_derivative_1_analyt_bfv3(float t) const;
   glm::vec3 get_single_derivative_1_numeric_v3(float t, float step = BF_MATH_DEFAULT_DERIVATIVE_STEP) const;
   glm::vec3 get_single_derivative_2_analyt_v3(float t) const;
   BfVertex3 get_single_derivative_2_analyt_bfv3(float t) const;
   // clang-format on

   virtual void createVertices() override;

   friend float bfMathGetBezierCurveLength(BfBezierCurve* curve);
   friend std::vector<glm::vec3> bfMathGetBezierCurveLengthDerivative(
       BfBezierCurve* curve
   );
};

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

#define BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS 0.01f

class BfBezierCurveFrame : public BfDrawLayer
{
   std::shared_ptr<BfBezierCurve> __curve;

   VkPipeline __lines_pipeline;
   VkPipeline __triangle_pipeline;

public:
   BfBezierCurveFrame(
       std::shared_ptr<BfBezierCurve> curve,
       VmaAllocator allocator,
       VkPipeline lines_pipeline,
       VkPipeline triangle_pipeline
   );

   void remake(
       std::shared_ptr<BfBezierCurve> curve, glm::vec3 c = {1.0f, 1.0f, 1.0f}
   );
};

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

class BfCubicSplineCurve : public BfDrawObj
{
   size_t __out_vertices_count;

public:
   BfCubicSplineCurve(size_t out_vertices_count, std::vector<BfVertex3>& dp);
   BfCubicSplineCurve(size_t out_vertices_count, std::vector<glm::vec3>& dp);

   virtual void createVertices();
};

#endif  // !BF_CURVES3_H
