#include "bfCurves3.h"


BfPlane::BfPlane(std::vector<BfVertex3> d_vertices)
	: BfDrawObj()
{
	__dvertices = d_vertices;
}

void BfPlane::create_vertices()
{
	__vertices.reserve(__dvertices.size());
	for (const auto& dvert : __dvertices) {
		__vertices.emplace_back(dvert);
	}
}

void BfPlane::create_indices()
{
	__indices.reserve(__vertices.size() * 2);
	for (size_t i = 0; i < __vertices.size(); ++i) {
		__indices.emplace_back( i);
	}
}

BfSingleLine::BfSingleLine()
	: BfSingleLine{ {{0.0f, 0.0f, 0.0f},{1.0f, 1.0f, 1.0f},{0.0f, 0.0f,0.0f}},
					{{0.0f, 0.0f, 0.0f},{1.0f, 1.0f, 1.0f},{0.0f, 0.0f,0.0f}} }
{

}

BfSingleLine::BfSingleLine(const BfVertex3& fp, const BfVertex3& sp)
{
	__dvertices.reserve(2);
	__vertices.reserve(2);
	__indices.reserve(2);

	__dvertices.emplace_back(fp);
	__dvertices.emplace_back(sp);
}

BfSingleLine::BfSingleLine(const glm::vec3 fp, const glm::vec3 sp)
	: BfSingleLine(
		{ fp, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f} },
		{ sp, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f} }
	)
{
}

const BfVertex3& BfSingleLine::get_first_point() const
{
	return __dvertices.at(0);
}

const BfVertex3& BfSingleLine::get_second_point() const
{
	return __dvertices.at(1);
}

glm::vec3 BfSingleLine::get_direction_from_start() const
{
	return this->get_second_point().pos - this->get_first_point().pos;
}

glm::vec3 BfSingleLine::get_direction_from_end() const
{
	return -this->get_direction_from_start();
}

void BfSingleLine::create_vertices()
{
	if (this->is_ok())
		throw std::runtime_error("BfSingleLine::create_vertices abort");

	__vertices.clear();

	__vertices.emplace_back(__dvertices.at(0));
	__vertices.at(0).color = __main_color;
	__vertices.emplace_back(__dvertices.at(1));
	__vertices.at(1).color = __main_color;
	
}

glm::vec3 bfMathFindLinesIntersection(const BfSingleLine& line1, const BfSingleLine& line2)
{
	glm::mat3 plane;
	

	plane[0][0] = line2.get_second_point().pos.x - line1.get_first_point().pos.x;
	plane[0][1] = line2.get_second_point().pos.y - line1.get_first_point().pos.y;
	plane[0][2] = line2.get_second_point().pos.z - line1.get_first_point().pos.z;
	plane[1][0] = line1.get_second_point().pos.x - line1.get_first_point().pos.x;
	plane[1][1] = line1.get_second_point().pos.y - line1.get_first_point().pos.y;
	plane[1][2] = line1.get_second_point().pos.z - line1.get_first_point().pos.z;
	plane[2][0] = line2.get_first_point().pos.x - line1.get_first_point().pos.x;
	plane[2][1] = line2.get_first_point().pos.y - line1.get_first_point().pos.y;
	plane[2][2] = line2.get_first_point().pos.z - line1.get_first_point().pos.z;

	if (glm::determinant(plane) == 0) {
		// a1.x + b1.x * t1 = a2.x + b2.x * t2
		// a1.y + b1.y * t1 = a2.y + b2.y * t2
		glm::vec3 a1 = line1.get_first_point().pos;
		glm::vec3 b1 = line1.get_direction_from_start();

		glm::vec3 a2 = line2.get_first_point().pos;
		glm::vec3 b2 = line2.get_direction_from_start();

		float frac = b1.x * b2.y - b1.y * b2.x;

		float t1 = (a1.x * b2.y - a1.y * b2.x - a2.x * b2.y + a2.y * b2.x) / (-frac);
		float t2 = (a1.x * b1.y - a1.y * b1.x - a2.x * b1.y + a2.y * b1.x) / (-frac);

		// if lines are parallel
		if (glm::isnan(t1) || glm::isnan(t2)) {
			return glm::vec3(std::nan(""), std::nan(""), std::nan(""));
		}

		if ((t1 >= 0.0f) && (t1 <= 1.0f) && (t2 >= 0.0f) && (t2 <= 1.0f)) {
			return a1 + b1 * t1;
		}
		else {
			return glm::vec3(std::nan(""), std::nan(""), std::nan(""));
		}
	}
	else {
		return glm::vec3(std::nan(""), std::nan(""), std::nan(""));
	}
}

size_t bfMathGetFactorial(size_t n)
{
	const static std::map<uint32_t, uint32_t> __factorial{
		{1,		1},
		{2,		2},
		{3,		6},
		{4,		24},
		{5,		120},
		{6,		720},
		{7,		5040},
		{8,		40320},
		{9,		362880},
		{10,	3628800}
	};

	if (n < 1)
		return 1;
	else if (n > 10)
		throw std::runtime_error("Factorial can't be calculated: n > 10");
	else {
		return __factorial.at(n);
	}
}

size_t bfMathGetBinomialCoefficient(size_t n, size_t k)
{
	return bfMathGetFactorial(n) / bfMathGetFactorial(k) / bfMathGetFactorial(n - k);
}

glm::vec3 bfMathGetNormal(const glm::vec3& p1, 
						  const glm::vec3& p2, 
						  const glm::vec3& p3)
{
	glm::vec3 _v1{ p2 - p1 };//{ p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
	glm::vec3 _v2{ p3 - p1 };//{ p3.x - p1.x, p3.y - p1.y, p3.z - p1.z };
	glm::vec3 normal = glm::cross(_v1, _v2);

	if (normal == glm::vec3(0.0f))
		return normal;
	else
		return normal / glm::length(normal);
}

glm::vec4 bfMathGetPlaneCoeffs(const glm::vec3& f, 
							   const glm::vec3& s, 
							   const glm::vec3& t)
{
	glm::mat3 xd = {
		f.y, f.z, 1,
		s.y, s.z, 1,
		t.y, t.z, 1
	};

	glm::mat3 yd = {
		f.x, f.z, 1,
		s.x, s.z, 1,
		t.x, t.z, 1
	};

	glm::mat3 zd = {
		f.x, f.y, 1,
		s.x, s.y, 1,
		t.x, t.y, 1
	};

	auto direction = glm::vec3(
		glm::determinant(xd),
		-glm::determinant(yd),
		glm::determinant(zd)
	);

	glm::mat3 D = {
		f.x, f.y, f.z,
		s.x, s.y, s.z,
		t.x, t.y, t.z
	};
	
	return { direction, -glm::determinant(D) };
}

bool bfMathIsVertexInPlain(const glm::vec4& plane, 
						   const glm::vec3& p)
{
	if (plane.x * p.x + plane.y * p.y + plane.z * p.z + plane.w <= BF_MATH_ABS_ACCURACY)
		return true;
	else
		return false;
}

bool bfMathIsVertexInPlain(const glm::vec3& np, 
						   const glm::vec3& f, 
						   const glm::vec3& s, 
						   const glm::vec3& t)
{
	glm::vec3 first = bfMathGetNormal(f, s, t);
	glm::vec3 second = bfMathGetNormal(np, f, s);
	glm::vec3 delta = second - first;

	if ((delta.x <= BF_MATH_ABS_ACCURACY) &&
		(delta.y <= BF_MATH_ABS_ACCURACY) &&
		(delta.z <= BF_MATH_ABS_ACCURACY))
		return true;
	else
		return false;

	/*if (((bfMathGetNormal(np, f, s).x - bfMathGetNormal(f, s, t).x) <= BF_MATH_ABS_ACCURACY) &&
		((bfMathGetNormal(np, f, s).y - bfMathGetNormal(f, s, t).y) <= BF_MATH_ABS_ACCURACY) &&
		((bfMathGetNormal(np, f, s).z - bfMathGetNormal(f, s, t).z) <= BF_MATH_ABS_ACCURACY))
		return true;
	else
		return false;*/
}

bool bfMathIsVerticesInPlain(const std::vector<BfVertex3>& np, 
							 const glm::vec3& f, 
							 const glm::vec3& s, 
							 const glm::vec3& t)
{
	bool decision = true;
	for (const auto& vert : np) {
		decision *= bfMathIsVertexInPlain(vert.pos, f, s, t);
	}
	
	return decision;
}

bool bfMathIsVerticesInPlain(const std::vector<BfVertex3>& np)
{
	if (np.size() <= 3)
		return true;
	
	bool decision = true;
	for (size_t i = 3; i < np.size(); ++i) {
		decision *= bfMathIsVertexInPlain(np[i].pos, np[0].pos, np[1].pos, np[2].pos);
	}

	return decision;
}

float bfMathGetBezierCurveLength(BfBezierCurve* curve)
{
	if (curve->__vertices.empty())
		throw std::runtime_error("BfBezierCurve vertices vector are empty");

	if (curve->__dvertices.empty())
		throw std::runtime_error("BfBezierCurve dvertices vector are empty");

	float len = 0;

	for (size_t i = 0; i < curve->__vertices.size() - 1; ++i) {
		len += glm::length(curve->__vertices[i + 1].pos - curve->__vertices[i].pos);
	}

	return len;
}

std::vector<glm::vec3> bfMathGetBezierCurveLengthDerivative(BfBezierCurve* curve)
{
	std::vector<glm::vec3> d;
	d.reserve(curve->get_dvertices_count());
	
	for (size_t i = 0; i < curve->get_dvertices_count(); ++i) {
		glm::vec3 axe_derivative{0.0f};
		for (size_t j = 0; j < 2; ++j) {
			std::vector<BfVertex3> right_dvert = curve->get_rdVertices();
			right_dvert[i].pos[j] += BF_MATH_ABS_ACCURACY;
			BfBezierCurve right_curve{ curve->get_n(), curve->get_dvertices_count(), right_dvert};
			right_curve.create_vertices();
			float right = bfMathGetBezierCurveLength(&right_curve);

			std::vector<BfVertex3> left_dvert = curve->get_rdVertices();
			left_dvert[i].pos[j] -= BF_MATH_ABS_ACCURACY;
			BfBezierCurve left_curve{ curve->get_n(), curve->get_dvertices_count(), left_dvert };
			left_curve.create_vertices();
			float left = bfMathGetBezierCurveLength(&left_curve);

			axe_derivative[j] = (right - left) / 2.0f / BF_MATH_ABS_ACCURACY;
		}
		d.emplace_back(axe_derivative);
	}
	return d;
}


BfBezierCurve::BfBezierCurve(size_t in_n, size_t in_m, std::vector<BfVertex3>&& dvert)
	: __n{in_n}
	, __out_vertices_count{in_m}
{
	if (in_n + 1 !=  dvert.size())
		throw std::runtime_error("Input bezier data is incorrect: vec.size() != in_n");

	if (!bfMathIsVerticesInPlain(dvert))
		throw std::runtime_error("Not all vertices in one plane of Bezier curve");

	__dvertices = std::move(dvert);
}

BfBezierCurve::BfBezierCurve(size_t in_n, size_t in_m, std::vector<BfVertex3>& dvert)
	: __n{in_n}
	, __out_vertices_count{ in_m }
{
	if (in_n + 1 != dvert.size())
		throw std::runtime_error("Input bezier data is incorrect: vec.size() != in_n");

	if (!bfMathIsVerticesInPlain(dvert))
		throw std::runtime_error("Not all vertices in one plane of Bezier curve");

	__dvertices = dvert;
}

BfBezierCurve::BfBezierCurve(BfBezierCurve&& ncurve) noexcept
	: __n {ncurve.__n}
	, __out_vertices_count {ncurve.__out_vertices_count}
{
	this->__dvertices = std::move(ncurve.__dvertices);
	this->__main_color = ncurve.__main_color;
	this->__pPipeline = ncurve.__pPipeline;
}

const size_t BfBezierCurve::get_n() const noexcept
{
	return __n;
}

glm::vec3 BfBezierCurve::get_single_vertex_v3(float t) const
{
	glm::vec3 _v{ 0.0f, 0.0f, 0.0f };
	for (size_t i = 0; i <= __n; i++) {
		_v += static_cast<float>(bfMathGetBinomialCoefficient(__n, i) *
			std::pow(1 - t, __n - i) *
			std::pow(t, i)) * __dvertices[i].pos;
	}
	return _v;
}

BfVertex3 BfBezierCurve::get_single_vertex_bfv3(float t) const
{
	BfVertex3 _v{};
	_v.pos = this->get_single_vertex_v3(t);
	_v.color = __main_color;
	_v.normals = bfMathGetNormal(__dvertices[0].pos, __dvertices[1].pos, __dvertices[2].pos);

	return _v;
}

BfBezierCurve BfBezierCurve::get_derivative()
{
	uint32_t k = __n - 1;
	std::vector<BfVertex3> new_define_vertices;
	new_define_vertices.reserve(k + 1);

	for (int i = 0; i <= k; i++) {
		new_define_vertices.emplace_back(
			static_cast<float>(__n) * (__dvertices[i + 1].pos - __dvertices[i].pos),
			this->__main_color,
			__dvertices[0].normals
		);
	}

	BfBezierCurve curve(k, this->__out_vertices_count, new_define_vertices);

	return curve;
}

glm::vec3 BfBezierCurve::get_single_derivative_1_analyt_v3(float t) const
{
	int k = __n - 1;
	glm::vec3 _v{0.0f};
	for (int i = 0; i <= k; i++) {
		_v += (float)(bfMathGetBinomialCoefficient(k, i) * 
					  std::pow(1 - t, k - i) * 
					  std::pow(t, i) * __n) * 
					  (__dvertices[i + 1].pos - __dvertices[i].pos);
	}
	return _v;
}

BfVertex3 BfBezierCurve::get_single_derivative_1_analyt_bfv3(float t) const
{
	BfVertex3 _v{};
	_v.pos = this->get_single_derivative_1_analyt_v3(t);
	_v.color = __main_color;
	_v.normals = bfMathGetNormal(__dvertices[0].pos, __dvertices[1].pos, __dvertices[2].pos);
	return _v;
}

glm::vec3 BfBezierCurve::get_single_derivative_1_numeric_v3(float t, float step) const
{
	glm::vec3 left = this->get_single_vertex_v3(t - step);
	glm::vec3 right = this->get_single_vertex_v3(t + step);

	return (right - left) / 2.0f / step;
}

glm::vec3 BfBezierCurve::get_single_derivative_2_analyt_v3(float t) const
{
	int k = __n - 2;
	glm::vec3 _v{0.0f};
	for (int i = 0; i <= k; i++) {
		_v += (float)((bfMathGetBinomialCoefficient(k, i) *
					   std::pow(1 - t, k - i) *
					   std::pow(t, i) * __n) *
			((__n - 1))) * 
			(__dvertices[i].pos - 2.0f * __dvertices[i + 1].pos + __dvertices[i + 2].pos);
	}
	return _v;
}

BfVertex3 BfBezierCurve::get_single_derivative_2_analyt_bfv3(float t) const
{
	BfVertex3 _v{};
	_v.pos = this->get_single_derivative_2_analyt_v3(t);
	_v.color = __main_color;
	_v.normals = bfMathGetNormal(__dvertices[0].pos, __dvertices[1].pos, __dvertices[2].pos);
	return _v;
}

const size_t BfBezierCurve::get_out_vertices_count() const noexcept
{
	return __out_vertices_count;
}

void BfBezierCurve::set_out_vertices_count(size_t in_m)
{
	__out_vertices_count = in_m;
}

void BfBezierCurve::create_vertices()
{
	__vertices.clear();

	float t;
	for (int i = 0; i < __out_vertices_count; i++) {

		t = static_cast<float>(i) / static_cast<float>(__out_vertices_count - 1);
		__vertices.emplace_back(this->get_single_vertex_bfv3(t));
	}
}

