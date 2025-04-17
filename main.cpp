#pragma warning(disable : 4244 4305 4267)

#include "BfMain.h"
#include "bfConsole.h"

#include "fmt/format.h"
#include "fmt/printf.h"

#include "bfVariative.hpp"

int
main()
{
   try
   {
      BfMain main;
      BfEventHandler::funcPtr = &BfConsole::print_single_single_event;
      BfEventHandler::funcPtrStr =
          &BfConsole::print_single_single_event_to_string;
      main.run();
   }
   catch (const std::exception& e)
   {
      fmt::printf("=========CATCHED EXCEPTION=========\n{}", e.what());
   }
}
