#include "bfBladeSection.h"

bool bfCheckBladeSectionCreateInfoEquality(
	const BfBladeSectionCreateInfo& i_1,
	const BfBladeSectionCreateInfo& i_2
) {
	
	
	/*if (i_1.width == 0 or i_2.width == 0) return true;
	if (i_1.install_angle	== 0 or i_2.install_angle == 0) return true;
	if (i_1.inlet_angle		== 0 or i_2.inlet_angle == 0) return true;
	if (i_1.outlet_angle	== 0 or i_2.outlet_angle == 0) return true;
	if (i_1.inlet_radius	== 0 or i_2.inlet_radius == 0) return true;
	if (i_1.outlet_radius	== 0 or i_2.outlet_radius == 0) return true;
	if (i_1.border_length	== 0 or i_2.border_length == 0) return true;
	*/
	if (CHECK_FLOAT_EQUALITY_TO_NULL(i_1.width, 0)				or CHECK_FLOAT_EQUALITY_TO_NULL(i_2.width			, 0)) return true;
	if (CHECK_FLOAT_EQUALITY_TO_NULL(i_1.install_angle, 0)		or CHECK_FLOAT_EQUALITY_TO_NULL(i_2.install_angle	, 0)) return true;
	if (CHECK_FLOAT_EQUALITY_TO_NULL(i_1.inlet_angle, 0)		or CHECK_FLOAT_EQUALITY_TO_NULL(i_2.inlet_angle		, 0)) return true;
	if (CHECK_FLOAT_EQUALITY_TO_NULL(i_1.outlet_angle, 0)		or CHECK_FLOAT_EQUALITY_TO_NULL(i_2.outlet_angle		, 0)) return true;
	if (CHECK_FLOAT_EQUALITY_TO_NULL(i_1.inlet_radius, 0)		or CHECK_FLOAT_EQUALITY_TO_NULL(i_2.inlet_radius		, 0)) return true;
	if (CHECK_FLOAT_EQUALITY_TO_NULL(i_1.outlet_radius, 0)		or CHECK_FLOAT_EQUALITY_TO_NULL(i_2.outlet_radius	, 0)) return true;
	if (CHECK_FLOAT_EQUALITY_TO_NULL(i_1.border_length	, 0)	or CHECK_FLOAT_EQUALITY_TO_NULL(i_2.border_length	, 0)) return true;


	return {
		i_1.width == i_2.width &&
		i_1.install_angle == i_2.install_angle &&

		i_1.inlet_angle == i_2.inlet_angle &&
		i_1.outlet_angle == i_2.outlet_angle &&

		i_1.inlet_radius == i_2.inlet_radius &&
		i_1.outlet_radius == i_2.outlet_radius &&

		i_1.border_length == i_2.border_length &&

		i_1.start_vertex == i_2.start_vertex &&
		i_1.grow_direction == i_2.grow_direction &&
		i_1.up_direction == i_2.up_direction &&

		i_1.l_pipeline == i_2.l_pipeline &&
		i_1.t_pipeline == i_2.t_pipeline
	};
}



BfBladeSection::BfBladeSection(const BfBladeSectionCreateInfo& info) 
	: BfDrawLayer(info.layer_create_info)
	, __info{info}
{
	__generate_outer_elements();
	__generate_blade_geometry();

	__generate_draw_data();
}

void BfBladeSection::remake(const BfBladeSectionCreateInfo& info) {
	__clear_draw_data();
	
	__info = info;

	__generate_outer_elements();
	__generate_blade_geometry();

	__generate_draw_data();
}

const BfVertex3& BfBladeSection::get_inlet_center() const noexcept {
	return __chord_inlet_center;
}
const BfVertex3& BfBladeSection::get_outlet_center() const noexcept {
	return __chord_outlet_center;
}

std::shared_ptr<BfSingleLine> BfBladeSection::get_top_border(){
	return std::dynamic_pointer_cast<BfSingleLine>(this->get_object_by_index(0));
}
std::shared_ptr<BfSingleLine> BfBladeSection::get_bot_border(){
	return std::dynamic_pointer_cast<BfSingleLine>(this->get_object_by_index(1));
}
std::shared_ptr<BfSingleLine> BfBladeSection::get_chord(){
	return std::dynamic_pointer_cast<BfSingleLine>(this->get_object_by_index(2));
}
std::shared_ptr<BfCircle> BfBladeSection::get_inlet_edge() {
	return std::dynamic_pointer_cast<BfCircle>(this->get_object_by_index(3));
}
std::shared_ptr<BfCircle> BfBladeSection::get_outlet_edge() {
	return std::dynamic_pointer_cast<BfCircle>(this->get_object_by_index(4));
}
std::shared_ptr<BfSingleLine> BfBladeSection::get_inlet_vector() {
	return std::dynamic_pointer_cast<BfSingleLine>(this->get_object_by_index(5));
}
std::shared_ptr<BfSingleLine> BfBladeSection::get_outlet_vector() {
	return std::dynamic_pointer_cast<BfSingleLine>(this->get_object_by_index(6));
}
std::shared_ptr<BfSingleLine> BfBladeSection::get_inlet_io() {
	return std::dynamic_pointer_cast<BfSingleLine>(this->get_object_by_index(7));
}
std::shared_ptr<BfSingleLine> BfBladeSection::get_outlet_io() {
	return std::dynamic_pointer_cast<BfSingleLine>(this->get_object_by_index(8));
}
std::shared_ptr<BfBezierCurve> BfBladeSection::get_ave_curve() {
	return std::dynamic_pointer_cast<BfBezierCurve>(this->get_object_by_index(9));
}



void BfBladeSection::__generate_outer_elements() {

	__chord_inlet_center = {
		__info.start_vertex,
		{1.0f, 1.0f, 1.0f},
		__info.up_direction
	};
	
	glm::vec3 left_border_dir = glm::normalize(
		glm::cross(__info.up_direction, __info.grow_direction)
	);
	glm::vec3 right_border_dir = -left_border_dir;

	{ // TOP BORDER
		auto top_border = std::make_shared<BfSingleLine>(
			BfVertex3{
				__chord_inlet_center.pos + left_border_dir * __info.border_length / 2.0f,
				{1.0f, 1.0f, 1.0f},
				__info.up_direction
			},
			BfVertex3{
				__chord_inlet_center.pos + right_border_dir * __info.border_length / 2.0f,
				{1.0f, 1.0f, 1.0f},
				__info.up_direction
			}
		);
		this->add_l(top_border);
	}
	
	{ // BOT BORDER
		auto bot_border = std::make_shared<BfSingleLine>(
			BfVertex3{
				__chord_inlet_center.pos + __info.grow_direction * __info.width
				+ left_border_dir * __info.border_length / 2.0f,
				{1.0f, 1.0f, 1.0f},
				__info.up_direction
			},
			BfVertex3{
				__chord_inlet_center.pos + __info.grow_direction * __info.width
				+ right_border_dir * __info.border_length / 2.0f,
				{1.0f, 1.0f, 1.0f},
				__info.up_direction
			}
		);
		this->add_l(bot_border);
	}
	
	glm::vec4 chord_dir = (
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f - __info.install_angle), __info.up_direction)
		* glm::vec4(__info.start_vertex + __info.grow_direction, 1.0f)
	);

	__chord_outlet_center = BfVertex3{
		__chord_inlet_center.pos +
		chord_dir.xyz * __info.width / glm::sin(glm::radians(__info.install_angle)),
		{0.5f, 0.5f, 1.0f},
		__info.up_direction
	};

	{ // CHORD 
		auto chord = std::make_shared<BfSingleLine>(
			__chord_inlet_center,
			__chord_outlet_center
		);
		this->add_l(chord);
	}

	
}

void BfBladeSection::__generate_blade_geometry() {
	
	// INLET-OUTLET potencial intersection
	glm::vec3 _inlet_dir = (
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f + __info.inlet_angle), __info.up_direction)
		* glm::vec4(__info.start_vertex + __info.grow_direction, 1.0f)
	);
	auto _inlet_vector = std::make_shared<BfSingleLine>(
		__chord_inlet_center,
		__chord_inlet_center.pos + glm::normalize(_inlet_dir)
	);

	glm::vec3 _outlet_dir = (
		glm::rotate(glm::mat4(1.0f), glm::radians(90.0f - __info.outlet_angle), __info.up_direction)
		* glm::vec4(__chord_outlet_center.pos + __info.grow_direction, 1.0f)
	);
	auto _outlet_vector = std::make_shared<BfSingleLine>(
		__chord_outlet_center,
		__chord_outlet_center.pos + glm::normalize(_outlet_dir)
	);

	glm::vec3 potencial_io_intersection = bfMathFindLinesIntersection(
		*_inlet_vector.get(),
		*_outlet_vector.get(),
		BF_MATH_FIND_LINES_INTERSECTION_ANY
	);

	
	glm::vec3 section_center_line_vert_1 =
		(get_top_border()->get_first().pos + get_bot_border()->get_first().pos) / 2.0f;

	glm::vec3 section_center_line_vert_2 =
		(get_top_border()->get_second().pos + get_bot_border()->get_second().pos) / 2.0f;

	auto center_line = std::make_shared<BfSingleLine>(
		BfVertex3{ section_center_line_vert_1, {1.0f,1.0f,1.0f}, {get_inlet_center().normals} },
		BfVertex3{ section_center_line_vert_2, {1.0f,1.0f,1.0f}, {get_inlet_center().normals} }
	);

		
	{ // INLET EDGE
		std::vector<BfCircle> c = bfMathGetInscribedCircles(
			100,
			*get_top_border().get(),
			*get_chord().get(),
			__info.inlet_radius
		);

		std::vector<std::pair<BfCircle*, float>> distances_to_center_line;
		for (size_t i = 0; i < c.size(); i++) {
			distances_to_center_line.push_back(std::make_pair(&c[i], bfMathGetDistanceToLine(*center_line, c[i].get_center().pos)));
		}
		
		std::sort(
			distances_to_center_line.begin(),
			distances_to_center_line.end(),
			[](const auto& a, const auto& b) { return a.second < b.second; }
		);

		distances_to_center_line.pop_back();
		distances_to_center_line.pop_back();


		float min_distance_to_intersection = FLT_MAX;
		BfCircle* circle = nullptr;

		for (auto& pair : distances_to_center_line) {
			float distance_to_intersection = glm::distance(potencial_io_intersection, pair.first->get_center().pos);

			if (distance_to_intersection < min_distance_to_intersection)
			{
				min_distance_to_intersection = distance_to_intersection;
				circle = pair.first;
			}
			//std::cout << std::setprecision(10) << distance_to_center_line << " ";//<< c[i].get_center().pos.x << " " << c[i].get_center().pos.y << " " << c[i].get_center().pos.z << "|";
		}

		if (circle != nullptr)
			this->add_l(std::make_shared<BfCircle>(*circle));
		else abort();
	}

	{ // OUTLET EDGE
		std::vector<BfCircle> c = bfMathGetInscribedCircles(
			100,
			*get_bot_border().get(),
			*get_chord().get(),
			__info.outlet_radius
		);

		
		std::vector<std::pair<BfCircle*, float>> distances_to_center_line;
		for (size_t i = 0; i < c.size(); i++) {
			distances_to_center_line.push_back(std::make_pair(&c[i], bfMathGetDistanceToLine(*center_line, c[i].get_center().pos)));
		}

		std::sort(
			distances_to_center_line.begin(),
			distances_to_center_line.end(),
			[](const auto& a, const auto& b) { return a.second < b.second; }
		);

		distances_to_center_line.pop_back();
		distances_to_center_line.pop_back();


		float min_distance_to_intersection = FLT_MAX;
		BfCircle* circle = nullptr;

		for (auto& pair : distances_to_center_line) {
			float distance_to_intersection = glm::distance(potencial_io_intersection, pair.first->get_center().pos);

			if (distance_to_intersection < min_distance_to_intersection)
			{
				min_distance_to_intersection = distance_to_intersection;
				circle = pair.first;
			}
			//std::cout << std::setprecision(10) << distance_to_center_line << " ";//<< c[i].get_center().pos.x << " " << c[i].get_center().pos.y << " " << c[i].get_center().pos.z << "|";
		}

		if (circle != nullptr)
			this->add_l(std::make_shared<BfCircle>(*circle));
		else abort();
	}


	{ // INLET VECTOR
		glm::vec3 inlet_dir = (
			glm::rotate(glm::mat4(1.0f), glm::radians(90.0f + __info.inlet_angle), __info.up_direction)
			*glm::vec4(__info.grow_direction, 1.0f)
		);
		auto inlet_vector = std::make_shared<BfSingleLine>(
			this->get_inlet_edge()->get_center(),
			this->get_inlet_edge()->get_center().pos + glm::normalize(inlet_dir)
		);
		inlet_vector->create_vertices();
		this->add_l(inlet_vector);
	}

	{ // OUTLET VECTOR
		glm::vec3 outlet_dir = (
			glm::rotate(glm::mat4(1.0f), glm::radians(90.0f - __info.outlet_angle), __info.up_direction)
			//*glm::vec4(this->get_outlet_edge()->get_center().pos + __info.grow_direction, 1.0f)
			*glm::vec4(__info.grow_direction, 1.0f)
		);
		auto outlet_vector = std::make_shared<BfSingleLine>(
			this->get_outlet_edge()->get_center(),
			this->get_outlet_edge()->get_center().pos + glm::normalize(outlet_dir)
		);
		outlet_vector->create_vertices();
		this->add_l(outlet_vector);
	}
	
	glm::vec3 io_intersection = bfMathFindLinesIntersection(
		*this->get_inlet_vector().get(),
		*this->get_outlet_vector().get(),
		BF_MATH_FIND_LINES_INTERSECTION_ANY
	);

	//{ // INLET IO-LINE
	//	auto inlet_io = std::make_shared<BfSingleLine>(
	//		this->get_inlet_edge()->get_center(),
	//		BfVertex3{
	//			io_intersection,
	//			{1.0f, 1.0f, 1.0f},
	//			this->get_inlet_edge()->get_center().normals
	//		}
	//	);
	//	this->add_l(inlet_io);
	//}

	//{ // OUTLET IO-LINE
	//	auto outlet_io = std::make_shared<BfSingleLine>(
	//		BfVertex3{
	//			io_intersection,
	//			{1.0f, 1.0f, 1.0f},
	//			this->get_outlet_edge()->get_center().normals
	//		},
	//		this->get_outlet_edge()->get_center()
	//	);
	//	this->add_l(outlet_io);
	//}

	{ // AVE_CURVE
		auto ave_curve = std::make_shared<BfBezierCurve>(
			2,
			100,
			std::vector<BfVertex3>{
				this->get_inlet_edge()->get_center(),
				BfVertex3(io_intersection, {1.0f, 1.0f, 1.0f}, this->get_inlet_center().normals),
				this->get_outlet_edge()->get_center()
			}
		);
		ave_curve->set_color({ .0f, 1.0f, 1.0f });
		this->add_l(ave_curve);

		/*auto ave_curve_frame = std::make_shared<BfBezierCurveFrame>(
			this->get_ave_curve(),
			__info.layer_create_info.allocator,
			__info.l_pipeline,
			__info.t_pipeline
		);
		ave_curve_frame->update_buffer();
		this->add(ave_curve_frame);*/
	}

	this->add_l(center_line);


	

	//{ // BACK
	//	glm::vec3 back_intersection = io_intersection + this->get_top_border()->get_direction_from_start() * 0.4f;
	//	// INLET
	//	
	//	auto back_inlet_tangents = this->get_inlet_edge()->get_tangent_vert(back_intersection);
	//	auto back_inlet_io_1 = std::make_shared<BfSingleLine>(back_inlet_tangents[0],
	//														  back_intersection);
	//	

	//	auto back_inlet_io_2 = std::make_shared<BfSingleLine>(back_inlet_tangents[1],
	//														  back_intersection);
	//	


	//	BfVertex3 back_inlet_tangent;
	//	if (bfMathGetDistanceToLine(*get_top_border().get(), back_inlet_tangents[0]) <
	//		bfMathGetDistanceToLine(*get_top_border().get(), back_inlet_tangents[1]))
	//	{
	//		back_inlet_tangent = back_inlet_tangents[0];
	//		this->add_l(back_inlet_io_1);
	//	}
	//	else
	//	{
	//		back_inlet_tangent = back_inlet_tangents[1];
	//		this->add_l(back_inlet_io_2);
	//	}

	//	// OUTLET

	//	auto back_outlet_tangents = this->get_outlet_edge()->get_tangent_vert(back_intersection);
	//	auto back_outlet_io_1 = std::make_shared<BfSingleLine>(back_outlet_tangents[0],
	//														   back_intersection);
	//	
	//	auto back_outlet_io_2 = std::make_shared<BfSingleLine>(back_outlet_tangents[1],
	//														   back_intersection);
	//	


	//	BfVertex3 back_outlet_tangent;
	//	if (bfMathGetDistanceToLine(*get_bot_border().get(), back_outlet_tangents[0]) <
	//		bfMathGetDistanceToLine(*get_bot_border().get(), back_outlet_tangents[1]))
	//	{
	//		back_outlet_tangent = back_outlet_tangents[0];
	//		this->add_l(back_outlet_io_1);
	//	}
	//	else
	//	{
	//		back_outlet_tangent = back_outlet_tangents[1];
	//		this->add_l(back_outlet_io_2);
	//	}

	//	auto back_curve = std::make_shared<BfBezierCurve>(
	//		2,
	//		100,
	//		std::vector<BfVertex3>{
	//			back_inlet_tangent,
	//			BfVertex3(back_intersection, { 1.0f, 1.0f, 1.0f }, this->get_inlet_center().normals),
	//			back_outlet_tangent
	//		}
	//	);
	//	back_curve->set_color({ .0f, 1.0f, 1.0f });
	//	this->add_l(back_curve);
	//}

	//{ // FACE
	//	glm::vec3 face_intersection = io_intersection - this->get_top_border()->get_direction_from_start() * 0.4f;
	//	// INLET

	//	auto face_inlet_tangents = this->get_inlet_edge()->get_tangent_vert(face_intersection);
	//	auto face_inlet_io_1 = std::make_shared<BfSingleLine>(face_inlet_tangents[0],
	//														  face_intersection);


	//	auto face_inlet_io_2 = std::make_shared<BfSingleLine>(face_inlet_tangents[1],
	//														  face_intersection);



	//	BfVertex3 face_inlet_tangent;
	//	if (bfMathGetDistanceToLine(*get_top_border().get(), face_inlet_tangents[0]) >
	//		bfMathGetDistanceToLine(*get_top_border().get(), face_inlet_tangents[1]))
	//	{
	//		face_inlet_tangent = face_inlet_tangents[0];
	//		this->add_l(face_inlet_io_1);
	//	}
	//	else
	//	{
	//		face_inlet_tangent = face_inlet_tangents[1];
	//		this->add_l(face_inlet_io_2);
	//	}

	//	// OUTLET

	//	auto face_outlet_tangents = this->get_outlet_edge()->get_tangent_vert(face_intersection);
	//	auto face_outlet_io_1 = std::make_shared<BfSingleLine>(face_outlet_tangents[0],
	//														   face_intersection);

	//	auto face_outlet_io_2 = std::make_shared<BfSingleLine>(face_outlet_tangents[1],
	//														   face_intersection);



	//	BfVertex3 face_outlet_tangent;
	//	if (bfMathGetDistanceToLine(*get_bot_border().get(), face_outlet_tangents[0]) >
	//		bfMathGetDistanceToLine(*get_bot_border().get(), face_outlet_tangents[1]))
	//	{
	//		face_outlet_tangent = face_outlet_tangents[0];
	//		this->add_l(face_outlet_io_1);
	//	}
	//	else
	//	{
	//		face_outlet_tangent = face_outlet_tangents[1];
	//		this->add_l(face_outlet_io_2);
	//	}

	//	auto face_curve = std::make_shared<BfBezierCurve>(
	//		2,
	//		100,
	//		std::vector<BfVertex3>{
	//		face_inlet_tangent,
	//			BfVertex3(face_intersection, { 1.0f, 1.0f, 1.0f }, this->get_inlet_center().normals),
	//		face_outlet_tangent
	//	}
	//	);
	//	face_curve->set_color({ .0f, 1.0f, 1.0f });
	//	this->add_l(face_curve);
	//}



	/*{
		auto ave_curve = std::make_shared<BfBezierCurveFrame>(this->ge)

	}*/

}


void BfBladeSection::__generate_draw_data() {
	

	for (size_t i = 0; i < this->get_obj_count(); ++i) {
		auto obj = this->get_object_by_index(i);
		//obj->set_color({ 1.0f, 1.0f, 1.0f });
		obj->create_vertices();
		obj->create_indices();
		obj->bind_pipeline(&__info.l_pipeline);
	}
	this->update_buffer();
}

void BfBladeSection::__clear_draw_data() {
	this->del_all();
	
	//for (size_t i = 0; i < this->get_obj_count(); ++i) {
	//	auto obj = this->get_object_by_index(i);
	//	//obj->set_color({ 1.0f, 1.0f, 1.0f });
	//	obj->clear_vetices();
	//	obj->clear_indices();
	//	obj->bind_pipeline(nullptr);
	//}
	//this->clear_buffer();
}