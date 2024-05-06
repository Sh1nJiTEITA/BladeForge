#ifndef BF_CURVES_HPP
#define BF_CURVES_HPP

#include "bfVariative.hpp"
#include "bfVertex2.hpp"
//#include "bfMatrix.hpp"

#include <vector>
#include <algorithm>
#include <math.h>

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

class BfHandle {
	std::array<BfVertex3, 4> __vertices;
	BfVertex3 __center;

	glm::vec3 __color;
	float __len;

public:

	BfHandle(const BfVertex3& center, const float& len, const glm::vec3& color)
		: __center{ center }
		, __color{ color }
		, __len{ len } {}

	BfHandle() : BfHandle(BfVertex3(), 1.0f, { 1.0f,0.0f,1.0f }) {}

	void update_vertices() {
		glm::vec3 v1 = glm::normalize(glm::cross(__center.normals, glm::vec3(1.0f, 0.0f, 0.0f)));
		glm::vec3 v2 = glm::normalize(glm::cross(__center.normals, v1));

		__vertices[0].pos = __center.pos + v1 * __len * 0.5f + v2 * __len * 0.5f;
		__vertices[1].pos = __center.pos - v1 * __len * 0.5f + v2 * __len * 0.5f;
		__vertices[2].pos = __center.pos - v1 * __len * 0.5f - v2 * __len * 0.5f;
		__vertices[3].pos = __center.pos + v1 * __len * 0.5f - v2 * __len * 0.5f;

		for (auto& it : __vertices) {
			it.color = __color;
			it.normals = __center.normals;
		}
	}

	const std::array<BfVertex3, 4>& get_vertices() {
		return __vertices;
	}

	const std::array<BfVertex3, 4>& update_and_get_vertices() {
		this->update_vertices();
		return this->get_vertices();
	}

	std::array<uint32_t, 4> get_indices() {
		return { 0, 1, 2, 3 };
	}
};


/*
* BfLine class provides opportunity to work with lines defined by 2 glm::vec3 points.
*/
class BfLine {
	BfVertex3 start_point;
	BfVertex3 finish_point;

public:

	BfLine(const glm::vec3& sp, const glm::vec3& fp) {
		start_point.pos = sp;
		finish_point.pos = fp;
	}

	BfLine(const BfVertex3& sp, const BfVertex3& fp) : start_point{ sp }, finish_point{ fp } {}

	glm::vec3 get_direction_from_start() const {
		return finish_point.pos - start_point.pos;
	}

	glm::vec3 get_direction_from_finish() const {
		return start_point.pos - finish_point.pos;
	}

	const BfVertex3& get_start_point() const {
		return start_point;
	}
	const BfVertex3& get_finish_point() const {
		return finish_point;
	}
	/*
	* Returns true if there are intersection between 2 lines else false;
	* 
	* Needed:	glm::vec3 intersection - vec3 to store position of intersection
	*			const BfLine& line1 - const reference to 1st line to work with
	*			const BfLine& line2 - const reference to 2st line to work with
	*/
	static inline bool find_lines_intersection(glm::vec3& intersection, const BfLine& line1, const BfLine& line2) {
		glm::mat3 plane;
		plane[0][0] = line2.finish_point.pos.x - line1.start_point.pos.x;
		plane[0][1] = line2.finish_point.pos.y - line1.start_point.pos.y;
		plane[0][2] = line2.finish_point.pos.z - line1.start_point.pos.z;
																  
		plane[1][0] = line1.finish_point.pos.x - line1.start_point.pos.x;
		plane[1][1] = line1.finish_point.pos.y - line1.start_point.pos.y;
		plane[1][2] = line1.finish_point.pos.z - line1.start_point.pos.z;
																  
		plane[2][0] =  line2.start_point.pos.x - line1.start_point.pos.x;
		plane[2][1] =  line2.start_point.pos.y - line1.start_point.pos.y;
		plane[2][2] =  line2.start_point.pos.z - line1.start_point.pos.z;

		if (glm::determinant(plane) == 0) {
			// a1.x + b1.x * t1 = a2.x + b2.x * t2
			// a1.y + b1.y * t1 = a2.y + b2.y * t2
			glm::vec3 a1 = line1.start_point.pos;
			glm::vec3 b1 = line1.get_direction_from_start();

			glm::vec3 a2 = line2.start_point.pos;
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
	
	static inline float get_line_length(const BfLine& l) {
		
		glm::vec3 delta = l.finish_point.pos - l.start_point.pos;
		return glm::sqrt(
			delta.x * delta.x
			+ 
			delta.y * delta.y
			+ 
			delta.z * delta.z
		);
	}
};



/*
* BfLine class provides opportunity to work with Bezier curves.
*/
class BfBezier {
	
	uint32_t n;
	glm::vec3 basic_color;

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
	BfBezier() : n{ 0 }, vertices{}, c_vertices{}, basic_color{1.0f,1.0f,1.0f} {}

	BfBezier(uint32_t in_n, std::vector<BfVertex3>& in_vertices, glm::vec3 in_color = {1.0f,1.0f,1.0f})
		: n{ in_n }
		, basic_color{ in_color }
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

	BfBezier(uint32_t in_n, std::vector<glm::vec3> in_vertices, glm::vec3 in_color = {1.0f,1.0f,1.0f})
		: n{in_n}
		, vertices{in_vertices} 
		, basic_color{ in_color } {
		if (in_n+1 != static_cast<uint32_t>(in_vertices.size())) {
			throw std::runtime_error("Input bezier data is incorrect: vec.size() != in_n");
		}
	}

	/*
	* Sets general color for whole Bezier curve.
	* 
	* Needed: glm::vec3 color - color
	* 
	*/

	void set_color(glm::vec3 color) {
		this->basic_color = color;
		if (!c_vertices.empty()) {
			this->update_vertices(c_vertices.size());
		}
	}

	/*
	* (CLASSIC_CALCULATION)
	* Returns calculated vertex based on input vertices and parameter (t).
	* 
	* Needed: 1. t - relative Bezier curve parameter, standart value of t = [0, 1]
	*		  
	*/
	glm::vec3 get_single_vertex(float t) const {
		glm::vec3 _v{ 0.0f,0.0f,0.0f };
		for (int i = 0; i <= n; i++) {
			_v += static_cast<float>(bfGetBinomialCoefficient(n, i) * glm::pow(1 - t, n - i) * glm::pow(t, i)) * vertices[i];
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
			c_vertices[i].color = basic_color;
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
	const std::vector<BfVertex3>& get_cvertices() const {
		return c_vertices;
	}
	
	const std::vector<glm::vec3>& get_vertices() const {
		return vertices;
	}

	/*
	* Calculates and outputs output arrays of vertices.
	* 
	* Needed: v_count - number of calculating vertices.
	* 
	*/
	std::vector<BfVertex3>& update_and_get_vertices(uint32_t v_count) {
		update_vertices(v_count);
		return c_vertices;
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
	glm::vec3 get_direction_tangent(float t) const {
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
	glm::vec3 get_direction_normal(float t) const {
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
	glm::vec3 get_single_derivative_1(float t) const {
		
		int k = n - 1;
		glm::vec3 out(0.0f);
		for (int i = 0; i <= k; i++) {
			out += (float)(bfGetBinomialCoefficient(k, i) * glm::pow(1 - t, k - i) * glm::pow(t, i) * n) * (vertices[i + 1] - vertices[i]);
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
			out += (float)((i - n*t)/(t*(1-t))*bfGetBinomialCoefficient(n, i) * glm::pow(1 - t, n - i) * glm::pow(t, i)) * vertices[i];
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
					glm::pow(1 - t, k - i) * 
					glm::pow(t, i) * n
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
	glm::vec3 get_single_derivative_2_e(float t) const {
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
				bfGetBinomialCoefficient(n, i) * glm::pow(1 - t, n - i) * glm::pow(t, i)) * vertices[i];
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
		

		return BfBezier(k, new_define_vertices, basic_color);
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
		return BfBezier(k, new_input_vertices, basic_color);
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
	//BfBezier get_alligned(unsigned int plane, unsigned int axis) {
	//	
	//	BfBezier o = *this;

	//	glm::vec3 to_start = *vertices.begin();

	//	glm::vec3 last = (*(o.vertices.end() - 1)) - to_start;
	//	float r = sqrt(last.x * last.x + last.y * last.y + last.z * last.z);


	//	float angle_x = glm::degrees(glm::asin(-last.y / r));
	//	float angle_y = glm::degrees(glm::asin(-last.x / r));
	//	float angle_z = glm::degrees(last.z / r);


	//	for (size_t i = 0; i < this->vertices.size(); i++) {
	//		o.vertices[i] -= to_start;
	//		//o.vertices[i] = BfMatrix::multiply(rotate_matrix, o.vertices[i]).get_vec3();
	//	}
	//	
	//	glm::vec3 last_m;

	//	switch (axis) {
	//		case BF_AXIS_X:
	//		{
	//			last_m = glm::normalize((*(o.vertices.end() - 1)));
	//			BfQuaternion qx = BfQuaternion::get_rotation_quaternion(
	//				glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)), 
	//				-asinf(last_m.y / sqrtf(last_m.y * last_m.y + last_m.z * last_m.z))
	//			);
	//			BfQuaternion totalx = qx * BfQuaternion{ 1,0,0,0 };
	//			BfMatrix mx = totalx.get_rotation_matrix4();
	//			
	//			last_m = BfMatrix::multiply(mx, glm::vec4(last_m.x, last_m.y, last_m.z, 1.0f)).get_vec4();
	//		
	//			BfQuaternion qy = BfQuaternion::get_rotation_quaternion(
	//				glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)),
	//				asinf(last_m.z / sqrtf(last_m.x * last_m.x + last_m.z * last_m.z))
	//			);
	//			BfQuaternion totaly = qy * BfQuaternion{ 1,0,0,0 };
	//			BfMatrix my = totaly.get_rotation_matrix4();
	//		
	//			for (size_t i = 0; i < this->vertices.size(); i++) {
	//				glm::vec4 l = { o.vertices[i].x, o.vertices[i].y, o.vertices[i].z, 1.0f };
	//				o.vertices[i] = (my * mx * l).get_vec4();
	//			}

	//			if (plane == BF_PLANE_XY) {

	//			}
	//			else if (plane == BF_PLANE_XZ) {

	//			}
	//			else {
	//				throw std::runtime_error("Input BF_AXIS_X can't be processed with BF_PLANE_YZ");
	//			}


	//			break;
	//		}
	//		case BF_AXIS_Y:
	//		{
	//			last_m = glm::normalize((*(o.vertices.end() - 1)));
	//			BfQuaternion qy = BfQuaternion::get_rotation_quaternion(
	//				glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)),
	//				-asinf(last_m.x / sqrtf(last_m.x * last_m.x + last_m.z * last_m.z))
	//			);
	//			BfQuaternion totalx = qy * BfQuaternion{ 1,0,0,0 };
	//			BfMatrix mx = totalx.get_rotation_matrix4();

	//			last_m = BfMatrix::multiply(mx, glm::vec4(last_m.x, last_m.y, last_m.z, 1.0f)).get_vec4();

	//			BfQuaternion qx = BfQuaternion::get_rotation_quaternion(
	//				glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)),
	//				asinf(last_m.z / sqrtf(last_m.y * last_m.y + last_m.z * last_m.z))
	//			);
	//			BfQuaternion totaly = qx * BfQuaternion{ 1,0,0,0 };
	//			BfMatrix my = totaly.get_rotation_matrix4();


	//			for (size_t i = 0; i < this->vertices.size(); i++) {
	//				glm::vec4 l = { o.vertices[i].x, o.vertices[i].y, o.vertices[i].z, 1.0f };
	//				o.vertices[i] = (my * mx * l).get_vec4();
	//			}
	//			break;
	//		}
	//		case BF_AXIS_Z:
	//		{
	//			last_m = glm::normalize((*(o.vertices.end() - 1)));
	//			BfQuaternion qy = BfQuaternion::get_rotation_quaternion(
	//				glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)),
	//				asinf(last_m.x / sqrtf(last_m.x * last_m.x + last_m.y * last_m.y))
	//			);
	//			BfQuaternion totalx = qy * BfQuaternion{ 1,0,0,0 };
	//			BfMatrix mx = totalx.get_rotation_matrix4();

	//			last_m = BfMatrix::multiply(mx, glm::vec4(last_m.x, last_m.y, last_m.z, 1.0f)).get_vec4();

	//			BfQuaternion qx = BfQuaternion::get_rotation_quaternion(
	//				glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)),
	//				-asinf(last_m.y / sqrtf(last_m.y * last_m.y + last_m.z * last_m.z))
	//			);
	//			BfQuaternion totaly = qx * BfQuaternion{ 1,0,0,0 };
	//			BfMatrix my = totaly.get_rotation_matrix4();


	//			for (size_t i = 0; i < this->vertices.size(); i++) {
	//				glm::vec4 l = { o.vertices[i].x, o.vertices[i].y, o.vertices[i].z, 1.0f };
	//				o.vertices[i] = (my * mx * l).get_vec4();
	//			}
	//			break;
	//		}
	//	}
	//		

	//
	//	return o;
	//}

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

	/*
	* Returns array of BfVertex3 vertices of calculated handles.
	* 
	* Needed: float length -> length of rectangle side
	*		  glm::vec3 color -> general color of whole handle
	* 
	*/
	std::vector<BfVertex3> get_handles_vertices(float length = 1.0f, glm::vec3 color = {1.0f, 0.0f, 1.0f}) const {
		std::vector<BfVertex3> vertices(4 * this->vertices.size());

		size_t offset = 0;
		for (size_t i = 0; i < this->vertices.size(); i++) {
			BfVertex3 center{};
			center.pos = this->vertices[i];
			center.normals = this->get_vertex_normal_by_curve(i);
			
			BfHandle handle(center, length, color);
			handle.update_vertices();

			std::copy(handle.get_vertices().begin(), handle.get_vertices().end(), vertices.begin() + offset);

			offset += 4;
		}
		return vertices;
	}

	/*
	* Returns array of uint32_t indices of calculated handles for index-drawing.
	*
	* Needed:	1. float length -> length of rectangle side
	*			2. glm::vec3 color -> general color of whole handle
	*
	*/
	std::vector<uint32_t> get_handles_indices() const {
		std::vector<uint32_t> indices(vertices.size() * 6);

		size_t offset = 0;
		for (size_t i = 0; i < indices.size(); i+=6) {
			indices[i + 0] = static_cast<uint32_t>(offset+0);
			indices[i + 1] = static_cast<uint32_t>(offset+1);
			indices[i + 2] = static_cast<uint32_t>(offset+2);
			indices[i + 3] = static_cast<uint32_t>(offset+2);
			indices[i + 4] = static_cast<uint32_t>(offset+3);
			indices[i + 5] = static_cast<uint32_t>(offset+0);
			offset += 4;
		}
		return indices;
	}

	std::vector<BfVertex3> get_carcass_vertices(glm::vec3 color = {1.0f, 0.0f, 1.0f}) const {
		std::vector<BfVertex3> vertices(this->vertices.size());

		for (size_t i = 0; i < vertices.size(); i++) {
			vertices[i].pos = this->vertices[i];
			vertices[i].color = color;
			vertices[i].normals = this->get_vertex_normal_by_curve(i);
		}

		return vertices;
	}

	std::vector<uint32_t> get_carcass_indices() const {
		std::vector<uint32_t> indices(this->vertices.size());

		for (size_t i = 0; i < indices.size(); i++) {
			indices[i] = static_cast<uint32_t>(i);
		}
		return indices;
	}

	std::vector<glm::vec3> get_length_grad(std::vector<bool> is_working) {
		if (is_working.size() != (this->n + 1)) {
			throw std::runtime_error("!_P");
		}
		
		std::vector<glm::vec3> grad(this->n + 1, {0.0f, 0.0f, 0.0f});

		// go for each defining point
		for (size_t p_index = 0; p_index < grad.size(); p_index++) {
			glm::vec3 grad_part{0.0f,0.0f,0.0f};

			// go along curve and get derivatices of lines
			// derivative of CURVE by point of p_index is sum 
			// of derivatives of all lines

			for (size_t j = 0; j < this->c_vertices.size(); j++) {

				// Calculate derivative of length of single line
				// dL_j / d(P[p_index]) = sum(dL_j / d(P[p_index))

				float t_j = (float)j / (this->c_vertices.size() - 1);
				float t_jpp = (float)(j + 1) / (this->c_vertices.size() - 1);

				glm::vec3 d_j{ 0.0f,0.0f,0.0f };
				glm::vec3 dd_j{ 0.0f,0.0f,0.0f };

				for (size_t i = 0; i < this->vertices.size(); i++) {
					float basis_i = bfGetBinomialCoefficient(this->n, i) * (
						glm::pow(t_jpp, i) * glm::pow(1 - t_jpp, this->n - i)
						-
						glm::pow(t_j, i) * glm::pow(1 - t_j, this->n - i)
					);

					d_j += basis_i * this->vertices[i];

					if ((i == p_index) && (is_working[p_index])) {
						dd_j = { basis_i, basis_i, basis_i };
					}
				}
				
				glm::vec3 dl = {
					(d_j.x * dd_j.x) / (glm::sqrt(d_j.x * d_j.x + d_j.y * d_j.y + d_j.z * d_j.z)),
					(d_j.y * dd_j.y) / (glm::sqrt(d_j.x * d_j.x + d_j.y * d_j.y + d_j.z * d_j.z)),
					(d_j.z * dd_j.z) / (glm::sqrt(d_j.x * d_j.x + d_j.y * d_j.y + d_j.z * d_j.z)),
				};

				grad_part += dl;
			}
		

			grad[p_index] = (grad_part);
		}

		return grad;
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

	/*
	* Returns true if there are intesections between B1 and B2 else no intersections;
	* 
	* Needed:	intersections - reference to std::vector<glm::vec3> container to store
	*							pos of intersections if there are more than 1;
	*			B1 - BfBezier curve 1;
	*			B2 - BfBezier curve 2;
	* 
	*/
	static inline bool get_intersections_simple(std::vector<glm::vec3>& intersections, const BfBezier& B1, const BfBezier& B2) {
		
		if (B1.c_vertices.empty() || B2.c_vertices.empty()) {
			throw std::runtime_error("intesections can't be found: no calculated points in one of curves");
		}

		bool has_intersections = false;

		for (size_t i = 0; i < B1.c_vertices.size() - 1; i++) {
			BfLine line1(B1.c_vertices[i].pos, B1.c_vertices[i + 1].pos);
			for (size_t j = 0; j < B2.c_vertices.size() - 1; j++) {
				BfLine line2(B2.c_vertices[j].pos, B2.c_vertices[j + 1].pos);
				
				glm::vec3 inter{};
				if (BfLine::find_lines_intersection(inter, line1, line2)) {
					intersections.push_back(inter);
					has_intersections = true;
				}
			}
		}
		return has_intersections;
	}

	/*
	* Returns true if there are intesections between Beizer curve and line else no 
	* intersections;
	*
	* Needed:	intersections - reference to std::vector<glm::vec3> container to store
	*							pos of intersections if there are more than 1;
	*			B - BfBezier curve;
	*			L - BfLine;
	*
	*/
	static inline bool get_intersections_simple(std::vector<glm::vec3>& intersections, const BfBezier& B, const BfLine& L) {
		bool is_intersection = false;
		for (int i = 0; i < B.c_vertices.size() - 1; i++) {
			BfLine local_line(B.c_vertices[i].pos, B.c_vertices[i + 1].pos);
			glm::vec3 intersection{};
			if (BfLine::find_lines_intersection(intersection, L, local_line)) {
				intersections.push_back(intersection);
				is_intersection = true;
			}
		}
		return is_intersection;
	}

	// TODO: finding intesections in way of checing curve box intersections;
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
				/*glm::vec3 line1_1 = *it1.first;
				glm::vec3 line1_2 = *(it1.second - 1);
				inter.push_back(std::make_pair(line1_1, line1_2));
				glm::vec3 line2_1 = *it2.first;
				glm::vec3 line2_2 = *(it2.second - 1);
				inter.push_back(std::make_pair(line2_1, line2_2));*/
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
		

		//std::vector<std::pair<it_pair, it_pair>> curve_parts;
		std::list<std::pair<it_pair, it_pair>> curve_parts;
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


			std::list<std::list<std::pair<it_pair, it_pair>>::iterator> del_it_list;
			for (auto it = curve_parts.begin(); it != curve_parts.end(); it++) {
				
				if (is_overlap(it->first, it->second)) {
					
					it_pair p1b;
					it_pair p2b;
					it_pair p1e;
					it_pair p2e;

					size_t p1b_l = (it->first.second - it->first.first) / 2;
					size_t p1e_l = it->first.second - it->first.first + (it->first.second - it->first.first) / 2;
					size_t p2b_l = (it->second.second - it->second.first) / 2;
					size_t p2e_l = it->second.second - it->second.first + (it->second.second - it->second.first) / 2;

					if (p1b_l >= 2) {
						p1b = it_pair(it->first.first, it->first.first + (it->first.second - it->first.first) / 2);
					}
					if (p1e_l >= 2) {
						p1e = it_pair(it->first.first + (it->first.second - it->first.first) / 2, it->first.second);
					}
					if (p2b_l >= 2) {
						p2b = it_pair(it->second.first, it->second.first + (it->second.second - it->second.first) / 2);
					}
					if (p2e_l >= 2) {
						p2e = it_pair(it->second.first + (it->second.second - it->second.first) / 2, it->second.second);
					}

					curve_parts.push_back(std::make_pair(p1b, p2e));
					curve_parts.push_back(std::make_pair(p1b, p2b));
					curve_parts.push_back(std::make_pair(p1e, p2e));
					curve_parts.push_back(std::make_pair(p1e, p2b));
				}
				else {
					del_it_list.push_back(it);
				}
				//count += 1;
			}
			for (auto it = del_it_list.begin(); it != del_it_list.end(); it++) {
				curve_parts.erase(*it);
			}
			
		}
		return true;
	}
	
	/*
	* Returns approximated float-length of input BfBezier by count of c_vertices
	* 
	* Needed:	B - BfBezier curve to work with;
	* 
	*/
	static inline float get_approx_length(const BfBezier& B) {
		float len = 0;
		for (size_t i = 0; i < B.c_vertices.size() - 1; i++) {
			len += BfLine::get_line_length(
				{ B.c_vertices[i + 1].pos, B.c_vertices[i].pos }
			);
		}
		return len;
	}
	
private:
	/*
	* Returns normal of defining vertex (defined by index in vertices array)
	* 
	* Needed: index - index of defining vertex in this->vertices
	* 
	* TODO make 3D workable
	*/
	glm::vec3 get_vertex_normal_by_curve(size_t index) const {
		return glm::cross(this->get_direction_normal(0.5f), this->get_direction_tangent(0.5f));
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
