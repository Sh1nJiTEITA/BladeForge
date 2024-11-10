#ifndef BF_GUI_SMART_LAYER_OBSERVER_H
#define BF_GUI_SMART_LAYER_OBSERVER_H

#include <bfDrawObject.h>
#include <bfDrawObjectDefineType.h>
#include <bfGuiWindow.h>
#include <bfLayerHandler.h>
#include <imgui.h>

#include <functional>
#include <queue>
#include <variant>

class BfGuiSmartLayerObserver : public BfGuiWindow
{
   static BfGuiSmartLayerObserver* __instance;

public:
   enum viewMode
   {
      STD,
      CHOOSE,
   };

protected:
   using ptrLayer = std::shared_ptr<BfDrawLayer>;
   using ptrObj = std::shared_ptr<BfDrawObj>;

   enum class __transaction_t
   {
      MOVE,
      SWAP
   };

   struct __sendingData_t
   {
      std::variant<ptrLayer, ptrObj> what;
      std::variant<ptrLayer, ptrObj> where;
      __transaction_t how;
   };

   // ----------------------------------------------------------------

   std::queue<__sendingData_t> __transactionQueue;
   std::queue<std::string> __popups;
   __transaction_t __current_transaction_type;
   static uint32_t __current_moving_id;
   viewMode __mode = viewMode::STD;
   size_t __selected_id = 0;
   std::function<void()> __choose_func = nullptr;

   // ----------------------------------------------------------------

   void __bodyRender() override;
   void __renderSingleLayer(std::shared_ptr<BfDrawLayer> l);

   void __renderDragDropSourceLayer(ptrLayer l, const char* name);
   void __renderDragDropSourceObj(ptrObj o, const char* name);
   void __renderDragDropTargetLayer(ptrLayer l, const char* name);
   void __renderDragDropTargetObj(ptrObj o, const char* name);

   void __renderDragDropSource(
       std::variant<ptrLayer, ptrObj> ptr, const char* name
   );
   void __renderDragDropTarget(
       std::variant<ptrLayer, ptrObj> ptr, const char* name
   );
   void __renderModularPopups();
   void __startModularPopups();

   void __pushMovingHandleStyle();
   void __popMovingHandleStyle();

public:
   BfGuiSmartLayerObserver();

   static BfGuiSmartLayerObserver* instance();
   static std::string layerName(std::shared_ptr<BfDrawLayer> l);
   static std::string objName(std::shared_ptr<BfDrawObj> o);
   static std::string objData(std::shared_ptr<BfDrawObj> o);

   void renderBodyView(viewMode mode = viewMode::STD);
   void renderChoser(std::function<void()> button);

   size_t selectedLayer() const noexcept;
   void pollEvents() override;
};

#endif  // !
