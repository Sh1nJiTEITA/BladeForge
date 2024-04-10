#ifndef BF_BLADE_SECTION_H
#define BF_BLADE_SECTION_H

#include <memory>

#include "bfCurves3.h"
#include "bfDrawObject.h"
#include "bfLayerKiller.h"


struct BfBladeSectionCreateInfo {
	BfDrawLayerCreateInfo layer_create_info;

	float width;

	float install_angle;

	float inlet_angle;
	float outlet_angle;

	float inlet_surface_angle;
	float outlet_surface_angle;

	float inlet_radius;
	float outlet_radius;
	
	float border_length;

	bool is_triangulate = false;
	bool is_center = true;

	// Other
	glm::vec3 start_vertex{ 0.0f, 0.0f, 0.0f };
	glm::vec3 grow_direction{ 0.0f, 1.0f, 0.0f };
	glm::vec3 up_direction{ 0.0f, 0.0f, 1.0f };	

	VkPipeline l_pipeline;
	VkPipeline t_pipeline;
};

bool bfCheckBladeSectionCreateInfoEquality(
	const BfBladeSectionCreateInfo& i_1,
	const BfBladeSectionCreateInfo& i_2
);


#define BF_BLADESECTION_AVE_COLOR glm::vec3(1.0, 0.5, 0.0)
#define BF_BLADESECTION_BACK_COLOR glm::vec3(0.5, 0.0, 1.0)
#define BF_BLADESECTION_FACE_COLOR glm::vec3(0.0, 0.8, 0.8)


class BfBladeSection : public BfDrawLayer {
protected:
	BfBladeSectionCreateInfo __info;

	BfVertex3 __chord_inlet_center;
	BfVertex3 __chord_outlet_center;


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

	// 
	std::shared_ptr<BfSingleLine> get_top_border();
	std::shared_ptr<BfSingleLine> get_bot_border();
	std::shared_ptr<BfSingleLine> get_chord();
	std::shared_ptr<BfCircle> get_inlet_edge();
	std::shared_ptr<BfCircle> get_outlet_edge();
	std::shared_ptr<BfSingleLine> get_inlet_vector();
	std::shared_ptr<BfSingleLine> get_outlet_vector();

	std::shared_ptr<BfBezierCurve> get_ave_curve();
	std::shared_ptr<BfBezierCurve> get_back_curve();
	std::shared_ptr<BfBezierCurve> get_face_curve();

	std::shared_ptr<BfArc> get_inlet_edge_a();
	std::shared_ptr<BfArc> get_outlet_edge_a();

	std::shared_ptr<BfBezierCurveFrame> get_ave_curve_frame();
	std::shared_ptr<BfBezierCurveFrame> get_back_curve_frame();
	std::shared_ptr<BfBezierCurveFrame> get_face_curve_frame();

	std::vector<std::shared_ptr<BfTriangle>> get_triangulated_shape();
	BfVertex3 get_center();

	virtual void remake(const BfBladeSectionCreateInfo& info);
	virtual void del_all() override;

	std::vector<BfVertex3> get_contour();

private:
	void __generate_outer_elements();
	void __generate_average_line_geometry();
	void __generate_surface_elements();
	void __generate_obj_frames();

	void __generate_center_circle();
	void __generate_triangular_shape();

	void __generate_draw_data();

	void __clear_draw_data();
};









#endif