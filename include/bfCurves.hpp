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
		c_vertices.resize(v_count);

		float t;
		for (int i = 0; i < v_count; i++) {
			
			t = static_cast<float>(i) / static_cast<float>(v_count-1);
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

	std::vector<glm::vec3> update_and_copy_vec3_vertices(uint32_t v_count) {
		update_vertices(v_count);

		std::vector<glm::vec3> o(v_count + 1);
		for (int i = 0; i < this->c_vertices.size(); i++) {
			o[i] = this->c_vertices[i].pos;
		}
		return o;
	}
	
	std::vector<glm::vec3> copy_vec3_vertices(uint32_t v_count) {
		std::vector<glm::vec3> o(v_count);
		float t;
		for (int i = 0; i < v_count; i++) {

			t = static_cast<float>(i) / static_cast<float>(v_count - 1);
			o[i] = get_single_vertex(t);
		}
		return o;
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


		for (size_t i = 0; i < this->vertices.size(); i++) {
			o.vertices[i] -= to_start;
			//o.vertices[i] = BfMatrix::multiply(rotate_matrix, o.vertices[i]).get_vec3();
		}
		
		glm::vec3 last_m;

		switch (axis) {
			case BF_AXIS_X:
			{
				last_m = glm::normalize((*(o.vertices.end() - 1)));
				BfQuaternion qx = BfQuaternion::get_rotation_quaternion(
					glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)), 
					-asinf(last_m.y / sqrtf(last_m.y * last_m.y + last_m.z * last_m.z))
				);
				BfQuaternion totalx = qx * BfQuaternion{ 1,0,0,0 };
				BfMatrix mx = totalx.get_rotation_matrix4();
				
				last_m = BfMatrix::multiply(mx, glm::vec4(last_m.x, last_m.y, last_m.z, 1.0f)).get_vec4();
			
				BfQuaternion qy = BfQuaternion::get_rotation_quaternion(
					glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)),
					asinf(last_m.z / sqrtf(last_m.x * last_m.x + last_m.z * last_m.z))
				);
				BfQuaternion totaly = qy * BfQuaternion{ 1,0,0,0 };
				BfMatrix my = totaly.get_rotation_matrix4();
			
				for (size_t i = 0; i < this->vertices.size(); i++) {
					glm::vec4 l = { o.vertices[i].x, o.vertices[i].y, o.vertices[i].z, 1.0f };
					o.vertices[i] = (my * mx * l).get_vec4();
				}

				if (plane == BF_PLANE_XY) {

				}
				else if (plane == BF_PLANE_XZ) {

				}
				else {
					throw std::runtime_error("Input BF_AXIS_X can't be processed with BF_PLANE_YZ");
				}


				break;
			}
			case BF_AXIS_Y:
			{
				last_m = glm::normalize((*(o.vertices.end() - 1)));
				BfQuaternion qy = BfQuaternion::get_rotation_quaternion(
					glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)),
					-asinf(last_m.x / sqrtf(last_m.x * last_m.x + last_m.z * last_m.z))
				);
				BfQuaternion totalx = qy * BfQuaternion{ 1,0,0,0 };
				BfMatrix mx = totalx.get_rotation_matrix4();

				last_m = BfMatrix::multiply(mx, glm::vec4(last_m.x, last_m.y, last_m.z, 1.0f)).get_vec4();

				BfQuaternion qx = BfQuaternion::get_rotation_quaternion(
					glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)),
					asinf(last_m.z / sqrtf(last_m.y * last_m.y + last_m.z * last_m.z))
				);
				BfQuaternion totaly = qx * BfQuaternion{ 1,0,0,0 };
				BfMatrix my = totaly.get_rotation_matrix4();


				for (size_t i = 0; i < this->vertices.size(); i++) {
					glm::vec4 l = { o.vertices[i].x, o.vertices[i].y, o.vertices[i].z, 1.0f };
					o.vertices[i] = (my * mx * l).get_vec4();
				}
				break;
			}
			case BF_AXIS_Z:
			{
				last_m = glm::normalize((*(o.vertices.end() - 1)));
				BfQuaternion qy = BfQuaternion::get_rotation_quaternion(
					glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)),
					asinf(last_m.x / sqrtf(last_m.x * last_m.x + last_m.y * last_m.y))
				);
				BfQuaternion totalx = qy * BfQuaternion{ 1,0,0,0 };
				BfMatrix mx = totalx.get_rotation_matrix4();

				last_m = BfMatrix::multiply(mx, glm::vec4(last_m.x, last_m.y, last_m.z, 1.0f)).get_vec4();

				BfQuaternion qx = BfQuaternion::get_rotation_quaternion(
					glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)),
					-asinf(last_m.y / sqrtf(last_m.y * last_m.y + last_m.z * last_m.z))
				);
				BfQuaternion totaly = qx * BfQuaternion{ 1,0,0,0 };
				BfMatrix my = totaly.get_rotation_matrix4();


				for (size_t i = 0; i < this->vertices.size(); i++) {
					glm::vec4 l = { o.vertices[i].x, o.vertices[i].y, o.vertices[i].z, 1.0f };
					o.vertices[i] = (my * mx * l).get_vec4();
				}
				break;
			}
		}
			

	
		return o;
	}

	/*
	* Returns average increment by axis along curve.
	* 
	* Needed: axis - one of defines: BF_AXIS_X, BF_AXIS_Y, BF_AXIS_Y
	* 
	*/
	float get_average_increament_by_axis(uint32_t axis) {
		size_t axis_index;
		if (axis == BF_AXIS_X) {
			axis_index = 0;
		}
		else if (axis == BF_AXIS_Y) {
			axis_index = 1;
		}
		else if (axis == BF_AXIS_Z) {
			axis_index = 2;
		}
		else {
			throw std::runtime_error("Unknown input axis in BfBezier.get_average_increament_by_axis(uint32_t axis)");
		}

		float ave = 0;
		for (auto it = c_vertices.begin() + 1; it != c_vertices.end(); it++) {
			ave += fabs(it->pos[axis_index] - (it-1)->pos[axis_index]);
		}
		return ave / (float)c_vertices.size();
	}

	static inline std::pair<glm::vec3, glm::vec3> get_bbox_by_cvertices(const std::vector<glm::vec3>& vert) {
		glm::vec3 max{0.f, 0.f, 0.f};
		glm::vec3 min{0.f, 0.f, 0.f};

		for (auto& it : vert) {
			if (it.x > max.x) {
				max.x = it.x;
			}
			if (it.y > max.y) {
				max.y = it.y;
			}
			if (it.z > max.z) {
				max.z = it.z;
			}
			if (it.x < min.x) {
				min.x = it.x;
			}
			if (it.y < min.y) {
				min.y = it.y;
			}
			if (it.z < min.z) {
				min.z = it.z;
			}
		}

		return std::pair<glm::vec3, glm::vec3>(min, max);
	}

	static inline std::pair<glm::vec3, glm::vec3> get_bbox_by_iterator(
		std::vector<glm::vec3>::const_iterator it_begin,
		std::vector<glm::vec3>::const_iterator it_end
	) {
		
		glm::vec3 max{ *(it_end-1) };
		glm::vec3 min{ *it_begin };

		for (auto it = it_begin; it != it_end; it++) {
			if (it->x > max.x) {
				max.x = it->x;
			}
			if (it->y > max.y) {
				max.y = it->y;
			}
			if (it->z > max.z) {
				max.z = it->z;
			}
			if (it->x < min.x) {
				min.x = it->x;
			}
			if (it->y < min.y) {
				min.y = it->y;
			}
			if (it->z < min.z) {
				min.z = it->z;
			}
		}

		return std::pair<glm::vec3, glm::vec3>(min, max);
	}


	static inline bool get_itersections(std::vector<std::pair<glm::vec3, glm::vec3>>& inter, BfBezier& b1, BfBezier& b2) {
		using it_pair  = std::pair<std::vector<glm::vec3>::const_iterator, std::vector<glm::vec3>::const_iterator>;
		
		struct box_pack {
			std::pair<glm::vec3, glm::vec3> box1;
			std::pair<glm::vec3, glm::vec3> box2;
			bool is;
		};

		std::vector<glm::vec3> vert1 = b1.copy_vec3_vertices(30);
		std::vector<glm::vec3> vert2 = b2.copy_vec3_vertices(30);

		float x_increment = 0;
		float y_increment = 0;
		float z_increment = 0;
	
		for (int i = 1; i < vert1.size(); i++) {
			x_increment += (vert1[i].x - vert1[i - 1].x) + (vert2[i].x - vert2[i - 1].x);
			y_increment += (vert1[i].y - vert1[i - 1].y) + (vert2[i].y - vert2[i - 1].y);
			z_increment += (vert1[i].z - vert1[i - 1].z) + (vert2[i].z - vert2[i - 1].z);
		}
		
		x_increment /= vert1.size();
		y_increment /= vert1.size();
		z_increment /= vert1.size();


		std::vector<box_pack> packs;

		auto get_line_intesection = [](it_pair line1, it_pair line2) {
			glm::vec3 a1 = *line1.first;
			glm::vec3 b1 = *(line1.second-1) - *line1.first;
			// vec3(x,y,z)1 = a1 + t * b1;
			//		x1 = a1.x + t1 * b1.x
			//		y1 = a1.y + t1 * b1.y
			//		z1 = a1.z + t1 * b1.z
			
			glm::vec3 a2 = *line2.first;
			glm::vec3 b2 = *(line2.second-1) - *line2.first;
			// vec3(x,y,z)2 = a2 + t * b2;
			//		x2 = a2.x + t2 * b2.x
			//		y2 = a2.y + t2 * b2.y
			//		z2 = a2.z + t2 * b2.z

			float t1 = (a1.x * b2.y - a1.y * b2.x - a2.x * b2.y + a2.y * a2.y * b2.x) / (b1.x * b2.x - b1.y * b2.x) * -1;
			float t2 = (a1.x * b1.y - a1.y * b1.x - a2.x * b1.y + a2.y * b1.x) / (b1.x * b2.y - b1.y * b2.x) * -1;

			return glm::vec3(t1, t2, 0);
		};

		auto is_overlap = [&x_increment, &y_increment, &z_increment, &inter, &packs, &get_line_intesection](it_pair it1, it_pair it2) {
			std::pair<glm::vec3, glm::vec3> box1 = get_bbox_by_iterator(it1.first, it1.second);
			std::pair<glm::vec3, glm::vec3> box2 = get_bbox_by_iterator(it2.first, it2.second);

			box_pack pack{};
			pack.box1 = box1;
			pack.box2 = box2;

			bool xOverlap = box1.second.x > box2.first.x && box2.second.x > box1.first.x;
			
			bool yOverlap = box1.second.y > box2.first.y && box2.second.y > box1.first.y;

			bool zOverlap = box1.second.z > box2.first.z && box2.second.z > box1.first.z;



			if (!xOverlap || !yOverlap) {
				pack.is = false;
				packs.push_back(pack);
				return false;
			}
			pack.is = true;
			packs.push_back(pack);

			
			/*if (
				((fabs(box1.first.x - box2.first.x) <= x_increment * 2) && (fabs(box1.second.x - box2.second.x) <= x_increment * 2)) &&
				((fabs(box1.first.y - box2.first.y) <= y_increment * 2) && (fabs(box1.second.y - box2.second.y) <= y_increment * 2)) &&
				((fabs(box1.first.z - box2.first.z) <= z_increment * 2) && (fabs(box1.second.z - box2.second.z) <= z_increment * 2))
			) {
				inter.push_back({ (box1.first.x + box2.first.x + box1.second.x + box2.second.x) / 4, 
								  (box1.first.y + box2.first.y + box1.second.y + box2.second.y) / 4,
								  (box1.first.z + box2.first.z + box1.second.z + box2.second.z) / 4 });
			}*/
			glm::vec3 f;
			auto itd1 = (it1.second) - (it1.first);
			/*if (itd1 == 2) {
				glm::vec3 line1_1 = *it1.first;
				glm::vec3 line1_2 = *(it1.second - 1);
				inter.push_back(std::make_pair(line1_1,line1_2));
			}*/
			auto itd2 = (it2.second) - (it2.first);
			/*if (itd2 == 2) {
				glm::vec3 line2_1 = *it2.first;
				glm::vec3 line2_2 = *(it2.second-1);
				inter.push_back(std::make_pair(line2_1, line2_2));
			}*/
			if ((itd1 == 2) && (itd2 == 2)) {
				f = get_line_intesection(it1, it2);
				glm::vec3 line1_1 = *it1.first;
				glm::vec3 line1_2 = *(it1.second - 1);
				inter.push_back(std::make_pair(line1_1, line1_2));
				glm::vec3 line2_1 = *it2.first;
				glm::vec3 line2_2 = *(it2.second - 1);
				inter.push_back(std::make_pair(line2_1, line2_2));
				if (((f.x >= 0) && (f.x <= 1)) && ((f.y >= 0) && (f.y <= 1))) {
					glm::vec3 line1_1 = *it1.first;
					glm::vec3 line1_2 = *(it1.second - 1);
					inter.push_back(std::make_pair(line1_1, line1_2));
					glm::vec3 line2_1 = *it2.first;
					glm::vec3 line2_2 = *(it2.second - 1);
					inter.push_back(std::make_pair(line2_1, line2_2));
				}
			}


 			return true;
		};
		

		std::vector<std::pair<it_pair, it_pair>> curve_parts;
		curve_parts.push_back(
			std::make_pair(
				it_pair(vert1.begin(), vert1.end()), 
				it_pair(vert2.begin(), vert2.end()))
		);

		
		int count = 0;
		while (true) {
			size_t size_for_current_iteration = curve_parts.size();
			/*if (count - size_for_current_iteration == 0) {
				
				

				return true;
			}*/
			if (size_for_current_iteration == count) {
				std::cout << "boxes-begin:" << std::endl;

				for (int i = 0; i < packs.size(); i++) {
					std::cout << "(";
					std::cout << "(";
					std::cout << "(" << packs[i].box1.first.x << ", " << packs[i].box1.first.y << ", " << packs[i].box1.first.z << "), ";
					std::cout << "(" << packs[i].box1.second.x << ", " << packs[i].box1.second.y << ", " << packs[i].box1.second.z << ")";
					std::cout << "),";

					std::cout << "(";
					std::cout << "(" << packs[i].box2.first.x << ", " << packs[i].box2.first.y << ", " << packs[i].box2.first.z << "), ";
					std::cout << "(" << packs[i].box2.second.x << ", " << packs[i].box2.second.y << ", " << packs[i].box2.second.z << ")";
					std::cout << "), ";
					std::cout << (int)packs[i].is;
					std::cout << "),";
					std::cout << std::endl;
				}


				std::cout << "boxes-end:" << std::endl;
				
				break;
			}


			for (int i = count; i < size_for_current_iteration; i++) {
				if (is_overlap(curve_parts[i].first, curve_parts[i].second)) {
					
					it_pair p1b;
					it_pair p2b;
					it_pair p1e;
					it_pair p2e;

					//// FIRST-BEGIN ~ FIRST-MIDDLE
					//p1b = it_pair(curve_parts[i].first.first, curve_parts[i].first.first + (curve_parts[i].first.second - curve_parts[i].first.first) / 2);
					//// FIRST-MIDDLE ~ FIRST-END
					//p1e = it_pair(curve_parts[i].first.first + (curve_parts[i].first.second - curve_parts[i].first.first) / 2, curve_parts[i].first.second);
					//// SECOND-BEGIN ~ SECOND_MIDDLE
					//p2b = it_pair(curve_parts[i].second.first, curve_parts[i].second.first + (curve_parts[i].second.second - curve_parts[i].second.first) / 2);
					//// SECOND_MIDDLE ~ SECOND_END
					//p2e = it_pair(curve_parts[i].second.first + (curve_parts[i].second.second - curve_parts[i].second.first) / 2, curve_parts[i].second.second);

					
						
					if (((curve_parts[i].first.second - curve_parts[i].first.first) % 2) != 0) {
						auto p1bf = curve_parts[i].first.first - curve_parts[i].first.first;
						auto p1bs = curve_parts[i].first.first + (curve_parts[i].first.second - curve_parts[i].first.first) / 2 + 1 - curve_parts[i].first.first;
						auto p1ef = curve_parts[i].first.first + (curve_parts[i].first.second - curve_parts[i].first.first) / 2 + 1 - curve_parts[i].first.first;
						auto p1es = curve_parts[i].first.second - curve_parts[i].first.first;
						
						p1b = it_pair(curve_parts[i].first.first + p1bf, curve_parts[i].first.first + p1bs);
						p1e = it_pair(curve_parts[i].first.first + p1ef, curve_parts[i].first.first + p1es);
					}
					else {
						auto p1bf = curve_parts[i].first.first - curve_parts[i].first.first;
						auto p1bs = curve_parts[i].first.first + (curve_parts[i].first.second - curve_parts[i].first.first) / 2 - curve_parts[i].first.first;
						auto p1ef = curve_parts[i].first.first + (curve_parts[i].first.second - curve_parts[i].first.first) / 2 - curve_parts[i].first.first;
						auto p1es = curve_parts[i].first.second - curve_parts[i].first.first;

						p1b = it_pair(curve_parts[i].first.first + p1bf, curve_parts[i].first.first + p1bs);
						p1e = it_pair(curve_parts[i].first.first + p1ef, curve_parts[i].first.first + p1es);
					} 

					if (((curve_parts[i].second.second - curve_parts[i].second.first) % 2) != 0) {
						auto p2bf = curve_parts[i].second.first - curve_parts[i].second.first;
						auto p2bs = curve_parts[i].second.first + (curve_parts[i].second.second - curve_parts[i].second.first) / 2 + 1 - curve_parts[i].second.first;
						auto p2ef = curve_parts[i].second.first + (curve_parts[i].second.second - curve_parts[i].second.first) / 2 + 1 - curve_parts[i].second.first;
						auto p2es = curve_parts[i].second.second - curve_parts[i].second.first;
						
						p2b = it_pair(curve_parts[i].second.first + p2bf, curve_parts[i].second.first + p2bs);
						p2e = it_pair(curve_parts[i].second.first + p2ef, curve_parts[i].second.first + p2es);
					}
					else {
						auto p2bf = curve_parts[i].second.first - curve_parts[i].second.first;
						auto p2bs = curve_parts[i].second.first + (curve_parts[i].second.second - curve_parts[i].second.first) / 2 - curve_parts[i].second.first;
						auto p2ef = curve_parts[i].second.first + (curve_parts[i].second.second - curve_parts[i].second.first) / 2 - curve_parts[i].second.first;
						auto p2es = curve_parts[i].second.second - curve_parts[i].second.first;

						p2b = it_pair(curve_parts[i].second.first + p2bf, curve_parts[i].second.first + p2bs);
						p2e = it_pair(curve_parts[i].second.first + p2ef, curve_parts[i].second.first + p2es);
					}
					
					curve_parts.push_back(std::make_pair(p1b, p2e));
					curve_parts.push_back(std::make_pair(p1b, p2b));
					curve_parts.push_back(std::make_pair(p1e, p2e));
					curve_parts.push_back(std::make_pair(p1e, p2b));
				}
				else {

				}
				count += 1;
			}
			
		}
		return true;
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