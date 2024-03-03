#include "main.h"



int main() {
	BfMain main;
	try {
		BfEventHandler::funcPtr = &BfConsole::print_single_single_event;
		main.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}