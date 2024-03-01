#include <catch2/catch_test_macros.hpp>

#include "BfMain.h"
#include "bfConsole.h"




TEST_CASE("BF_MAIN_H", "[single-file]") {

	BfMain main;
	try {
		BfEventHandler::funcPtr = &BfConsole::print_single_single_event;
		main.run( );
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}