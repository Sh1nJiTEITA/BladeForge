#include "bfBladeBody.h"
#include "bfBladeSection2.h"
#include <algorithm>
#include <fmt/base.h>
#include <fmt/format.h>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace obj
{
namespace body
{

BfBladeSurface::BfBladeSurface(std::vector< sectionw_t >&& sections)
    : BfDrawObject("Blade surface", BF_PIPELINE(BfPipelineType_Lines), 0)
    , m_sections{std::move(sections)}
{
}

void
BfBladeSurface::make()
{
   for (size_t i = 1; i < m_sections.size(); ++i)
   {
      size_t prev_c = _section(i - 1)->outputShape()->vertices().size();
      size_t next_c = _section(i)->outputShape()->vertices().size();
      if (prev_c != next_c)
      {
         const std::string msg = fmt::format(
             "Cant create blade surface. "
             "Section {} and {} has not equal "
             "number of vertices -> {} | {}",
             i - 1,
             i,
             prev_c,
             next_c
         );
         throw std::runtime_error(msg);
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

/* ============================================================= */

BfBladeBody::InfoIterator::InfoIterator(
    std::vector< base_pointer >::iterator begin
)
    : m_begin(begin)
{
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
   m_begin++;
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
   return InfoIterator(this->m_children.begin());
}

BfBladeBody::InfoIterator
BfBladeBody::endInfo()
{
   return InfoIterator(this->m_children.end());
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
    std::vector< base_pointer >::iterator begin
)
    : m_begin(begin)
{
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
   m_begin++;
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
   return SectionIterator(this->m_children.begin());
}

BfBladeBody::SectionIterator
BfBladeBody::endSection()
{
   return SectionIterator(this->m_children.end());
}

void
BfBladeBody::sortSections()
{
   // clang-format off
   std::sort(
      this->m_children.begin(),
      this->m_children.end(),
      [this](const auto& lhs, const auto& rhs) {
         return grabExtInfo(lhs).get().z < grabExtInfo(rhs).get().z;
      }
   );
   // clang-format on
}

void
BfBladeBody::eraseUnactiveSections()
{
   std::erase_if(this->m_children, [this](const auto& base) {
      return !grabExtInfo(base).get().isActive;
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
