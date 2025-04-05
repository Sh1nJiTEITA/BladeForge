#include <catch2/catch_test_macros.hpp>

#include "bfCurves.hpp"
#include "bfMatrix.hpp"

TEST_CASE("CURVES_H", "[single-file]")
{
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

   std::vector<BfVertex3> plot_points2 = bezier2.update_and_get_vertices(50);
   for (int i = 0; i < plot_points2.size(); i++)
   {
      std::cout << "(" << plot_points2[i].pos.x << ", " << plot_points2[i].pos.y
                << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << std::endl;

   BfBezier bezier1 = bezier2.get_derivative();

   std::vector<BfVertex3> plot_points1 = bezier1.update_and_get_vertices(50);
   for (int i = 0; i < plot_points1.size(); i++)
   {
      std::cout << "(" << plot_points1[i].pos.x << ", " << plot_points1[i].pos.y
                << "), ";
   }

   std::vector<glm::vec3> def_points10{{0.0, 0.0, 0.0}, {1.0f, 1.0f, 0.0f}};

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << std::endl;

   int count = 50;
   std::vector<glm::vec3> out_points(count + 1);
   std::vector<glm::vec3> out_points_e(count + 1);
   std::vector<glm::vec3> out_points_der2(count + 1);
   std::vector<glm::vec3> out_points_der2_e(count + 1);
   std::vector<glm::vec3> out_points_normals(count + 1);
   std::vector<glm::vec3> out_points_der2_n(count + 1);
   for (int i = 0; i < count + 1; i++)
   {
      out_points[i] =
          bezier2.get_single_derivative_1((float)1 / (float)count * (float)i);
      out_points_e[i] =
          bezier2.get_single_derivative_1((float)1 / (float)count * (float)i);
      out_points_normals[i] =
          bezier2.get_direction_normal((float)1 / (float)count * (float)i);
      out_points_der2[i] =
          bezier2.get_single_derivative_2((float)1 / (float)count * (float)i);
      out_points_der2_e[i] =
          bezier2.get_single_derivative_2_e((float)1 / (float)count * (float)i);
      BfBezier local = bezier2.get_derivative_n(2);

      out_points_der2_n[i] =
          local.get_single_vertex((float)1 / (float)count * (float)i);
   }

   for (int i = 0; i < out_points.size(); i++)
   {
      std::cout << "(" << out_points[i].x << ", " << out_points[i].y << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << std::endl;

   for (int i = 0; i < out_points_e.size(); i++)
   {
      std::cout << "(" << out_points_e[i].x << ", " << out_points_e[i].y
                << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "normals" << std::endl;

   for (int i = 0; i < out_points_normals.size(); i++)
   {
      std::cout << "(" << out_points_normals[i].x << ", "
                << out_points_normals[i].y << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "der2 :" << std::endl;

   for (int i = 0; i < out_points_der2.size(); i++)
   {
      std::cout << "(" << out_points_der2[i].x << ", " << out_points_der2[i].y
                << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "der2_e :" << std::endl;

   for (int i = 0; i < out_points_der2_e.size(); i++)
   {
      std::cout << "(" << out_points_der2_e[i].x << ", "
                << out_points_der2_e[i].y << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "der2_n :" << std::endl;

   for (int i = 0; i < out_points_der2_n.size(); i++)
   {
      std::cout << "(" << out_points_der2_n[i].x << ", "
                << out_points_der2_n[i].y << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "Eleveting" << std::endl;

   std::vector<glm::vec3> def_points2{
       {0.0f, 0.0f, 0.0f},
       {0.0f, 1.0f, 0.0f},
       {1.0f, 1.0f, 0.0f}
   };
   BfBezier bezier2_(2, def_points2);

   BfBezier elev_bezier2_ = bezier2_.get_elevated_order()
                                .get_elevated_order()
                                .get_elevated_order()
                                .get_elevated_order();
   std::vector<BfVertex3> elev_bezier2_out =
       elev_bezier2_.update_and_get_vertices(30);

   for (int i = 0; i < elev_bezier2_out.size(); i++)
   {
      std::cout << "(" << elev_bezier2_out[i].pos.x << ", "
                << elev_bezier2_out[i].pos.y << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << std::endl;

   BfMatrix mat_1(3, 2, {1, 2, 3, 4, 5, 6});
   BfMatrix mat_2(2, 2, {{1, 2}, {3, 4}});

   BfMatrix mat_1_tr = mat_1.transpose();

   BfMatrix mat_1_ml = BfMatrix::multiply(mat_1, mat_2);

   float det_mat_1 = BfMatrix::det(mat_2);

   BfMatrix mat_5(
       5,
       5,
       {
           /*0,1,0,0,0,
           6,7,8,9,10,
           11,12,1,14,15,
           16,17,18,19,20,
           21,22,23,24,25*/
           6.0,  1.0, 0.0, 4.0, 0,   6.0, 3.0, 8.0, 0.0, 4,   1.0, 0.0, 0.0,
           14.0, 0,   0.0, 1.0, 0.0, 2.0, 0,   0.0, 0.0, 2.0, 0.0, 2,
       }
   );
   float det_mat_5 = BfMatrix::det(mat_5);

   BfMatrix inv_mat5 = BfMatrix::inverse(mat_5);

   BfMatrix mat5__1 = BfMatrix::multiply(mat_5, inv_mat5);
   BfMatrix mat5__2 = BfMatrix::multiply(inv_mat5, mat_5);

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "Components" << std::endl;

   std::vector<glm::vec3> com_bez_def{/*{ 70,250,0.0 },
                                      { 20,110,0.0 },
                                      { 220,60,0.0 },*/
                                      {0.0f, 150.0f, 30.0f},
                                      {100.0f, 250.0f, 50.0f},
                                      {250.0f, 200.0f, 70.0f}
   };

   BfBezier com_bez(2, com_bez_def);
   std::vector<BfVertex3> com_bez_v = com_bez.update_and_get_vertices(30);

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << std::endl;

   for (int i = 0; i < com_bez_v.size(); i++)
   {
      std::cout << "(" << com_bez_v[i].pos.x << ", " << com_bez_v[i].pos.y
                << "), ";
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
       {10, 250, 0.0},
       {20, 110, 0.0},
       {220, 60, 0.0},
       {350, 70, 0.0},
       //{ 400, 80, 0.0}
   };

   BfBezier com_bez2(3, com_bez_def2);
   std::vector<BfVertex3> com_bez_v2 = com_bez2.update_and_get_vertices(30);

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "Single com2" << std::endl;

   for (int i = 0; i < com_bez_v2.size(); i++)
   {
      std::cout << "(" << com_bez_v2[i].pos.x << ", " << com_bez_v2[i].pos.y
                << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "Single com_der 2" << std::endl;

   BfBezier com_bez_der = com_bez2.get_derivative();
   std::vector<BfVertex3> com_bez_der_v =
       com_bez_der.update_and_get_vertices(30);

   for (int i = 0; i < com_bez_der_v.size(); i++)
   {
      std::cout << "(" << com_bez_der_v[i].pos.x << ", "
                << com_bez_der_v[i].pos.y << "), ";
   }

   std::pair<glm::vec3, glm::vec3> fff = com_bez2.get_bbox_t();
   std::pair<glm::vec3, glm::vec3> fff_po;

   fff_po.first.x = com_bez2.get_single_vertex(fff.first.x).x;
   fff_po.first.y = com_bez2.get_single_vertex(fff.first.y).y;
   fff_po.first.z = com_bez2.get_single_vertex(fff.first.z).z;

   fff_po.second.x = com_bez2.get_single_vertex(fff.second.x).x;
   fff_po.second.y = com_bez2.get_single_vertex(fff.second.y).y;
   fff_po.second.z = com_bez2.get_single_vertex(fff.second.z).z;

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "allign2" << std::endl;

   BfBezier all_b = com_bez.get_alligned(BF_PLANE_XZ, BF_AXIS_X);
   std::vector<BfVertex3> com_bez_vert333 = com_bez.update_and_get_vertices(30);
   std::vector<BfVertex3> all_b_vert = all_b.update_and_get_vertices(30);

   for (int i = 0; i < com_bez_vert333.size(); i++)
   {
      std::cout << "(" << com_bez_vert333[i].pos.x << ", "
                << com_bez_vert333[i].pos.y << ", " << com_bez_vert333[i].pos.z
                << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "allign2" << std::endl;

   for (int i = 0; i < all_b_vert.size(); i++)
   {
      std::cout << "(" << all_b_vert[i].pos.x << ", " << all_b_vert[i].pos.y
                << ", " << all_b_vert[i].pos.z << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "INTERSECTIONS" << std::endl;

   BfBezier _bezier2(
       3,
       {{0.2, 1.5, 0.0f}, {1.2, 0.2, 0.0f}, {2.2, 0.95, 0.0f}, {1.4, 2.4, 0.0f}}
   );
   // BfBezier bezier2a = bezier2.get_alligned(BF_PLANE_XY, BF_AXIS_Z);
   BfBezier _bezier2a(
       3,
       {{0.5, 0.35, 0.0f},
        {0.45, 2.35, 0.0f},
        {2.2, 2.35, 0.0f},
        {2.2, 1.35, 0.0f}}
   );

   std::vector<BfVertex3> bezier2_vert = _bezier2.update_and_get_vertices(30);
   std::vector<BfVertex3> bezier2a_vert = _bezier2a.update_and_get_vertices(30);

   std::vector<glm::vec3> inters;
   BfBezier::get_intersections_simple(inters, _bezier2, _bezier2a);

   //{110.0f, 150.0f, 0.0f}, {25.0f, 190.0f, 0.0}, {210.0f, 250.0f, 0.0},
   //{210.0f, 30.0f, 0.0}
   std::vector<glm::vec3> bez3_dp = {

       /*
       (0, 0+150, 0),

       (500, 1500, 0.0),

       (1200, 400+300,0.0),

       (2000, 1500, 0.0),

       (2400, 0+150,0.0)
       */

       {0.0f, 150.0f, 0.0f},

       {500, 1500, 0.0f},

       {1200.0f, 400.0f + 300.0f, 0.0f},

       {2000, 1500, 0.0f},

       {2400.0f, 0 + 150.0f, 0.0f}
   };

   BfBezier _bezier3(4, bez3_dp);
   std::vector<BfVertex3> bezier3_vert = _bezier3.update_and_get_vertices(30);
   float len3 = BfBezier::get_approx_length(_bezier3);

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << std::endl;

   for (int i = 0; i < bezier2_vert.size(); i++)
   {
      std::cout << "(" << bezier2_vert[i].pos.x << ", " << bezier2_vert[i].pos.y
                << ", " << bezier2_vert[i].pos.z << "), ";
   }
   std::cout << std::endl;
   for (int i = 0; i < bezier2a_vert.size(); i++)
   {
      std::cout << "(" << bezier2a_vert[i].pos.x << ", "
                << bezier2a_vert[i].pos.y << ", " << bezier2a_vert[i].pos.z
                << "), ";
   }

   std::vector<glm::vec3> grad = _bezier3.get_length_grad({0, 1, 0, 1, 0});

   for (int i = 0; i < bez3_dp.size(); i++)
   {
      bez3_dp[i] = bez3_dp[i] - 1000.0f * grad[i];
   }

   BfBezier bezier3_lessl(4, bez3_dp);
   std::vector<BfVertex3> bezier3_lessl_vert =
       bezier3_lessl.update_and_get_vertices(30);

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << std::endl;

   for (int i = 0; i < bezier3_vert.size(); i++)
   {
      std::cout << "(" << bezier3_vert[i].pos.x << ", " << bezier3_vert[i].pos.y
                << ", " << bezier3_vert[i].pos.z << "), ";
   }
   std::cout << std::endl;
   for (int i = 0; i < bezier3_lessl_vert.size(); i++)
   {
      std::cout << "(" << bezier3_lessl_vert[i].pos.x << ", "
                << bezier3_lessl_vert[i].pos.y << ", "
                << bezier3_lessl_vert[i].pos.z << "), ";
   }

   std::cout << std::endl;
   std::cout << std::endl;
   std::cout << "Handles" << std::endl;

   std::vector<BfVertex3> handles_vertices = _bezier3.get_handles_vertices(100);
   std::vector<uint16_t> handles_indices = _bezier3.get_handles_indices();
   std::vector<glm::vec3> handles_centers = _bezier3.get_vertices();

   for (int i = 0; i < handles_vertices.size(); i++)
   {
      std::cout << "(" << handles_vertices[i].pos.x << ", "
                << handles_vertices[i].pos.y << ", "
                << handles_vertices[i].pos.z << "), ";
   }
   std::cout << std::endl;
   for (int i = 0; i < handles_centers.size(); i++)
   {
      std::cout << "(" << handles_centers[i].x << ", " << handles_centers[i].y
                << ", " << handles_centers[i].z << "), ";
   }
}
