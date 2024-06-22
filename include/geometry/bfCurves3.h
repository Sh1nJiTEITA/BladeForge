#ifndef BF_CURVES3_H
#define BF_CURVES3_H


#include "bfDrawObject.h"
#include "bfMatrix2.h"
// #include <unsupported/Eigen/Splines>
#include <Splines.h>
#include <cfloat>
#include <glm/gtx/vector_angle.hpp>
#include <iomanip>
#include <memory>
#include "bfDrawObjectDefineType.h"

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

glm::vec3 bfMathFindLinesIntersection(const BfSingleLine& line1, 
									  const BfSingleLine& line2,
									  int mode = BF_MATH_FIND_LINES_INTERSECTION_BETWEEN_VERTICES);

size_t bfMathGetFactorial(size_t n);
size_t bfMathGetBinomialCoefficient(size_t n, size_t k);

glm::vec3 bfMathGetNormal(const glm::vec3& p1, 
						  const glm::vec3& p2, 
						  const glm::vec3& p3);

glm::vec4 bfMathGetPlaneCoeffs(const glm::vec3& p1, 
							   const glm::vec3& p2, 
							   const glm::vec3& p3);

std::array<glm::vec3, 3> bfMathGetPlaneOrths(glm::vec4 plane);

std::array<glm::vec3, 2> bfMathGetOrthsByNormal(glm::vec3 normal);


bool bfMathIsVertexInPlain(const glm::vec4& plane, const glm::vec3& p);

bool bfMathIsVertexInPlain(const glm::vec3& np, 
						   const glm::vec3& f, 
						   const glm::vec3& s, 
						   const glm::vec3& t);

bool bfMathIsVerticesInPlain(const std::vector<BfVertex3>& np, 
	   						 const glm::vec3& f, 
	   						 const glm::vec3& s, 
	   						 const glm::vec3& t);

bool bfMathFindLinesIntersection(glm::vec3& intersection, 
								 const BfSingleLine& line1, 
								 const BfSingleLine& line2);

bool bfMathIsVerticesInPlain(const std::vector<BfVertex3>& np);
bool bfMathIsVerticesInPlain(const std::vector<glm::vec3>& np);
bool bfMathIsVerticesInPlain(std::initializer_list<glm::vec3> np);

bool bfMathIsSingleLinesInPlain(const BfSingleLine& L1, const BfSingleLine& L2);

float bfMathGetBezierCurveLength(BfBezierCurve* curve);
std::vector<glm::vec3> bfMathGetBezierCurveLengthDerivative(BfBezierCurve* curve);

std::vector<BfCircle> bfMathGetInscribedCircles(size_t m, 
	const BfSingleLine& L1,	
	const BfSingleLine& L2, 
	float radius
);


float bfMathGetDistanceToLine(const BfSingleLine& L, BfVertex3 P);

std::vector<BfVertex3> bfMathStickObjVertices(std::initializer_list<std::shared_ptr<BfDrawObj>> objs);

std::vector<std::shared_ptr<BfTriangle>> bfMathGenerateTriangleField(std::vector<BfVertex3> v);
glm::vec3 bfMathFindMassCenter(std::vector<BfVertex3> v);


std::vector<float> bfMathGetRelativeSplineArgument(const std::vector<glm::vec3>& v);
std::vector<glm::vec2> bfMathSplineFit(const std::vector<float>& x, const std::vector<float>& y);
	
std::vector<SplineLib::cSpline3> bfMathSplineFitExternal3D(const std::vector<BfVertex3>& v);


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

class BfPlane : public BfDrawObj {
public:
	BfPlane(std::vector<BfVertex3> d_vertices);
	virtual void create_vertices() override;
	virtual void create_indices() override;
private:

};



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //










#define BF_SINGLE_LINE_PROJ_XY 0x1
#define BF_SINGLE_LINE_PROJ_XZ 0x2
#define BF_SINGLE_LINE_PROJ_YZ 0x3
//#define BF_SINGLE_LINE_PROJ_YX BF_SINGLE_LINE_PROJ_XY
//#define BF_SINGLE_LINE_PROJ_ZX BF_SINGLE_LINE_PROJ_XZ
//#define BF_SINGLE_LINE_PROJ_ZY BF_SINGLE_LINE_PROJ_YZ
#define BF_SINGLE_LINE_PROJ_YX 0x4
#define BF_SINGLE_LINE_PROJ_ZX 0x5
#define BF_SINGLE_LINE_PROJ_ZY 0x6



class BfSingleLine : public  BfDrawObj {

public:
	
	BfSingleLine();
	BfSingleLine(const BfVertex3& fp,
				 const BfVertex3& sp);

	BfSingleLine(const glm::vec3& fp, 
				 const glm::vec3& sp);


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

	virtual void create_vertices() override;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

class BfTriangle : public BfDrawObj {

public:
	BfTriangle(const BfVertex3& P_1,
			   const BfVertex3& P_2,
			   const BfVertex3& P_3);

	const BfVertex3& get_first() const;
	const BfVertex3& get_second() const;
	const BfVertex3& get_third() const;



	float get_area() const;
	BfVertex3 get_center() const;

	virtual void create_vertices() override;

};




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

#define BF_CIRCLE_DEFINE_TYPE_CENTER_RADIUS 0x1
#define BF_CIRCLE_DEFINE_TYPE_3_VERTICES 0x2

class BfCircle : public BfDrawObj {
protected:
	float __radius;
	size_t __out_vertices_count;

	int __define_type;
public:
	BfCircle(size_t m, const BfVertex3& center, float radius);
	BfCircle(size_t m, 
			 const BfVertex3& P_1, 
			 const BfVertex3& P_2, 
			 const BfVertex3& P_3);

	const BfVertex3& get_center() const noexcept;
	const BfVertex3& get_first() const noexcept;
	const BfVertex3& get_second() const noexcept;
	const BfVertex3& get_third() const noexcept;
	
	const float get_radius() const noexcept;

	std::array<BfVertex3, 2> get_tangent_vert(const BfVertex3& P) const;

	virtual void create_vertices() override;
};

class BfArc : public BfCircle {
public:
	BfArc(size_t m,
		  const BfVertex3& P_1,
		  const BfVertex3& P_2,
		  const BfVertex3& P_3);

	virtual void create_vertices() override;
};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
#define BF_BEZIER_CURVE_VERT_COUNT 70

class BfBezierCurve : public BfDrawObj {
	const size_t __n;
	size_t __out_vertices_count;
public:
	BfBezierCurve();
	BfBezierCurve(size_t in_n, size_t in_m, std::vector<BfVertex3>&& dvert);
	BfBezierCurve(size_t in_n, size_t in_m, std::vector<BfVertex3>& dvert);
	BfBezierCurve(BfBezierCurve&& ncurve) noexcept;

	const size_t get_n() const noexcept;

	glm::vec3 get_single_vertex_v3(float t) const;
	BfVertex3 get_single_vertex_bfv3(float t) const;

	BfBezierCurve get_derivative();

	glm::vec3 get_single_derivative_1_analyt_v3(float t) const;
	BfVertex3 get_single_derivative_1_analyt_bfv3(float t) const;
	glm::vec3 get_single_derivative_1_numeric_v3(float t, float step = BF_MATH_DEFAULT_DERIVATIVE_STEP) const;

	glm::vec3 get_single_derivative_2_analyt_v3(float t) const;
	BfVertex3 get_single_derivative_2_analyt_bfv3(float t) const;

	const size_t get_out_vertices_count() const noexcept;
	void set_out_vertices_count(size_t in_m);

	virtual void create_vertices() override;


	friend float bfMathGetBezierCurveLength(BfBezierCurve* curve);
	friend std::vector<glm::vec3> bfMathGetBezierCurveLengthDerivative(BfBezierCurve* curve);
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

#define BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS 0.01f

class BfBezierCurveFrame : public BfDrawLayer {
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
	
	void remake(std::shared_ptr<BfBezierCurve> curve, glm::vec3 c = { 1.0f, 1.0f, 1.0f });

};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

class BfCubicSplineCurve : public BfDrawObj {

	size_t __out_vertices_count;

public:
	BfCubicSplineCurve(size_t out_vertices_count, std::vector<BfVertex3>& dp);
	BfCubicSplineCurve(size_t out_vertices_count, std::vector<glm::vec3>& dp);


	virtual void create_vertices();
};





#endif // !BF_CURVES3_H
