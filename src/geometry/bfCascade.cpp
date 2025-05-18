#include "bfCascade.h"
#include "bfChain.h"
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_BezierCurve.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <algorithm>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <iterator>

namespace cascade
{

gp_XYZ
BfVertex3CASCADE::toXYZ() const
{
   return gp_XYZ(pos.x, pos.y, pos.z);
}

gp_Pnt
BfVertex3CASCADE::toPoint() const
{
   return gp_Pnt{toXYZ()};
}

gp_Dir
BfVertex3CASCADE::toNormal() const
{
   return gp_Dir(normals.x, normals.y, normals.z);
}

BfVertex3CASCADE::
operator gp_Pnt() const
{
   return toPoint();
}

BfVertex3CASCADE::
operator gp_Dir() const
{
   return toNormal();
}

TopoDS_Shape
createArc(
    const BfVertex3CASCADE& begin,
    const BfVertex3CASCADE& center,
    const BfVertex3CASCADE& end
)
{
   fmt::println(
       "Creating arc from {} => {} => {}",
       begin.pos,
       center.pos,
       end.pos
   );
   Handle(Geom_TrimmedCurve) arc = GC_MakeArcOfCircle(begin, center, end);
   BRepBuilderAPI_MakeEdge edge(arc);
   return edge;
}

TopoDS_Shape
createBezierCurve(const std::vector< BfVertex3CASCADE >& control)
{
   // clang-format off
   NCollection_Array1< gp_Pnt > cascade_vertices{ 1, static_cast< int >(control.size()) };

   for (int i = 0; i < control.size(); ++i) { 
      cascade_vertices.SetValue(i + 1, control.at(i));
   }

   Handle(Geom_BezierCurve) curve = new Geom_BezierCurve(cascade_vertices);
   BRepBuilderAPI_MakeEdge edge(curve);
   return edge;
   // clang-format on
}

std::vector< gp_Pnt >
toCascadePoints(const std::vector< BfVertex3Uni >& v)
{
   // clang-format off
   std::vector< gp_Pnt > transformed;
   std::transform(v.begin(), v.end(), std::back_inserter(transformed),
                  [](const auto& vert) { 
                     return static_cast< BfVertex3CASCADE >(vert.get());
                  });
   return transformed;
   // clang-format on
}

std::vector< gp_Pnt >
toCascadePoints(const std::vector< BfVertex3 >& v)
{
   // clang-format off
   std::vector< gp_Pnt > transformed;
   std::transform(v.begin(), v.end(), std::back_inserter(transformed),
                  [](const auto& vert) { 
                     return static_cast< BfVertex3CASCADE >(vert);
                  });
   return transformed;
   // clang-format on
}

std::vector< BfVertex3CASCADE >
toCascadeBfPoints(const std::vector< BfVertex3Uni >& v)
{
   // clang-format off
   std::vector< BfVertex3CASCADE > transformed;
   std::transform(v.begin(), v.end(), std::back_inserter(transformed),
                  [](const auto& vert) { 
                     return static_cast< BfVertex3CASCADE >(vert.get());
                  });
   return transformed;
   // clang-format on
}

std::vector< gp_Pnt >
toCascadePoints(const std::vector< glm::vec3 >& v)
{
   // clang-format off
   std::vector< gp_Pnt > transformed;
   std::transform(v.begin(), v.end(), std::back_inserter(transformed),
                  [](const auto& vert) { 
                     return static_cast< BfVertex3CASCADE >(vert);
                  });
   return transformed;
   // clang-format on
}

TopoDS_Wire
createWireFromShapes(const std::vector< TopoDS_Shape >& shapes)
{
   BRepBuilderAPI_MakeWire wireBuilder;

   for (const auto& shape : shapes)
   {
      if (shape.IsNull())
         continue;

      if (shape.ShapeType() == TopAbs_EDGE)
      {
         const TopoDS_Edge& edge = TopoDS::Edge(shape);
         wireBuilder.Add(edge);
      }
      else
      {
         fmt::print("Warning: Non-edge shape passed to wire builder.\n");
      }
   }

   if (!wireBuilder.IsDone())
   {
      fmt::print("Error: Failed to build wire.\n");
      return TopoDS_Wire();
   }

   return wireBuilder.Wire();
}

std::vector< TopoDS_Shape >
createSection(std::shared_ptr< obj::section::BfBladeSection > section)
{
   std::vector< TopoDS_Shape > shapes;

   // clang-format off
   auto inlet = section->inletCircle();
   auto inlet_vertices = inlet->arcVertices();
   auto cascade_inlet_circle = createArc(
      static_cast<BfVertex3CASCADE>(inlet_vertices[0].get()), 
      static_cast<BfVertex3CASCADE>(inlet_vertices[1].get()), 
      static_cast<BfVertex3CASCADE>(inlet_vertices[2].get())
   );
   shapes.push_back(std::move(cascade_inlet_circle));

   auto chain = section->chain();

   fmt::println("Converting front bezier chain");
   int i = 0;
   for (auto &front_elem : chain->bezierCurveChain(obj::curves::BfChain::ChainType::Front)) { 
      auto vert = toCascadeBfPoints(*front_elem);
      shapes.push_back(createBezierCurve(vert));
   }

   auto outlet = section->outletCircle();
   auto outlet_vertices = outlet->arcVertices();
   auto cascade_outlet_circle = createArc(
      static_cast<BfVertex3CASCADE>(outlet_vertices[2].get()), 
      static_cast<BfVertex3CASCADE>(outlet_vertices[1].get()), 
      static_cast<BfVertex3CASCADE>(outlet_vertices[0].get())
   );
   shapes.push_back(std::move(cascade_outlet_circle));

   fmt::println("Converting back bezier chain");
   i = 0;
   for (auto &back_elem : chain->bezierCurveChain(obj::curves::BfChain::ChainType::Back)) { 
      auto vert = toCascadeBfPoints(*back_elem);
      shapes.push_back(createBezierCurve(vert));
   }

   TopoDS_Wire wire = createWireFromShapes(shapes);

   return {wire}; 
}


TopoDS_Wire wireFromBfPoints(const std::vector< BfVertex3 >& v) 
{

   // clang-format on

   auto points = toCascadePoints(v);
   if (points.size() < 2)
      throw std::runtime_error("Not enough points");
   if (points.front().Distance(points.back()) > 1e-6)
      points.push_back(points.front()); // Close the loop

   TColgp_Array1OfPnt arr(1, points.size());
   for (int i = 0; i < points.size(); ++i)
      arr.SetValue(i + 1, points[i]);

   Handle(Geom_BSplineCurve) spline = GeomAPI_PointsToBSpline(arr).Curve();
   TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(spline);
   TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge);

   if (!wire.Closed())
      throw std::runtime_error("Wire is still not closed");

   return wire;

   // auto points = toCascadePoints(v);
   // if (points.size() < 2)
   //    throw std::runtime_error("Not enough points");
   //
   // TColgp_Array1OfPnt arr(1, points.size());
   // for (int i = 0; i < points.size(); ++i)
   //    arr.SetValue(i + 1, points[i]);
   //
   // Handle(Geom_BSplineCurve) spline = GeomAPI_PointsToBSpline(arr).Curve();
   // TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(spline);
   // TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge);
   //
   // if (!wire.Closed())
   // {
   //    throw std::runtime_error("Wire is not closed");
   // }
   //
   // return wire;
   //
   // clang-format off
}

// TopoDS_Edge
// edgeFromBfPoints(const std::vector< BfVertex3 >& v)  { 
//    auto wire = wireFromBfPoints(v);
//
//    if (BRep_Tool::IsClosed(wire)) {
//       TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);
//       result.push_back(face);
//    }
//
//
// }
//


TopoDS_Wire
wireFromSection(std::shared_ptr< obj::section::BfBladeSection > sec)
{
   // clang-format on
   auto shape = sec->outputShape();
   //
   std::vector< BfVertex3 > translated;
   translated.reserve(shape->vertices().size());

   const glm::mat4 center = shape->toCenterMtx();
   const glm::mat4 rotate = shape->rotateMtx();

   float z = sec->info().get().z;
   for (const auto& vert : shape->vertices())
   {
      glm::vec3 new_vert = rotate * center * glm::vec4(vert.pos, 1.0f);
      new_vert.z = z;
      translated.push_back(vert.otherPos(new_vert));
   }

   return cascade::wireFromBfPoints(translated);
}

}; // namespace cascade
