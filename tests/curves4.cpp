#include <catch2/catch_all.hpp>

// #include <catch2/catch_session.hpp>

#define VMA_IMPLEMENTATION
#include "bfCurves4.h"

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
TEST_CASE("Coplanar points - XY plane (glm::vec3)", "[isVerticesInPlain]")
{
   REQUIRE(obj::curves::math::isVerticesInPlain(
       glm::vec3(0, 0, 0),
       glm::vec3(1, 0, 0),
       glm::vec3(0, 1, 0),
       glm::vec3(1, 1, 0),
       glm::vec3(0.5, 0.5, 0)
   ));
}

TEST_CASE(
    "Coplanar points - diagonal line (same plane) (glm::vec3)",
    "[isVerticesInPlain]"
)
{
   REQUIRE(obj::curves::math::isVerticesInPlain(
       glm::vec3(0, 0, 0),
       glm::vec3(1, 1, 1),
       glm::vec3(2, 2, 2),
       glm::vec3(3, 3, 3)
   ));
}

TEST_CASE(
    "Non-coplanar point breaks the condition (glm::vec3)", "[isVerticesInPlain]"
)
{
   REQUIRE_FALSE(obj::curves::math::isVerticesInPlain(
       glm::vec3(0, 0, 0),
       glm::vec3(1, 0, 0),
       glm::vec3(0, 1, 0),
       glm::vec3(0.5, 0.5, 0.01)  // Slightly off the plane
   ));
}

TEST_CASE(
    "Identical points (degenerate but coplanar) (glm::vec3)",
    "[isVerticesInPlain]"
)
{
   REQUIRE(obj::curves::math::isVerticesInPlain(
       glm::vec3(1, 1, 1),
       glm::vec3(1, 1, 1),
       glm::vec3(1, 1, 1),
       glm::vec3(1, 1, 1)
   ));
}

TEST_CASE("XZ plane check (glm::vec3)", "[isVerticesInPlain]")
{
   REQUIRE(obj::curves::math::isVerticesInPlain(
       glm::vec3(0, 0, 0),
       glm::vec3(1, 0, 0),
       glm::vec3(0, 0, 1),
       glm::vec3(1, 0, 1),
       glm::vec3(0.5, 0, 0.5)
   ));
}

TEST_CASE("Non-coplanar random point (glm::vec3)", "[isVerticesInPlain]")
{
   REQUIRE_FALSE(obj::curves::math::isVerticesInPlain(
       glm::vec3(0, 0, 0),
       glm::vec3(1, 1, 0),
       glm::vec3(1, 0, 1),
       glm::vec3(0.3, 0.2, 0.8)
   ));
}

TEST_CASE("Coplanar points - XY plane", "[isVerticesInPlain]")
{
   REQUIRE(obj::curves::math::isVerticesInPlain(
       BfVertex3(0, 0, 0),
       BfVertex3(1, 0, 0),
       BfVertex3(0, 1, 0),
       BfVertex3(1, 1, 0),
       BfVertex3(0.5, 0.5, 0)
   ));
}

TEST_CASE("Coplanar points - diagonal line (same plane)", "[isVerticesInPlain]")
{
   REQUIRE(obj::curves::math::isVerticesInPlain(
       BfVertex3(0, 0, 0),
       BfVertex3(1, 1, 1),
       BfVertex3(2, 2, 2),
       BfVertex3(3, 3, 3)
   ));
}

TEST_CASE("Non-coplanar point breaks the condition", "[isVerticesInPlain]")
{
   REQUIRE_FALSE(obj::curves::math::isVerticesInPlain(
       BfVertex3(0, 0, 0),
       BfVertex3(1, 0, 0),
       BfVertex3(0, 1, 0),
       BfVertex3(0.5, 0.5, 0.01)  // Slightly off the plane
   ));
}

TEST_CASE("Identical points (degenerate but coplanar)", "[isVerticesInPlain]")
{
   REQUIRE(obj::curves::math::isVerticesInPlain(
       BfVertex3(1, 1, 1),
       BfVertex3(1, 1, 1),
       BfVertex3(1, 1, 1),
       BfVertex3(1, 1, 1)
   ));
}

TEST_CASE("XZ plane check", "[isVerticesInPlain]")
{
   REQUIRE(obj::curves::math::isVerticesInPlain(
       BfVertex3(0, 0, 0),
       BfVertex3(1, 0, 0),
       BfVertex3(0, 0, 1),
       BfVertex3(1, 0, 1),
       BfVertex3(0.5, 0, 0.5)
   ));
}

TEST_CASE("Non-coplanar random point", "[isVerticesInPlain]")
{
   REQUIRE_FALSE(obj::curves::math::isVerticesInPlain(
       BfVertex3(0, 0, 0),
       BfVertex3(1, 1, 0),
       BfVertex3(1, 0, 1),
       BfVertex3(0.3, 0.2, 0.8)
   ));
}

TEST_CASE("BfSingleLine", "BfSingleLine")
{
   // clang-format off
   // auto line_bf = obj::curves::BfSingleLine(BfVertex3(1, 2, 3), BfVertex3(4, 5, 6));
   // line_bf.make();
   // auto line_glm = obj::curves::BfSingleLine(glm::vec3(1, 2, 3), glm::vec3(4, 5, 6));
   // line_glm.make();
   // auto line_mixed = obj::curves::BfSingleLine(glm::vec3(1, 2, 3), BfVertex3(4, 5, 6));
   // line_mixed.make();
   // 
   // auto check_line = [](const obj::curves::BfSingleLine& line) { 
   //    REQUIRE(line.first() == BfVertex3(1,2,3));
   //    REQUIRE(line.first().pos == BfVertex3(1,2,3));
   //    REQUIRE(line.first().color == BfVertex3{1.0f, 1.0f, 1.0f});
   //    REQUIRE(line.first().normals == BfVertex3{0.0f, 0.0f, 0.0f});
   //
   //    REQUIRE(line.second() == BfVertex3(4,5,6));
   //    REQUIRE(line.second().pos == BfVertex3(4,5,6));
   //    REQUIRE(line.second().color == BfVertex3{1.0f, 1.0f, 1.0f});
   //    REQUIRE(line.second().normals == BfVertex3{0.0f, 0.0f, 0.0f});
   //    
   //    
   //    REQUIRE(line.vertices().size() == 2);
   //    REQUIRE(line.vertices()[0].pos == line.first().pos);
   //    REQUIRE(line.vertices()[0].color == line.first().color);
   //    REQUIRE(line.vertices()[0].normals == line.first().normals);
   //
   //    REQUIRE(line.vertices()[1].pos == line.second().pos);
   //    REQUIRE(line.vertices()[1].color == line.second().color);
   //    REQUIRE(line.vertices()[1].normals == line.second().normals);
   //
   //    REQUIRE(line.indices()[0] == 0);
   //    REQUIRE(line.indices()[1] == 1);
   // };
   //
   // check_line(line_bf);
   // check_line(line_glm);
   // check_line(line_mixed);

   // clang-format on
}

// int
// main(int argc, char* argv[])
// {
//    BfBase base;
//    BfHolder holder;
//    setup(base, holder);
//    // your setup ...
//
//    int result = Catch::Session().run(argc, argv);
//
//    end(base);
//
//    return result;
// }
