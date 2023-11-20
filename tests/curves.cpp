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
		{1.0f, 1.0f, 0.0f}
	};

	BfBezier bezier3(def_points.size(), def_points);
	
	std::vector<glm::vec3> plot_points3 = bezier3.get_cal_r_c_vertices(50);

	/*for (int i = 0; i < plot_points3.size(); i++ ) {
		std::cout << "(" << plot_points3[i].x << ", " << plot_points3[i].y << "), ";
	}*/

	BfBezier bezier2 = bezier3.get_derivative();

	std::cout << bezier2.get_single_point(0.7).x << ", " << bezier2.get_single_point(0.7).y << "\n";


	std::vector<glm::vec3> plot_points2 = bezier2.get_cal_r_c_vertices(50);

	/*for (int i = 0; i < plot_points2.size(); i++) {
		std::cout << "(" << plot_points2[i].x << ", " << plot_points2[i].y << "), ";
	}*/

	std::vector<glm::vec3> def_points4{
		{0.0, 0.0, 0.0},
		{0.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f},
		{3.0f, 0.0f, 0.0f}
	};

	BfBezier bezier4(4, def_points4);

	std::vector<glm::vec3> plot_points4 = bezier4.get_cal_r_c_vertices(50);

	for (int i = 0; i < plot_points4.size(); i++) {
		std::cout << "(" << plot_points4[i].x << ", " << plot_points4[i].y << "), ";
	}

	std::cout << "\n";

	BfBezier bezier43 = bezier4.get_derivative();

	std::vector<glm::vec3> plot_points43 = bezier43.get_cal_r_c_vertices(50);

	for (int i = 0; i < plot_points43.size(); i++) {
		std::cout << "(" << plot_points43[i].x << ", " << plot_points43[i].y << "), ";
	}

}