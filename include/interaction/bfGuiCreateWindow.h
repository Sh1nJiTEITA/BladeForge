#ifndef BF_GUI_CREATE_WINDOW_H
#define BF_GUI_CREATE_WINDOW_H

#include <imgui.h>
#include <imgui_internal.h>

#include <memory>
#include <optional>
#include <stack>

#include "bfGuiCreateWindowContainer.h"

class BfGuiCreateWindow
{
   using swapPair       = std::pair<std::string, std::string>;
   using ptrContainer   = BfGuiCreateWindowContainer::ptrContainer;
   using foundContainer = std::optional<std::list<ptrContainer>::iterator>;
   // --------------------------------------------------------------------

   static BfGuiCreateWindow* __instance;
   //
   bool __is_render = true;

   std::list<BfGuiCreateWindowContainer::ptrContainer> __containers;
   //
   std::stack<swapPair> __swap_pairs;

   // --------------------------------------------------------------------

   foundContainer __findContainer(std::list<ptrContainer>::iterator begin,
                                  std::list<ptrContainer>::iterator end,
                                  const std::string&                name);

   void __renderManagePanel();
   void __renderContainers();

   void __renderDragDropZone();

   void __addBlankContainer();
   void __addSection();
   void __addBase();

   void __processSwaps();
   void __processEvents();

public:
   BfGuiCreateWindow();

   // TODO: REMOVE INSTANCE LOGIC
   static BfGuiCreateWindow* instance() noexcept;

   void removeByName(std::string);
   void render();
};

#endif
