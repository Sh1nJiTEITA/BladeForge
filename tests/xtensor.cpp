#define CATCH_CONFIG_MAIN

#include "catch2/catch_all.hpp"

// #include <xtensor/containers/xarray.hpp>

// #include <xtensor/io/xio.hpp>

// #include <xtensor/xlinalg.hpp> // optional: for linalg tests (solve, dot,
// etc.)
#include <xtensor-blas/xblas.hpp>
#include <xtensor-blas/xlinalg.hpp>
#include <xtensor.hpp>

TEST_CASE("Basic xtensor operations", "[xtensor]")
{
   xt::xarray< double > a = {1.0, 2.0, 3.0};
   xt::xarray< double > b = {4.0, 5.0, 6.0};

   SECTION("Element-wise addition")
   {
      auto result = a + b;
      REQUIRE(result(0) == Catch::Approx(5.0));
      REQUIRE(result(1) == Catch::Approx(7.0));
      REQUIRE(result(2) == Catch::Approx(9.0));
   }

   SECTION("Broadcasting addition with scalar")
   {
      auto result = a + 1.0;
      REQUIRE(result(0) == Catch::Approx(2.0));
      REQUIRE(result(1) == Catch::Approx(3.0));
      REQUIRE(result(2) == Catch::Approx(4.0));
   }

   SECTION("Matrix multiplication")
   {
      xt::xarray< double > A = {{1.0, 2.0}, {3.0, 4.0}};
      xt::xarray< double > B = {{2.0, 0.0}, {1.0, 2.0}};

      auto result = xt::linalg::dot(A, B);
      REQUIRE(result(0, 0) == Catch::Approx(4.0));  // 1*2 + 2*1
      REQUIRE(result(0, 1) == Catch::Approx(4.0));  // 1*0 + 2*2
      REQUIRE(result(1, 0) == Catch::Approx(10.0)); // 3*2 + 4*1
      REQUIRE(result(1, 1) == Catch::Approx(8.0));  // 3*0 + 4*2
   }

   SECTION("Norm and power")
   {
      auto squared = xt::pow(a, 2);
      REQUIRE(squared(0) == Catch::Approx(1.0));
      REQUIRE(squared(1) == Catch::Approx(4.0));
      REQUIRE(squared(2) == Catch::Approx(9.0));
   }
}
