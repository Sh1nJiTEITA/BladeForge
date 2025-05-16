#include "bfStep.h"
#include "bfChain.h"
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom_BezierCurve.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <algorithm>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <iterator>

namespace saveas
{

void
exportToSTEP(const std::vector< TopoDS_Shape >& shapes, const fs::path& path)
{
   STEPControl_Writer writer;
   for (const auto& shape : shapes)
   {
      writer.Transfer(shape, STEPControl_AsIs);
   }

   IFSelect_ReturnStatus status = writer.Write(path.c_str());
   if (status != IFSelect_RetDone)
   {
      std::cerr << "Failed to write the STEP file." << std::endl;
   }
   else
   {
      std::cout << "STEP file written successfully." << std::endl;
   }
}

} // namespace saveas
