#ifndef BF_GUI_CREATE_WINDOW_H
#define BF_GUI_CREATE_WINDOW_H

#include <imgui.h>
#include <imgui_internal.h>

#include <stack>

#include "bfGuiCreateWindowContainer.h"

class BfGuiCreateWindow
{
   static BfGuiCreateWindow* __instance;
   //
   bool __is_render = true;

   std::list<BfGuiCreateWindowContainer::ptrContainer> __containers;

   void __renderManagePanel();
   void __renderContainers();

   void __renderDragDropZone();
   void __addBlankContainer();

public:
   BfGuiCreateWindow();
   static BfGuiCreateWindow* instance() noexcept;

   void removeByName(std::string);
   void render();
};

#endif
