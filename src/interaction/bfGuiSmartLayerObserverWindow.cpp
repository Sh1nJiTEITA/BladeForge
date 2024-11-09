#include <bfGuiSmartLayerObserverWindow.h>

#include <type_traits>

#include "bfIconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_internal.h"

uint32_t BfGuiSmartLayerObserver::__current_moving_id = 0;

BfGuiSmartLayerObserver::BfGuiSmartLayerObserver()
    : BfGuiWindow{{.name = "Smart layer observer", .isRender = true, .toggleRenderMapping = {ImGuiKey_LeftCtrl, ImGuiKey_T}}
      }
    , __current_transaction_type{__transaction_t::MOVE}
{
}

void
BfGuiSmartLayerObserver::__bodyRender()
{
   __startModularPopups();

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
   __renderModularPopups();
}

void
BfGuiSmartLayerObserver::__renderSingleLayer(std::shared_ptr<BfDrawLayer> l)
{
   std::string l_name = layerName(l).c_str();

   __pushMovingHandleStyle();
   ImGui::PushID(l_name.c_str());
   {
      if (!__current_moving_id || __current_moving_id == l->id.get())
      {
         ImGui::Button("\uf256");
         // __renderDragDropSourceLayer(l, l_name.c_str());
         __renderDragDropSource(l, l_name.c_str());
      }
      else
      {
         ImGui::Button(ICON_FA_TAG);
         // __renderDragDropTargetLayer(l, l_name.c_str());
         __renderDragDropTarget(l, l_name.c_str());
      }
   }
   ImGui::PopID();

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

         ImGui::PushID(o_name.c_str());
         {
            ImGui::Button("\uf256");
            // __renderDragDropSourceObj(obj, o_name.c_str());
         }
         ImGui::PopID();

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
       ("\tVertices " + std::to_string(o->get_vertices_count())) +
       ("\n\tIndices " + std::to_string(o->get_indices_count())) +
       ("\n\tDVertices " + std::to_string(o->get_dvertices_count()))
   );
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
             [&](auto&& what, auto&& where) {
                using what_t = std::decay_t<decltype(what)>;
                using where_t = std::decay_t<decltype(where)>;

                if constexpr (std::is_same_v<what_t, ptrLayer> &&
                              std::is_same_v<where_t, ptrLayer>)
                {
                   if (how == __transaction_t::MOVE)
                   {
                   }
                   else
                   {
                      ptrLayer& ref_what =
                          lh->get_ref_find_layer(what->id.get());
                      ptrLayer& ref_where =
                          lh->get_ref_find_layer(where->id.get());
                      std::cout << ref_what << "(" << ref_what.use_count()
                                << ") : (" << ref_where << "(" << ref_where
                                << ")\n";
                      std::cout << "what: " << ref_what->id.get() << "\n";
                      std::cout << "where: " << ref_where->id.get() << "\n";
                      //

                      if ((!ref_where->is_nested() && ref_what->is_nested()) ||
                          (ref_where->is_nested() && !ref_what->is_nested()))
                      {
                         // ImGui
                         std::cout << "HERE\n";
                         // ImGui::OpenPopup(
                         //     "Smart layer objserver nested/non nested layer"
                         // );
                         //
                         __popups.push(
                             "Smart layer objserver nested/non nested layer"
                         );
                      }
                      else
                      {
                         std::swap(ref_what, ref_where);
                      }
                   }
                }
                if constexpr (std::is_same_v<what_t, ptrLayer> &&
                              std::is_same_v<where_t, ptrObj>)
                {
                }
                if constexpr (std::is_same_v<what_t, ptrObj> &&
                              std::is_same_v<where_t, ptrLayer>)
                {
                }
                if constexpr (std::is_same_v<what_t, ptrObj> &&
                              std::is_same_v<where_t, ptrObj>)
                {
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

         std::cout
             << "Moved "
             << std::visit([](auto&& arg) { return arg->id.get(); }, input)
             << " to "
             << std::visit([](auto&& arg) { return arg->id.get(); }, ptr)
             << "\n";

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
   if (ImGui::BeginPopupModal("Smart layer objserver nested/non nested layer"))
   {
      ImGui::Text("Swapping nested layer with not nested is not possible");
      if (ImGui::Button("OK", ImVec2(120, 0)))
      {
         ImGui::CloseCurrentPopup();
      }
      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(120, 0)))
      {
         ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
   }
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
