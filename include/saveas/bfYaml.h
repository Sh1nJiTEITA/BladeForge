#pragma once

#ifndef BF_YAML_H
#define BF_YAML_H

#include "bfBladeSection2.h"
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>

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
      if (!node.IsMap() || node.size() != 3)
      {
         return false;
      }
      rhs.pos = node["pos"].as< glm::vec3 >();
      rhs.color = node["color"].as< glm::vec3 >();
      rhs.normals = node["norm"].as< glm::vec3 >();
      return true;
   }
};

template <>
struct convert< obj::section::CenterCircle >
{
   static Node encode(const obj::section::CenterCircle& rhs)
   {
      Node node;
      node["relativePos"] = rhs.relativePos;
      node["radius"] = rhs.radius;
      node["backVertexAngle"] = rhs.backVertexAngle;
      node["frontVertexAngle"] = rhs.frontVertexAngle;
      return node;
   }

   static bool decode(const Node& node, obj::section::CenterCircle& rhs)
   {
      if (!node.IsMap() || node.size() != 4)
      {
         return false;
      }
      rhs.relativePos = node["relativePos"].as< float >();
      rhs.radius = node["radius"].as< float >();
      rhs.backVertexAngle = node["backVertexAngle"].as< float >();
      rhs.frontVertexAngle = node["frontVertexAngle"].as< float >();
      return true;
   }
};

template <>
struct convert< obj::section::SectionCreateInfo >
{
   static Node encode(const obj::section::SectionCreateInfo& rhs)
   {
      // clang-format off
      Node node;
      node["z"] = rhs.z;
      node["chord"] = rhs.chord;
      node["installAngle"] = rhs.installAngle;
      node["step"] = rhs.step;
      node["inletAngle"] = rhs.inletAngle;
      node["outletAngle"] = rhs.outletAngle;
      node["inletRadius"] = rhs.inletRadius;
      node["outletRadius"] = rhs.outletRadius;
      node["initialBezierCurveOrder"] = rhs.initialBezierCurveOrder;
      node["renderBitSet"] = rhs.renderBitSet;
      node["vertChordLeft"] = rhs.vertChordLeft;
      node["vertChordRight"] = rhs.vertChordRight;
      node["vertLeftChordBorder"] = rhs.vertLeftChordBorder;
      node["vertRightChordBorder"] = rhs.vertRightChordBorder;
      node["inletBackVertexAngle"] = rhs.inletBackVertexAngle;
      node["inletFrontVertexAngle"] = rhs.inletFrontVertexAngle;
      node["outletBackVertexAngle"] = rhs.outletBackVertexAngle;
      node["outletFrontVertexAngle"] = rhs.outletFrontVertexAngle;

      node["centerCircles"] = YAML::Node(YAML::NodeType::Sequence);
      for (const auto& circle : rhs.centerCircles)
      {
         node["centerCircles"].push_back(circle);
      }

      node["initialCurveControlVertices"] = YAML::Node(YAML::NodeType::Sequence);
      for (const auto& vertex : rhs.initialCurveControlVertices)
      {
         node["initialCurveControlVertices"].push_back(vertex);
      }

      return node;

      // clang-format on
   }

   static bool decode(const Node& node, obj::section::SectionCreateInfo& rhs)
   {
      // clang-format off
      
      if (!node.IsMap() || !node["centerCircles"] || !node["centerCircles"].IsSequence() ||
          !node["initialCurveControlVertices"] || !node["initialCurveControlVertices"].IsSequence())
      {
          return false;
      }

      rhs.z = node["z"].as< float >();
      rhs.chord = node["chord"].as< float >();
      rhs.installAngle = node["installAngle"].as< float >();

      rhs.step = node["step"].as< float >();
      rhs.isEqMode = false;
      rhs.inletAngle = node["inletAngle"].as< float >();
      rhs.outletAngle = node["outletAngle"].as< float >();
      rhs.inletRadius = node["inletRadius"].as< float >();
      rhs.outletRadius = node["outletRadius"].as< float >();
      rhs.initialBezierCurveOrder = node["initialBezierCurveOrder"].as< int >();
      rhs.renderBitSet = node["renderBitSet"].as< uint32_t >();
      rhs.vertChordLeft = node["vertChordLeft"].as< BfVertex3 >();
      rhs.vertChordRight = node["vertChordRight"].as< BfVertex3 >();
      rhs.vertLeftChordBorder = node["vertLeftChordBorder"].as< BfVertex3 >();
      rhs.vertRightChordBorder = node["vertRightChordBorder"].as< BfVertex3 >();
      rhs.inletBackVertexAngle = node["inletBackVertexAngle"].as< float >();
      rhs.inletFrontVertexAngle = node["inletFrontVertexAngle"].as< float >();
      rhs.outletBackVertexAngle = node["outletBackVertexAngle"].as< float >();
      rhs.outletFrontVertexAngle = node["outletFrontVertexAngle"].as< float >();
      rhs.centerCircles = node["centerCircles"].as< std::list< obj::section::CenterCircle > >();
      rhs.initialCurveControlVertices = node["initialCurveControlVertices"].as< std::vector< BfVertex3 > >();

      return true;

      // clang-format on
   }
};

} // namespace YAML

namespace saveas
{

auto exportToYaml(const YAML::Node& node, const fs::path& path) -> bool;
auto loadFromYaml(YAML::Node& node, const fs::path& path) -> bool;

} // namespace saveas

#endif
