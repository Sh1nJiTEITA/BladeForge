#ifndef BF_GUI_CREATE_WINDOW_CIRCLE_FILLED_WITH_HANDLES_H
#define BF_GUI_CREATE_WINDOW_CIRCLE_FILLED_WITH_HANDLES_H

#include "bfCurves3.h"
#include "bfGuiCreateWindowContainer.h"

// clang-format off
//
//
//
//
//
//
//
//
//
class BfGuiCreateWindowCircleFilledWithHandles
    : public BfGuiCreateWindowContainerObj
{
   BF_GUI_CREATE_WINDOW_REGISTER_TYPE(BfGuiCreateWindowCircleFilledWithHandles);

   std::weak_ptr<BfDrawLayer> __ptr_root;

protected:
   std::shared_ptr<BfGuiCreateWindowContainerPopup> __layer_choser;
   std::shared_ptr<BfGuiCreateWindowContainerPopup> __height_choser;

   void __createObj() override;
   virtual void __renderChildContent() override;

public:
   BfGuiCreateWindowCircleFilledWithHandles(
       wptrContainer root, bool is_target = true
   );

   friend BfGuiCreateWindow;
   friend BfGuiCreateWindowContainerPopup;
   friend BfConfigManager;
};
BF_GUI_CREATE_WINDOW_REGISTER_TYPE_SOURCE(BfGuiCreateWindowCircleFilledWithHandles);

// clang-format on
#endif
