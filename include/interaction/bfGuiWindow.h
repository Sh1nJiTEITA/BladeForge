#ifndef BF_GUI_WINDOW_H
#define BF_GUI_WINDOW_H

#include <imgui.h>

#include <array>
#include <ctime>
#include <string>
#include <vector>

struct BfGuiWindowCreateInfo
{
   std::string name;
   ImGuiWindowFlags flags = ImGuiWindowFlags_None;
   bool isRender = true;
   std::array<ImGuiKey, 2> toggleRenderMapping = {};
};

class BfGuiWindow
{
protected:
   BfGuiWindowCreateInfo __wi;

   virtual void __preRender();
   virtual void __bodyRender();
   virtual void __postRender();

public:
   BfGuiWindow(BfGuiWindowCreateInfo info);
   BfGuiWindow();

   void render();
   void hide();
   void show();
   void toggleRender();

   virtual void pollEvents();
};

#endif  // !BF_GUI_WINDOW_H
