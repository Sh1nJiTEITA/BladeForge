#include <catch2/catch_test_macros.hpp>

#include "bfCurves.hpp"
#include "bfMatrix.hpp"


TEST_CASE("CURVES_H", "[single-file]") {

	// Factorial
	REQUIRE(bfGetFactorial(-10) == 1);
	REQUIRE(bfGetFactorial(1) == 1);
	REQUIRE(bfGetFactorial(10) == 3628800);
	
	// Binomial coefficient
	REQUIRE(bfGetBinomialCoefficient(0, 0) == 1);
	REQUIRE(bfGetBinomialCoefficient(1, 0) == 1);
	REQUIRE(bfGetBinomialCoefficient(2, 0) == 1);
	REQUIRE(bfGetBinomialCoefficient(2, 1) == 2);
	REQUIRE(bfGetBinomialCoefficient(3, 3) == 1);
	REQUIRE(bfGetBinomialCoefficient(3, 0) == 1);
	REQUIRE(bfGetBinomialCoefficient(4, 1) == 4);
	REQUIRE(bfGetBinomialCoefficient(4, 3) == 4);
	REQUIRE(bfGetBinomialCoefficient(4, 2) == 6);
	REQUIRE(bfGetBinomialCoefficient(5, 2) == 10);
	REQUIRE(bfGetBinomialCoefficient(5, 3) == 10);
	
	std::vector<glm::vec3> def_points{
		{0.0, 0.0, 0.0}, 
		{0.0f, 1.0f, 0.0f}, 
		{1.0f, 1.0f, 0.0f},
		//{2.0f, 3.0f, 0.0f},
		//{2.0f, 2.6f, 0.0f}
	};

	
	BfBezier bezier2(2, def_points);

	std::vector<glm::vec3> plot_points2 = bezier2.update_and_get_vertices(50);
	for (int i = 0; i < plot_points2.size(); i++) {
		std::cout << "(" << plot_points2[i].x << ", " << plot_points2[i].y << "), ";
	}

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;


	BfBezier bezier1 = bezier2.get_derivative();

	std::vector<glm::vec3> plot_points1 = bezier1.update_and_get_vertices(50);
	for (int i = 0; i < plot_points1.size(); i++) {
		std::cout << "(" << plot_points1[i].x << ", " << plot_points1[i].y << "), ";
	}

	std::vector<glm::vec3> def_points10{
		{0.0, 0.0, 0.0},
		{1.0f, 1.0f, 0.0f}
	};

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	int count = 50;
	std::vector<glm::vec3> out_points(count+1);
	std::vector<glm::vec3> out_points_e(count+1);
	std::vector<glm::vec3> out_points_der2(count + 1);
	std::vector<glm::vec3> out_points_der2_e(count + 1);
	std::vector<glm::vec3> out_points_normals(count+1);
	std::vector<glm::vec3> out_points_der2_n(count + 1);
	for (int i = 0; i < count+1; i++) {
		out_points[i] = bezier2.get_single_derivative_1((float)1 / (float)count * (float)i);
		out_points_e[i] = bezier2.get_single_derivative_1((float)1 / (float)count * (float)i);
		out_points_normals[i] = bezier2.get_direction_normal((float)1 / (float)count * (float)i);
		out_points_der2[i] = bezier2.get_single_derivative_2((float)1 / (float)count * (float)i);
		out_points_der2_e[i] = bezier2.get_single_derivative_2_e((float)1 / (float)count * (float)i);
		BfBezier local = bezier2.get_derivative_n(2);

		out_points_der2_n[i] = local.get_single_vertex((float)1 / (float)count * (float)i);
	}

	for (int i = 0; i < out_points.size(); i++) {
		std::cout << "(" << out_points[i].x << ", " << out_points[i].y << "), ";
	}


	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	for (int i = 0; i < out_points_e.size(); i++) {
		std::cout << "(" << out_points_e[i].x << ", " << out_points_e[i].y << "), ";
	}

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "normals" << std::endl;

	for (int i = 0; i < out_points_normals.size(); i++) {
		std::cout << "(" << out_points_normals[i].x << ", " << out_points_normals[i].y << "), ";
	}

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "der2 :" << std::endl;

	for (int i = 0; i < out_points_der2.size(); i++) {
		std::cout << "(" << out_points_der2[i].x << ", " << out_points_der2[i].y << "), ";
	}

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "der2_e :" << std::endl;

	for (int i = 0; i < out_points_der2_e.size(); i++) {
		std::cout << "(" << out_points_der2_e[i].x << ", " << out_points_der2_e[i].y << "), ";
	}

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "der2_n :" << std::endl;

	for (int i = 0; i < out_points_der2_n.size(); i++) {
		std::cout << "(" << out_points_der2_n[i].x << ", " << out_points_der2_n[i].y << "), ";
	}

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "Eleveting" << std::endl;

	std::vector<glm::vec3> def_points2{
		{0.0f, 0.0f,0.0f},
		{0.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f}
	};
	BfBezier bezier2_(2, def_points2);

	BfBezier elev_bezier2_ = bezier2_.get_elevated_order().get_elevated_order().get_elevated_order().get_elevated_order();
	std::vector<glm::vec3> elev_bezier2_out = elev_bezier2_.update_and_get_vertices(30);

	for (int i = 0; i < elev_bezier2_out.size(); i++) {
		std::cout << "(" << elev_bezier2_out[i].x << ", " << elev_bezier2_out[i].y << "), ";
	}


	std::cout << std::endl; 
	std::cout << std::endl;
	std::cout << std::endl;

	BfMatrix mat_1(3, 2, { 1,2,3,4,5,6 });
	BfMatrix mat_2(2, 2, { {1,2},{3,4} });

	BfMatrix mat_1_tr = mat_1.transpose();

	BfMatrix mat_1_ml = BfMatrix::multiply(mat_1, mat_2);

	float det_mat_1 = BfMatrix::det(mat_2);

	BfMatrix mat_5(5, 5, {
		/*0,1,0,0,0,
		6,7,8,9,10,
		11,12,1,14,15,
		16,17,18,19,20,
		21,22,23,24,25*/
		6.0,	1.0,	0.0,	4.0, 0 ,
		6.0,	3.0,	8.0,	0.0, 4 ,
		1.0,	0.0,	0.0,	14.0,0 ,
		0.0,	1.0,	0.0,	2.0, 0 ,
		0.0,	0.0,	2.0,	0.0, 2 ,
		});
	float det_mat_5 = BfMatrix::det(mat_5);

	BfMatrix inv_mat5 = BfMatrix::inverse(mat_5);

	BfMatrix mat5__1 = BfMatrix::multiply(mat_5, inv_mat5);
	BfMatrix mat5__2 = BfMatrix::multiply(inv_mat5, mat_5);


	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "Components" << std::endl;

	std::vector<glm::vec3> com_bez_def{
		{ 70,250,0.0 },
		{ 20,110,0.0 },
		{ 220,60,0.0 },
	};

	BfBezier com_bez(2, com_bez_def);
	std::vector<glm::vec3> com_bez_v = com_bez.update_and_get_vertices(30);

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	for (int i = 0; i < com_bez_v.size(); i++) {
		std::cout << "(" << com_bez_v[i].x << ", " << com_bez_v[i].y << "), ";
	}
	
	auto print_vec3 = [](glm::vec3 v) {
		std::cout << "(" << v.x << ", " << v.y << ", " << v.z << "), ";
	};
	auto print_vec2 = [](glm::vec3 v) {
		std::cout << "(" << v.x << ", " << v.y << "), ";
	};

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "Single com" << std::endl;

	std::vector<glm::vec3> com_bez_def2{
		{ 70,250,0.0 },
		{ 20,110,0.0 },
		{ 220,60,0.0 },
		{ 350,70, 0.0},
		{ 400, 80, 0.0}
	};

	BfBezier com_bez2(4, com_bez_def2);
	std::vector<glm::vec3> fff = com_bez2.get_extremites_t();
	std::vector<glm::vec3> com_bez_v2 = com_bez2.update_and_get_vertices(30);

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "Single com2" << std::endl;

	for (int i = 0; i < com_bez_v2.size(); i++) {
		std::cout << "(" << com_bez_v2[i].x << ", " << com_bez_v2[i].y << "), ";
	}

	/*std::vector<float> extr = com_bez.get_extremites_t();

	print_vec2(com_bez.get_single_vertex(extr[0]));
	print_vec2(com_bez.get_single_vertex(extr[1]));
	print_vec2(com_bez.get_single_vertex(extr[2]));*/


	//BfBezier bezier10(1, def_points10);
	//
	//std::vector<glm::vec3> plot_points10 = bezier10.update_and_get_vertices(50);
	//for (int i = 0; i < plot_points10.size(); i++) {
	//	std::cout << "(" << plot_points10[i].x << ", " << plot_points10[i].y << "), ";
	//}


	/*std::vector<glm::vec3> plot_points3 = bezier3.update_and_get_vertices(50);

	for (int i = 0; i < plot_points3.size(); i++ ) {
		std::cout << "(" << plot_points3[i].x << ", " << plot_points3[i].y << "), ";
	}

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	BfBezier bezier2 = bezier3.get_derivative();

	std::vector<glm::vec3> plot_points2 = bezier2.update_and_get_vertices(50);

	for (int i = 0; i < plot_points2.size(); i++) {
		std::cout << "(" << plot_points2[i].x << ", " << plot_points2[i].y << "), ";
	}
	
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	std::vector<glm::vec3> def_points4{
		{0.0, 0.0, 0.0},
		{0.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f},
		{3.0f, 0.0f, 0.0f}
	};

	BfBezier bezier4(3, def_points4);

	std::vector<glm::vec3> plot_points4 = bezier4.update_and_get_vertices(50);

	for (int i = 0; i < plot_points4.size(); i++) {
		std::cout << "(" << plot_points4[i].x << ", " << plot_points4[i].y << "), ";
	}

	std::cout << "\n";

	BfBezier bezier43 = bezier4.get_derivative();

	std::vector<glm::vec3> plot_points43 = bezier43.update_and_get_vertices(50);

	for (int i = 0; i < plot_points43.size(); i++) {
		std::cout << "(" << plot_points43[i].x << ", " << plot_points43[i].y << "), ";
	}*/

}