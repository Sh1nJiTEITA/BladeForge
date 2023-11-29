#ifndef BF_CURVES_HPP
#define BF_CURVES_HPP

#include "bfVariative.hpp"
#include "bfVertex2.hpp"
#include "bfMatrix.hpp"

#include <vector>
#include <algorithm>


#define BF_ALLIGN_BEZIER_X 0
#define BF_ALLIGN_BEZIER_Y 1
#define BF_ALLIGN_BEZIER_Z 2

#define BF_PLANE_XY 0x1
#define BF_PLANE_XZ 0x2
#define BF_PLANE_YZ 0x4

#define BF_AXIS_X 0x10
#define BF_AXIS_Y 0x20
#define BF_AXIS_Z 0x40

int32_t bfGetFactorial(int32_t n);
uint32_t bfGetBinomialCoefficient(uint32_t n, uint32_t k);


class BfBezier {
	
	uint32_t n;
	std::vector<glm::vec3> vertices;
	std::vector<BfVertex3> c_vertices;

public:

	/*
	* Default initialization:
	* 
	* Needed: nothing.
	* 
	* Initializes inner input vertices and output vertices arrays with length 0.
	* 
	*/
	BfBezier() : n{ 0 }, vertices{}, c_vertices{} {}

	BfBezier(uint32_t in_n, std::vector<BfVertex3>& in_vertices) : n{ in_n } 
	{
		if (in_n+1 != static_cast<uint32_t>(in_vertices.size())) {
			throw std::runtime_error("Input bezier data is incorrect: vec.size() != in_n");
		}
		
		vertices.resize(in_n);
		
		for (int i = 0; i < in_n+1; i++) {
			vertices[i] = in_vertices[i].pos;
		}
	}

	

	/*
	* Certain initialization:
	* 
	* Needed: 1. in_n - Bezier curve degree
	*		  2. in_vertices - Input Bezier curve vertices
	* 
	* Number of input vertices must equal (in_n - 1).
	*
	*/

	BfBezier(uint32_t in_n, std::vector<glm::vec3> in_vertices) : n{in_n}, vertices{in_vertices} {
		if (in_n+1 != static_cast<uint32_t>(in_vertices.size())) {
			throw std::runtime_error("Input bezier data is incorrect: vec.size() != in_n");
		}
	}

	/*
	* (CLASSIC_CALCULATION)
	* Returns calculated vertex based on input vertices and parameter (t).
	* 
	* Needed: 1. t - relative Bezier curve parameter, standart value of t = [0, 1]
	*		  
	*/
	glm::vec3 get_single_vertex(float t) {
		glm::vec3 _v{ 0.0f,0.0f,0.0f };
		for (int i = 0; i <= n; i++) {
			_v += static_cast<float>(bfGetBinomialCoefficient(n, i) * std::pow(1 - t, n - i) * std::pow(t, i)) * vertices[i];
		}
		return _v;
	}

	/*
	* Updates output arrays for vertices (glm::vec3).
	* 
	* Needed: v_count - number of calculating vertices.
	* 
	* Uses get_singel_vertex with (t), which is split by v_count.
	* 
	*/
	void update_vertices(uint32_t v_count) {
		c_vertices.clear();
		c_vertices.resize(v_count + 1);

		float t;
		for (int i = 0; i < v_count + 1; i++) {
			
			t = static_cast<float>(i) / static_cast<float>(v_count);
			c_vertices[i].pos = get_single_vertex(t);
			c_vertices[i].normals = get_direction_normal(t);
		}
	}
	
	/*
	* Returns output array of vertices.
	* 
	* Needed: nothing.
	* 
	* If vertices weren't calculated before, output array will be empty.
	* 
	*/
	std::vector<BfVertex3>& get_vertices() {
		return c_vertices;
	}

	/*
	* Calculates and outputs output arrays of vertices.
	* 
	* Needed: v_count - number of calculating vertices.
	* 
	*/
	std::vector<BfVertex3>& update_and_get_vertices(uint32_t v_count) {
		update_vertices(v_count);
		return get_vertices();
	}

	/*
	* Returns tangent of current Bezier curve point defined by (t).
	* 
	* Needed: t - relative Bezier curve parameter, standart value of t = [0, 1].
	* 
	*/
	glm::vec3 get_tangent(float t) {
		return this->get_single_derivative_1(t);
	}

	/*
	* Returns directional tangent (vector with length of 1) of current Bezier
	* curve point defined by (t).
	* 
	* Needed: t - relative Bezier curve parameter, standart value of t = [0, 1]
	* 
	*/
	glm::vec3 get_direction_tangent(float t) {
		glm::vec3 tangent = this->get_single_derivative_1(t);
		float d = glm::sqrt(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);

		return tangent / d;
	}

	/*
	* Returns directional normal (vector with length of 1) of current Bezier
	* curve point defined by (t).
	*
	* Needed: t - relative Bezier curve parameter, standart value of t = [0, 1]
	*
	*/
	glm::vec3 get_direction_normal(float t) {
		
		

		glm::vec3 a = this->get_direction_tangent(t);
		glm::vec3 b = glm::normalize(a + this->get_single_derivative_2_e(t));
		glm::vec3 r = glm::normalize(glm::cross(b, a));
		// TODO
		//return glm::vec3(-tangent.y, tangent.x, 0);
		return glm::normalize(glm::cross(r, a));
	}

	/*
	* (CLASSIC_CALCULATION)
	* Returns first derivative value of current Bezier
	* curve point defined by (t).
	*
	* Needed: t - relative Bezier curve parameter, standart value of t = [0, 1]
	*
	*/
	glm::vec3 get_single_derivative_1(float t) {
		
		int k = n - 1;
		glm::vec3 out(0.0f);
		for (int i = 0; i <= k; i++) {
			out += (float)(bfGetBinomialCoefficient(k, i) * std::pow(1 - t, k - i) * std::pow(t, i) * n) * (vertices[i + 1] - vertices[i]);
		}
		return out;
	}

	/*
	* (ALTERNATIVE_CALCULATION)
	* Returns first derivative value of current Bezier
	* curve point defined by (t).
	*
	* Needed: t - relative Bezier curve parameter, standart value of t = [0, 1]
	*
	*/
	glm::vec3 get_single_derivative_1_e(float t) {
		glm::vec3 out(0.0f);
		for (int i = 0; i <= n; i++) {
			out += (float)((i - n*t)/(t*(1-t))*bfGetBinomialCoefficient(n, i) * std::pow(1 - t, n - i) * std::pow(t, i)) * vertices[i];
		}
		return out;
	}

	/*
	* !(CLASSIC_CALCULATION)
	* Returns second derivative value of current Bezier
	* curve point defined by (t).
	*
	* Needed: t - relative Bezier curve parameter, standart value of t = [0, 1]
	*
	*/
	glm::vec3 get_single_derivative_2(float t) {
		int k = n - 2;
		glm::vec3 out(0.0f);
		for (int i = 0; i <= k; i++) {
			out += (float)(
				(
					bfGetBinomialCoefficient(k, i) * 
					std::pow(1 - t, k - i) * 
					std::pow(t, i) * n
				) 
				* 
				((n - 1))
				
				) * (vertices[i] - 2.0f * vertices[i + 1] + vertices[i + 2]);
		}
		return out;
	}

	/*
	* !(ALTERNATIVE_CALCULATION)
	* Returns second derivative value of current Bezier
	* curve point defined by (t).
	*
	* Needed: t - relative Bezier curve parameter, standart value of t = [0, 1]
	*
	*/
	glm::vec3 get_single_derivative_2_e(float t) {
		glm::vec3 out(0.0f);
		
		if (this->n == 1) return out;

		for (int i = 0; i <= n; i++) {
			if (t == 0)
			{
				out += (float)(n)/(n-1) * (vertices[0] - 2.0f * vertices[1] + vertices[2]);
				continue;
			}
			if (t == 1)
			{
				out += (float)(n)/(n-1) * (vertices[n] - 2.0f * vertices[n-1] + vertices[n-2]);
				continue;
			}
			
			
			out +=
				(float)((
					((i - n * t) * (i - n * t) - n * t * t  - i * (1 - 2 * t)) 
					/ 
					(t * t * (1 - t) * (1 - t))
				) * 
				bfGetBinomialCoefficient(n, i) * std::pow(1 - t, n - i) * std::pow(t, i)) * vertices[i];
		}
		return out;
	}

	/*
	* (CLASSIC_CALCULATION)
	* Returns Bezier curve witch is obtained by first derivative of current Bezier
	* curve
	*
	* Needed: nothing.
	*
	* New Bezier curve contains recalculated input vertices; 
	* its count = (n - 1); n - number of input vertices in Base Bezier curve.
	* 
	*/
	BfBezier get_derivative() {
		/*if (n == 1) 
			throw std::runtime_error("Bezier derivative is undefined -> n will be < 1");*/
		

		uint32_t k = this->n - 1;
		std::vector<glm::vec3> new_define_vertices(k+1);

		for (int i = 0; i <= k; i++) {

			new_define_vertices[i] = static_cast<float>(n) * (this->vertices[i + 1] - this->vertices[i]);
		}
		

		return BfBezier(k, new_define_vertices);
	}

	/*
	* (CLASSIC_CALCULATION)
	* Returns Bezier curve witch is obtained by m derivative of current Bezier
	* curve
	*
	* Needed: m - derivative degree;
	*
	* New Bezier curve contains recalculated input vertices;
	* its count = (n - m); n - number of input vertices in Base Bezier curve.
	*
	*/
	BfBezier get_derivative_n(uint32_t m) {
		BfBezier loc{*this};
		for (int i = 0; i < m; i++) {
			loc = loc.get_derivative();
		}
		return loc;
	}

	/*
	* Returns bezier curve with +1 order.
	* 
	* Needed: nothing.
	* 
	*/
	BfBezier get_elevated_order() {
		uint32_t k = this->n + 1;
		std::vector<glm::vec3> new_input_vertices(k + 1);

		for (int i = 0; i <= k; i++) {
			if (i == 0) {
				new_input_vertices[i] = this->vertices[i];
			}
			else if (i == k) {
				new_input_vertices[i] = this->vertices[i-1];
			}
			else {
				new_input_vertices[i] = ((float)(k - i) * this->vertices[i] + (float)i * this->vertices[i-1]) / (float)(k);
			}
		}
		return BfBezier(k, new_input_vertices);
	}
	// TODO 
	BfBezier get_lower_order() {
		const size_t _k = this->n;
		const size_t _n = _k - 1;
		
		if (_k <= 3)
			return *this;

		std::vector<std::vector<float>> mat(_k, std::vector<float>(_k - 1, 0));

		for (size_t i = 0; i < _k; ++i) {
			if (i == 0) {
				mat[i][0] = 1;
			}
			else if (i == n) {
				mat[i][i - 1] = 1; 
			}
			else {
				mat[i][i - 1] = i / static_cast<double>(_k);
				mat[i][i] = 1 - mat[i][i - 1];
			}
		}

		std::vector<std::vector<float>> mat_t(_k - 1, std::vector<float>(_k, 0));

		for (size_t i = 0; i < _k; i++) {
			for (size_t j = 0; j < _k - 1; j++) {
				mat_t[j][i] = mat[i][j];
			}
		}
	}

	/*
	* Returns 2 vec3 of t, each of which points to min and max t for each axes.
	* 
	* Needed: count = number of points for search.
	* 
	*/
	std::pair<glm::vec3, glm::vec3> get_bbox_t(size_t count = 100) {

		float max_x_less;
		float max_y_less;
		float max_z_less;

		float max_x_more;
		float max_y_more;
		float max_z_more;

		float t_x_less;
		float t_y_less;
		float t_z_less;

		float t_x_more;
		float t_y_more;
		float t_z_more;

		if ((*vertices.begin()).x < (*(vertices.end() - 1)).x) {
			max_x_less = (*vertices.begin()).x;
			max_x_more = (*(vertices.end() - 1)).x;

			t_x_less = 0.0f;
			t_x_more = 1.0f;
		}
		else {
			max_x_less = (*(vertices.end() - 1)).x;
			max_x_more = (*vertices.begin()).x;

			t_x_less = 1.0f;
			t_x_more = 0.0f;
		}

		if ((*vertices.begin()).y < (*(vertices.end() - 1)).y) {
			max_y_less = (*vertices.begin()).y;
			max_y_more = (*(vertices.end() - 1)).y;

			t_y_less = 0.0f;
			t_y_more = 1.0f;
		}
		else {
			max_y_less = (*(vertices.end() - 1)).y;
			max_y_more = (*vertices.begin()).y;

			t_y_less = 1.0f;
			t_y_more = 0.0f;
		}

		if ((*vertices.begin()).z < (*(vertices.end() - 1)).z) {
			max_z_less = (*vertices.begin()).z;
			max_z_more = (*(vertices.end() - 1)).z;

			t_z_less = 0.0f;
			t_z_more = 1.0f;
		}
		else {
			max_z_less = (*(vertices.end() - 1)).z;
			max_z_more = (*vertices.begin()).z;

			t_z_less = 1.0f;
			t_z_more = 0.0f;
		}
		
		float t;

		for (int i = 0; i < count; i++) {
			t = (float)i / ((float)count + 1);

			glm::vec3 v = this->get_single_vertex(t);

			if (v.x < max_x_less) {
				max_x_less = v.x;
				t_x_less = t;
			}
			if (v.x > max_x_more) {
				max_x_more = v.x;
				t_x_more = t;
			}

			if (v.y < max_y_less) {
				max_y_less = v.y;
				t_y_less = t;
			}
			if (v.y > max_y_more) {
				max_y_more = v.y;
				t_y_more = t;
			}

			if (v.z < max_z_less) {
				max_z_less = v.z;
				t_z_less = t;
			}
			if (v.z > max_z_more) {
				max_z_more = v.z;
				t_z_more = t;
			}
		}

		return std::pair<glm::vec3, glm::vec3>({ 
			{ t_x_less, t_y_less, t_z_less }, 
			{ t_x_more, t_y_more, t_z_more } }
		);
	}

	/*
	* Returns 2 points (positions), each of which points to min and max point of bounding
	* box.
	*
	* Needed: count = number of points for search.
	*
	*/
	std::pair<glm::vec3, glm::vec3> get_bbox() {
		std::pair<glm::vec3, glm::vec3> t_s = this->get_bbox_t();

	
		glm::vec3 min_point;
		
		glm::vec3 min_x = this->get_single_vertex(t_s.first.x);
		glm::vec3 min_y = this->get_single_vertex(t_s.first.y);
		glm::vec3 min_z = this->get_single_vertex(t_s.first.z);
		
		min_point.x = std::min({ min_x.x, min_y.x, min_z.x });
		min_point.y = std::min({ min_x.y, min_y.y, min_z.y });
		min_point.z = std::min({ min_x.z, min_y.z, min_z.z });
		

		glm::vec3 max_point;

		glm::vec3 max_x = this->get_single_vertex(t_s.second.x);
		glm::vec3 max_y = this->get_single_vertex(t_s.second.y);
		glm::vec3 max_z = this->get_single_vertex(t_s.second.z);

		max_point.x = std::max({ max_x.x, max_y.x, max_z.x });
		max_point.y = std::max({ max_x.y, max_y.y, max_z.y });
		max_point.z = std::max({ max_x.z, max_y.z, max_z.z });

		return std::pair<glm::vec3, glm::vec3>({ min_point, max_point });
	}

	/*
	* Returns same Bezier curve alligned in way of setting start defining point in 
	* (x=0, y=0, z=0), end defining point to input axe.
	* 
	* Needed: allign_mode - one of defines: BF_ALLIGN_BEZIER_X, BF_ALLIGN_BEZIER_Y,
	* BF_ALLIGN_BEZIER_Z
	* 
	*/
	BfBezier get_alligned(unsigned int plane, unsigned int axis) {
		
		BfBezier o = *this;

		glm::vec3 to_start = *vertices.begin();

		glm::vec3 last = (*(o.vertices.end() - 1)) - to_start;

		float r = sqrt(last.x * last.x + last.y * last.y + last.z * last.z);

		float angle_x = glm::degrees(glm::asin(-last.y / r));
		float angle_y = glm::degrees(glm::asin(-last.x / r));
		float angle_z = glm::degrees(last.z / r);

		BfMatrix rotate_matrix = BfMatrix::get_rotate_mat3(
			glm::vec3(
				0.0,		// YAW
				angle_y,	// PITCH
				0.0f		// ROLL
			)
		);

		for (size_t i = 0; i < this->vertices.size(); i++) {
			o.vertices[i] -= to_start;
			o.vertices[i] = BfMatrix::multiply(rotate_matrix, o.vertices[i]).get_vec3();
		}
		
		switch (plane) {
			case BF_PLANE_XY: // YAW
			{
				float angle_x = glm::degrees(glm::asin(-last.y / r));

				if (axis == BF_AXIS_X) // OVER Y + Z
				{

				}
				else if (axis == BF_AXIS_Y) // OVER X + Z
				{

				}
				else { // 
					// ERROR
				}
				break;
			}
			case BF_PLANE_XZ: // PITCH
			{
				float angle_y = glm::degrees(glm::asin(-last.y / r));

				if (axis == BF_AXIS_X) { // Z + Y

				}
				else if (axis == BF_AXIS_Z) { // X + Y

				}
				else {
					// ERROR
				}
				break;
			}
			case BF_PLANE_YZ: // ROLL
			{
				float angle_z = glm::degrees(glm::asin(-last.y / r));
				
				if (axis == BF_AXIS_Y) { // Z + X

				}
				else if (axis == BF_AXIS_Z) { // Y + X

				}
				else {
					// ERROR
				}
				break;
			}
		}

	
		return o;
	}

};


class BfLine {

};



/*
 * (CLASSIC_CALCULATION)
 * Returns value of factor defined by n
 *
 * Needed: n - factor argument.
 *
 * Calculations were premade for best perfomance.
 * 
 * Max n = 10.
 */ 
inline int32_t bfGetFactorial(int32_t n) {

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

inline uint32_t bfGetBinomialCoefficient(uint32_t n, uint32_t k) {
	return bfGetFactorial(n) / bfGetFactorial(k) / bfGetFactorial(n - k);
}




#endif