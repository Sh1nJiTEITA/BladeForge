#include "bfGuiCreateWindow.h"

#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>

#include "bfGuiCreateWindowBladeBase.h"
#include "bfGuiCreateWindowBladeSection.h"
#include "imgui.h"

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
          "could not be 2 at the same time"
      );
   }

   __instance = this;

   /*
         This is what happens when ROOT-container is 'Create' window!
         Function below deletes cointeiner from this staring 'Create' window.
   */

   BfGuiCreateWindowContainerObj::bindSwapFunction([&](const std::string& a,
                                                       const std::string& b) {
      __swap_pairs.push({a, b});
   });

   BfGuiCreateWindowContainerObj::bindMoveFunction(
       [&](const std::string& what, const std::string& where) {
          __move_pairs.push({what, where});
       }
   );
}

BfGuiCreateWindow*
BfGuiCreateWindow::instance() noexcept
{
   return __instance;
}

BfGuiCreateWindow::foundContainer
BfGuiCreateWindow::__findContainer(
    std::list<ptrContainer>::iterator begin,
    std::list<ptrContainer>::iterator end,
    const std::string& name
)
{
   for (auto it = begin; it != end; ++it)
   {
      if ((*it)->name() == name)
      {
         return it;  // Found the container, return iterator
      }
      if (!(*it)->isEmpty())
      {
         auto nested_result =
             __findContainer((*it)->begin(), (*it)->end(), name);
         if (nested_result)
         {
            return nested_result;  // Found in nested container
         }
      }
   }
   return std::nullopt;  // Not found
}

BfGuiCreateWindow::ptrPair
BfGuiCreateWindow::__findAndCheckPair(const pair& name)
{
   auto a =
       __findContainer(__containers.begin(), __containers.end(), name.first);
   auto b =
       __findContainer(__containers.begin(), __containers.end(), name.second);

   if (!a || !b)
   {
      std::stringstream ss;
      ss << "Some of the containers were not found: ";
      ss << "a " << (a ? " was found" : " was not found");
      ss << "; ";
      ss << "b " << (b ? " was found" : " was not found");

      std::cout << ss.str() << "\n";

      throw std::runtime_error(ss.str());
   }
   return {a, b};
}

void
BfGuiCreateWindow::__renderManagePanel()
{
   ImGui::BeginChild("##CreateWindowManagePanel", {0.0, 37.0f}, true);
   {
      if (!BfGuiCreateWindowContainerObj::isAnyMoving())
      {
         if (ImGui::Button("Container"))
         {
            __addContainerT<BfGuiCreateWindowContainer>();
         }
         ImGui::SameLine();
         if (ImGui::Button("ContainerObj"))
         {
            __addContainerT<BfGuiCreateWindowContainerObj>();
         }
         ImGui::SameLine();
         if (ImGui::Button("BladeSection"))
         {
            __addContainerT<BfGuiCreateWindowBladeSection>();
         }
         ImGui::SameLine();
         if (ImGui::Button("BladeBase"))
         {
            __addContainerT<BfGuiCreateWindowBladeBase>();
         }
         ImGui::SameLine();
         if (ImGui::Button("SingleLine"))
         {
            __addContainerT<BfGuiCreateWindowSingleLine>();
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

void
BfGetWindowsUnderMouse(std::vector<ImGuiWindow*>& www)
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

void
BfGuiCreateWindow::__renderContainers()
{
   std::stack<BfGuiCreateWindowContainer::ptrContainer> render_stack;
   std::list<std::pair<std::string, int>> l;

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

         if (current_window->isCollapsed())
         {
         }

         for (auto child = current_window->begin();
              child != current_window->end();
              ++child)
         {
            if (current_window->isCollapsed())
            {
               if (!child->get()->isForceRender()) continue;
            }

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

// TODO: REMAKE -> to work with std::function ...
void
BfGuiCreateWindow::__renderDragDropZone()
{
   ImGui::Text("<=== Move here ===>");
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

         // Меняем режим отображения для 'BladeSection'-контейнера после дропа
         if (auto casted =
                 std::dynamic_pointer_cast<BfGuiCreateWindowBladeSection>(
                     dropped_container
                 ))
         {
            casted->setView(BfGuiCreateWindowBladeSection::viewMode::STD);
            if (casted->__layer_choser.get())
            {
               casted->rem(casted->__layer_choser);
            }
            if (casted->__height_choser.get())
            {
               casted->rem(casted->__height_choser);
            }
         }
      }
      ImGui::EndDragDropTarget();
   }
}

void
BfGuiCreateWindow::__processSwaps()
{
   while (!__swap_pairs.empty())
   {
      auto pair = __swap_pairs.top();
      //
      auto [a, b] = __findAndCheckPair(pair);
      ImVec2 pos_a = (*a)->get()->pos();
      ImVec2 pos_b = (*b)->get()->pos();
      //
      (*a)->get()->pos() = pos_b;
      (*b)->get()->pos() = pos_a;
      //
      BfGuiCreateWindowContainerObj::wptrContainer root_a = (*a)->get()->root();
      BfGuiCreateWindowContainerObj::wptrContainer root_b = (*b)->get()->root();
      //
      (*a)->get()->root() = root_b;
      (*b)->get()->root() = root_a;

      std::iter_swap(*a, *b);

      __swap_pairs.pop();
   }
}

void
BfGuiCreateWindow::__processMoves()
{
   while (!__move_pairs.empty())
   {
      std::cout << "MOVE__1\n";
      auto pair = __move_pairs.top();
      //
      auto optPair = __findAndCheckPair(pair);
      auto what = *(*optPair.first);
      auto where = *(*optPair.second);

      where->add(what);

      auto wptr_old_root = what->root();
      if (auto shared_old_root = wptr_old_root.lock())
      {
         shared_old_root->clearEmptyContainersByName(what->name());
      }
      else
      {
         __containers.remove_if([&what](auto c) {
            return c->name() == what->name();
         });
      }
      what->root() = where;

      __move_pairs.pop();
   }
}

void
BfGuiCreateWindow::__processEvents()
{
   __processSwaps();
   __processMoves();
}

void
BfGuiCreateWindow::removeByName(std::string name)
{
   std::erase_if(__containers, [&name](auto c) {
      return std::string(c->name()) == name;
   });
}

void
BfGuiCreateWindow::render()
{
   if (__is_render)
   {
      ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
      if (ImGui::Begin(
              "Create",
              &__is_render,
              ImGuiWindowFlags_NoBringToFrontOnFocus |
                  ImGuiWindowFlags_NoDocking
          ))
      {
         ImGui::PopStyleColor();
         __renderManagePanel();
         __renderContainers();
      }
      ImGui::End();
   }
   __processEvents();
}

void
BfGuiCreateWindow::toggleRender()
{
   __is_render = !__is_render;
}

/*
   зарендерить все окна слоями! сначала на одном
   уровне потом на другом и тд
*/
