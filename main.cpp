#include "main.h"

#include "fmt/format.h"
#include "fmt/printf.h"

#include "bfVariative.hpp"
#include "include/other/bfVariative.hpp"

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
