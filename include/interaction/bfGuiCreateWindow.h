#ifndef BF_GUI_CREATE_WINDOW_H
#define BF_GUI_CREATE_WINDOW_H

#include <imgui.h>
#include <imgui_internal.h>

#include <memory>
#include <optional>
#include <stack>

#include "bfGuiCreateWindowBladeBase.h"
#include "bfGuiCreateWindowContainer.h"

class BfGuiCreateWindow
{
   using pair           = std::pair<std::string, std::string>;
   using ptrContainer   = BfGuiCreateWindowContainer::ptrContainer;
   using foundContainer = std::optional<std::list<ptrContainer>::iterator>;
   using ptrPair        = std::pair<foundContainer, foundContainer>;
   // --------------------------------------------------------------------

   static BfGuiCreateWindow* __instance;
   //
   bool __is_render = true;

   std::list<BfGuiCreateWindowContainer::ptrContainer> __containers;
   //
   std::stack<pair> __swap_pairs;
   std::stack<pair> __move_pairs;

   // --------------------------------------------------------------------

   foundContainer __findContainer(std::list<ptrContainer>::iterator begin,
                                  std::list<ptrContainer>::iterator end,
                                  const std::string&                name);
   //
   ptrPair __findAndCheckPair(const pair& name);

   void __renderManagePanel();
   void __renderContainers();

   void __renderDragDropZone();

   void __addBlankContainer();
   void __addSection();
   void __addBase();

   void __processSwaps();
   void __processMoves();
   void __processEvents();

public:
   BfGuiCreateWindow();

   // TODO: REMOVE INSTANCE LOGIC
   static BfGuiCreateWindow* instance() noexcept;

   void removeByName(std::string);
   void render();
   void toggleRender();
};

#endif
