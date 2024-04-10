#ifndef BF_MATRIX2_H
#define BF_MATRIX2_H

#include "bfMath.hpp"

#include <initializer_list>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <bfVertex2.hpp>
#include <glm/glm.hpp>



class BfVec2;

class BfVector {
protected:
	std::vector<float> __data;
public:
	using value_type = float;
	
	BfVector() {}

	BfVector(const std::vector<float>& v) 
		: __data{v} {}

	BfVector(std::initializer_list<float> l) {
		__data.reserve(l.size());
		forit(l) {
			__data.emplace_back(it);
		}
	}
	
	BfVector(size_t n, float v) {
		__data.resize(n, v);
	}

#ifdef BF_VERTEX2_H
	BfVector(const BfVertex3& v) : BfVector{ v.pos } {}
	BfVector(const std::vector<BfVertex3>& v) {
		__data.reserve(v.capacity());
		forit(v) {
			fori(0, 3) {
				__data.emplace_back(it.pos[i]);
			}
		}
	}
#endif


#ifdef GLM_SETUP_INCLUDED
	template<size_t n>
	BfVector(glm::vec<n, float, glm::highp> v) {
		__data.reserve(n);
		fori(0, n) {
			__data.emplace_back(v[i]);
		}
	}

	template<size_t n>
	BfVector operator+(glm::vec<n, float, glm::highp> v) {
		CHECK_DIMENSION_NUM(this->size(), n);
		BfVector _v{ *this };
		fori(0, n) {
			_v[i] += v[i];
		}
		return _v;
	}

	template<size_t n>
	BfVector operator-(glm::vec<n, float, glm::highp> v) {
		return this->operator+(-v);
	}

	template<size_t n>
	BfVector operator*(glm::vec<n, float, glm::highp> v) {
		CHECK_DIMENSION_NUM(this->size(), n);
		BfVector _v{ *this };
		fori(0, n) {
			_v[i] *= v[i];
		}
		return _v;
	}

	template<size_t n>
	BfVector operator/(glm::vec<n, float, glm::highp> v) {
		CHECK_DIMENSION_NUM(this->size(), n);
		BfVector _v{ *this };
		fori(0, n) {
			_v[i] /= v[i];
		}
		return _v;
	}

	template<size_t n>
	BfVector operator==(glm::vec<n, float, glm::highp> v) const {
		CHECK_DIMENSION_NUM(this->size(), n);
		bool is = true;
		fori(0, n) {
			if ((*this)[i] == 0.0f || v[i] == 0.0f)
				is = is && CHECK_FLOAT_EQUALITY_TO_NULL((*this)[i], v[i]);
			else
				is = is && CHECK_FLOAT_EQUALITY((*this)[i], v[i]);
			if (!is) return false;
		}
		return true;
	}
#endif

	void fill_v(size_t n, float v) {
		CHECK_CAPACITY_NUM_LESS(n, __data.capacity());
		fori(0, n) {
			(*this).emplace_back(v);
		}
	}
	void fill_v(std::initializer_list<float> l) {
		CLEAR_VEC;
		CHECK_CAPACITY_NUM_LESS(l.size(), __data.capacity());
		forit(l) {
			(*this).emplace_back(it);
		}
	}

	void push_back(const float& v) {
		__data.push_back(v);
	}

	size_t size() const noexcept {
		return __data.size();
	}

	size_t capacity() const noexcept {
		return __data.capacity();
	}

	void reserve(size_t n) {
		__data.reserve(n);
	}

	void emplace_back(float v) {
		__data.emplace_back(v);
	}

	BfVector get_erased_to(size_t n) const {
		CHECK_CAPACITY_NUM_LESS_EQ(this->size(), n);
		BfVector _v{};
		_v.reserve(n);
		fori(0, n) {
			_v.emplace_back((*this)[i]);
		}
		return _v;
	}


	std::vector<float>::iterator begin() { return __data.begin(); }
	std::vector<float>::iterator end() { return __data.end(); }
	std::vector<float>::const_iterator begin() const { return __data.begin(); }
	std::vector<float>::const_iterator end() const { return __data.end(); }

	std::vector<float>::reverse_iterator rbegin() { return __data.rbegin(); }
	std::vector<float>::reverse_iterator rend() { return __data.rend(); }
	std::vector<float>::const_reverse_iterator rbegin() const { return __data.rbegin(); }
	std::vector<float>::const_reverse_iterator rend() const { return __data.rend(); }

	// OVERLOADING ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	float& operator[](size_t i) { return __data[i]; }
	const float& operator[](size_t i) const { return __data[i]; }

	BfVector operator*(const BfVector& iv) {
		CHECK_DIMENSION(*this, iv);
		BfVector _v{ *this };
		fori(0, _v.size()) { _v[i] *= iv[i]; }
		return _v;
	}
	BfVector& operator*=(const BfVector& iv) {
		CHECK_DIMENSION(*this, iv);
		fori(0, __data.size()) { __data[i] *= iv[i]; }
		return *this;
	}


	BfVector operator*(float v) noexcept {
		BfVector _v{ *this };
		forit(_v) { it *= v; }
		return _v;
	}
	BfVector& operator*=(float v) noexcept {
		forit(__data) { it *= v; }
		return *this;
	}


	BfVector operator/(const BfVector& iv) {
		CHECK_DIMENSION(*this, iv);
		BfVector _v{ *this };
		fori(0, _v.size()) { _v[i] /= iv[i]; }
		return _v;
	}
	BfVector& operator/=(const BfVector& iv) {
		CHECK_DIMENSION(*this, iv);
		fori(0, __data.size()) { __data[i] /= iv[i]; }
		return *this;
	}


	BfVector operator/(float v) {
		BfVector _v{ *this };
		forit(_v) { it /= v; }
		return _v;
	}
	BfVector& operator/=(float v) noexcept {
		forit(__data) { it /= v; }
		return *this;
	}


	BfVector operator+(const BfVector& v) noexcept {
		CHECK_DIMENSION(*this, v);
		BfVector _v{ *this };
		fori(0, _v.size()) {
			_v[i] += v[i];
		}
		return _v;
	}
	BfVector& operator+=(const BfVector& v) noexcept {
		CHECK_DIMENSION(*this, v);
		fori(0, this->size()) {
			__data[i] += v[i];
		}
		return *this;
	}


	BfVector operator+(float v) noexcept {
		BfVector _v{ *this };
		forit(_v) { it += v; }
		return _v;
	}
	BfVector& operator+=(float v) noexcept {
		forit(__data) { it += v; }
		return *this;
	}


	BfVector operator-(const BfVector& v)  noexcept {
		CHECK_DIMENSION(*this, v);
		BfVector _v{ *this };
		fori(0, _v.size()) { _v[i] -= v[i]; }
		return _v;
	}
	BfVector& operator-=(const BfVector& v) noexcept {
		CHECK_DIMENSION(*this, v);
		fori(0, this->size()) { __data[i] -= v[i]; }
		return *this;
	}


	BfVector operator-(float v) noexcept {
		BfVector _v{ *this };
		forit(_v) {
			it -= v;
		}
		return _v;
	}
	BfVector& operator-=(float v) noexcept {
		forit(__data) {
			it -= v;
		}
		return *this;
	}


	bool operator==(const BfVector& v) const {
		CHECK_DIMENSION_NUM(this->size(), v.size());
		bool is = true;
		fori(0, v.size()) {
			if ((*this)[i] == 0.0f || v[i] == 0.0f)
				is = is && CHECK_FLOAT_EQUALITY_TO_NULL((*this)[i], v[i]);
			else
				is = is && CHECK_FLOAT_EQUALITY((*this)[i], v[i]);
			if (!is) return false;
		}
		return true;
	}


	friend std::ostream& operator<<(std::ostream& os, const BfVector& v) {
		os << "BfVector(";
		fori(0, v.size()) {
			if (i == v.size() - 1)
				os << v[i] << ")";
			else
				os << v[i] << ", ";
		}
		return os;
	}

	
};

class BfVec2 : public BfVector {
public:
	float& x;
	float& y;

	BfVec2(float x, float y)
		: BfVector{ x,y }
		, x{ __data[0] }
		, y{ __data[1] }
	{}

	BfVec2(const BfVector& v) 
		: BfVector{ v.get_erased_to(2) }
		, x{ __data[0] }
		, y{ __data[1] }
	{}
#ifdef GLM_SETUP_INCLUDED
	template<size_t n>
	BfVec2(glm::vec<n, float, glm::highp> v) 
		: BfVector{glm::vec2(v)}
		, x{ __data[0] }
		, y{ __data[1] }
	{
		
	}

#endif

#ifdef BF_VERTEX2_H
	BfVec2(const BfVertex3& vert)
		: BfVector{ vert.pos }
		, x{ __data[0] }
		, y{ __data[1] }
	{}
#endif


};



enum BfEnMatrixFillType {
	BF_MATRIX_FILL_TYPE_WHOLE,
	BF_MATRIX_FILL_TYPE_DIAGONAL_LEFT,
	BF_MATRIX_FILL_TYPE_DIAGONAL_RIGHT,
	BF_MATRIX_FILL_TYPE_DIAGONAL = BF_MATRIX_FILL_TYPE_DIAGONAL_LEFT
};

class BfMatrix {
	std::vector<BfVector> __data;

public:
	BfMatrix(size_t n, 
			 size_t m, 
			 float v = 0.0f, 
			 BfEnMatrixFillType mode = BF_MATRIX_FILL_TYPE_WHOLE) 
	{
		__data.reserve(n);
		switch (mode) {
		case BF_MATRIX_FILL_TYPE_WHOLE:
			fori(0, n) {
				__data.emplace_back(m, v);
			}
			break;

		case BF_MATRIX_FILL_TYPE_DIAGONAL_LEFT:
			CHECK_DIMENSION_NUM(n, m);
			fori(0, n) {
				__data.push_back(BfVector());
				forj(0, m) {
					if (i == j)
						__data[i].emplace_back(v);
					else 
						__data[i].emplace_back(0.0f);
				}
			}
			break;

		case BF_MATRIX_FILL_TYPE_DIAGONAL_RIGHT:
			CHECK_DIMENSION_NUM(n, m);
			fori(0, n) {
				__data.push_back(BfVector());
				forj(0, m) {
					if (i + j == m  - 1)
						__data[i].emplace_back(v);
					else
						__data[i].emplace_back(0.0f);
				}
			}
			break;

		}
		
	}
	BfMatrix(size_t n, size_t m, const std::vector<float>& v) {
		__data.reserve(n);
		fori(0, n) {
			__data[i].reserve(m);
			forj(0, m) {
				__data[i].emplace_back(v[j + i * n]);
			}
		}
	}
	BfMatrix(std::initializer_list<BfVector> v) {
		__data.reserve(v.size());
		forit(v) {
			__data.emplace_back(it);
		}
	}

	BfMatrix add_col_to_end(const BfVector& v) const {
		BfMatrix _m{};

		_m.reserve(this->rsize());
		fori(0, this->rsize()) {
			_m.push_back(BfVector());
			_m.rbegin()->reserve(this->csize() + 1);
			forj(0, this->csize() + 1) {
				if (j == this->csize()) {
					_m.rbegin()->emplace_back(v[i]);
				}
				else {
					_m.rbegin()->emplace_back((*this)[i][j]);
				}
			}
		}
		return _m;
	}

	BfMatrix get_triangular_form() const {
		BfMatrix _m{ *this };
		float sign = 1.0f;
		fork(0, this->rsize()) {
			if (_m[k][k] == 0.0f) {
				size_t swap_index = -1;
				fori(k + 1, this->rsize()) {
					if (_m[i][i] != 0.0f) {
						swap_index = i;
						break;
					}
				}
				if (swap_index == -1) abort();
				std::swap(_m[k], _m[swap_index]);
				sign *= -1;
			}
			BfVector wrow{ _m[k] };
			fori(k + 1, this->rsize()) {
				BfVector mrow{ _m[i] };
				forj(0, this->csize()) {
					mrow[j] -= wrow[j] * _m[i][k] / _m[k][k];
				}
				std::swap(mrow, _m[i]);
			}
		}
		return _m * sign;
	}

	float det_gauss() const {
		CHECK_DIMENSION_NUM(this->csize(), this->rsize());
		BfMatrix triangular_form{ this->get_triangular_form() };
		float det = 1;
		fori(0, this->csize()) {
			det *= triangular_form[i][i];
		}
		return det;
	}

	float det() const {
		CHECK_DIMENSION_NUM(this->csize(), this->csize());

		float D = 0.0f;
		if (this->csize() == 1)
			return (*this)[0][0];

		int sign = 1; 
		fori(0, this->csize()) {
			BfMatrix cof = this->get_minor(0, i);
			D += sign * (*this)[0][i] * cof.det();
			sign = -sign;
		}
		return D;
	}

	BfMatrix get_minor(size_t _i, size_t _j) const {
		CHECK_DIMENSION_NUM(this->rsize(), this->csize());
		
		BfMatrix _m{};
		size_t minor_size = this->rsize() - 1;
		_m.reserve(minor_size);
		
		fori(0, this->rsize()) {
			if (i == _i) { continue; }
			
			_m.push_back(BfVector());
			_m.rbegin()->reserve(minor_size);
			
			forj(0, this->csize()) {
				if (j == _j) { continue; } 
				_m.rbegin()->emplace_back((*this)[i][j]);
			}
		}
		return _m;
	}

	BfMatrix get_transposed() const {
		BfMatrix _m{};
		_m.reserve(this->csize());
		fori(0, this->csize()) {
			_m.push_back(BfVector());
			_m.rbegin()->reserve(this->csize());
			forj(0, this->rsize()) {
				_m.rbegin()->emplace_back((*this)[j][i]);
			}
		}
		return _m;
	}


	float get_single_cofactor(size_t _i, size_t _j) const {
		CHECK_DIMENSION_NUM(this->rsize(), this->csize());

		int sign = ((_i + _j) % 2 == 0) ? 1 : -1;
		
		BfMatrix minor = this->get_minor(_i, _j);
		return sign * minor.det();
	}

	BfMatrix get_cofactor() const {
		CHECK_DIMENSION_NUM(this->csize(), this->rsize());
		
		BfMatrix _m{};
		_m.reserve(this->rsize());

		fori(0, this->rsize()) {
			_m.push_back(BfVector());
			_m.rbegin()->reserve(this->csize());
			forj(0, this->csize()) {
				_m.rbegin()->emplace_back(this->get_single_cofactor(i, j));
			}
		}
		return _m;
	}

	BfMatrix get_inversed() const {
		CHECK_DIMENSION_NUM(this->csize(), this->rsize());

		float det = this->det();
		if (det == 0)  throw std::runtime_error("Matrix is singular");
		
		BfMatrix adj = this->get_adjugated();
		BfMatrix _m{};
		_m.reserve(this->rsize());

		fori(0, this->rsize()) {
			_m.push_back(BfVector());
			_m.rbegin()->reserve(this->csize());
			forj(0, this->csize()) {
				_m.rbegin()->emplace_back(adj[i][j] / det);
			}
		}
		return _m;
	}

	BfMatrix& inverse() {
		CHECK_DIMENSION_NUM(this->csize(), this->rsize());

		float det = this->det();
		if (det == 0)  throw std::runtime_error("Matrix is singular");

		BfMatrix adj = this->get_adjugated();
	
		fori(0, this->rsize()) {
			forj(0, this->csize()) {
				(*this)[i][j] = adj[i][j] / det;
			}
		}
		return *this;
	}

	BfMatrix get_adjugated() const {
		return this->get_cofactor().get_transposed();
	}

	BfVector to_bfvec() const {
		size_t vec_size = this->rsize() * this->csize();
		BfVector _v{};
		_v.reserve(vec_size);
		
		fori(0, this->rsize()) {
			forj(0, this->csize()) {
				_v.emplace_back((*this)[i][j]);
			}
		}
		return _v;
	}

	// row size
	size_t rsize() const noexcept { return __data.size(); }
	// col size
	size_t csize() const noexcept { return __data.begin()->size(); }
	void reserve(size_t n) { __data.reserve(n); }
	void push_back(const BfVector& v) { __data.push_back(v); }

	std::vector<BfVector>::iterator begin() { return __data.begin(); }
	std::vector<BfVector>::iterator end() { return __data.end(); }
	std::vector<BfVector>::const_iterator begin() const { return __data.begin(); }
	std::vector<BfVector>::const_iterator end() const { return __data.end(); }

	std::vector<BfVector>::reverse_iterator rbegin() { return __data.rbegin(); }
	std::vector<BfVector>::reverse_iterator rend() { return __data.rend(); }
	std::vector<BfVector>::const_reverse_iterator rbegin() const { return __data.rbegin(); }
	std::vector<BfVector>::const_reverse_iterator rend() const { return __data.rend(); }

	// OVERLOADING ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	BfVector& operator[](size_t i) {
		return __data[i];
	}
	const BfVector& operator[](size_t i) const {
		return __data[i];
	}

	BfMatrix operator>>(const BfMatrix& v) {
		CHECK_DIMENSION_NUM(this->csize(), v.rsize());

		BfMatrix _m{};
		_m.reserve(this->rsize());
		fori(0, this->rsize()) {
			_m.push_back(BfVector());
			_m.rbegin()->reserve(v.csize());
			forj(0, v.csize()) {
				float c = 0.0f;
				fork(0, v.rsize()) {
					c += (*this)[i][k] * v[k][j];
				}
				_m.rbegin()->emplace_back(c);
			}
		}
		return _m;
	}


	BfMatrix operator+(const BfMatrix& v) {
		CHECK_DIMENSION_NUM(this->rsize(), v.rsize());
		CHECK_DIMENSION_NUM(this->csize(), v.csize());
		BfMatrix _v{ *this };
		fori(0, this->rsize()) { _v[i] += v[i]; }
		return _v;
	}
	BfMatrix& operator+=(const BfMatrix& v) {
		CHECK_DIMENSION_NUM(this->rsize(), v.rsize());
		CHECK_DIMENSION_NUM(this->csize(), v.csize());
		fori(0, this->rsize()) { __data[i] += v[i]; }
		return *this;
	}
	BfMatrix operator+(float v) {
		BfMatrix _v{ *this };
		fori(0, this->rsize()) { _v[i] += v; }
		return _v;
	}
	BfMatrix& operator+=(float v) {
		fori(0, this->rsize()) { __data[i] += v; }
		return *this;
	}


	BfMatrix operator-(const BfMatrix& v) {
		CHECK_DIMENSION_NUM(this->rsize(), v.rsize());
		CHECK_DIMENSION_NUM(this->csize(), v.csize());
		BfMatrix _v{ *this };
		fori(0, this->rsize()) { _v[i] -= v[i]; }
		return _v;
	}
	BfMatrix& operator-=(const BfMatrix& v) {
		CHECK_DIMENSION_NUM(this->rsize(), v.rsize());
		CHECK_DIMENSION_NUM(this->csize(), v.csize());
		fori(0, this->rsize()) { __data[i] -= v[i]; }
		return *this;
	}
	BfMatrix operator-(float v) {
		BfMatrix _v{ *this };
		fori(0, this->rsize()) { _v[i] -= v; }
		return _v;
	}
	BfMatrix& operator-=(float v) {
		fori(0, this->rsize()) { __data[i] -= v; }
		return *this;
	}


	BfMatrix operator*(const BfMatrix& v) {
		CHECK_DIMENSION_NUM(this->rsize(), v.rsize());
		CHECK_DIMENSION_NUM(this->csize(), v.csize());
		BfMatrix _v{ *this };
		fori(0, this->rsize()) { _v[i] *= v[i]; }
		return _v;
	}
	BfMatrix& operator*=(const BfMatrix& v) {
		CHECK_DIMENSION_NUM(this->rsize(), v.rsize());
		CHECK_DIMENSION_NUM(this->csize(), v.csize());
		fori(0, this->rsize()) { __data[i] *= v[i]; }
		return *this;
	}
	BfMatrix operator*(float v) {
		BfMatrix _v{ *this };
		fori(0, this->rsize()) { _v[i] *= v; }
		return _v;
	}
	BfMatrix& operator*=(float v) {
		fori(0, this->rsize()) { __data[i] *= v; }
		return *this;
	}


	BfMatrix operator/(const BfMatrix& v) {
		CHECK_DIMENSION_NUM(this->rsize(), v.rsize());
		CHECK_DIMENSION_NUM(this->csize(), v.csize());
		BfMatrix _v{ *this };
		fori(0, this->rsize()) { _v[i] /= v[i]; }
		return _v;
	}
	BfMatrix& operator/=(const BfMatrix& v) {
		CHECK_DIMENSION_NUM(this->rsize(), v.rsize());
		CHECK_DIMENSION_NUM(this->csize(), v.csize());
		fori(0, this->rsize()) { __data[i] /= v[i]; }
		return *this;
	}
	BfMatrix operator/(float v) {
		BfMatrix _v{ *this };
		fori(0, this->rsize()) { _v[i] /= v; }
		return _v;
	}
	BfMatrix& operator/=(float v) {
		fori(0, this->rsize()) { __data[i] /= v; }
		return *this;
	}


	bool operator==(const BfMatrix& v) const {
		CHECK_DIMENSION_NUM(this->rsize(), v.rsize());
		CHECK_DIMENSION_NUM(this->csize(), v.csize());
		bool is = true;
		fori(0, this->rsize()) {
			is = (*this)[i] == v[i];
		}
		return is;
	}
	friend std::ostream& operator<<(std::ostream& os, const BfMatrix& v) {
		os << "BfMatrix(";
		fori(0, v.rsize()) {
			if (i == v.rsize() - 1)
				os << v[i];
			else
				os << v[i] << ", ";
		}
		return os << ")";
	}
};

inline BfVector solve_linear_gauss(const BfMatrix& _A, const BfVector& B) {
	CHECK_DIMENSION_NUM(_A.rsize(), B.size());
	BfMatrix A = _A.add_col_to_end(B);
	A = A.get_triangular_form();
	std::cout << A << "\n";
	
	fori(0, A.rsize()) {
		float d = A[i][i];
		forj(0, A.csize()) {
			A[i][j] /= d;
		}
	}
	std::cout << A << "\n";


	BfVector _v(A.rsize(),0.0f);


	for (int i = A.rsize() - 1; i >= 0; i--) {
		_v[i] = B[i];
		for (int j = i + 1; j < A.rsize(); j++) {
			_v[i] -= A[i][j] * _v[j];
		}
		_v[i] /= A[i][i];
	}
	return _v;
}

inline BfVector solve_linear_mtx(const BfMatrix& A, const BfMatrix& B) {
	return (A.get_inversed() >> B).to_bfvec();
}


class BfLineProp {
public:
	float __k;
	float __b;

	BfLineProp(const BfVec2& P1, const BfVec2& P2)
		: __k{ (P2.y - P1.y) / (P2.x - P1.x) }
		, __b{ P1.y - __k * P1.x }
	{}

	BfLineProp(float k, float b) 
		: __k{k}
		, __b{b}
	{}

	float get_k() const noexcept { return __k; }
	float get_b() const noexcept { return __b; }
	float get_y(float x) const noexcept { return __k * x + __b; }
	float get_x(float y) const noexcept { return (y - __b) / __k; }

	BfLineProp get_perpendicular(const BfVec2& P) {
		float k = -1 / __k;
		float b = P.y - k * P.x;
		return BfLineProp(k, b);
	}
};

class BfCircleProp {
	float __radious;
	
};



#endif // !BF_MATRIX2_H


