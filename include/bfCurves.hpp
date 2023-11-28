#ifndef BF_CURVES_HPP
#define BF_CURVES_HPP

#include "bfVariative.hpp"
#include "bfVertex2.hpp"

#include <vector>
#include <algorithm>

int32_t bfGetFactorial(int32_t n);
uint32_t bfGetBinomialCoefficient(uint32_t n, uint32_t k);

class BfBezier {
public:
	uint32_t n;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> c_vertices;


	/*
	* Default initialization:
	* 
	* Needed: nothing.
	* 
	* Initializes inner input vertices and output vertices arrays with length 0.
	* 
	*/
	BfBezier() : n{ 0 }, vertices{}, c_vertices{} {}

	BfBezier(uint32_t in_n, std::vector<bfVertex> in_vertices) : n{ in_n } {
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

		for (int i = 0; i < v_count + 1; i++) {
			c_vertices[i] = get_single_vertex(1 / static_cast<float>(v_count) * static_cast<float>(i));
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
	std::vector<glm::vec3>& get_vertices() {
		return c_vertices;
	}

	/*
	* Calculates and outputs output arrays of vertices.
	* 
	* Needed: v_count - number of calculating vertices.
	* 
	*/
	std::vector<glm::vec3>& update_and_get_vertices(uint32_t v_count) {
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

	std::vector<glm::vec3> get_extremites_t() {

		if (this->n == 1) {
			/*std::vector<float> out(1);

			glm::vec3 m = *std::max_element(this->vertices.begin(), this->vertices.end());

			if (m == this->vertices[0]) {
				out[0] = 0.0f;
			}
			else {
				out[0] = 1.0f;
			}
			return out;*/
		}

		if (this->n == 2) {
			std::vector<glm::vec3> out(1);
			
			BfBezier dBezier = this->get_derivative();
			glm::vec3 a = dBezier.vertices[0];
			glm::vec3 b = dBezier.vertices[1];

			glm::vec3 t = -a / (b - a);
			
			for (int i = 0; i < 3; i++) {

			}
			out[0] = t;

			return out;
		}

		if (this->n == 3) {
			std::vector<glm::vec3> out(2);

			BfBezier dBezier = this->get_derivative();

			glm::vec3 a = dBezier.vertices[0] - 2.0f * dBezier.vertices[1] + dBezier.vertices[2];
			glm::vec3 b = 2.0f * (dBezier.vertices[1] - dBezier.vertices[0]);
			glm::vec3 c = dBezier.vertices[0];

			glm::vec3 t_p = (-b + glm::sqrt(b * b - 4.0f * a * c)) / (2.0f * a);
			glm::vec3 t_m = (-b - glm::sqrt(b * b - 4.0f * a * c)) / (2.0f * a);;
			
			out[0] = t_p;
			out[1] = t_m;

			return out;
		}

		if (this->n > 3) {
			BfBezier dBezier = this->get_derivative();

			std::vector<glm::vec3> out;
			std::vector<float> out2;

			float eps = 3.0f;
			 
			float tx = 0;
			float _x;
			float x_;

			float lam = -0.05f;

			float out_t;

			while (true) {
				_x = this->get_single_vertex(tx).x;
				tx = tx - lam * dBezier.get_single_vertex(tx).x;
				x_ = this->get_single_vertex(tx).x;
				
				if (std::abs(x_ - _x) < eps) {
					out_t = tx;
					break;
				}
			}
			



			/*-----------------------------------------------------------------------------*/
			/*
			float x;
			float y;
			float z;

			float dx;
			float dy;
			float dz;

			bool is_dx = false;
			bool is_dy = false;
			bool is_dz = false;

			glm::vec3 v;
			glm::vec3 dv;

			glm::vec3 t = glm::vec3(0.5f);
			while ((!is_dx) || (!is_dy) || (!is_dz)) {
				
				v.x = this->get_single_vertex(t.x).x;
				v.y = this->get_single_vertex(t.y).y;
				v.z = this->get_single_vertex(t.z).z;
				
				dv.x = dBezier.get_single_vertex(t.x).x;
				dv.y = dBezier.get_single_vertex(t.y).y;
				dv.z = dBezier.get_single_vertex(t.z).z;

				x = v.x;
				y = v.y;
				z = v.z;

				dx = dv.x;
				dy = dv.y;
				dz = dv.z;

				if (glm::isnan(dx)) {
					is_dx = true;
				}
				if (glm::isnan(dy)) {
					is_dy = true;
				}
				if (glm::isnan(dz)) {
					is_dz = true;
				}

				if (!is_dx) {
					if ((dx < 3) && (dx > -3)) {

						out2.push_back(t.x);
						is_dx = true;
					}
				}
				
				if (!is_dy) {
					if ((dy < 3) && (dy > -3)) {

						out2.push_back(t.y);
						is_dy = true;
					}
				}

				if (!is_dz) {
					if ((dx < 3) && (dx > -3)) {

						out2.push_back(t.z);
						is_dz = true;
					}
				}

				

				t = t - v / dv;
			}*/
			/*-----------------------------------------------------------------------------*/
			//const size_t count = 500;
			//is_x && is_y && is_z
			/*for (int i = 0; i < count + 1; i++) {
				
				float t = 1 / (float)count * (float)i;

				glm::vec3 v = dBezier.get_single_vertex(t);

				if ((v.x < 0.5f) && (v.x > -0.5f)) {
				
					out.push_back(glm::vec3(t, 0.0f, 0.0f));
					
				}
				if ((v.y < 0.5f) && (v.y > -0.5f)) {
					
					out.push_back(glm::vec3(0.0f, t, 0.0f));
					
				}*/
				/*if ((v.z < 20.0f) && (v.z > -20.0f)) {
					for (float t0 = t; t0 < t0 + 5; t0 += 0.05) {
						out.push_back(glm::vec3(0.0f, 0.0f, t));
					}
				}*/
			return std::vector<glm::vec3>();
			
			//return out;
		}

		return std::vector<glm::vec3>();

	}

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