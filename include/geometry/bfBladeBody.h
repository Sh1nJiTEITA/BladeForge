#pragma once

#include "bfBladeSection.h"
#include <filesystem>
#ifndef BF_BLADE_BODY_H
#define BF_BLADE_BODY_H

#include "bfBladeSection2.h"
#include "bfDrawObject2.h"
#include <TopoDS_Wire.hxx>
#include <fmt/base.h>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace obj
{
namespace body
{

class BfBladeSurface : public obj::BfDrawObject
{
public:
   using sectionw_t = std::weak_ptr< section::BfBladeSection >;
   using sections_t = std::shared_ptr< section::BfBladeSection >;

public:
   BfBladeSurface(std::vector< sectionw_t >&& sections);

   auto make() -> void override;
   auto setSections(const std::vector< sectionw_t >& sections) -> void;
   auto dumbSectionsToStep(const fs::path& path) -> void;

   virtual void prerender(uint32_t elem) override;
   virtual void postrender(uint32_t elem) override;

   auto exportShape() -> TopoDS_Shape { return m_loft; }

protected:
   auto _section(uint32_t index) -> sections_t;
   auto _makeCascadeWire(uint32_t index) -> TopoDS_Wire;
   auto _loft() -> TopoDS_Shape;

private:
   std::vector< sectionw_t > m_sections;
   TopoDS_Shape m_loft;
};

//
//
//
//
//
//
//
//

struct SectionCreateInfoExtended : public section::SectionCreateInfo
{
   bool isActive = true;
   bool isAlive = true;
};
//
//
//
//
//
//
//

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

      InfoIterator(std::vector< base_pointer >::iterator begin,
                   std::vector< base_pointer >::iterator end);

      reference operator*() const;
      pointer operator->() const;
      InfoIterator& operator++();
      InfoIterator operator++(int);
      bool operator==(const InfoIterator& other) const;
      bool operator!=(const InfoIterator& other) const;

   private:
      std::vector< base_pointer >::iterator m_begin;
      std::vector< base_pointer >::iterator m_end;
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
      SectionIterator(std::vector< base_pointer >::iterator begin,
                      std::vector< base_pointer >::iterator end);

      reference operator*() const;
      pointer operator->() const;
      SectionIterator& operator++();
      SectionIterator operator++(int);
      bool operator==(const SectionIterator& other) const;
      bool operator!=(const SectionIterator& other) const;

   private:
      std::vector< base_pointer >::iterator m_begin;
      std::vector<base_pointer>::iterator m_end;
   };
   auto beginSection() -> SectionIterator;
   auto endSection() -> SectionIterator;
   
   auto createSurface() -> std::shared_ptr< BfBladeSurface > ;
   auto getSurface() -> std::shared_ptr< BfBladeSurface > ;

public:
   void sortSections();
   void eraseUnaliveSections();

   // clang-format off
   template < typename INFO >
   auto addSection(INFO&& info) -> section_t { 
      return addf< section::BfBladeSection >(std::forward< INFO >(info));
   }

   auto activeSections() -> std::vector< std::weak_ptr< section::BfBladeSection > >;
   auto addSection() -> section_t;
   auto lastInfoCopy() -> SectionCreateInfoExtended;
   
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
