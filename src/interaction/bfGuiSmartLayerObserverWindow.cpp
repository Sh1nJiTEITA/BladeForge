#include <bfGuiSmartLayerObserverWindow.h>

#include <functional>
#include <type_traits>

#include "bfIconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_internal.h"

uint32_t BfGuiSmartLayerObserver::__current_moving_id = 0;
BfGuiSmartLayerObserver* BfGuiSmartLayerObserver::__instance = nullptr;

BfGuiSmartLayerObserver::BfGuiSmartLayerObserver()
    : BfGuiWindow{{.name = "Smart layer observer", .isRender = true, .toggleRenderMapping = {ImGuiKey_LeftCtrl, ImGuiKey_T}}
      }
    , __current_transaction_type{__transaction_t::MOVE}
{
   if (!__instance) __instance = this;
}

void
BfGuiSmartLayerObserver::__bodyRender()
{
   __startModularPopups();
   {
      if (__mode == viewMode::STD)
      {
         ImGui::Text("Handle function");
         ImGui::SameLine();
         if (ImGui::RadioButton(
                 "Move",
                 __current_transaction_type == __transaction_t::MOVE
             ))
         {
            __current_transaction_type = __transaction_t::MOVE;
         }
         ImGui::SameLine();
         if (ImGui::RadioButton(
                 "Swap",
                 __current_transaction_type == __transaction_t::SWAP
             ))
         {
            __current_transaction_type = __transaction_t::SWAP;
         }
      }
      else if (__mode == viewMode::CHOOSE)
      {
         if (ImGui::Button("Complete adding"))
         {
            if (__choose_func) __choose_func();
         }
      }
      ImGui::Separator();

      auto lh = BfLayerHandler::instance();
      for (int i = 0; i < lh->get_layer_count(); ++i)
      {
         __renderSingleLayer(lh->get_layer_by_index(i));
      }

      if (!ImGui::IsDragDropActive())
      {
         __current_moving_id = 0;
      }
   }
   // else if (__mode == viewMode::CHOOSE)
   {
   }

   __renderModularPopups();
}

void
BfGuiSmartLayerObserver::__renderSingleLayer(std::shared_ptr<BfDrawLayer> l)
{
   std::string l_name = layerName(l).c_str();

   __pushMovingHandleStyle();
   if (__mode == viewMode::STD)
   {
      ImGui::PushID(l_name.c_str());
      {
         if (!__current_moving_id || __current_moving_id == l->id.get())
         {
            ImGui::Button("\uf256");
            __renderDragDropSource(l, l_name.c_str());
         }
         else
         {
            ImGui::Button(ICON_FA_TAG);
            __renderDragDropTarget(l, l_name.c_str());
         }
      }
      ImGui::PopID();
   }
   else
   {
      ImGui::PushID(l_name.c_str());
      if (ImGui::RadioButton(
              std::string("##" + l_name).c_str(),
              __selected_id == l->id.get()
          ))
      {
         __selected_id = l->id.get();
      }
      ImGui::PopID();
   }

   ImGui::SameLine();
   // if (ImGui::TreeNodeEx(l_name.c_str(), ImGuiTreeNodeFlags_FramePadding))
   if (ImGui::TreeNodeEx(l_name.c_str()))
   {
      // if (ImGui::IsItemHovered() && ImGui::IsItemClicked(0))

      for (int i = 0; i < l->get_layer_count(); ++i)
      {
         __renderSingleLayer(l->get_layer_by_index(i));
      }

      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.55f, 0.1f, 1.0f));
      for (int i = 0; i < l->get_obj_count(); ++i)
      {
         auto obj = l->get_object_by_index(i);
         std::string o_name = objName(obj);

         if (__mode == viewMode::STD)
         {
            ImGui::PushID(o_name.c_str());
            {
               if (!__current_moving_id || __current_moving_id == obj->id.get())
               {
                  ImGui::Button("\uf256");
                  __renderDragDropSource(obj, o_name.c_str());
               }
               else
               {
                  ImGui::Button(ICON_FA_TAG);
                  __renderDragDropTarget(obj, o_name.c_str());
               }
            }
            ImGui::PopID();
         }
         else
         {
            ImGui::PushID(o_name.c_str());
            if (ImGui::RadioButton(
                    std::string("##" + o_name).c_str(),
                    __selected_id == obj->id.get()
                ))
            {
               __selected_id = obj->id.get();
            }
            ImGui::PopID();
         }

         ImGui::SameLine();
         if (ImGui::TreeNodeEx(o_name.c_str()))
         {
            ImGui::PushStyleColor(
                ImGuiCol_Text,
                ImVec4(1.0f, 0.35f, 0.1f, 1.0f)
            );
            {
               ImGui::Text("%s", objData(obj).c_str());
            }
            ImGui::PopStyleColor();
            ImGui::TreePop();
         }
      }
      ImGui::PopStyleColor();
      ImGui::TreePop();
   }
   __popMovingHandleStyle();
}

void
BfGuiSmartLayerObserver::__renderDragDropSourceLayer(
    ptrLayer l, const char* name
)
{
   if (ImGui::BeginDragDropSource())
   {
      ImGui::SetDragDropPayload("Moving draw layer", &l, sizeof(l));
      if (ImGui::TreeNodeEx(name)) ImGui::TreePop();
      ImGui::EndDragDropSource();
      __current_moving_id = l->id.get();
   }
}

void
BfGuiSmartLayerObserver::__renderDragDropSourceObj(ptrObj o, const char* name)
{
   if (ImGui::BeginDragDropSource())
   {
      ImGui::SetDragDropPayload("Moving draw object", o.get(), sizeof(*o));
      if (ImGui::TreeNodeEx(name)) ImGui::TreePop();
      ImGui::EndDragDropSource();
      __current_moving_id = o->id.get();
   }
}

void
BfGuiSmartLayerObserver::__renderDragDropTargetLayer(
    ptrLayer l, const char* name
)
{
   if (ImGui::BeginDragDropTarget())
   {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("Moving draw layer"))
      {
         ptrLayer input_l = *(ptrLayer*)payload->Data;
         __current_moving_id = 0;
      }
      ImGui::EndDragDropTarget();
   }
}

void
BfGuiSmartLayerObserver::__renderDragDropTargetObj(ptrObj o, const char* name)
{
   if (ImGui::BeginDragDropTarget())
   {
      //    // if (const ImGuiPayload* payload =
      //            // ImGui::AcceptDragDropPayload("Moving draw object"))
      //    {
      //       // ptrObj l = *(ptrObj*)payload->Data;
      //       // __current_moving_id = 0;
      //       std::cout << "DONE->";  // << l->id.get();
      //    }

      ImGui::EndDragDropTarget();
   }
}

BfGuiSmartLayerObserver*
BfGuiSmartLayerObserver::instance()
{
   return __instance;
}

std::string
BfGuiSmartLayerObserver::layerName(std::shared_ptr<BfDrawLayer> l)
{
   return std::string(
       "\uf5fd"
       " (" +
       bfGetStrNameDrawObjType(l->id.get_type()) + ") " +
       std::to_string(l->id.get())
   );
}

std::string
BfGuiSmartLayerObserver::objName(std::shared_ptr<BfDrawObj> o)
{
   return std::string(
       "\uf1b2"
       " (" +
       bfGetStrNameDrawObjType(o->id.get_type()) + ") " +
       std::to_string(o->id.get())
   );
}

std::string
BfGuiSmartLayerObserver::objData(std::shared_ptr<BfDrawObj> o)
{
   return std::string(
       ("\tVertices " + std::to_string(o->vertices().size())) +
       ("\n\tIndices " + std::to_string(o->indices().size())) +
       ("\n\tDVertices " + std::to_string(o->dVertices().size()))
   );
}

void
BfGuiSmartLayerObserver::renderBodyView(viewMode mode)
{
   auto old_mode = __mode;
   __mode = mode;
   {
      __bodyRender();
   }
   __mode = old_mode;
}

void
BfGuiSmartLayerObserver::renderChoser(std::function<void()> button)
{
   __choose_func = button;
   this->renderBodyView(viewMode::CHOOSE);
   __choose_func = nullptr;
}

size_t
BfGuiSmartLayerObserver::selectedLayer() const noexcept
{
   return __selected_id;
}

void
BfGuiSmartLayerObserver::pollEvents()
{
   auto lh = BfLayerHandler::instance();
   while (!__transactionQueue.empty())
   {
      auto transaction = __transactionQueue.back();
      __transactionQueue.pop();
      auto how = transaction.how;
      {
         std::visit(
             [this, &how, &transaction, &lh](auto&& what, auto&& where) {
                using what_t = std::decay_t<decltype(what)>;
                using where_t = std::decay_t<decltype(where)>;

                if (how == __transaction_t::MOVE)
                {
                   lh->move_inner(
                       what->id.get(),
                       where->id.get(),
                       [this](int mode) {
                          switch (mode)
                          {
                             case 0:
                                this->__popups.push(
                                    "Source or target layer/object does not "
                                    "exist"
                                );
                                break;
                             case 1:
                                this->__popups.push(
                                    "Not nested layer can't be moved"
                                );
                                break;
                             case 2:
                                this->__popups.push(
                                    "Moving layer/object inside object"
                                );
                                break;
                          }
                       }
                   );
                }
                else
                {
                   lh->swap_inner(
                       what->id.get(),
                       where->id.get(),
                       [this](int mode) {
                          switch (mode)
                          {
                             case 0:
                                this->__popups.push(
                                    "Source or target layer/object does not "
                                    "exist"
                                );
                                break;
                          }
                       }

                   );
                }
             },
             transaction.what,
             transaction.where
         );
      }
   }
}

void
BfGuiSmartLayerObserver::__renderDragDropSource(
    std::variant<ptrLayer, ptrObj> ptr, const char* name
)
{
   if (ImGui::BeginDragDropSource())
   {
      ImGui::SetDragDropPayload(
          "Moving draw layer or draw obj",
          &ptr,
          sizeof(ptr)
      );
      if (ImGui::TreeNodeEx(name)) ImGui::TreePop();
      ImGui::EndDragDropSource();

      std::visit(
          [](auto&& arg) {
             // using T = std::decay_t<decltype(arg)>;
             // if constexpr (std::is_same_v<T, ptrLayer>)
             // {
             //    __current_moving_id = arg->id.get();
             // }
             // else if constexpr (std::is_same_v<T, ptrObj>)
             // {
             //    __current_moving_id = arg->id.get();
             // }
             __current_moving_id = arg->id.get();
          },
          ptr
      );
   }
}

void
BfGuiSmartLayerObserver::__renderDragDropTarget(
    std::variant<ptrLayer, ptrObj> ptr, const char* name
)
{
   if (ImGui::BeginDragDropTarget())
   {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("Moving draw layer or draw obj"))
      {
         std::variant<ptrLayer, ptrObj> input =
             *(std::variant<ptrLayer, ptrObj>*)payload->Data;

         // std::cout
         //     << "Moved "
         //     << std::visit([](auto&& arg) { return arg->id.get(); }, input)
         //     << " to "
         //     << std::visit([](auto&& arg) { return arg->id.get(); }, ptr)
         //     << "\n";

         __transactionQueue.push({input, ptr, __current_transaction_type});

         __current_moving_id = 0;
      }
      ImGui::EndDragDropTarget();
   }
}

void
BfGuiSmartLayerObserver::__renderModularPopups()
{
   ImVec2 center = ImGui::GetMainViewport()->GetCenter();
   ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

   auto show_modal_popup = [](const char* popup_name, const char* message) {
      ImGui::SetNextWindowSize(ImVec2(350, 150), ImGuiCond_Always);

      if (ImGui::BeginPopupModal(popup_name))
      {
         ImGui::TextWrapped("%s", message);

         ImGui::Dummy(ImVec2(0.0f, 30.0f));

         float windowWidth = ImGui::GetWindowSize().x;
         float buttonWidth = 120.0f;
         float spacing = ImGui::GetStyle().ItemSpacing.x;

         ImGui::SetCursorPosX(
             (windowWidth - (buttonWidth * 2.0f + spacing)) / 2.0f
         );

         if (ImGui::Button("OK", ImVec2(buttonWidth, 0)))
         {
            ImGui::CloseCurrentPopup();
         }

         ImGui::SameLine();

         if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
         {
            ImGui::CloseCurrentPopup();
         }

         ImGui::EndPopup();
      }
   };

   show_modal_popup(  // 0
       "Source or target layer/object does not exist",
       "Unhandled strange exception, nothing you could do"
   );
   show_modal_popup(  // 1
       "Not nested layer can't be moved",
       "You cant move nested layers. These layers holds buffers for rendering"
   );
   show_modal_popup(  // 2
       "Moving layer/object inside object",
       "Obly layers can hold objects or another layers, "
       "objects are single containers of vertices, simple geometry"
   );
}

void
BfGuiSmartLayerObserver::__startModularPopups()
{
   while (!__popups.empty())
   {
      ImGui::OpenPopup(__popups.front().c_str());
      __popups.pop();
   }
}

void
BfGuiSmartLayerObserver::__pushMovingHandleStyle()
{
   ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
   ImGui::PushStyleColor(
       ImGuiCol_ButtonHovered,
       ImVec4(1.0f, 1.0f, 1.0f, 0.0f)
   );
   ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 0.1f);
}

void
BfGuiSmartLayerObserver::__popMovingHandleStyle()
{
   ImGui::PopStyleColor(2);
   ImGui::PopStyleVar();
}
