#include "bfGuiCreateWindowBladeBase.h"

#include <memory>

#include "bfGuiCreateWindowBladeSection.h"
#include "bfGuiCreateWindowContainer.h"
#include "bfIconsFontAwesome6.h"
#include "imgui.h"

void
BfGuiCreateWindowBladeBase::__setContainersPos()
{
   ImVec2 avail = size();
   // float next_container_h = ImGui::GetStyle().WindowPadding.y * 9.5;
   float next_container_h = ImGui::GetStyle().WindowPadding.y * 9.5;
   for (auto& c : __containers)
   {
      if (__isWindowContainerBladeSection(c))
      {
         c->pos().y = pos().y + next_container_h;
         next_container_h += c->size().y * 0.8;
      }
      else
      {
      }
   }
}

void
BfGuiCreateWindowBladeBase::__createObj()
{
   bfFillBladeBaseStandart(&__create_info);
   __create_info.layer_create_info = __layer_create_info;
   for (auto c : __containers)
   {
      // if (__isWindowContainerBladeSection(c))
      if (auto section_container =
              std::dynamic_pointer_cast<BfGuiCreateWindowBladeSection>(c))
      {
         __create_info.section_infos.push_back(
             std::move(section_container->createInfo())
         );
      }
   }
   __layer_obj = std::make_shared<BfBladeBase>(__create_info);
}

bool
BfGuiCreateWindowBladeBase::__isWindowContainerBladeSection(ptrContainer c)
{
   // return __settings_popup || __layer_choser
   //            ? c->id() != __settings_popup->id() &&
   //                  c->id() != __layer_choser->id()
   //            : true;
   //
   return (bool)std::dynamic_pointer_cast<BfGuiCreateWindowBladeSection>(c);
}

void
BfGuiCreateWindowBladeBase::__renderChildContent()
{
   if (__settings_popup)
   {
   }
   if (ImGui::Button("Open section settings"))
   {
      if (!__settings_popup)
      {
         __settings_popup = std::make_shared<BfGuiCreateWindowContainerPopup>(
             shared_from_this(),
             BfGuiCreateWindowContainerPopup_Side_Left,
             true,
             [this](wptrContainer root) {
                for (auto c : this->__containers)
                {
                   if (c->id() == __selected_id)
                   {
                      c->renderBodyView();
                   }
                }
             }
         );
         __settings_popup->setSize({500, 350});
         this->add(__settings_popup);
      }
      else
      {
         __settings_popup->toggleRender();
      }
   }
   ImGui::SameLine();
   if (ImGui::Button("Add to layer"))
   {
      if (!__layer_choser)
      {
         __layer_choser = std::make_shared<BfGuiCreateWindowContainerPopup>(
             shared_from_this(),
             BfGuiCreateWindowContainerPopup_Side_Right,
             true,
             [this](wptrContainer root) {
                BfGuiSmartLayerObserver::instance()->renderChoser([&]() {
                   size_t selected_id =
                       BfGuiSmartLayerObserver::instance()->selectedLayer();
                   std::shared_ptr<BfDrawLayer> selected_layer =
                       BfLayerHandler::instance()->get_layer_by_id(selected_id);
                   __addToLayer(selected_layer);
                   __layer_choser->toggleRender(false);
                });
             }
         );
         __layer_choser->setSize({400, 350});
         this->add(__layer_choser);
      }
      else
      {
         __layer_choser->toggleRender();
      }
   }
}

BfGuiCreateWindowBladeBase::BfGuiCreateWindowBladeBase(
    wptrContainer root, bool is_target
)
    : BfGuiCreateWindowContainerObj(root, is_target)
{
   __window_size = {600, 400};

   bfFillBladeBaseStandart(&__create_info);
   __create_info.layer_create_info = __layer_create_info;
}

void
BfGuiCreateWindowBladeBase::addHeightPopup(
    std::shared_ptr<BfGuiCreateWindowBladeSection> s
)
{
   s.get()->__mode = BfGuiCreateWindowBladeSection::viewMode::SHORT;

   auto h_popup = std::make_shared<BfGuiCreateWindowContainerPopup>(
       s,
       BfGuiCreateWindowContainerPopup_Side_Left,
       true,
       [](wptrContainer root) {}
   );

   h_popup->__renderPopupContentFunc = [](wptrContainer root) {
      if (auto shared_root = root.lock())
      {
         if (auto casted_root =
                 std::dynamic_pointer_cast<BfGuiCreateWindowBladeSection>(
                     shared_root
                 ))
         {
            ImGui::PushID("InputSectionHeight");
            ImGui::PushStyleColor(ImGuiCol_FrameBg, {1.0, 1.0f, 1.0f, 0.0f});
            ImGui::PushStyleColor(
                ImGuiCol_FrameBgHovered,
                {1.0, 1.0f, 1.0f, 0.2f}
            );
            ImGui::PushStyleColor(
                ImGuiCol_FrameBgActive,
                {1.0, 1.0f, 1.0f, 0.5f}
            );
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.5f);
            ImGui::Text(ICON_FA_RULER_VERTICAL);
            ImGui::SameLine();
            ImGui::SetCursorPosY(ImGui::GetCursorStartPos().y);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputFloat(
                "##InputSectionHeight",
                &casted_root->zCoordinate()
            );
            ImGui::PopID();
            ImGui::PopStyleColor(3);
         }
      }
   };

   h_popup->toggleButton(BfGuiCreateWindowContainer_ButtonType_Left, false);
   h_popup->toggleButton(BfGuiCreateWindowContainer_ButtonType_Top, false);
   h_popup->toggleButton(BfGuiCreateWindowContainer_ButtonType_Right, false);
   h_popup->toggleButton(BfGuiCreateWindowContainer_ButtonType_Bot, false);
   h_popup->toggleHeader(false);
   h_popup->setSize({200, 80});

   s->add(h_popup);
   s->__height_choser = h_popup;
}

uint32_t*
BfGuiCreateWindowBladeBase::selectedId() noexcept
{
   return &__selected_id;
}

void
BfGuiCreateWindowBladeBase::__processDragDropTarget()
{
   if (ImGui::BeginDragDropTarget())
   {
      if (const ImGuiPayload* payload =
              ImGui::AcceptDragDropPayload("Container"))
      {
         std::shared_ptr<BfGuiCreateWindowContainerObj> dropped_container =
             *(std::shared_ptr<BfGuiCreateWindowContainerObj>*)payload->Data;

         if (auto other_base =
                 std::dynamic_pointer_cast<BfGuiCreateWindowBladeBase>(
                     dropped_container
                 ))
         {
            __swapFunc(other_base->name(), this->name());
         }
         else if (auto other_section =
                      std::dynamic_pointer_cast<BfGuiCreateWindowBladeSection>(
                          dropped_container
                      ))
         {
            addHeightPopup(other_section);
            __moveFunc(other_section->name(), this->name());
         }
         else
         {
         }
      }
      ImGui::EndDragDropTarget();
   }
}

void
BfGuiCreateWindowBladeBase::__prerender()
{
   __setContainersPos();
}

void
BfGuiCreateWindowBladeBase::__postrender()
{
   for (auto& c : __containers)
   {
      if (__isWindowContainerBladeSection(c))
      {
         auto section =
             std::dynamic_pointer_cast<BfGuiCreateWindowBladeSection>(c);

         section->__prerender_external_func = nullptr;
         section->__postrender_external_func = nullptr;
      }
   }
}
