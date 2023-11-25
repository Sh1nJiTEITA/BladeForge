#ifndef BF_CURVES_HPP
#define BF_CURVES_HPP

#include "bfVariative.hpp"
#include "bfVertex2.hpp"

#include <vector>

int32_t bfGetFactorial(int32_t n);
uint32_t bfGetBinomialCoefficient(uint32_t n, uint32_t k);

class BfBezier {
public:
	uint32_t n;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> c_vertices;

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

	BfBezier(uint32_t in_n, std::vector<glm::vec3> in_vertices) : n{in_n}, vertices{in_vertices} {
		if (in_n+1 != static_cast<uint32_t>(in_vertices.size())) {
			throw std::runtime_error("Input bezier data is incorrect: vec.size() != in_n");
		}
	}


	glm::vec3 get_single_vertex(float t) {
		glm::vec3 _v{ 0.0f,0.0f,0.0f };
		for (int i = 0; i <= n; i++) {
			_v += static_cast<float>(bfGetBinomialCoefficient(n, i) * std::pow(1 - t, n - i) * std::pow(t, i)) * vertices[i];
			
		}
		return _v;
	}

	void update_vertices(uint32_t v_count) {
		c_vertices.clear();
		c_vertices.resize(v_count + 1);

		for (int i = 0; i < v_count + 1; i++) {
			c_vertices[i] = get_single_vertex(1 / static_cast<float>(v_count) * static_cast<float>(i));
		}
	}

	std::vector<glm::vec3>& get_vertices() {
		return c_vertices;
	}

	std::vector<glm::vec3>& update_and_get_vertices(uint32_t v_count) {
		update_vertices(v_count);
		return get_vertices();
	}

	glm::vec3 get_tangent(float t) {
		return this->get_single_derivative_1(t);
	}

	glm::vec3 get_direction_tangent(float t) {
		glm::vec3 tangent = this->get_single_derivative_1(t);
		float d = glm::sqrt(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);

		return tangent / d;
	}

	glm::vec3 get_direction_normal(float t) {
		glm::vec3 a = this->get_direction_tangent(t);
		glm::vec3 b = glm::normalize(a + this->get_single_derivative_2_e(t));
		glm::vec3 r = glm::normalize(glm::cross(b, a));
		// TODO
		//return glm::vec3(-tangent.y, tangent.x, 0);
		return glm::normalize(glm::cross(r, a));
	}

	glm::vec3 get_single_derivative_1(float t) {
		
		int k = n - 1;
		glm::vec3 out(0.0f);
		for (int i = 0; i <= k; i++) {
			out += (float)(bfGetBinomialCoefficient(k, i) * std::pow(1 - t, k - i) * std::pow(t, i) * n) * (vertices[i + 1] - vertices[i]);
		}
		return out;
	}

	glm::vec3 get_single_derivative_1_e(float t) {
		glm::vec3 out(0.0f);
		for (int i = 0; i <= n; i++) {
			out += (float)((i - n*t)/(t*(1-t))*bfGetBinomialCoefficient(n, i) * std::pow(1 - t, n - i) * std::pow(t, i)) * vertices[i];
		}
		return out;
	}

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

	BfBezier get_derivative_n(uint32_t __n) {
		/*if ((this->n - __n) <= 1) 
			throw std::runtime_error("Bezier derivative is underfines -> n - __n <= n");*/
		
		BfBezier loc{*this};
		for (int i = 0; i < __n; i++) {
			loc = loc.get_derivative();
		}
		return loc;
	}
};


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