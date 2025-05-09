#pragma once
#include "bfBladeSection2.h"
#ifndef BF_STEP_H
#define BF_STEP_H

#include "bfVertex2.hpp"
#include <BRepPrimAPI_MakeBox.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Writer.hxx>
#include <TopoDS_Shape.hxx>
#include <filesystem>

namespace saveas
{

namespace fs = std::filesystem;

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
void exportToSTEP(const std::vector<TopoDS_Shape>& shapes, const fs::path& path);

auto toCascadePoints(const std::vector< BfVertex3Uni >& v) -> std::vector< gp_Pnt >;
auto toCascadeBfPoints(const std::vector< BfVertex3Uni >& v) -> std::vector< BfVertex3CASCADE  >;
auto createWireFromShapes(const std::vector<TopoDS_Shape>& shapes) -> TopoDS_Wire;

auto createSection(std::shared_ptr< obj::section::BfBladeSection > section) -> std::vector<TopoDS_Shape>;



}; // namespace saveas

#endif
