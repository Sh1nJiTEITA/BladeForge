#include <bfGuiWindow.h>

void
BfGuiWindow::__preRender()
{
}

void
BfGuiWindow::__bodyRender()
{
}

void
BfGuiWindow::__postRender()
{
}

BfGuiWindow::BfGuiWindow(BfGuiWindowCreateInfo info)
    : __wi{info}
{
}

BfGuiWindow::BfGuiWindow()
    : __wi{std::to_string(std::time(nullptr))}
{
}

void
BfGuiWindow::render()
{
   if (__wi.isRender)
   {
      __preRender();
      ImGui::Begin(__wi.name.c_str(), &__wi.isRender, __wi.flags);
      {
         __bodyRender();
      }
      ImGui::End();
      __postRender();
   }
}

void
BfGuiWindow::hide()
{
   __wi.isRender = false;
}
void

BfGuiWindow::show()
{
   __wi.isRender = true;
}

void
BfGuiWindow::toggleRender()
{
   __wi.isRender = !__wi.isRender;
}

void
BfGuiWindow::pollEvents()
{
}
