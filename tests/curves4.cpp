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
TEST_CASE("BfSingleLine", "BfSingleLine")
{
   // clang-format off
   auto line_bf = curves::BfSingleLine(BfVertex3(1, 2, 3), BfVertex3(4, 5, 6));
   auto line_glm = curves::BfSingleLine(glm::vec3(1, 2, 3), glm::vec3(4, 5, 6));
   auto line_mixed = curves::BfSingleLine(glm::vec3(1, 2, 3), BfVertex3(4, 5, 6));
   
   // auto check_line = [](const curves::BfSingleLine& line) { 
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
