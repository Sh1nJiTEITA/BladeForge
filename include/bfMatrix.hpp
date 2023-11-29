#ifndef BF_MATRIX_HPP
#define BF_MATRIX_HPP

#include <vector>
#include <stdexcept>
#include <string>

#ifdef GLM_SETUP_INCLUDED

#include <glm/glm.hpp>

#endif


#define fori(beg, end) for (int i = beg; i < end; i++)
#define forj(beg, end) for (int j = beg; j < end; j++)

class BfMatrix {
	
	const size_t __n;
	const size_t __m;

	std::vector<std::vector<float>> data;
public:
	BfMatrix(size_t n, size_t m) :
		__n{n},
		__m{m},
		data{n, std::vector<float>(m, 0.0f)}
	{}

	BfMatrix(size_t n, size_t m, const std::vector<float>& in_data) :
		__n{n},
		__m{m},
		data{ n, std::vector<float>(m, 0.0f) }
	{
		if (in_data.size() != __n * __m) {
			throw std::runtime_error("size of input data is not n * m");
		}
		
		for (size_t i = 0; i < n ; i++) {
			std::copy(in_data.begin() + i * m, in_data.begin() + (i + 1) * m, data[i].begin() );
		}
	}
	
	BfMatrix(size_t n, size_t m, const std::vector<std::vector<float>>& in_data) :
		__n{ n },
		__m{ m },
		data{ n, std::vector<float>(m, 0.0f) }
	{
		for (size_t i = 0; i < in_data.size(); i++) {
			if (in_data[i].size() != __m) {
				throw std::runtime_error("row " + std::to_string(i) + " has not m-values");
			}
		}
		if (in_data[0].size() * in_data.size() != __n * __m) {
			throw std::runtime_error("size of input data is not n * m");
		}

		for (size_t i = 0; i < n; i++) {
			std::copy(in_data[i].begin(), in_data[i].end(), data[i].begin());
		}
	}

	BfMatrix transpose() {
		BfMatrix o(__m, __n);

		for (size_t i = 0; i < __n; i++) {
			for (size_t j = 0; j < __m; j++) {
				o.data[j][i] = this->data[i][j];
			}
		}
		return o;
	}

	static inline BfMatrix multiply(const BfMatrix& m1, const BfMatrix& m2) {
		if (m1.__m != m2.__n) {
			throw std::runtime_error("m of m1 != n of m2");
		}

		BfMatrix o(m1.__m, m2.__m);

		for (int i = 0; i < m1.__m; i++) {
			for (int j = 0; j < m2.__m; j++) {
				for (int k = 0; k < m2.__n; k++) {
					o.data[i][j] += m1.data[i][k] * m2.data[k][j];
				}
			}
		}
		return o;
	}
#ifdef GLM_SETUP_INCLUDED
	static inline BfMatrix multiply(const BfMatrix& m1, const glm::vec3 vec3) {
		if (m1.__m != 3) {
			throw std::runtime_error("m of m1 != n of m2");
		}
		
		BfMatrix o(m1.__m, 3);

		for (int i = 0; i < m1.__m; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					o.data[i][j] += m1.data[i][k] * vec3[k];
				}
			}
		}
		return o;
	}
#endif
#ifdef GLM_SETUP_INCLUDED
	static inline BfMatrix multiply(const BfMatrix& m1, const glm::vec4 vec4) {
		if (m1.__m != 4) {
			throw std::runtime_error("m of m1 != n of m2");
		}

		BfMatrix o(m1.__m, 4);

		for (int i = 0; i < m1.__m; i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					o.data[i][j] += m1.data[i][k] * vec4[k];
				}
			}
		}
		return o;
	}
#endif
	static inline float det(BfMatrix& m0) {
		if (m0.__m != m0.__n) {
			throw std::runtime_error("det could not be calculated: n != m");
		}

		int n = m0.__n;

		int index;
		int det = 1;
		int num1;
		int num2;
		int total = 1;

		std::vector<int> temp(n + 1);

		for (int i = 0; i < n; i++)
		{
			index = i;


			while (index < n && m0.data[index][i] == 0)
			{
				index++;
			}
			if (index == n) 
			{

				continue;
			}
			if (index != i)
			{

				for (int j = 0; j < n; j++)
				{
					std::swap(m0.data[index][j], m0.data[i][j]);
				}

				det = det * pow(-1, index - i);
			}


			for (int j = 0; j < n; j++)
			{
				temp[j] = m0.data[i][j];
			}

			for (int j = i + 1; j < n; j++)
			{
				num1 = temp[i];
				num2 = m0.data[j][i];
				for (int k = 0; k < n; k++)
				{
					m0.data[j][k]
						= (num1 * m0.data[j][k]) - (num2 * temp[k]);
				}
				total = total * num1; // Det(kA)=kDet(A);
			}
		}

		for (int i = 0; i < n; i++)
		{
			det = det * m0.data[i][i];
		}

		if (total == 0) return 0;
		else  return (det / total); // Det(kA)/k=Det(A);
	}

	static inline BfMatrix cofactor(BfMatrix& m0, int p, int q, int n) {
		if (m0.__m != m0.__n) {
			throw std::runtime_error("cofactor could not be calculated: n != m");
		}
		
		int i = 0, j = 0;

		BfMatrix o(m0.__n, m0.__n);

		// Looping for each element of the matrix
		for (int row = 0; row < n; row++) {
			for (int col = 0; col < n; col++) {
				if (row != p && col != q) {
					o.data[i][j++] = m0.data [row] [col] ;
					if (j == n - 1) {
						j = 0;
						i++;
					}
				}
			}
		}
		return o;
	}

	static inline BfMatrix adjoint(BfMatrix& m0) {
		if (m0.__m != m0.__n) {
			throw std::runtime_error("adj could not be calculated: n != m");
		}

		BfMatrix adj(m0.__n, m0.__n);

		if (m0.__n == 1) {
			adj.data[0][0] = 1;
			return adj;
		}

		int sign = 1;

		for (int i = 0; i < m0.__n; i++) {
			for (int j = 0; j < m0.__n; j++) {

				BfMatrix temp = BfMatrix::cofactor(m0, i, j, m0.__n);

				sign = ((i + j) % 2 == 0) ? 1 : -1;

				adj.data[j][i] = (sign) * (BfMatrix::det2(temp, m0.__n-1));
			}
		}
		return adj;
	}

	static inline float det2(BfMatrix& m0, int n) {
		if (m0.__m != m0.__n) {
			throw std::runtime_error("adj could not be calculated: n != m");
		}
		
		int D = 0; // Initialize result

		//  Base case : if matrix contains single element
		if (n == 1)
			return m0.data[0][0];

		BfMatrix temp(n, n); // To store cofactors

		int sign = 1; // To store sign multiplier

		// Iterate for each element of first row
		for (int f = 0; f < n; f++) {
			// Getting Cofactor of A[0][f]
			BfMatrix cof = BfMatrix::cofactor(m0, 0, f, n);
			D += sign * m0.data[0][f] * det2(cof, n-1);

			// terms are to be added with alternate sign
			sign = -sign;
		}

		return D;

	}

	static inline BfMatrix inverse(BfMatrix& m0) {
		float det = BfMatrix::det2(m0, m0.__n);
		if (det == 0) {
			throw std::runtime_error("matrix is singular");
		}


		BfMatrix adj = BfMatrix::adjoint(m0);
		BfMatrix o(m0.__n, m0.__n);

		for (int i = 0; i < m0.__n; i++)
			for (int j = 0; j < m0.__n; j++)
				o.data[i][j] = adj.data[i][j] / float(det);

		return o;
	}

#ifdef GLM_SETUP_INCLUDED
	static inline BfMatrix get_rotate_mat3(glm::vec3 angles) {
		BfMatrix o(3, 3);

		float a = glm::radians(angles.x); // z
		float b = glm::radians(angles.y); // y
		float g = glm::radians(angles.z); // x

		o[0][0] = cos(a) * cos(b);
		o[0][1] = cos(a) * sin(b) * sin(g) - sin(a) * cos(g);
		o[0][2] = cos(a) * sin(b) * cos(g) - sin(a) * sin(g);

		o[1][0] = sin(a) * cos(b);
		o[1][1] = sin(a) * sin(b) * sin(g) + cos(a) * cos(g);
		o[1][2] = sin(a) * sin(b) * cos(g) - cos(a) * sin(g);

		o[2][0] = -sin(b);
		o[2][1] = cos(b) * sin(g);
		o[2][2] = cos(b) * cos(g);

		return o;
	}
#endif
#ifdef GLM_SETUP_INCLUDED
	glm::vec3 get_vec3() {
		if (this->__n != 3) {
			std::runtime_error("can't get glm::vec3, n != 3");
		}

		glm::vec3 o;
		o.x = (*this)[0][0];
		o.y = (*this)[1][0];
		o.z = (*this)[2][0];
		return o;
	}
#endif

	std::vector<float>& operator[](int index)
	{
		if (index >= this->__n) {
			throw std::runtime_error("index of [] in BfMatrix > n of matrix");
		}
		return this->data[index];
	}
};



#endif