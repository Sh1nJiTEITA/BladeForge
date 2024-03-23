#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
//#include <catch2/catch_test_macros.hpp>

//#include <glm/glm.hpp>
//#include <bfVertex2.hpp>

#include "bfMatrix2.h"
#include "bfCurves3.h"


std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}


TEST_CASE("CURVES_H", "[single-file]") {
	glm::vec3 glm_vec{ 1.0f };
	BfVertex3 bf_v{glm_vec};
	std::vector<BfVertex3> stl_bf_vec(2);


	BfVector v_from_glm(glm_vec);
	BfVector v_from_bf(bf_v);
	BfVector v_from_stl_bf_vec(stl_bf_vec);

	BfVector v_from_init{
		1.0f, 2.0f, 3.0f
	};


	// Vector
	REQUIRE(v_from_init == (v_from_glm + glm::vec3(0.0f, 1.0f, 2.0f)));
	REQUIRE(v_from_init == BfVector{ 1.0,2.0f,3.0f });
	REQUIRE(BfVector{ 1.0f,2.0f,3.0f } * 2 == BfVector{ 2.0f, 4.0f, 6.0f });
	REQUIRE(BfVector{ 1.0f,2.0f,3.0f } * glm::vec3(2.0f, 3.0f, 4.0f) == BfVector{2.0f, 6.0f, 12.0f});
	REQUIRE(BfVector{ 1.0f,2.0f,3.0f } / 10 == BfVector{ 0.1f, 0.2f, 0.3f });
	REQUIRE(BfVector{ 1.0f,2.0f,3.0f } * BfVector{ 1.0f,2.0f,3.0f } == BfVector{ 1.0f, 4.0f, 9.0f });

	// Matrix
	BfMatrix m123{ {0.0f, 0.0f, 0.0f},
				  {0.0f, 0.0f, 0.0f},
				  {0.0f, 0.0f, 0.0f} };
	REQUIRE(
		BfMatrix{ {0.0f, 0.0f, 0.0f},
				  {0.0f, 0.0f, 0.0f},
				  {0.0f, 0.0f, 0.0f} }
		==
		BfMatrix{ 3,3, 0.0f }
	);

	BfMatrix mtx_null{ 3,3,0.0f };

	REQUIRE(
		(mtx_null[0] + 3.0f) ==
		BfMatrix{ {3.0f, 3.0f, 3.0f},
				  {0.0f, 0.0f, 0.0f},
				  {0.0f, 0.0f, 0.0f} }[0]
	);
	
	REQUIRE(
		BfMatrix{3,3,1.0f} ==
		BfMatrix{ {1.0f, 1.0f, 1.0f},
				  {1.0f, 1.0f, 1.0f},
				  {1.0f, 1.0f, 1.0f} }
	);

	REQUIRE(
		BfMatrix{ 3,3,1.0f, BF_MATRIX_FILL_TYPE_DIAGONAL } ==
		BfMatrix{ {1.0f, 0.0f, 0.0f},
				  {0.0f, 1.0f, 0.0f},
				  {0.0f, 0.0f, 1.0f} }
	);

	REQUIRE(
		BfMatrix{ 3,3,1.0f, BF_MATRIX_FILL_TYPE_DIAGONAL_RIGHT } ==
		BfMatrix{ {0.0f, 0.0f, 1.0f},
				  {0.0f, 1.0f, 0.0f},
				  {1.0f, 0.0f, 0.0f} }
	);

	BfMatrix mtx_tri{
		{0, 1, 2},
		{3, 4, 5},
		{1, 1, 3}
	};
	REQUIRE(mtx_tri.det_gauss() == -6);
	REQUIRE(mtx_tri.det() == -6);

	BfMatrix mtx_tri_2{
		{ 1.5, 2.3, 0.7 },
		{ 4.2, 0.8, 2.1 },
		{ 3.6, 1.2, 5.0 }
	};
	REQUIRE(CHECK_FLOAT_EQUALITY(-27.18f, mtx_tri_2.det_gauss()));
	REQUIRE(CHECK_FLOAT_EQUALITY(-27.18f, mtx_tri_2.det()));
		 
	BfMatrix mtx_tri_3{
		{ 3.8, 1.2, 0.5 },
		{ 0.3, 2.9, 4.6 },
		{ 1.7, 4.2, 0.8 }
	};
	REQUIRE(CHECK_FLOAT_EQUALITY(-57.339, mtx_tri_3.det_gauss()));
	REQUIRE(CHECK_FLOAT_EQUALITY(-57.339, mtx_tri_3.det()));

	/*BfMatrix mtx_tri_{
		{0, 1, 2},
		{3, 4, 5},
		{1, 1, 3}
	};
	REQUIRE(mtx_tri.det() == -6);

	BfMatrix mtx_tri_2_{
		{ 1.5, 2.3, 0.7 },
		{ 4.2, 0.8, 2.1 },
		{ 3.6, 1.2, 5.0 }
	};
	REQUIRE(CHECK_FLOAT_EQUALITY(-27.18f, mtx_tri_2_.det()));

	BfMatrix mtx_tri_3_{
		{ 3.8, 1.2, 0.5 },
		{ 0.3, 2.9, 4.6 },
		{ 1.7, 4.2, 0.8 }
	};
	REQUIRE(CHECK_FLOAT_EQUALITY(-57.339, mtx_tri_3_.det()));
	*/
	BfMatrix mtx_minor_1_1{
		{4, 5},
		{1, 3}
	};

	REQUIRE(mtx_minor_1_1 == mtx_tri.get_minor(0, 0));

	BfMatrix mtx_cofactor_1{
		{-3,6, - 3},
		{6,-12, 6},
		{-3,6,-3}
	};
	BfMatrix mtx_cofactor_1_{
		{1,2,3},
		{4,5,6},
		{7,8,9}
	};
	REQUIRE(
		mtx_cofactor_1 == mtx_cofactor_1_.get_cofactor()
	);
	
	mtx_cofactor_1_ = mtx_cofactor_1_.get_cofactor();
	
	REQUIRE(
		mtx_cofactor_1 == mtx_cofactor_1_
	);

	BfMatrix mtx_multiply_left{
		{1, 2, 3},
		{4, 5, 6}
	};
	BfMatrix mtx_multiply_right{
		{7,8},
		{9,1},
		{2,3}
	};
	BfMatrix mtx_multiply_result{
		{31, 19},
		{85, 55}
	};
	REQUIRE(mtx_multiply_left >> mtx_multiply_right == mtx_multiply_result);

	BfMatrix mtx_multiply_left_1{
		{3},
		{2},
		{0},
		{1}
	};
	BfMatrix mtx_multiply_right_1{
		{-1,1,0,2}
	};
	BfMatrix mtx_multiply_result_1{
		{-3, 3, 0,6 },
		{-2,2,0,4},
		{0,0,0,0},
		{-1,1,0,2}
	};
	REQUIRE(mtx_multiply_left_1 >> mtx_multiply_right_1 == mtx_multiply_result_1);

	// GAUSS LINEAR
	/*BfMatrix mtx_A = {
		{1,2,3},
		{1,1,5},
		{2,-1,2}
	};
	BfVector v_B = {
		1,-1,6
	};
	BfVector gauss_sol = solve_linear_gauss(mtx_A.get_triangular_form(), v_B);

	REQUIRE(gauss_sol == BfVector{ 4, 0, -1 });*/

	BfMatrix mtx_A = {
		{8,7,3},
		{-7,-4,-4},
		{-6,5,-4}
	};
	BfMatrix v_B = {
		{18},
		{-11},
		{-15}
	};
	BfMatrix mtx_x{
		{ 5 }, {-1}, {-5}
	};
	REQUIRE(mtx_A.get_inversed() >> v_B == mtx_x);
	REQUIRE(solve_linear_mtx(mtx_A, v_B) == mtx_x.to_bfvec());

	//BfVector gauss_sol = solve_linear_gauss(mtx_A, v_B);
	//BfMatrix mtx_sol = mtx_A.get_inversed() >> v_B;
	//REQUIRE(gauss_sol == BfVector{ 5, -1, -5 });

	BfMatrix transpose{
		{1,2,3},
		{4,5,6},
		{7,8,9}
	};
	BfMatrix transpose_{
		{1,4,7},
		{2,5,8},
		{3,6,9}
	};
	REQUIRE(transpose.get_transposed() == transpose_);


	REQUIRE(
		mtx_A >> mtx_A.get_inversed() == BfMatrix(3, 3, 1.0f, BF_MATRIX_FILL_TYPE_DIAGONAL)
	);


	BfVec2 bf_vec_glm_1{ glm::vec2(1.0f, 2.0f) };
	REQUIRE(bf_vec_glm_1.x == 1.0f);
	REQUIRE(bf_vec_glm_1.y == 2.0f);

	BfVec2 bf_vec_glm_3{ glm::vec3(1.0f, 2.0f, 3.0f) };
	REQUIRE(bf_vec_glm_3.x == 1.0f);
	REQUIRE(bf_vec_glm_3.y == 2.0f);

	BfVec2 bf_vec_glm_2{ glm::vec4(1.0f, 2.0f, 3.0f, 4.0f) };
	REQUIRE(bf_vec_glm_2.x == 1.0f);
	REQUIRE(bf_vec_glm_2.y == 2.0f);

	BfVertex3 vert{ {1.0,2.0,3.0f} };
	BfVec2 bf_vec_bf_1{ vert };
	REQUIRE(bf_vec_bf_1.x == 1.0f);
	REQUIRE(bf_vec_bf_1.y == 2.0f);

	BfLineProp line_prop_1{ {-10, 13},{3.2, -44} };
	REQUIRE(CHECK_FLOAT_EQUALITY(line_prop_1.get_b(), (-332.0f / 11.0f)));
	REQUIRE(CHECK_FLOAT_EQUALITY(line_prop_1.get_k(), (-95.0f / 22.0f)));

	BfLineProp line_prop_1_per = line_prop_1.get_perpendicular({ 1.0f, 2.0f });
	REQUIRE(CHECK_FLOAT_EQUALITY(line_prop_1_per.get_b(), 1.76842105));
	REQUIRE(CHECK_FLOAT_EQUALITY(line_prop_1_per.get_k(), 0.23157894736842));
}


TEST_CASE("BfSingleLine::get_k;get_b;get_single_proj", "[get_k][get_b][get_single_proj]") {
	BfSingleLine line
	(
		BfVertex3(glm::vec3(1.0f, 2.0f, 3.0f)),
		BfVertex3(glm::vec3(4.0f, 5.0f, 6.0f))
	);
	SECTION("Test get_k function") {
		REQUIRE(line.get_k(BF_SINGLE_LINE_PROJ_XY) == 1.0f);
		REQUIRE(line.get_k(BF_SINGLE_LINE_PROJ_YX) == 1.0f);

		REQUIRE(line.get_k(BF_SINGLE_LINE_PROJ_XZ) == 1.0f);
		REQUIRE(line.get_k(BF_SINGLE_LINE_PROJ_ZX) == 1.0f);

		REQUIRE(line.get_k(BF_SINGLE_LINE_PROJ_YZ) == 1.0f);
		REQUIRE(line.get_k(BF_SINGLE_LINE_PROJ_ZY) == 1.0f);
	}

	SECTION("Test get_b function") {
		REQUIRE(line.get_b(BF_SINGLE_LINE_PROJ_XY) == 1.0f);
		REQUIRE(line.get_b(BF_SINGLE_LINE_PROJ_YX) == -1.0f);
		
		REQUIRE(line.get_b(BF_SINGLE_LINE_PROJ_XZ) == 2.0f);
		REQUIRE(line.get_b(BF_SINGLE_LINE_PROJ_ZX) == -2.0f);

		REQUIRE(line.get_b(BF_SINGLE_LINE_PROJ_YZ) == 1.0f);
		REQUIRE(line.get_b(BF_SINGLE_LINE_PROJ_ZY) == -1.0f);
	}
}

TEST_CASE("BfCircle") {
	SECTION("Test BfCircle constructor") {
		
		BfVertex3 v1({ -6, 3, 0 });
		BfVertex3 v2({ -3, 2, 0 });
		BfVertex3 v3({  0, 3, 0 });
		
		BfCircle circle(50, v1, v2, v3);
	}
}


TEST_CASE("BfMath", "[BfMath]") {

	SECTION("Test Are Vertices in plain") {
		REQUIRE(bfMathIsVerticesInPlain(
			{ 
				{0.0f,0.0f,0.0f}, 
				{1.0f, 3.0f, 0.0f}, 
				{33.0, 3123.0f, 0.0f}, 
				{313.0f, 3123.0f, 0.0f}
			}
		));
	}
	
	SECTION("Test line_intersection") {
		// https://mathter.pro/angem/5_5_5_peresekayuschiesya_pryamye_v_prostranstve.html
		BfSingleLine l1({ 3.0f, -3.0f, 2.0f }, { 3.0f - 1.0f, -3.0f + 1.0f, 2.0f + 2.0f });
		BfSingleLine l2({ -1.0f, 4.0f, -26.0f }, { -1.0f + 3.0f, -4.0f + 4.0f, -26.0f + 6.0f});

		REQUIRE(bfMathIsSingleLinesInPlain(l1, l2));

		glm::vec3 inter = bfMathFindLinesIntersection(l1, l2, BF_MATH_FIND_LINES_INTERSECTION_ANY);
		
		REQUIRE(inter.x == 8.0f);
		REQUIRE(inter.y == -8.0f);
		REQUIRE(inter.z == -8.0f);
	}
}