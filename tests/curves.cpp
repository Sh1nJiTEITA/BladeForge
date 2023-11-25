#include <catch2/catch_test_macros.hpp>

#include "bfCurves.hpp"



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