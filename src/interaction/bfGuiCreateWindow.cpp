#include "bfGuiCreateWindow.h"

//
//
//
//
//
//
//

BfGuiCreateWindow* BfGuiCreateWindow::__instance = nullptr;

BfGuiCreateWindow::BfGuiCreateWindow()
{
   if (__instance)
   {
      throw std::runtime_error(
          "Create window was already created, "
          "could not be 2 at the same time");
   }

   __instance = this;

   /*
         This is what happens when ROOT-container is 'Create' window!
         Function below deletes cointeiner from this staring 'Create' window.
    */
   BfGuiCreateWindowContainerObj::setRootDelete(
       [&](std::string name) { BfGuiCreateWindow::removeByName(name); });
}

BfGuiCreateWindow* BfGuiCreateWindow::instance() noexcept { return __instance; }

void BfGuiCreateWindow::__renderManagePanel()
{
   ImGui::BeginChild("##CreateWindowManagePanel", {}, true);
   {
      if (!BfGuiCreateWindowContainerObj::isAnyMoving())
      {
         if (ImGui::Button("Add container"))
         {
            __addBlankContainer();
         }
         ImGui::SameLine();
      }
      else
      {
         __renderDragDropZone();
      }
   }
   ImGui::EndChild();
}

void BfGetWindowsUnderMouse(std::vector<ImGuiWindow*>& www)
{
   ImVec2 mouse_pos = ImGui::GetMousePos();

   for (auto window = ImGui::GetCurrentContext()->Windows.begin();
        window != ImGui::GetCurrentContext()->Windows.end();
        ++window)
   {
      if ((*window)->Active && (*window)->Hidden == false &&
          (*window)->ParentWindow == nullptr)
      {
         ImVec2 min = (*window)->Pos;
         ImVec2 max = min;
         max.x += (*window)->Size.x;
         max.y += (*window)->Size.y;

         if (ImGui::IsMouseHoveringRect(min, max))
         {
            www.push_back(*window);
         }
      }
   }
}

void BfGuiCreateWindow::__renderContainers()
{
   std::stack<BfGuiCreateWindowContainer::ptrContainer> render_stack;
   std::list<std::pair<std::string, int>>               l;

   BfGuiCreateWindowContainer::resetResizeHover();
   int level = 0;
   for (auto container : __containers)
   {
      render_stack.push(container);
      level = 0;
      while (!render_stack.empty())
      {
         bool is_level = true;
         //
         BfGuiCreateWindowContainer::ptrContainer current_window =
             render_stack.top();
         render_stack.pop();

         current_window->render();
         BfGuiCreateWindowContainer::changeCursorStyle();
         l.push_back({current_window->name(), level});

         if (current_window->isCollapsed()) continue;
         for (auto child = current_window->begin();
              child != current_window->end();
              ++child)
         {
            render_stack.push(*child);
            if (is_level)
            {
               level++;
               is_level = false;
            }
         }
      }
   }

   l.sort([](std::pair<std::string, int> a, std::pair<std::string, int> b) {
      return a.second < b.second;
   });

   std::vector<ImGuiWindow*> windows_under_mouse;
   BfGetWindowsUnderMouse(windows_under_mouse);

   ImGuiWindow* last = nullptr;
   for (auto name = l.rbegin(); name != l.rend(); ++name)
   {
      bool is_found = false;
      for (auto& under : windows_under_mouse)
      {
         if (std::string(under->Name) == name->first)
         {
            ImGui::SetWindowFocus(under->Name);
            is_found = true;
            break;
         }
      }
      if (is_found) break;
   }
}

void BfGuiCreateWindow::__renderDragDropZone()
{
   ImGui::Text("<=== Move here ===>");
   // {ImGui::GetContentRegionAvail().x, 10.0f});
   if (ImGui::BeginDragDropTarget())
   {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("Container"))
      {
         /*
            Получаем указатель на окошко которое было сюда перемещено
         */
         std::shared_ptr<BfGuiCreateWindowContainerObj> dropped_container =
             *(std::shared_ptr<BfGuiCreateWindowContainerObj>*)payload->Data;

         /*
            Добавляем его к другим окошками в ДАННОМ ОКНЕ (куда было
            перемещено)
         */
         __containers.push_back(dropped_container);

         /*
            Добавляем его к другим окошками в ДАННОМ ОКНЕ (куда было
            перемещено)
         */
         auto wptr_old_root = (*__containers.rbegin())->root();

         /*
            Получаем указатель на внешнее окошко которое хранило то,
            что было перемещено
         */

         std::string dropped_name = (*__containers.rbegin())->name();
         if (auto shared_obj = wptr_old_root.lock())
         {
            // Удаляем из прошлого root-окна контейнер, который
            // был перемещен, чтобы он не дублировался
            shared_obj->clearEmptyContainersByName(dropped_name);
         }
         else
         {
            BfGuiCreateWindow::instance()->removeByName(dropped_name);
         }

         /*
            Меняем перемещенному окну 'root'-указатель и 'root'-имя
         */
         (*__containers.rbegin())->root() =
             std::weak_ptr<BfGuiCreateWindowContainer>();
      }
      ImGui::EndDragDropTarget();
   }
}

void BfGuiCreateWindow::__addBlankContainer()
{
   __containers.push_back(std::make_shared<BfGuiCreateWindowContainerObj>(
       std::weak_ptr<BfGuiCreateWindowContainer>()));
}

void BfGuiCreateWindow::removeByName(std::string name)
{
   std::erase_if(__containers,
                 [&name](auto c) { return std::string(c->name()) == name; });
}

void BfGuiCreateWindow::render()
{
   if (__is_render)
   {
      if (ImGui::Begin("Create",
                       &__is_render,
                       ImGuiWindowFlags_NoBringToFrontOnFocus |
                           ImGuiWindowFlags_NoDocking))
      {
         __renderManagePanel();
         __renderContainers();
      }
      ImGui::End();
   }
}

/*
   зарендерить все окна слоями! сначала на одном
   уровне потом на другом и тд
*/
