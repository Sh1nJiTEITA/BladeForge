#ifndef BF_CURVES3_H
#define BF_CURVES3_H

#include "bfDrawObject.h"
#include "bfMatrix2.h"

class BfPlane;
class BfSingleLine;
class BfBezierCurve;
class BfEllipse;
class BfArc;



#define BF_PI glm::pi<float>()
#define BF_MATH_ABS_ACCURACY 10e-5
#define BF_MATH_DEFAULT_DERIVATIVE_STEP 10e-5

glm::vec3 bfMathFindLinesIntersection(const BfSingleLine& line1, 
									  const BfSingleLine& line2);
size_t bfMathGetFactorial(size_t n);
size_t bfMathGetBinomialCoefficient(size_t n, size_t k);

glm::vec3	   bfMathGetNormal(const glm::vec3& p1, 
							   const glm::vec3& p2, 
							   const glm::vec3& p3);

glm::vec4 bfMathGetPlaneCoeffs(const glm::vec3& p1, 
							   const glm::vec3& p2, 
							   const glm::vec3& p3);

bool	 bfMathIsVertexInPlain(const glm::vec4& plane, 
							   const glm::vec3& p);

bool	 bfMathIsVertexInPlain(const glm::vec3& np, 
							   const glm::vec3& f, 
							   const glm::vec3& s, 
							   const glm::vec3& t);

bool   bfMathIsVerticesInPlain(const std::vector<BfVertex3>& np, 
	   						   const glm::vec3& f, 
	   						   const glm::vec3& s, 
	   						   const glm::vec3& t);

bool bfMathIsVerticesInPlain(const std::vector<BfVertex3>& np);

float bfMathGetBezierCurveLength(BfBezierCurve* curve);
std::vector<glm::vec3> bfMathGetBezierCurveLengthDerivative(BfBezierCurve* curve);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

class BfPlane : public BfDrawObj {
public:
	BfPlane(std::vector<BfVertex3> d_vertices);
	virtual void create_vertices() override;
	virtual void create_indices() override;
private:

};



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
class BfSingleLine : public  BfDrawObj {

public:
	BfVertex3& first;
	BfVertex3& second;
	
	
	BfSingleLine();
	BfSingleLine(const BfVertex3& fp, const BfVertex3& sp);
	BfSingleLine(const glm::vec3& fp, const glm::vec3& sp);
	BfSingleLine(const BfVec2& fp, const BfVec2& sp);

	const BfVertex3& get_first_point() const;
	const BfVertex3& get_second_point() const;

	glm::vec3 get_direction_from_start() const;
	glm::vec3 get_direction_from_end() const;

	virtual void create_vertices() override;
};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
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


class BfCircle : public BfDrawObj {
	float __radius;
	size_t __out_vertices_count; 
public:
	BfCircle(size_t m, const BfVertex3& center, float radius);

	const BfVertex3& get_center();

	virtual void create_vertices() override;
};


//class BfArc : public BfDrawObj {
//	float __radius;
//	
//	size_t __out_vertices_count;
//
//	
//public:
//
//	BfVertex3& left;	// __dvertices[0]
//	BfVertex3& mid;		// __dvertices[1]
//	BfVertex3& right;	// __dvertices[2]
//	BfVertex3& center;	// __dvertices[3]
//
//	BfArc(size_t vertices_count,
//		  const BfVertex3& l,		//1
//		  const BfVertex3& m,	//2
//		  const BfVertex3& r);
//};




//class BfArc : public BfDrawObj {};





#endif // !BF_CURVES3_H
