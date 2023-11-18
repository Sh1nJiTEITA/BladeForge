#include <catch2/catch_test_macros.hpp>

#include "BfMain.h"



TEST_CASE("BF_MAIN_H", "[single-file]") {

	BfMain main;
	try {
		main.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}