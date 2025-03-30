#include <bfObjectId.h>

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <memory>

struct BfTypeManager_test : public obj::BfTypeManager
{
   static void reset() { _reset(); }
};

TEST_CASE("BfTypeManager", "regType")
{
   auto m = obj::BfTypeManager::inst();

   REQUIRE(m.regType("1") == 0);
   REQUIRE(m.regType("1") == 0);
   REQUIRE(m.regType("2") == 1);
   REQUIRE(m.regType("3") == 2);
   REQUIRE(m.regType("1") == 0);

   REQUIRE(m.regType("3") == 2);
   REQUIRE(m.regType("3") == 2);
   REQUIRE(m.regType("3") == 2);
   REQUIRE(m.regType("3") == 2);

   REQUIRE(m.regType("2") == 1);
   REQUIRE(m.regType("2") == 1);
   REQUIRE(m.regType("2") == 1);
   REQUIRE(m.regType("2") == 1);
   REQUIRE(m.regType("2") == 1);
}

namespace obj
{

TEST_CASE("BfObjectId", "constructors")
{
   BfTypeManager_test::reset();

   struct A : public BfObjectId
   {
      A()
          : BfObjectId("123")
      {
      }
   };

   A a{};

   struct B : public BfObjectId
   {
      B()
          : BfObjectId("321")
      {
      }
   };

   B b{};

   REQUIRE(std::strcmp(a.typeName(), "123") == 0);
   REQUIRE(a.type() == 0);
   REQUIRE(std::strcmp(b.typeName(), "321") == 0);
   REQUIRE(b.type() == 1);
}

}  // namespace obj
