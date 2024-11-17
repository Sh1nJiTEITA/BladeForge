#ifndef BF_GUI_CREATE_WINDOW_BLADE_SECTION_H
#define BF_GUI_CREATE_WINDOW_BLADE_SECTION_H

#include <bfBladeSection.h>
#include <bfGuiSmartLayerObserverWindow.h>
#include <bfLayerHandler.h>

#include <cstdint>
#include <memory>

#include "bfGuiCreateWindowContainer.h"

class BfGuiCreateWindowBladeBase;

class BfGuiCreateWindowBladeSection : public BfGuiCreateWindowContainerObj
{
public:
   enum class viewMode
   {
      STD,
      SHORT
   };

private:
   BfBladeSectionCreateInfo __create_info;
   std::weak_ptr<BfDrawLayer> __ptr_root;
   std::shared_ptr<BfBladeSection> __ptr_section;
   //
   std::string __section_name;
   bool __is_settings = false;
   bool __is_popup_open = false;
   bool __is_selected = false;

   std::function<void()> __prerender_external_func = nullptr;
   std::function<void()> __postrender_external_func = nullptr;

protected:
   std::shared_ptr<BfGuiCreateWindowContainerPopup> __layer_choser;
   std::shared_ptr<BfGuiCreateWindowContainerPopup> __height_choser;

   viewMode __mode = viewMode::STD;

   void __renderSettingsWindow();
   void __renderSettings();

   void __createObj() override;
   void __addToLayer(std::shared_ptr<BfDrawLayer> add_to) override;

   virtual void __renderHeaderName() override;
   virtual void __renderChildContent() override;
   virtual void __processDragDropTarget() override;
   virtual void __renderInfoTooltip() override;
   virtual void __prerender() override;
   virtual void __postrender() override;

public:
   BfGuiCreateWindowBladeSection(
       BfGuiCreateWindowContainer::wptrContainer root, bool is_target = true
   );

   void setView(viewMode) noexcept;
   BfBladeSectionCreateInfo createInfo() const noexcept;

   void clearSinglePopup(uint32_t id);
   void clearPopups();

   friend BfGuiCreateWindowBladeBase;
   friend BfGuiCreateWindowContainerPopup;
};

#endif
