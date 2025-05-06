#include "bfGuiCreateWindowCircleWithHandles.h"

#include "bfGuiSmartLayerObserverWindow.h"

BfGuiCreateWindowCircleFilledWithHandles::
    BfGuiCreateWindowCircleFilledWithHandles(wptrContainer root, bool is_target)
    : BfGuiCreateWindowContainerObj(root, is_target)
{
   __window_size = {500, 500};
   __old_size = {500, 350};
   __name = "CircleFilledWithHandles";
}

void
BfGuiCreateWindowCircleFilledWithHandles::__createObj()
{
   __layer_obj = std::make_shared< BfCircleFilledWithHandles >(
       100,
       BfVertex3({0.0f, 0.f, 0.f}, {1.0, .0f, 0.f}, {0.0f, 0.0f, 1.0f}),
       0.3f
   );
}

void
BfGuiCreateWindowCircleFilledWithHandles::__renderChildContent()
{
   if (ImGui::Button("Add to layer"))
   {
      if (!__layer_choser)
      {
         __layer_choser = std::make_shared< BfGuiCreateWindowContainerPopup >(
             shared_from_this(),
             BfGuiCreateWindowContainerPopup_Side_Right,
             false,
             [&](wptrContainer root) {
                BfGuiSmartLayerObserver::instance()->renderChoser([&]() {
                   size_t selected_id =
                       BfGuiSmartLayerObserver::instance()->selectedLayer();
                   std::shared_ptr< BfDrawLayer > selected_layer =
                       BfLayerHandler::instance()->get_layer_by_id(selected_id);
                   // __createObj();
                   __addToLayer(selected_layer);
                   __layer_choser->toggleRender(false);
                });
             }
         );
         this->add(__layer_choser);
      }
      else
      {
         __layer_choser->toggleRender();
      }
   }
};
