#pragma once

#include "bfVertex2.hpp"
#ifndef BF_YAML_H
#define BF_YAML_H

#include "bfCurves4.h"
#include <memory>
#include <yaml-cpp/node/node.h>

namespace YAML
{
template <>
struct convert< glm::vec3 >
{
   static Node encode(const glm::vec3& rhs)
   {
      Node node;
      node.push_back(rhs.x);
      node.push_back(rhs.y);
      node.push_back(rhs.z);
      return node;
   }

   static bool decode(const Node& node, glm::vec3& rhs)
   {
      if (!node.IsSequence() || node.size() != 3)
      {
         return false;
      }

      rhs.x = node[0].as< float >();
      rhs.y = node[1].as< float >();
      rhs.z = node[2].as< float >();
      return true;
   }
};

template <>
struct convert< BfVertex3 >
{
   static Node encode(const BfVertex3& rhs)
   {
      Node node;
      node["pos"] = rhs.pos;
      node["color"] = rhs.color;
      node["norm"] = rhs.normals;
      return node;
   }

   static bool decode(const Node& node, BfVertex3& rhs)
   {
      if (!node.IsSequence() || node.size() != 3)
      {
         return false;
      }
      rhs.pos = node["pos"].as< glm::vec3 >();
      rhs.color = node["color"].as< glm::vec3 >();
      rhs.normals = node["norm"].as< glm::vec3 >();
      return true;
   }
};

// template <>
// struct convert< ::curves:: >
// {
//    static Node encode(const BfVertex3& rhs)
//    {
//       Node node;
//       node["pos"] = rhs.pos;
//       node["color"] = rhs.color;
//       node["norm"] = rhs.normals;
//       return node;
//    }
//
//    static bool decode(const Node& node, BfVertex3& rhs)
//    {
//       if (!node.IsSequence() || node.size() != 3)
//       {
//          return false;
//       }
//       rhs.pos = node["pos"].as< glm::vec3 >();
//       rhs.color = node["color"].as< glm::vec3 >();
//       rhs.normals = node["norm"].as< glm::vec3 >();
//       return true;
//    }
// };
//

} // namespace YAML

namespace saveas
{

// auto to_yaml(std::shared_ptr< obj::curves::BfSingleLine > line) ->
// YAML::Node;

}

#endif
