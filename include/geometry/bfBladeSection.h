#ifndef BF_BLADE_SECTION_H
#define BF_BLADE_SECTION_H

#include <memory>

#include "bfCurves3.h"
#include "bfDrawObject.h"


struct BfBladeSectionCreateInfo {
	BfDrawLayerCreateInfo layer_create_info;

	float width;

	float install_angle;

	float inlet_angle;
	float outlet_angle;

	float inlet_radius;
	float outlet_radius;
	
	float border_length;
	// Other
	glm::vec3 start_vertex{ 0.0f, 0.0f, 0.0f };
	glm::vec3 grow_direction{ 0.0f, 1.0f, 0.0f };
	glm::vec3 up_direction{ 0.0f, 0.0f, 1.0f };	

	VkPipeline l_pipeline;
	VkPipeline t_pipeline;
};

class BfBladeSection : public BfDrawLayer {
protected:
	BfBladeSectionCreateInfo __info;

	BfVertex3 __chord_inlet_center;
	BfVertex3 __chord_outlet_center;

	std::vector<std::shared_ptr<BfDrawLayer>> inside_layers;

public:

	BfBladeSection(const BfBladeSectionCreateInfo& info);

	/*
		__objects[..]
		1. Top border
		2. Bot border
		3. Chord
		4. Inlet edge circle
		5. Outlet edge circle
		4. Inlet vector
		5. Outlet vector
		6. Inlet io-line
		7. Outlet io_line
	*/

	const BfVertex3& get_inlet_center() const noexcept;
	const BfVertex3& get_outlet_center() const noexcept;

	std::shared_ptr<BfSingleLine> get_top_border();
	std::shared_ptr<BfSingleLine> get_bot_border();
	std::shared_ptr<BfSingleLine> get_chord();
	std::shared_ptr<BfCircle> get_inlet_edge();
	std::shared_ptr<BfCircle> get_outlet_edge();
	std::shared_ptr<BfSingleLine> get_inlet_vector();
	std::shared_ptr<BfSingleLine> get_outlet_vector();
	std::shared_ptr<BfSingleLine> get_inlet_io();
	std::shared_ptr<BfSingleLine> get_outlet_io();

	std::shared_ptr<BfBezierCurve> get_ave_curve();
	std::shared_ptr<BfBezierCurveFrame> get_ave_curve_frame();

	std::shared_ptr<BfSingleLine> get_back_inlet_tangent();
	std::shared_ptr<BfSingleLine> get_back_outlet_tangent();
	std::shared_ptr<BfBezierCurve> get_back_curve();
	std::shared_ptr<BfBezierCurveFrame> get_back_curve_frame();

	std::shared_ptr<BfSingleLine> get_face_inlet_tangent();
	std::shared_ptr<BfSingleLine> get_face_outlet_tangent();
	std::shared_ptr<BfBezierCurve> get_face_curve();
	std::shared_ptr<BfBezierCurveFrame> get_face_curve_frame();



private:
	void __generate_outer_elements();
	void __generate_blade_geometry();

	void __generate_draw_data();
};









#endif