#include "bfBladeBody.h"
#include "bfBladeSection2.h"
#include "bfCascade.h"
#include "bfStep.h"
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <algorithm>
#include <fmt/base.h>
#include <fmt/format.h>
#include <gp_Pnt.hxx>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace obj
{
namespace body
{

BfBladeSurface::BfBladeSurface(std::vector< sectionw_t >&& sections)
    : BfDrawObject(
          "Blade surface", BF_PIPELINE(BfPipelineType_TrianglesFramed), 0
      )
    , m_sections{std::move(sections)}
{
}

void
BfBladeSurface::setSections(const std::vector< sectionw_t >& sections)
{
   m_sections = sections;
}

void
BfBladeSurface::make()
{
   TopoDS_Shape loft = _loft(); // Assuming this returns a valid TopoDS_Shape

   // Create the mesh with deflection 0.01
   BRepMesh_IncrementalMesh mesh(loft, 0.01);
   mesh.Perform();

   m_vertices.clear();

   const glm::vec3 col{1.0f};

   for (TopExp_Explorer exp(loft, TopAbs_FACE); exp.More(); exp.Next())
   {
      // Correct safe downcast to face
      TopoDS_Face face = TopoDS::Face(exp.Current());

      TopLoc_Location loc;
      Handle(Poly_Triangulation) tri = BRep_Tool::Triangulation(face, loc);
      if (tri.IsNull())
         continue;

      for (size_t i = 1; i < tri->NbTriangles(); ++i)
      {
         const Poly_Triangle& t = tri->Triangle(i);
         int n1, n2, n3;
         t.Get(n1, n2, n3);

         gp_Pnt _n1 = tri->Node(n1);
         // gp_Dir _nn1 = tri->Normal(n1);
         m_vertices.push_back(
             BfVertex3{/* position */ {_n1.X(), _n1.Y(), _n1.Z()},
                       /* color    */ col,
                       // /* normals  */ {_nn1.X(), _nn1.Y(), _nn1.Z()}}
                       /* normals  */ {0, 0, -1}}
         );
         m_indices.push_back(n1);

         gp_Pnt _n2 = tri->Node(n2);
         // gp_Dir _nn2 = tri->Normal(n2);
         m_vertices.push_back(
             BfVertex3{/* position */ {_n2.X(), _n2.Y(), _n2.Z()},
                       /* color    */ col,
                       // /* normals  */ {_nn2.X(), _nn2.Y(), _nn2.Z()}}
                       /* normals  */ {0, 0, -1}}
         );

         gp_Pnt _n3 = tri->Node(n3);
         // gp_Dir _nn3 = tri->Normal(n3);
         m_vertices.push_back(
             BfVertex3{/* position */ {_n3.X(), _n3.Y(), _n3.Z()},
                       /* color    */ col,
                       // /* normals  */ {_nn3.X(), _nn3.Y(), _nn3.Z()}}
                       /* normals  */ {0, 0, -1}}
         );
      }
   }
   _genIndicesStandart();
}

auto
BfBladeSurface::_section(uint32_t index) -> sections_t
{
   if (auto s = m_sections.at(index).lock())
   {
      return s;
   }
   else
   {
      const std::string msg = fmt::format(
          "Cant lock section with index={} of total={}",
          index,
          m_sections.size()
      );
      throw std::runtime_error(msg);
   }
}

TopoDS_Wire
BfBladeSurface::_makeCascadeWire(uint32_t index)
{
   auto sec = _section(index);
   auto shape = sec->outputShape();

   std::vector< BfVertex3 > translated;
   translated.reserve(shape->vertices().size());

   const glm::mat4 center = shape->toCenterMtx();
   const glm::mat4 rotate = shape->rotateMtx();

   // fmt::println("_makeCascadeWire BEGIN");
   float z = sec->info().get().z;
   for (const auto& vert : shape->vertices())
   {
      glm::vec3 new_vert = rotate * center * glm::vec4(vert.pos, 1.0f);
      new_vert.z = z;
      // fmt::println("NewV={}", new_vert);
      translated.push_back(vert.otherPos(new_vert));
   }
   // fmt::println("_makeCascadeWire END");

   return cascade::wireFromBfPoints(translated);
}

TopoDS_Shape
BfBladeSurface::_loft()
{
   BRepOffsetAPI_ThruSections builder(/*isSolid=*/false, /*ruled=*/false);

   for (size_t i = 0; i < m_sections.size(); ++i)
   {
      auto wire = cascade::wireFromSection(_section(i));
      fmt::println("Made wire from section with index={}", i);
      builder.AddWire(std::move(wire));
   }

   try
   {
      builder.CheckCompatibility();
      builder.Build();

      if (!builder.IsDone())
      {
         std::cerr << "Error: BRepOffsetAPI_ThruSections build failed.\n";
         return TopoDS_Shape();
      }
   }
   catch (const Standard_OutOfRange& e)
   {
      std::cerr << "Standard_OutOfRange: " << e.GetMessageString() << "\n";
      return TopoDS_Shape();
   }
   catch (const Standard_Failure& e)
   {
      std::cerr << "OpenCascade Exception: " << e.GetMessageString() << "\n";
      return TopoDS_Shape();
   }

   fmt::println("Loft done");
   return builder.Shape();
}

void
BfBladeSurface::dumbSectionsToStep(const fs::path& path)
{
   std::vector< TopoDS_Shape > w;
   w.reserve(m_sections.size());

   for (size_t i = 0; i < m_sections.size(); ++i)
   {
      // TopoDS_Wire wire = _makeCascadeWire(i);
      TopoDS_Wire wire = cascade::wireFromSection(_section(i));

      std::cerr << "Wire " << i << ": ";
      if (wire.IsNull())
      {
         std::cerr << "Null. Skipping.\n";
         continue;
      }

      if (!wire.Closed())
      {
         std::cerr << "Not closed. Skipping.\n";
         continue;
      }

      std::cerr << "Valid and closed.\n";
      w.push_back(wire);
   }

   saveas::exportToSTEP(w, path);
}

/* ============================================================= */

BfBladeBody::InfoIterator::InfoIterator(
    std::vector< base_pointer >::iterator begin,
    std::vector< base_pointer >::iterator end
)
    : m_begin(begin), m_end{end}
{
   while (m_begin != m_end &&
          !std::dynamic_pointer_cast< section::BfBladeSection >(*m_begin))
      ++m_begin;
}

BfBladeBody::InfoIterator::reference
BfBladeBody::InfoIterator::operator*() const
{
   auto casted = std::static_pointer_cast< section::BfBladeSection >(*m_begin);
   return static_cast< BfVar< SectionCreateInfoExtended > >(casted->info());
}

BfBladeBody::InfoIterator::pointer
BfBladeBody::InfoIterator::operator->() const
{
   auto casted = std::static_pointer_cast< section::BfBladeSection >(*m_begin);
   return static_cast< BfVar< SectionCreateInfoExtended > >(casted->info());
}

BfBladeBody::InfoIterator&
BfBladeBody::InfoIterator::operator++()
{
   do
   {
      ++m_begin;
   } while (m_begin != m_end &&
            !std::dynamic_pointer_cast< section::BfBladeSection >(*m_begin));
   return *this;
}

BfBladeBody::InfoIterator
BfBladeBody::InfoIterator::operator++(int)
{
   auto tmp = *this;
   ++(*this);
   return tmp;
}

bool
BfBladeBody::InfoIterator::operator==(const BfBladeBody::InfoIterator& other
) const
{
   return m_begin == other.m_begin;
}
bool
BfBladeBody::InfoIterator::operator!=(const BfBladeBody::InfoIterator& other
) const
{
   return m_begin != other.m_begin;
}

BfBladeBody::InfoIterator
BfBladeBody::beginInfo()
{
   return InfoIterator(m_children.begin(), m_children.end());
}

BfBladeBody::InfoIterator
BfBladeBody::endInfo()
{
   return InfoIterator(m_children.end(), m_children.end());
}

BfVar< SectionCreateInfoExtended >
BfBladeBody::grabExtInfo(base_t base)
{
   auto sec = std::static_pointer_cast< section::BfBladeSection >(base);
   return static_cast< BfVar< SectionCreateInfoExtended > >(sec->info());
}

BfBladeBody::section_t
BfBladeBody::addSection()
{
   return addf< section::BfBladeSection >(lastInfoCopy());
}

SectionCreateInfoExtended
BfBladeBody::lastInfoCopy()
{
   if (!m_children.empty())
   {
      auto last_raw = m_children.back();
      auto last = std::static_pointer_cast< section::BfBladeSection >(last_raw);
      return SectionCreateInfoExtended(last->info().get());
   }
   return SectionCreateInfoExtended{};
}

// INFO ITERATOR END
//

BfBladeBody::SectionIterator::SectionIterator(
    std::vector< base_pointer >::iterator begin,
    std::vector< base_pointer >::iterator end
)
    : m_begin(begin), m_end{end}
{
   while (m_begin != m_end &&
          !std::dynamic_pointer_cast< section::BfBladeSection >(*m_begin))
      ++m_begin;
}

BfBladeBody::SectionIterator::reference
BfBladeBody::SectionIterator::operator*() const
{
   return std::static_pointer_cast< section::BfBladeSection >(*m_begin);
}

BfBladeBody::SectionIterator::pointer
BfBladeBody::SectionIterator::operator->() const

{
   return std::static_pointer_cast< section::BfBladeSection >(*m_begin).get();
}

BfBladeBody::SectionIterator&
BfBladeBody::SectionIterator::operator++()
{
   do
   {
      ++m_begin;
   } while (m_begin != m_end &&
            !std::dynamic_pointer_cast< section::BfBladeSection >(*m_begin));
   return *this;
}

BfBladeBody::SectionIterator
BfBladeBody::SectionIterator::operator++(int)
{
   auto tmp = *this;
   ++(*this);
   return tmp;
}

bool
BfBladeBody::SectionIterator::operator==(
    const BfBladeBody::SectionIterator& other
) const
{
   return m_begin == other.m_begin;
}
bool
BfBladeBody::SectionIterator::operator!=(
    const BfBladeBody::SectionIterator& other
) const
{
   return m_begin != other.m_begin;
}

BfBladeBody::SectionIterator
BfBladeBody::beginSection()
{
   return SectionIterator(m_children.begin(), m_children.end());
}

BfBladeBody::SectionIterator
BfBladeBody::endSection()
{
   return SectionIterator(m_children.end(), m_children.end());
}

std::shared_ptr< BfBladeSurface >
BfBladeBody::createSurface()
{
   // m_children.clear();
   std::vector< std::weak_ptr< section::BfBladeSection > > s;
   for (auto it = beginSection(); it != endSection(); ++it)
   {
      auto info = grabExtInfo(*it);
      if (info.get().isActive)
      {
         s.push_back(*it);
      }
   }

   auto sur = std::make_shared< BfBladeSurface >(std::move(s));
   this->m_children.push_back(sur);
   return sur;
}

void
BfBladeBody::sortSections()
{
   // clang-format off
   std::sort(
       this->m_children.begin(),
       this->m_children.end(),
       [this](const auto& lhs, const auto& rhs) {
          auto lhs_blade = std::dynamic_pointer_cast< BfBladeSection >(lhs);
          auto rhs_blade = std::dynamic_pointer_cast< BfBladeSection >(rhs);
          if (lhs_blade && rhs_blade)
          {
             return grabExtInfo(lhs).get().z < grabExtInfo(rhs).get().z;
          }
          return lhs_blade && !rhs_blade; // lhs blade comes before non-blade
       }
   );
   // clang-format on
}

void
BfBladeBody::eraseUnactiveSections()
{
   std::erase_if(this->m_children, [this](const auto& base) {
      auto blade = std::dynamic_pointer_cast< BfBladeSection >(base);
      if (blade)
      {
         return !grabExtInfo(base).get().isActive;
      }
      return false;
   });
}

void
BfBladeBody::_updateSectionList()
{
   // m_list.clear();
   // std::transform(
   //     beginSection(),
   //     endSection(),
   //     std::back_inserter(m_list),
   //     [](const base_t& c) {
   //        return {}
   //     }
   // )
}

} // namespace body
} // namespace obj
