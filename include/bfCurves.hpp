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
		if (in_n != static_cast<uint32_t>(in_vertices.size())) {
			throw std::runtime_error("Input bezier data is incorrect: vec.size() != in_n");
		}
		
		vertices.resize(in_n);
		
		for (int i = 0; i < in_n; i++) {
			vertices[i] = in_vertices[i].pos;
		}
	}

	BfBezier(uint32_t in_n, std::vector<glm::vec3> in_vertices) : n{in_n}, vertices{in_vertices} {
		if (in_n != static_cast<uint32_t>(in_vertices.size())) {
			throw std::runtime_error("Input bezier data is incorrect: vec.size() != in_n");
		}
	}


	glm::vec3 get_single_point(float t) {
		glm::vec3 _v{ 0.0f,0.0f,0.0f };
		for (int i = 1; i < n+1; i++) {
			_v += static_cast<float>(bfGetBinomialCoefficient(n, i) * std::pow(1 - t, n - i) * std::pow(t, i)) * vertices[i-1];
			
		}
		return _v;
	}

	void calculate_c_vertices(uint32_t v_count) {
		c_vertices.clear();
		c_vertices.resize(v_count + 1);

		for (int i = 0; i < v_count + 1; i++) {
			c_vertices[i] = get_single_point(1 / static_cast<float>(v_count) * static_cast<float>(i));
		}
	}

	std::vector<glm::vec3>& get_r_c_vertices() {
		return c_vertices;
	}

	std::vector<glm::vec3>& get_cal_r_c_vertices(uint32_t v_count) {
		calculate_c_vertices(v_count);
		return get_r_c_vertices();
	}

	BfBezier get_derivative() {
		if (n == 1) throw std::runtime_error("Bezier derivative is undefined -> n will be < 1");
		return BfBezier(this->n - 1, std::vector<glm::vec3>(this->vertices.begin(), this->vertices.end() - 1));
	}

	BfBezier get_derivative_n(uint32_t __n) {
		if ((this->n - __n) <= 1) throw std::runtime_error("Bezier derivative is underfines -> n - __n <= n");
		
		BfBezier loc{*this};
		for (int i = 0; i < __n; i++) {
			loc = loc.get_derivative();
		}
		return loc;
	}
};


int32_t bfGetFactorial(int32_t n) {

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

uint32_t bfGetBinomialCoefficient(uint32_t n, uint32_t k) {
	return bfGetFactorial(n) / bfGetFactorial(k) / bfGetFactorial(n - k);
}




#endif