#include <bfGuiFileDialog.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("FileDialog->basic", "[single-file]")
{
   std::vector<fs::path> dirs;
   std::vector<fs::path> files;

   bfGetDirectoryItemsList("./", dirs, files);

   std::cout << "dirs:\n";
   for (const auto& dir : dirs)
   {
      std::cout << dir << "\n";
   }
   std::cout << "files:\n";
   for (const auto& file : files)
   {
      std::cout << file << "\n";
   }
};
