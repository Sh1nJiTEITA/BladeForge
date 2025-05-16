#pragma once
#ifndef BF_STEP_H
#define BF_STEP_H

#include "bfBladeSection2.h"
#include "bfVertex2.hpp"
#include <BRepPrimAPI_MakeBox.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Writer.hxx>
#include <TopoDS_Shape.hxx>
#include <filesystem>

namespace saveas
{

void
exportToSTEP(const std::vector< TopoDS_Shape >& shapes, const fs::path& path);

}; // namespace saveas

#endif
