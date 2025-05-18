#pragma once
#include <TopoDS_Wire.hxx>
#ifndef BF_CASCADE_H
#define BF_CASCADE_H

#include "bfBladeSection2.h"
#include "bfVertex2.hpp"
#include <BRepPrimAPI_MakeBox.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Writer.hxx>
#include <TopoDS_Shape.hxx>
#include <filesystem>

namespace cascade
{

struct BfVertex3CASCADE : public BfVertex3
{
   gp_XYZ toXYZ() const;
   gp_Pnt toPoint() const;
   gp_Dir toNormal() const;

   operator gp_Pnt() const;
   operator gp_Dir() const;
};

// clang-format off
auto createArc(const BfVertex3CASCADE& begin,
               const BfVertex3CASCADE& center,
               const BfVertex3CASCADE& end) -> TopoDS_Shape;
auto createBezierCurve(const std::vector< BfVertex3CASCADE >& control) -> TopoDS_Shape;

auto toCascadePoints(const std::vector< BfVertex3Uni >& v) -> std::vector< gp_Pnt >;
auto toCascadePoints(const std::vector< BfVertex3 >& v) -> std::vector< gp_Pnt >;
auto toCascadePoints(const std::vector< glm::vec3 >& v) -> std::vector< gp_Pnt >;

auto toCascadeBfPoints(const std::vector< BfVertex3Uni >& v) -> std::vector< BfVertex3CASCADE  >;

auto createWireFromShapes(const std::vector<TopoDS_Shape>& shapes) -> TopoDS_Wire;

auto createSection(std::shared_ptr< obj::section::BfBladeSection > section) -> std::vector<TopoDS_Shape>;

auto wireFromBfPoints(const std::vector< BfVertex3 >& v) -> TopoDS_Wire;
auto wireFromSection(std::shared_ptr< obj::section::BfBladeSection > sec) -> TopoDS_Wire;


} // namespace cascade

#endif
