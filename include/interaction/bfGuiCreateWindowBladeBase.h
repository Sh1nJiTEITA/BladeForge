#ifndef BF_GUI_CREATE_WINDOW_BLADE_BASE_H
#define BF_GUI_CREATE_WINDOW_BLADE_BASE_H

#include "bfGuiCreateWindowBladeSection.h"
#include "bfGuiCreateWindowContainer.h"

class BfGuiCreateWindowBladeBase : public BfGuiCreateWindowContainerObj
{
   std::string __base_name;

   void __setContainersPos();

protected:
   virtual void __renderHeaderName() override;
   virtual void __renderChildContent() override;
   virtual void __processDragDropTarget() override;
   virtual void __prerender() override;
   virtual void __postrender() override;

public:
   BfGuiCreateWindowBladeBase(
       BfGuiCreateWindowContainer::wptrContainer root, bool is_target = true
   );
};

#endif
