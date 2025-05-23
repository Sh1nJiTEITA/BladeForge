#include "bfYaml.h"
#include "yaml-cpp/yaml.h"
#include <fstream>
#include <ios>
#include <istream>
#include <sstream>
#include <yaml-cpp/node/parse.h>

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
         fmt::println("Can't open file with path=\"{}\"", path.string());
         return false;
      }

      YAML::Emitter out;
      out << node;

      if (!out.good())
      {
         fmt::println("YAML Emitter error: {}", out.GetLastError());
         return false;
      }

      file << out.c_str();
      file.close();

      if (!file)
      {
         fmt::println(
             "Failed to write YAML to file path=\"{}\"",
             path.string()
         );
         return false;
      }

      return true;
   }
   catch (const std::exception& e)
   {
      fmt::println("Exception during YAML save:\n{}", e.what());
      return false;
   }
}

bool
loadFromYaml(YAML::Node& node, const fs::path& path)
{
   try
   {
      std::ifstream file(path, std::ios_base::in);
      if (!file.is_open())
      {
         fmt::println("Cant open file with path=\"{}\"", path.string());
         return false;
      }
      std::stringstream con;
      con << file.rdbuf();
      node = YAML::Load(con.str());
      return true;
   }
   catch (const std::exception& e)
   {
      fmt::println("Error during saving to yaml:\n{}", e.what());
      return false;
   }
}

} // namespace saveas
