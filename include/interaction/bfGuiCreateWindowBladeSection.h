#ifndef BF_GUI_CREATE_WINDOW_BLADE_SECTION_H
#define BF_GUI_CREATE_WINDOW_BLADE_SECTION_H

#include <bfBladeSection.h>
#include <bfLayerHandler.h>

#include <cstdint>
#include <memory>

#include "bfGuiCreateWindowContainer.h"

class BfGuiCreateWindowBladeSection : public BfGuiCreateWindowContainerObj
{
   BfBladeSectionCreateInfo        __create_info;
   std::shared_ptr<BfBladeSection> __ptr_section;
   //
   std::string __section_name;
   bool        __is_settings   = false;
   bool        __is_popup_open = false;

protected:
   void __renderSettingsWindow();
   void __renderSettings();

   void __createObj() override;
   void __addToLayer(std::shared_ptr<BfDrawLayer> add_to) override;

   virtual void __renderHeaderName() override;
   virtual void __renderChildContent() override;
   virtual void __processDragDropTarget() override;

public:
   BfGuiCreateWindowBladeSection(BfGuiCreateWindowContainer::wptrContainer root,
                                 bool is_target = true);
};

#endif
