#include "bfCurves3.h"


BfPlane::BfPlane(std::vector<BfVertex3> d_vertices)
	: BfDrawObj(BF_DRAW_OBJ_TYPE_PLANE)
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

BfSingleLine::BfSingleLine(const BfVertex3& fp, 
						   const BfVertex3& sp)
	: BfDrawObj{{ fp, sp }, BF_DRAW_OBJ_TYPE_SINGLE_LINE}
{
	__dvertices.reserve(2);
	__vertices.reserve(2);
	__indices.reserve(2);
}

BfSingleLine::BfSingleLine(const glm::vec3& fp, 
						   const glm::vec3& sp)
	: BfSingleLine(
		{ fp, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f} },
		{ sp, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f} }
	)
{
}



float BfSingleLine::get_k(int proj) {
	
	glm::vec3 f{ this->get_first().pos };
	glm::vec3 s{ this->get_second().pos };
	
	switch (proj) {
	case BF_SINGLE_LINE_PROJ_XY:
		return	(s.y - f.y) / 
				(s.x - f.x); 
	case BF_SINGLE_LINE_PROJ_XZ:
		return	(s.z - f.z) /
				(s.x - f.x);
	case BF_SINGLE_LINE_PROJ_YX:
		return	(s.x - f.x) /
				(s.y - f.y);
	case BF_SINGLE_LINE_PROJ_YZ:
		return	(s.z - f.z) /
				(s.y - f.y);
	case BF_SINGLE_LINE_PROJ_ZX:
		return	(s.x - f.x) /
				(s.z - f.z);
	case BF_SINGLE_LINE_PROJ_ZY:
		return	(s.y - f.y) /
				(s.z - f.z);
	default:
		throw std::runtime_error("BfSingleLine::get_k - invalid proj");
	}
}

float BfSingleLine::get_k_perpendicular(int proj) {
	return -1.0f / this->get_k(proj);
}

float BfSingleLine::get_b(int proj) {
	glm::vec3 f{ this->get_first().pos };
	glm::vec3 s{ this->get_second().pos };

	switch (proj) {
	case BF_SINGLE_LINE_PROJ_XY:
		return f.y - this->get_k(proj) * f.x;
	case BF_SINGLE_LINE_PROJ_XZ:
		return f.z - this->get_k(proj) * f.x;
	case BF_SINGLE_LINE_PROJ_YX:
		return f.x - this->get_k(proj) * f.y;
	case BF_SINGLE_LINE_PROJ_YZ:
		return f.z - this->get_k(proj) * f.y;
	case BF_SINGLE_LINE_PROJ_ZX:
		return f.x - this->get_k(proj) * f.z;
	case BF_SINGLE_LINE_PROJ_ZY:
		return f.y - this->get_k(proj) * f.z;
	default:
		throw std::runtime_error("BfSingleLine::get_b - invalid proj");
	}
}

float BfSingleLine::get_b_perpendicular(glm::vec3 f, int proj) {
	float k_per{ this->get_k_perpendicular(proj) };

	switch (proj) {
	case BF_SINGLE_LINE_PROJ_XY:
		return f.y - k_per * f.x;
	case BF_SINGLE_LINE_PROJ_XZ:
		return f.z - k_per * f.x;
	case BF_SINGLE_LINE_PROJ_YX:
		return f.x - k_per * f.y;
	case BF_SINGLE_LINE_PROJ_YZ:
		return f.z - k_per * f.y;
	case BF_SINGLE_LINE_PROJ_ZX:
		return f.x - k_per * f.z;
	case BF_SINGLE_LINE_PROJ_ZY:
		return f.y - k_per * f.z;
	default:
		throw std::runtime_error("BfSingleLine::get_b_perpenduclar - invalid proj");
	}
}

float BfSingleLine::get_single_proj(float ival, int proj) {
	return this->get_k(proj) * ival + this->get_b(proj);
}

float BfSingleLine::get_length() {
	return glm::distance(get_first().pos, get_second().pos);
}

const BfVertex3& BfSingleLine::get_first() const
{
	return __dvertices.at(0);
}

const BfVertex3& BfSingleLine::get_second() const
{
	return __dvertices.at(1);
}

glm::vec3 BfSingleLine::get_direction_from_start() const
{
	return glm::normalize(this->get_second().pos - this->get_first().pos);
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

BfTriangle::BfTriangle(const BfVertex3& P_1,
					   const BfVertex3& P_2,
					   const BfVertex3& P_3) 
	: BfDrawObj({ {P_1, P_2, P_3} }, BF_DRAW_OBJ_TYPE_TRIANGLE)
{ 
	__vertices.reserve(4);

}

const BfVertex3& BfTriangle::get_first() const {
	return __dvertices[0];
}
const BfVertex3& BfTriangle::get_second() const {
	return __dvertices[1];
}
const BfVertex3& BfTriangle::get_third() const {
	return __dvertices[2];
}

float BfTriangle::get_area() const {
	glm::vec3 u = get_second().pos - get_first().pos;
	glm::vec3 v = get_third().pos - get_first().pos;
	return glm::length(glm::cross(u, v)) / 2.0f;
}

BfVertex3 BfTriangle::get_center() const {
	return {
		(get_first().pos + get_second().pos + get_third().pos) / 3.0f,
		get_first().color,
		bfMathGetNormal(get_first().pos, get_second().pos, get_third().pos)
	};
}

void BfTriangle::create_vertices() {
	for (auto& it : __dvertices) {
		__vertices.emplace_back(it);
	}
	__vertices.emplace_back(__dvertices[0]);
}



glm::vec3 bfMathFindLinesIntersection(
	const BfSingleLine& line1, 
	const BfSingleLine& line2,
	int mode
) {
	if (bfMathIsVerticesInPlain({
			line1.get_first(),
			line1.get_second(),
			line2.get_first(),
			line2.get_second()
		})) 
	{
		// a1.x + b1.x * t1 = a2.x + b2.x * t2
		// a1.y + b1.y * t1 = a2.y + b2.y * t2
		glm::vec3 a1 = line1.get_first().pos;
		glm::vec3 b1 = line1.get_direction_from_start();

		glm::vec3 a2 = line2.get_first().pos;
		glm::vec3 b2 = line2.get_direction_from_start();

		float frac = b1.x * b2.y - b1.y * b2.x;

		float t1 = (a1.x * b2.y - a1.y * b2.x - a2.x * b2.y + a2.y * b2.x) / (-frac);
		float t2 = (a1.x * b1.y - a1.y * b1.x - a2.x * b1.y + a2.y * b1.x) / (-frac);

		// if lines are parallel
		if (glm::isnan(t1) || glm::isnan(t2)) {
			return glm::vec3(std::nan(""), std::nan(""), std::nan(""));
		}
		if (mode == BF_MATH_FIND_LINES_INTERSECTION_BETWEEN_VERTICES) {
			if ((t1 >= 0.0f) && (t1 <= 1.0f) && (t2 >= 0.0f) && (t2 <= 1.0f)) {
				return a1 + b1 * t1;
			}
			else {
				return glm::vec3(std::nan(""), std::nan(""), std::nan(""));
			}
		}
		else if (mode == BF_MATH_FIND_LINES_INTERSECTION_ANY) {
			return a1 + b1 * t1;
		}
		else {
			throw std::runtime_error("Invalid bfMathFindLinesIntersection mode inputed");
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
	glm::vec3 _v1{ p2 - p1 };
	glm::vec3 _v2{ p3 - p1 };
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

std::array<glm::vec3, 3> bfMathGetPlaneOrths(glm::vec4 plane) {
	std::array<glm::vec3, 3> orths;
	orths[0] = {plane.x, plane.y, plane.z }; // normal
	orths[1] = glm::normalize(glm::cross(orths[0], glm::vec3(0.0f, 0.0f, 1.0f)));
	orths[2] = glm::normalize(glm::cross(glm::cross(orths[0], orths[1]), orths[1]));
	return orths;
}

std::array<glm::vec3, 2> bfMathGetOrthsByNormal(glm::vec3 normal) {
	std::array<glm::vec3, 2> orths;
	orths[0] = glm::normalize(glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
	orths[1] = glm::normalize(glm::cross(glm::cross(orths[0], normal), orths[0]));
	return orths;
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
	
	// Вычисляем векторы между точками
	glm::vec3 fs = s - f;
	glm::vec3 ft = t - f;
	glm::vec3 fn = np - f;

	// Вычисляем скалярное тройное произведение
	float volume = glm::dot(glm::cross(fs, ft), fn);

	// Проверяем, если объем равен нулю (точки лежат в одной плоскости)
	if (glm::abs(volume) < BF_MATH_ABS_ACCURACY)
		return true;
	else
		return false;
	
	//glm::vec3 first = bfMathGetNormal(f, s, t);
	//glm::vec3 second = bfMathGetNormal(np, f, s);
	//
	//// On 1 line
	//if (glm::length(first) < BF_MATH_ABS_ACCURACY && 
	//	glm::length(second) < BF_MATH_ABS_ACCURACY) {
	//	return true;
	//}
	//if (glm::dot(first, second) > 1.0f - BF_MATH_ABS_ACCURACY) {
	//	glm::vec3 fs = s - f;
	//	glm::vec3 ft = t - f;
	//	glm::vec3 npf = np - f;
	//	float volume = glm::dot(glm::cross(fs, ft), npf);

	//	if (glm::abs(volume) < BF_MATH_ABS_ACCURACY) {
	//		return true;
	//	}
	//}

	///*if ((CHECK_FLOAT_EQUALITY_TO_NULL(first.x, 0.0f) &&
	//	 CHECK_FLOAT_EQUALITY_TO_NULL(first.y, 0.0f) &&
	//	 CHECK_FLOAT_EQUALITY_TO_NULL(first.z, 0.0f)) 
	//	 ||
	//	(CHECK_FLOAT_EQUALITY_TO_NULL(second.x, 0.0f) &&
	//	 CHECK_FLOAT_EQUALITY_TO_NULL(second.y, 0.0f) &&
	//	 CHECK_FLOAT_EQUALITY_TO_NULL(second.z, 0.0f))) 
	//{
	//	first = bfMathGetNormal(f, s, t);
	//	second = bfMathGetNormal(np, f, s);
	//}*/

	//glm::vec3 delta = second - first;

	//if ((delta.x <= BF_MATH_ABS_ACCURACY) &&
	//	(delta.y <= BF_MATH_ABS_ACCURACY) &&
	//	(delta.z <= BF_MATH_ABS_ACCURACY))
	//	return true;
	//else
	//	return false;
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

bool bfMathFindLinesIntersection(glm::vec3& intersection,
								 const BfSingleLine& line1,
								 const BfSingleLine& line2) 
{
	glm::vec3 P1_first{ line1.get_first().pos };
	glm::vec3 P1_second{ line1.get_second().pos };
	glm::vec3 P2_first{ line2.get_first().pos };
	glm::vec3 P2_second{ line2.get_second().pos };

	if (bfMathIsVerticesInPlain({ P1_first, P1_second, P2_first, P2_second })) {
		
		glm::vec3 a1 = P1_first;
		glm::vec3 b1 = line1.get_direction_from_start();

		glm::vec3 a2 = P2_first;
		glm::vec3 b2 = line2.get_direction_from_start();

		float frac = b1.x * b2.y - b1.y * b2.x;

		float t1 = (a1.x * b2.y - a1.y * b2.x - a2.x * b2.y + a2.y * b2.x) / (-frac);
		float t2 = (a1.x * b1.y - a1.y * b1.x - a2.x * b1.y + a2.y * b1.x) / (-frac);

		// if lines are parallel
		if (glm::isnan(t1) || glm::isnan(t2)) {
			return false;
		}

		if ((t1 >= 0.0f) && (t1 <= 1.0f) && (t2 >= 0.0f) && (t2 <= 1.0f)) {
			intersection = a1 + b1 * t1;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
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

bool bfMathIsVerticesInPlain(const std::vector<glm::vec3>& np) {

	std::vector<BfVertex3> _np;
	_np.reserve(np.size());
	for (const auto& it : np) {
		_np.emplace_back(it);
	}
	return bfMathIsVerticesInPlain(_np);
}


bool bfMathIsVerticesInPlain(std::initializer_list<glm::vec3> np) {
	std::vector<glm::vec3> _np;
	_np.reserve(np.size());
	for (const auto& it : np) {
		_np.emplace_back(it);
	}
	
	return bfMathIsVerticesInPlain(_np);
}

bool bfMathIsSingleLinesInPlain(const BfSingleLine& L1, const BfSingleLine& L2) {
	return bfMathIsVerticesInPlain({
		L1.get_first(),
		L1.get_second(),
		L2.get_first(),
		L2.get_second()
	});
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

std::vector<BfCircle> bfMathGetInscribedCircles(size_t m,
												const BfSingleLine& L1,
												const BfSingleLine& L2,
												float radius)
{
	glm::vec3 line_intersection = bfMathFindLinesIntersection(L1, L2, BF_MATH_FIND_LINES_INTERSECTION_ANY);

	glm::vec3 L1_dir = L1.get_direction_from_start();
	glm::vec3 L2_dir = L2.get_direction_from_start();

	float angle_1 = glm::degrees(glm::angle(L1_dir, L2_dir));
	float angle_2 = 180.0f - angle_1;
	
	glm::vec3 normal = bfMathGetNormal(
		L1.get_first().pos,
		L1.get_second().pos,
		L2.get_first().pos
	);
	if (normal == glm::vec3(0.0f)) {
		normal = bfMathGetNormal(
			L1.get_first().pos,
			L2.get_second().pos,
			L2.get_first().pos
		);
	}

	L1_dir = glm::vec3(glm::rotate(glm::mat4(1.0f), -glm::radians(angle_1 / 2.0f), normal) * glm::vec4(L1_dir, 1.0f));
	L2_dir = glm::vec3(glm::rotate(glm::mat4(1.0f), -glm::radians(angle_2 / 2.0f), normal) * glm::vec4(L2_dir, 1.0f));

	auto c1 = line_intersection + glm::normalize(L1_dir) * radius / glm::sin(glm::radians(angle_1) / 2.0f);
	auto c2 = line_intersection + -glm::normalize(L1_dir) * radius / glm::sin(glm::radians(angle_1) / 2.0f);
	auto c3 = line_intersection + glm::normalize(L2_dir) * radius / glm::sin(glm::radians(angle_2) / 2.0f);
	auto c4 = line_intersection + -glm::normalize(L2_dir) * radius / glm::sin(glm::radians(angle_2) / 2.0f);

	return {
		{m, {c1, {1.0f, 1.0f, 1.0f}, normal}, radius},
		{m, {c2, {1.0f, 1.0f, 1.0f}, normal}, radius},
		{m, {c3, {1.0f, 1.0f, 1.0f}, normal}, radius},
		{m, {c4, {1.0f, 1.0f, 1.0f}, normal}, radius},
	};
}

float bfMathGetDistanceToLine(const BfSingleLine& L, BfVertex3 P) {
	
	/*glm::vec3 lineVector = L.get_second().pos - L.get_first().pos;
	
	glm::vec3 pointToLinePointVector = L.get_first().pos - P.pos;
	
	float projectionLength = glm::dot(pointToLinePointVector, lineVector) / glm::dot(lineVector, lineVector);
	glm::vec3 projection = projectionLength * lineVector;
	
	glm::vec3 distanceVector = pointToLinePointVector - projection;
	
	return glm::length(distanceVector);*/

	/*glm::vec3 lineVector = L.get_second().pos - L.get_first().pos;
	glm::vec3 pointToLineVector = P.pos - L.get_first().pos;

	float t = glm::dot(pointToLineVector, lineVector) / glm::dot(lineVector, lineVector);

	glm::vec3 projectionPoint = L.get_first().pos + t * lineVector;

	return glm::distance(projectionPoint, P.pos);*/

	glm::vec3 normal = bfMathGetNormal(L.get_first().pos, L.get_second().pos, P.pos);
	glm::vec3 dir_to_line = glm::normalize(glm::cross(L.get_direction_from_start(), normal));
	
	BfSingleLine line(P, P.pos + dir_to_line);

	glm::vec3 intersection = bfMathFindLinesIntersection(L, line, BF_MATH_FIND_LINES_INTERSECTION_ANY);

	return glm::distance(intersection, P.pos);
}

std::vector<BfVertex3> bfMathStickObjVertices(std::initializer_list<std::shared_ptr<BfDrawObj>> objs) {
	size_t total_size = 0;
	for (auto& obj : objs) {
		if (obj->get_vertices_count() == 0) abort();
		total_size += obj->get_vertices_count();
	}
	std::vector<BfVertex3> out;
	out.reserve(total_size);

	for (auto& obj : objs) {
		out.insert(out.end(), obj->get_rVertices().begin(), obj-> get_rVertices().end());
	}

	return out;
}

std::vector<std::shared_ptr<BfTriangle>> bfMathGenerateTriangleField(std::vector<BfVertex3> v) {
	
	//auto get_triangle_count = [](size_t current) {
	//	/*if (current <= 5)
	//		return (size_t)0;*/
	//	// even
	//	if (current % 2 == 0) 
	//		return (size_t)(current / 2.0f);
	//	// odd 
	//	else 
	//		return (size_t)((current) / 2.0f) - 1;
	//};

	//std::vector<std::shared_ptr<BfTriangle>> out;
	//out.reserve(get_triangle_count(v.size()) * 2);

	//for (size_t i = 0; i < get_triangle_count(v.size()) -1; i++) {
	//	auto t = std::make_shared<BfTriangle>(
	//		v[0 + i * 2],
	//		v[1 + i * 2],
	//		v[2 + i * 2]
	//	);
	//	out.emplace_back(t);
	//	if (i == get_triangle_count(v.size()) - 2) {
	//		int a = 10;
	//	}
	//}

	//return out;

	//// Подготовка входных данных для библиотеки Triangle
	//std::vector<double> points;
	//std::vector<int> segments;

	//for (const auto& vertex : v) {
	//	points.push_back(static_cast<double>(vertex.pos.x));
	//	points.push_back(static_cast<double>(vertex.pos.y));
	//	points.push_back(static_cast<double>(vertex.pos.z));
	//}

	//// Опции для триангуляции
	//char options[] = "p";

	//// Создаем структуру triangulateio для входных данных и результатов
	//struct triangulateio in, out;

	//// Инициализируем структуры triangulateio
	//memset(&in, 0, sizeof(struct triangulateio));
	//memset(&out, 0, sizeof(struct triangulateio));

	//// Заполняем структуру in данными
	//in.numberofpoints = points.size() / 3;
	//in.pointlist = &points[0];

	//// Выполняем триангуляцию
	//triangulate(options, &in, &out, NULL);

	//// Результаты триангуляции хранятся в структуре out
	//std::vector<std::array<glm::vec3, 3>> triangles;
	//for (int i = 0; i < out.numberoftriangles; ++i) {
	//	std::array<glm::vec3, 3> triangle;
	//	for (int j = 0; j < 3; ++j) {
	//		int vertexIndex = out.trianglelist[i * 3 + j];
	//		triangle[j] = glm::vec3(
	//			static_cast<float>(out.pointlist[vertexIndex * 2]),
	//			static_cast<float>(out.pointlist[vertexIndex * 2 + 1]),
	//			0.0f // Здесь предполагается, что треугольники находятся в плоскости XY
	//		);
	//	}
	//	triangles.push_back(triangle);
	//}

	//// Освобождаем память, выделенную для результатов
	//free(out.pointlist);
	//free(out.trianglelist);

	return std::vector<std::shared_ptr<BfTriangle>>();
}

glm::vec3 bfMathFindMassCenter(std::vector<BfVertex3> v) {


	return glm::vec3(0.0f);
}


BfBezierCurve::BfBezierCurve(size_t in_n, size_t in_m, std::vector<BfVertex3>&& dvert)
	: BfDrawObj(BF_DRAW_OBJ_TYPE_BEZIER_CURVE)
	, __n{in_n}
	, __out_vertices_count{in_m}
{
	if (in_n + 1 !=  dvert.size())
		throw std::runtime_error("Input bezier data is incorrect: vec.size() != in_n");

	if (!bfMathIsVerticesInPlain(dvert))
		throw std::runtime_error("Not all vertices in one plane of Bezier curve");

	__dvertices = std::move(dvert);
}

BfBezierCurve::BfBezierCurve(size_t in_n, size_t in_m, std::vector<BfVertex3>& dvert)
	: BfDrawObj(BF_DRAW_OBJ_TYPE_BEZIER_CURVE)
	, __n{in_n}
	, __out_vertices_count{ in_m }
{
	if (in_n + 1 != dvert.size())
		throw std::runtime_error("Input bezier data is incorrect: vec.size() != in_n");

	if (!bfMathIsVerticesInPlain(dvert))
		throw std::runtime_error("Not all vertices in one plane of Bezier curve");

	__dvertices = dvert;
}

BfBezierCurve::BfBezierCurve(BfBezierCurve&& ncurve) noexcept
	: BfDrawObj(BF_DRAW_OBJ_TYPE_BEZIER_CURVE)
	, __n {ncurve.__n}
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

BfCircle::BfCircle(size_t m, const BfVertex3& center, float radius)
	: BfDrawObj(BF_DRAW_OBJ_TYPE_CIRCLE)
	, __out_vertices_count{ m }
	, __radius{ radius }
	, __define_type{ BF_CIRCLE_DEFINE_TYPE_CENTER_RADIUS }
{
	__dvertices = { center };
	__vertices.reserve(__out_vertices_count);
	__indices.reserve(__out_vertices_count * 2);
}

BfCircle::BfCircle(size_t m,
				   const BfVertex3& P_1, 
				   const BfVertex3& P_2, 
				   const BfVertex3& P_3)
	: BfDrawObj(BF_DRAW_OBJ_TYPE_CIRCLE)
	, __out_vertices_count{ m }
	, __define_type{ BF_CIRCLE_DEFINE_TYPE_3_VERTICES }
{

	BfSingleLine l_12(P_1, P_2);
	BfSingleLine l_23(P_2, P_3);
	BfSingleLine l_31(P_3, P_1);

	glm::vec3 ave_12 = (P_1.pos + P_2.pos) / 2.0f;
	glm::vec3 ave_23 = (P_2.pos + P_3.pos) / 2.0f;
	glm::vec3 ave_31 = (P_3.pos + P_1.pos) / 2.0f;

	glm::vec3 v_12 = l_12.get_direction_from_start();
	glm::vec3 v_23 = l_23.get_direction_from_start();
	glm::vec3 v_31 = l_31.get_direction_from_start();

	glm::vec3 n_12 = glm::cross(glm::cross(v_12, v_23), v_12);
	glm::vec3 n_23 = glm::cross(glm::cross(v_23, v_12), v_23);
	glm::vec3 n_31 = glm::cross(glm::cross(v_31, v_23), v_31);

	BfSingleLine per_l_12(ave_12, ave_12 + n_12);
	BfSingleLine per_l_23(ave_23, ave_23 + n_23);
	BfSingleLine per_l_31(ave_31, ave_31 + n_31);
	
	BfVertex3 center;
	center.pos = bfMathFindLinesIntersection(
		per_l_12, 
		per_l_23,
		BF_MATH_FIND_LINES_INTERSECTION_ANY
	);
	// TODO
	if (glm::isnan(center.pos.x) || glm::isnan(center.pos.y) || glm::isnan(center.pos.z)) {
		//throw std::runtime_error("Circle define points are invalid");
		if (P_1.equal(P_2)) {
			center = BfVertex3(
				{ P_1.pos.x + P_2.pos.x / 2.0f,
				  P_1.pos.y + P_2.pos.y / 2.0f,
				  P_1.pos.z + P_2.pos.z / 2.0f },

				P_1.color,
				P_1.normals);
			__radius = glm::distance(P_1.pos, P_2.pos) / 2.0f;
			__dvertices = { center, P_1, P_2, P_3 };
		}
		else if (P_2.equal(P_3)) {
			center = BfVertex3(
				{ P_3.pos.x + P_2.pos.x / 2.0f,
				  P_3.pos.y + P_2.pos.y / 2.0f,
				  P_3.pos.z + P_2.pos.z / 2.0f },

				P_3.color,
				P_3.normals);
			__radius = glm::distance(P_2.pos, P_3.pos) / 2.0f;
			__dvertices = { center, P_1, P_2, P_3 };
		}
		else if (P_3.equal(P_1)) {
			center = BfVertex3(
				{ P_1.pos.x + P_3.pos.x / 2.0f,
				  P_1.pos.y + P_3.pos.y / 2.0f,
				  P_1.pos.z + P_3.pos.z / 2.0f },

				P_1.color,
				P_1.normals);
			__radius = glm::distance(P_3.pos, P_1.pos) / 2.0f;
			__dvertices = { center, P_1, P_2, P_3 };
		}
		return;
	}

	float rad_1 = glm::distance(center.pos, P_1.pos);
	float rad_2 = glm::distance(center.pos, P_1.pos);
	float rad_3 = glm::distance(center.pos, P_1.pos);

	if (!CHECK_FLOAT_EQUALITY(rad_1, rad_2) ||
		!CHECK_FLOAT_EQUALITY(rad_2, rad_3) ||
		!CHECK_FLOAT_EQUALITY(rad_3, rad_1)) 
	{
		
		
		throw std::runtime_error("Circle was'n made -> different radious");
	}
	
	center.normals = bfMathGetPlaneCoeffs(P_1.pos, P_2.pos, P_3.pos);

	__radius = rad_1;
	__dvertices = { center, P_1, P_2, P_3 };
}


const BfVertex3& BfCircle::get_center()  const noexcept {
	return __dvertices[0];
}
const BfVertex3& BfCircle::get_first()  const noexcept {
	return __dvertices[1];
}
const BfVertex3& BfCircle::get_second()  const noexcept {
	return __dvertices[2];
}
const BfVertex3& BfCircle::get_third()  const noexcept {
	return __dvertices[3];
}

const float BfCircle::get_radius() const noexcept {
	return __radius;
}

std::array<BfVertex3, 2> BfCircle::get_tangent_vert(const BfVertex3& P) const {

	glm::vec4 plane{
		this->get_center().normals,
		-(this->get_center().normals.x * P.pos.x +
		this->get_center().normals.y * P.pos.y +
		this->get_center().normals.z * P.pos.z)
	};

	if (!bfMathIsVertexInPlain(plane, P.pos)) {
		throw std::runtime_error("BfCircle::get_tangent_vert - point not in the same plane as circle");
	}

	BfSingleLine dist{ P, this->get_center() };
	float distance = glm::distance(dist.get_first().pos, dist.get_second().pos);

	glm::vec3 dir = dist.get_direction_from_start();

	glm::vec3 c1 = glm::rotate(glm::mat4(1.0f), 
		 					   glm::acos(this->get_radius() / distance), 
							   this->get_center().normals) * glm::vec4(dir, 1.0f);
	
	glm::vec3 c2 = glm::rotate(glm::mat4(1.0f),
							   -glm::acos(this->get_radius() / distance),
							   this->get_center().normals) * glm::vec4(dir, 1.0f);



	return {
		BfVertex3{{this->get_center().pos - glm::normalize(c1) * __radius}, {1.0f, 1.0f, 1.0f}, this->get_center().normals },
		BfVertex3{{this->get_center().pos - glm::normalize(c2) * __radius}, {1.0f, 1.0f, 1.0f}, this->get_center().normals }
	};
}


void BfCircle::create_vertices() {

	if (!__vertices.empty()) __vertices.clear();


	glm::vec3 orth_1;
	glm::vec3 orth_2;
	glm::vec3 normal = this->get_center().normals;

	if (std::abs(normal.x) > std::abs(normal.y)) {
		orth_1 = glm::normalize(glm::vec3(-normal.z, 0.0f, normal.x));
	}
	else {
		orth_1 = glm::normalize(glm::vec3(0.0f, normal.z, -normal.y));
	}
	orth_2 = glm::normalize(glm::cross(normal, orth_1));


	float theta;
	glm::vec3 center = this->get_center().pos;
	for (size_t i = 0; i < __out_vertices_count+1; ++i) {
		theta = 2 * BF_PI * i / __out_vertices_count;
		__vertices.emplace_back(
			center +
			__radius * cosf(theta) * orth_1 +
			__radius * sinf(theta) * orth_2,

			this->__main_color,
			normal
		);
	}
	
}

BfArc::BfArc(size_t m,
	const BfVertex3& P_1,
	const BfVertex3& P_2,
	const BfVertex3& P_3) 
	: BfCircle(m, P_1, P_2, P_3)
{}

void BfArc::create_vertices()
{
	if (!__vertices.empty()) __vertices.clear();

	glm::vec3 center = this->get_center().pos;
	glm::vec3 firstPoint = this->get_first().pos;
	glm::vec3 secondPoint = this->get_second().pos;
	glm::vec3 thirdPoint = this->get_third().pos;

	glm::vec3 vecToFirst = firstPoint - center;
	glm::vec3 vecToSecond = secondPoint - center;
	glm::vec3 vecToThird = thirdPoint - center;

	float angle_12 = acos(glm::dot(vecToFirst, vecToSecond) / 
		(glm::length(vecToFirst) * glm::length(vecToSecond)));

	float angle_23 = acos(glm::dot(vecToSecond, vecToThird) / 
		(glm::length(vecToSecond) * glm::length(vecToThird)));


	float theta;
	//glm::vec3 center = this->get_center().pos;
	for (size_t i = 0; i <= __out_vertices_count; ++i) {
		float theta = (angle_12 + angle_23) * i / (__out_vertices_count);
		
		__vertices.emplace_back(
			center -
			glm::vec3(glm::rotate(glm::mat4(1.0f), theta, this->get_center().normals) * 
				glm::vec4((center - firstPoint), 1.0f)),

			this->__main_color,
			glm::vec3(0.0f, 0.0f, 0.0f)
		);
	}

}

BfBezierCurveFrame::BfBezierCurveFrame(
	std::shared_ptr<BfBezierCurve> curve, 
	VmaAllocator allocator,
	VkPipeline lines_pipeline,
	VkPipeline triangle_pipeline
) 
	: BfDrawLayer(allocator, sizeof(BfVertex3), 50, 20)
	, __curve{curve}
	, __lines_pipeline{ lines_pipeline }
	, __triangle_pipeline{ triangle_pipeline }
	
{
	for (size_t i = 0; i < curve->get_dvertices_count(); i++) {
		auto handle = std::make_shared<BfCircle>(
			20,
			curve->get_rdVertices()[i],
			BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS
		);
		handle->bind_pipeline(&__lines_pipeline);
		this->add_l(handle);
	}

	this->generate_draw_data();
	this->update_buffer();
}

void BfBezierCurveFrame::remake(
	std::shared_ptr<BfBezierCurve> curve,
	glm::vec3 c
) {
	this->del_all();
	__curve = curve;
	for (size_t i = 0; i < curve->get_dvertices_count(); i++) {
		auto handle = std::make_shared<BfCircle>(
			20,
			curve->get_rdVertices()[i],
			BF_BEZIER_CURVE_FRAME_HANDLE_RADIOUS
		);
		handle->bind_pipeline(&__lines_pipeline);
		handle->set_color(c);
		this->add_l(handle);
	}

	this->generate_draw_data();
	this->update_buffer();
}


//BfArc::BfArc(size_t vertices_count,  
//			 const BfVertex3& l,		//1
//			 const BfVertex3& m,	//2
//			 const BfVertex3& r)	//3
//	: __out_vertices_count{vertices_count}
//	, left(__dvertices[0])
//	, mid(__dvertices[1])
//	, right(__dvertices[2])
//	, center(__dvertices[3])
//
//{
//	auto p_12 = BfLineProp(l, m).get_perpendicular((l.pos + r.pos) / 2.0f);
//	auto p_23 = BfLineProp(m, r).get_perpendicular((m.pos + r.pos) / 2.0f);
//	auto p_31 = BfLineProp(r, l).get_perpendicular((r.pos + l.pos) / 2.0f);
//
//	BfMatrix A{
//		{-p_12.get_k(), 1},
//		{-p_23.get_k(), 1}
//	};
//	BfMatrix B{
//		{-p_12.get_b()},
//		{-p_23.get_b()}
//	};
//	BfVec2 center = solve_linear_mtx(A, B);
//	this->__dvertices.emplace_back(center.x, center.y, 0.0f);
//	
//
//
//}
