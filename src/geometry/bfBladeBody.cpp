#include "bfBladeBody.h"
#include "bfBladeSection2.h"
#include <algorithm>
#include <fmt/base.h>
#include <iterator>
#include <memory>

namespace obj
{
namespace body
{

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
