#ifndef BF_GUI_SMART_LAYER_OBSERVER_H
#define BF_GUI_SMART_LAYER_OBSERVER_H

#include <bfDrawObject.h>
#include <bfDrawObjectDefineType.h>
#include <bfGuiWindow.h>
#include <bfLayerHandler.h>
#include <imgui.h>

class BfGuiSmartLayerObserver : public BfGuiWindow
{
   static uint32_t __current_moving_id;

protected:
   using ptrLayer = std::shared_ptr<BfDrawLayer>;
   using ptrObj = std::shared_ptr<BfDrawObj>;

   void __bodyRender() override;
   void __renderSingleLayer(std::shared_ptr<BfDrawLayer> l);

   void __renderDragDropSourceLayer(ptrLayer l, const char* name);
   void __renderDragDropSourceObj(ptrObj o, const char* name);
   void __renderDragDropTargetLayer(ptrLayer l, const char* name);
   void __renderDragDropTargetObj(ptrObj o, const char* name);

   void __pushMovingHandleStyle();
   void __popMovingHandleStyle();

public:
   BfGuiSmartLayerObserver();

   static std::string layerName(std::shared_ptr<BfDrawLayer> l);
   static std::string objName(std::shared_ptr<BfDrawObj> o);
   static std::string objData(std::shared_ptr<BfDrawObj> o);
};

#endif  // !
