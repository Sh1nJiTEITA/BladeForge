#include "bfYaml.h"
#include "yaml-cpp/yaml.h"
#include <fstream>

namespace saveas
{

bool
exportToYaml(const YAML::Node& node, const fs::path& path)
{
   try
   {
      std::ofstream file(path);
      if (!file.is_open())
      {
         fmt::println("Cant open file with path=\"{}\"", path.string());
         return false;
      }

      file << node;
      if (!file)
      {
         fmt::println(
             "Write to yaml file this path=\"{}\" failed",
             path.string()
         );
         return false;
      }

      return true;
   }
   catch (const std::exception& e)
   {
      fmt::println("Error during saving to yaml:\n{}", e.what());
      return false;
   }
}

} // namespace saveas
