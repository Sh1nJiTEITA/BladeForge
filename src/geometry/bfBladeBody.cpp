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
#include <execution>
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
   m_loft = _loft();

   BRepMesh_IncrementalMesh mesh(m_loft, 0.01);
   mesh.Perform();

   m_vertices.clear();
   m_indices.clear();

   const glm::vec3 col{1.0f};

   // Maps OCCT node index (1-based) + triangulation pointer to our local index
   std::unordered_map<
       std::pair< const Poly_Triangulation*, int >,
       uint32_t,
       boost::hash< std::pair< const Poly_Triangulation*, int > > >
       index_map;

   for (TopExp_Explorer exp(m_loft, TopAbs_FACE); exp.More(); exp.Next())
   {
      TopoDS_Face face = TopoDS::Face(exp.Current());

      TopLoc_Location loc;
      Handle(Poly_Triangulation) tri = BRep_Tool::Triangulation(face, loc);
      if (tri.IsNull())
         continue;

      for (int i = 1; i <= tri->NbTriangles(); ++i)
      {
         const Poly_Triangle& t = tri->Triangle(i);
         int ids[3];
         t.Get(ids[0], ids[1], ids[2]);

         for (int j = 0; j < 3; ++j)
         {
            int id = ids[j];

            // Unique key: triangulation pointer + index
            auto key = std::make_pair(tri.get(), id);
            auto it = index_map.find(key);
            if (it != index_map.end())
            {
               m_indices.push_back(it->second);
            }
            else
            {
               const gp_Pnt& p = tri->Node(id);

               uint32_t new_index = static_cast< uint32_t >(m_vertices.size());
               m_vertices.push_back(
                   BfVertex3{{p.X(), p.Y(), p.Z()}, col, {0.0f, 0.0f, -1.0f}}
               );

               index_map[key] = new_index;
               m_indices.push_back(new_index);
            }
         }
      }
   }
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

   // for (size_t i = 0; i < m_sections.size(); ++i)
   // {
   //    auto wire = cascade::wireFromSection(_section(i));
   //    fmt::println("Made wire from section with index={}", i);
   //    builder.AddWire(std::move(wire));
   //    // return TopoDS_Shape{};
   // }

   std::vector< TopoDS_Wire > wires(m_sections.size());

   std::for_each(
       std::execution::par,
       m_sections.begin(),
       m_sections.end(),
       [&](auto& sec) {
          size_t idx = &sec - &m_sections[0];
          wires[idx] = cascade::wireFromSection(_section(idx));
          fmt::println("Made wire from section with index={}", idx);
       }
   );

   // Now add wires sequentially (assuming builder is NOT thread-safe)
   for (auto& wire : wires)
   {
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

void
BfBladeSurface::prerender(uint32_t elem)
{
   switch (elem)
   {
   case 0:
      isRender() = false;
      break;
   case 1:
      isRender() = false;
      break;
   case 2:
      isRender() = true;
      break;
   };
}

void
BfBladeSurface::postrender(uint32_t elem)
{
   switch (elem)
   {
   case 0:
      isRender() = true;
      break;
   case 1:
      isRender() = true;
      break;
   case 2:
      isRender() = false;
      break;
   };
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

std::vector< std::weak_ptr< section::BfBladeSection > >
BfBladeBody::activeSections()
{
   std::vector< std::weak_ptr< section::BfBladeSection > > s;
   for (auto it = beginSection(); it != endSection(); ++it)
   {
      auto info = grabExtInfo(*it);
      if (info.get().isActive)
      {
         s.push_back(*it);
      }
   }
   return s;
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
   auto s = activeSections();
   auto sur = std::make_shared< BfBladeSurface >(std::move(s));
   this->m_children.push_back(sur);
   return sur;
}

std::shared_ptr< BfBladeSurface >
BfBladeBody::getSurface()
{
   for (auto it : m_children)
   {
      if (auto sur = std::dynamic_pointer_cast< BfBladeSurface >(it))
      {
         return sur;
      }
   }
   return nullptr;
}

void
BfBladeBody::sortSections()
{
   // clang-format off
   std::sort(
       this->m_children.begin(),
       this->m_children.end(),
       [this](const auto& lhs, const auto& rhs) {
          auto lhs_blade = std::dynamic_pointer_cast< section::BfBladeSection >(lhs);
          auto rhs_blade = std::dynamic_pointer_cast< section::BfBladeSection >(rhs);
          if (lhs_blade && rhs_blade)
          {
             // return grabExtInfo(lhs).get().z < grabExtInfo(rhs).get().z;
             return lhs_blade->info().get().z < rhs_blade->info().get().z;
          }
          return lhs_blade && !rhs_blade; // lhs blade comes before non-blade
       }
   );
   // clang-format on
}

void
BfBladeBody::eraseUnaliveSections()
{
   std::erase_if(this->m_children, [this](const auto& base) {
      auto blade = std::dynamic_pointer_cast< section::BfBladeSection >(base);
      if (blade)
      {
         return !grabExtInfo(base).get().isAlive;
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
