#ifndef BF_GUI_CREATE_WINDOW_BLADE_SECTION_H
#define BF_GUI_CREATE_WINDOW_BLADE_SECTION_H

#include <bfBladeSection.h>
#include <bfGuiSmartLayerObserverWindow.h>
#include <bfLayerHandler.h>

#include <cstdint>
#include <memory>

#include "bfGuiCreateWindowContainer.h"

class BfGuiCreateWindowBladeSection;
class BfGuiCreateWindowBladeBase;
class BfConfigManager;

namespace std
{
string to_string(const BfGuiCreateWindowBladeSection&, int);
}  // namespace std

class BfGuiCreateWindowBladeSection : public BfGuiCreateWindowContainerObj
{
   BF_GUI_CREATE_WINDOW_REGISTER_TYPE(BfGuiCreateWindowBladeSection);

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
   void __renderZCoordinate();

   void __createObj() override;

   virtual void __renderHeaderName() override;
   virtual void __renderChildContent() override;
   virtual void __processDragDropTarget() override;
   virtual void __renderInfoTooltip() override;
   virtual void __prerender() override;
   virtual void __postrender() override;

public:
   BfGuiCreateWindowBladeSection(wptrContainer root, bool is_target = true);

   void setView(viewMode) noexcept;
   BfBladeSectionCreateInfo createInfo() const noexcept;

   void clearSinglePopup(uint32_t id);
   void clearPopups();
   float& zCoordinate();

   friend BfGuiCreateWindow;
   friend BfGuiCreateWindowBladeBase;
   friend BfGuiCreateWindowContainerPopup;
   friend BfConfigManager;

   friend std::string std::to_string(const BfGuiCreateWindowBladeSection&, int);

#if defined(BF_CONFIG_MANAGER_TESTING)
   friend void checkBaseContainer(
       std::shared_ptr<BfGuiCreateWindowBladeSection> l,
       std::shared_ptr<BfGuiCreateWindowBladeSection> r
   );
#endif
};
BF_GUI_CREATE_WINDOW_REGISTER_TYPE_SOURCE(BfGuiCreateWindowBladeSection);

class BfGuiCreateWindowBladeSection2 : public BfGuiCreateWindowContainerObj
{
   BF_GUI_CREATE_WINDOW_REGISTER_TYPE(BfGuiCreateWindowBladeSection2);

   BfBladeSectionCreateInfo2 __create_info;
   std::weak_ptr<BfDrawLayer> __ptr_root;
   std::shared_ptr<BfBladeSection> __ptr_section;

protected:
   std::shared_ptr<BfGuiCreateWindowContainerPopup> __layer_choser;
   std::shared_ptr<BfGuiCreateWindowContainerPopup> __height_choser;

   void __createObj() override;
   virtual void __renderChildContent() override;

public:
   BfGuiCreateWindowBladeSection2(wptrContainer root, bool is_target = true);

   friend BfGuiCreateWindow;
   friend BfGuiCreateWindowBladeBase;
   friend BfGuiCreateWindowContainerPopup;
   friend BfConfigManager;
};
BF_GUI_CREATE_WINDOW_REGISTER_TYPE_SOURCE(BfGuiCreateWindowBladeSection2);

#endif
