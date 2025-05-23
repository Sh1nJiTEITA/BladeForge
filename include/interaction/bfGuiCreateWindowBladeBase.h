#ifndef BF_GUI_CREATE_WINDOW_BLADE_BASE_H
#define BF_GUI_CREATE_WINDOW_BLADE_BASE_H

#include "bfBladeSection.h"
#include "bfGuiCreateWindowBladeSection.h"
#include "bfGuiCreateWindowContainer.h"

class BfGuiCreateWindowBladeBase;

namespace std
{
string to_string(const BfGuiCreateWindowBladeBase&, int);
}  // namespace std

class BfGuiCreateWindowBladeBase : public BfGuiCreateWindowContainerObj
{
   BF_GUI_CREATE_WINDOW_REGISTER_TYPE(BfGuiCreateWindowBladeBase);

   BfBladeBaseCreateInfo __create_info;
   uint32_t __selected_id = 0;

   std::shared_ptr<BfGuiCreateWindowContainerPopup> __settings_popup;
   std::shared_ptr<BfGuiCreateWindowContainerPopup> __layer_choser;

   std::weak_ptr<BfDrawLayer> __ptr_root;
   std::shared_ptr<BfBladeBase> __ptr_;

   void __setContainersPos();
   void __createObj() override;
   bool __isWindowContainerBladeSection(ptrContainer c);

protected:
   virtual void __renderChildContent() override;
   virtual void __processDragDropTarget() override;
   virtual void __prerender() override;
   virtual void __postrender() override;

public:
   BfGuiCreateWindowBladeBase(wptrContainer root, bool is_target = true);
   uint32_t* selectedId() noexcept;

   void addHeightPopup(std::shared_ptr<BfGuiCreateWindowBladeSection> s);

   friend std::string std::to_string(const BfGuiCreateWindowBladeBase&, int);
   friend class BfConfigManager;
};
BF_GUI_CREATE_WINDOW_REGISTER_TYPE_SOURCE(BfGuiCreateWindowBladeBase);

#endif
