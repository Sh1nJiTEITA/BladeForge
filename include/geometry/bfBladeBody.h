#pragma once

#ifndef BF_BLADE_BODY_H
#define BF_BLADE_BODY_H

#include "bfBladeSection2.h"
#include "bfDrawObject2.h"
#include <fmt/base.h>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace obj
{
namespace body
{

struct SectionCreateInfoExtended : public section::SectionCreateInfo
{
   bool isActive = true;
};

class BfBladeBody : public BfDrawLayer
{
public:
   using section_t = std::shared_ptr< section::BfBladeSection >;
   using base_t = std::shared_ptr< obj::BfDrawObjectBase >;

public: // ASSIGNMENT
   //

   BfBladeBody(float length)
       : BfDrawLayer("Blade body"), m_length(length)
   {
   }

public: // ITERATOR
   //
   // clang-format off
   class InfoIterator
   {
   public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = BfVar< SectionCreateInfoExtended >;
      using difference_type = std::ptrdiff_t;
      using pointer = BfVar< SectionCreateInfoExtended >;
      using reference = BfVar< SectionCreateInfoExtended >;
      using base_pointer = std::shared_ptr< BfDrawObjectBase >;

      InfoIterator(std::vector< base_pointer >::iterator begin);

      reference operator*() const;
      pointer operator->() const;
      InfoIterator& operator++();
      InfoIterator operator++(int);
      bool operator==(const InfoIterator& other) const;
      bool operator!=(const InfoIterator& other) const;

   private:
      std::vector< base_pointer >::iterator m_begin;
   };
   auto beginInfo() -> InfoIterator;
   auto endInfo() -> InfoIterator;

   class SectionIterator
   {
   public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = section_t;
      using difference_type = std::ptrdiff_t;
      using pointer = section::BfBladeSection*;
      using reference = section_t;

      using base_pointer = std::shared_ptr< BfDrawObjectBase >;
      SectionIterator(std::vector< base_pointer >::iterator begin);

      reference operator*() const;
      pointer operator->() const;
      SectionIterator& operator++();
      SectionIterator operator++(int);
      bool operator==(const SectionIterator& other) const;
      bool operator!=(const SectionIterator& other) const;

   private:
      std::vector< base_pointer >::iterator m_begin;
   };
   auto beginSection() -> SectionIterator;
   auto endSection() -> SectionIterator;

public:
   void sortSections();
   void eraseUnactiveSections();

   // clang-format off
   template < typename INFO >
   auto addSection(INFO&& info) -> section_t { 
      return addf< section::BfBladeSection >(BfVar< INFO >(std::forward< INFO >(info)));
   }
   auto addSection() -> section_t;
   auto length() -> float& { return m_length; }

public:
   static auto grabExtInfo(base_t base) -> BfVar< SectionCreateInfoExtended >;
   // clang-format on
private:
   void _updateSectionList();

   float m_length;
};

} // namespace body
} // namespace obj

#endif
