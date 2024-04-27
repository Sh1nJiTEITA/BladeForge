#include "main.h"

#include <iostream>
#include "bfVariative.hpp"
#include "include/other/bfVariative.hpp"




int main() {
	
	BfMain main;
	try {
		BfEventHandler::funcPtr = &BfConsole::print_single_single_event;
		BfEventHandler::funcPtrStr = &BfConsole::print_single_single_event_to_string;
		main.run();
		int a = 10;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	std::cout << "FSDFSFSDDF" << std::endl;
	return 0;
}
