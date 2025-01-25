#include "bfGuiLuaInteraction.h"

#include <algorithm>
#include <filesystem>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>

#include "bfConfigManager.h"
#include "bfCurves3.h"
#include "bfDrawObjectDefineType.h"
#include "imgui.h"
#include "imgui_internal.h"

BfGuiLuaDragDropElement::BfGuiLuaDragDropElement(BfObj obj)
    : __obj{obj}
{
}

std::string BfGuiLuaDragDropElement::type()
{
   std::string type_str = "INVALID";
   std::visit(
       [&](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, std::shared_ptr<BfDrawObj>>)
          {
             type_str = bfGetStrNameDrawObjType(arg->id.get_type());
          }
          else if constexpr (std::is_same_v<T, std::shared_ptr<BfDrawLayer>>)
          {
             type_str = bfGetStrNameDrawObjType(arg->id.get_type());
          }
          /*return arg->id.get_type();*/
       },
       __obj);
   return type_str;
}

std::string BfGuiLuaDragDropElement::id()
{
   std::string id_str = "INVALID";
   std::visit(
       [&](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, std::shared_ptr<BfDrawObj>>)
          {
             id_str = std::to_string(arg->id.get());
          }
          else if constexpr (std::is_same_v<T, std::shared_ptr<BfDrawLayer>>)
          {
             id_str = std::to_string(arg->id.get());
          }
          /*return arg->id.get_type();*/
       },
       __obj);
   return id_str;
}

void BfGuiLuaDragDropElement::draw(const ImVec2& out_window_pos,
                                   const ImVec2& out_window_size)
{
   std::string string_id =
       std::format("##{}{}BfGuiLuaDragDropSingleElement", type(), id());

   ImGui::SetNextWindowPos(__pos);
   ImGui::Begin(string_id.c_str(),
                nullptr,
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
   {
      ImVec2 element_size = ImGui::GetWindowSize();
      if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
      {
         __is_dragging = true;
      }
      if (ImGui::IsMouseReleased(0))
      {
         __is_dragging = false;
      }
      if (__is_dragging)
      {
         ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
         __pos.x += mouse_delta.x;
         __pos.y += mouse_delta.y;
         __pos.x =
             std::clamp(__pos.x,
                        out_window_pos.x,
                        out_window_pos.x + out_window_size.x - element_size.x);
         __pos.y =
             std::clamp(__pos.y,
                        out_window_pos.y,
                        out_window_pos.y + out_window_size.y - element_size.y);
      }

      ImGui::Text("Name: %s", __name.c_str());
      ImGui::Text("Type: %s", type().c_str());
   }
   ImGui::End();
}

ImVec2& BfGuiLuaDragDropElement::pos() noexcept { return __pos; }
bool&   BfGuiLuaDragDropElement::isDragging() noexcept { return __is_dragging; }

void BfGuiLuaInteraction::__render()
{
   static ImVec2 c_1_w = {ImGui::GetContentRegionAvail().x * 0.1f,
                          ImGui::GetContentRegionAvail().y};
   static ImVec2 c_2_w = {ImGui::GetContentRegionAvail().x * 0.45f,
                          ImGui::GetContentRegionAvail().y};
   static ImVec2 c_3_w = {ImGui::GetContentRegionAvail().x * 0.45f,
                          ImGui::GetContentRegionAvail().y};
   //
   bool is_button_resize_hovered = false;
   //
   static ImVec2 list_text_size = ImGui::GetContentRegionAvail();
   if (__is_render)
   {
      if (ImGui::Begin("Lua Interaction", &__is_render))
      {
         ImVec2 window_size = ImGui::GetWindowSize();
         float  button_w    = 10.0f;
         float  button_h    = ImGui::GetContentRegionAvail().y;
         //
         __renderImportScriptPannel();
         {
            ImGui::BeginChild("##BfGuiLuaInteractionListChild",
                              {-button_w + c_1_w.x, 0.0f},
                              false);
            {
               ImGui::SeparatorText("Files list");
               __renderList();
            }
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
            ImGui::InvisibleButton("##BfGuiLuaInteractionListAndTextResize1",
                                   {button_w, -1.0f});
            if (ImGui::IsItemHovered())
            {
               ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
               is_button_resize_hovered = true;
            }

            if (ImGui::IsItemActive())
            {
               c_1_w.x += ImGui::GetIO().MouseDelta.x;
               c_1_w.x =
                   std::ranges::clamp(c_1_w.x, 50.0f, window_size.x - button_w);
            }

            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);

            ImGui::BeginChild("##BfGuiLuaInteractionTextChild",
                              {-button_w * 2 + c_2_w.x, 0.0f},
                              false);
            {
               ImGui::SeparatorText("File preview");
               __renderFileText();
            }
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
            ImGui::InvisibleButton("##BfGuiLuaInteractionListAndTextResize2",
                                   {button_w, -1.0f});

            if (ImGui::IsItemHovered())
            {
               ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
               is_button_resize_hovered = true;
            }

            if (ImGui::IsItemActive())
            {
               c_2_w.x += ImGui::GetIO().MouseDelta.x;
               c_2_w.x =
                   std::ranges::clamp(c_2_w.x, 50.0f, window_size.x - button_w);
            }
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
            ImGui::BeginChild("##BfGuiLuaInteractionTableChild", {}, false);
            {
               ImGui::SeparatorText("Table preview");
               if (__selected_script)
                  __renderLuaTable(__selected_script->read_table);
            }
            ImGui::EndChild();
         }
         __renderDrop();
      }
      if (!is_button_resize_hovered)
      {
         ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
      }
      ImGui::End();
   }
}

void BfGuiLuaInteraction::__renderImportScriptPannel()
{
   if (ImGui::Button("Open single script"))
   {
      BfGuiFileDialog::instance()->openFile(&__new_script, {".lua"});
      __is_file_exploring = true;
   }
   ImGui::SameLine();
   if (ImGui::Button("Open many scripts"))
   {
      BfGuiFileDialog::instance()->openDir(&__new_script);
      __is_file_exploring = true;
   }
   ImGui::SameLine();
   if (ImGui::Button("Run script"))
   {
      __runSelectedScript();
   }
}

void BfGuiLuaInteraction::__renderList()
{
   // ImGui::BeginListBox("##Added lua-scripts",
   ImGui::BeginListBox("##Added lua-scripts", ImGui::GetContentRegionAvail());
   {
      for (auto script = __scripts.begin(); script != __scripts.end(); ++script)
      {
         const bool is_selected = (__selected_script == &(*script));
         // if (ImGui::Selectable(script->path.filename().c_str(), is_selected))
         // {
         //    __selected_script = &(*script);
         // }

         if (is_selected) ImGui::SetItemDefaultFocus();
      }
   }
   ImGui::EndListBox();
}

void BfGuiLuaInteraction::__renderFileText()
{
   if (__selected_script)
      ImGui::InputTextMultiline("##BfGuiLuaInteractionFileTextExplorer",
                                __selected_script->text.data(),
                                __selected_script->text.size(),
                                ImGui::GetContentRegionAvail(),
                                ImGuiInputTextFlags_ReadOnly);
}

void BfGuiLuaInteraction::__renderLuaTable(std::shared_ptr<BfLuaTable> script)
{
   if (script)
   {
      for (const auto& [key, val] : *script)
      {
         if (auto nested_table = std::get_if<std::shared_ptr<BfLuaTable>>(&val))
         {
            if (ImGui::TreeNode(BfLuaTable::convert(key).c_str()))
            {
               __renderLuaTable(*nested_table);
               ImGui::TreePop();
            }
         }
         else
         {
            ImGui::Text("%s = %s",
                        BfLuaTable::convert(key).c_str(),
                        BfLuaTable::convert(val).c_str());
         }
      }
   }
}

void BfGuiLuaInteraction::__renderDrop()
{
   // BfGuiLuaDragDropEvent_ mode = BfGuiLuaDragDropEvent_Swap;
   // //
   // static const char* names[6] = {
   //     "Section 1",
   //     "Section 2",
   //     "Section 3",
   //     "Section 4",
   //     "Section 5",
   //     "Section 6",
   // };
   // ImGui::BeginChild("##BfGUiLuaDragDropList", {}, true);
   // ImGui::Text("Base");
   // for (int n = 0; n < IM_ARRAYSIZE(names); n++)
   // {
   //    ImGui::PushID(n);
   //    /*if ((n % 3) != 0) ImGui::SameLine();*/
   //    ImGui::Button(names[n], ImVec2(80, 20));
   //    // Our buttons are both drag sources and drag targets here!
   //    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
   //    {
   //       // Set payload to carry the index of our item (could be anything)
   //       ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));
   //
   //       // Display preview (could be anything, e.g. when dragging an
   //       image we
   //       // could decide to display the filename and a small preview of
   //       the
   //       // image, etc.)
   //       if (mode == BfGuiLuaDragDropEvent_Copy)
   //       {
   //          ImGui::Text("Copy %s", names[n]);
   //       }
   //       if (mode == BfGuiLuaDragDropEvent_Move)
   //       {
   //          ImGui::Text("Move %s", names[n]);
   //       }
   //       if (mode == BfGuiLuaDragDropEvent_Swap)
   //       {
   //          ImGui::Text("Swap %s", names[n]);
   //       }
   //       ImGui::EndDragDropSource();
   //    }
   //    if (ImGui::BeginDragDropTarget())
   //    {
   //       if (const ImGuiPayload* payload =
   //               ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
   //       {
   //          IM_ASSERT(payload->DataSize == sizeof(int));
   //          int payload_n = *(const int*)payload->Data;
   //          if (mode == BfGuiLuaDragDropEvent_Copy)
   //          {
   //             names[n] = names[payload_n];
   //          }
   //          if (mode == BfGuiLuaDragDropEvent_Move)
   //          {
   //             names[n]         = names[payload_n];
   //             names[payload_n] = "";
   //          }
   //          if (mode == BfGuiLuaDragDropEvent_Swap)
   //          {
   //             const char* tmp  = names[n];
   //             names[n]         = names[payload_n];
   //             names[payload_n] = tmp;
   //          }
   //       }
   //       ImGui::EndDragDropTarget();
   //    }
   //    ImGui::PopID();
   // }
   // ImGui::EndChild();
   //
   // static bool                                 is_start = true;
   // static std::vector<BfGuiLuaDragDropElement> elems;
   // if (is_start)
   // {
   //    BfVertex3 f({0.0f, 0.0f, 0.0f});
   //    BfVertex3 s({1.0f, 1.0f, 0.0f});
   //    auto      line = std::make_shared<BfSingleLine>(f, s);
   //    elems.push_back({line});
   //
   //    is_start = false;
   // }
   //
   // static ImVec2 oldwsize = ImGui::GetWindowSize();
   // static ImVec2 oldwpos  = ImGui::GetWindowPos();
   // //
   // ImVec2 wsize = ImGui::GetWindowSize();
   // ImVec2 wpos  = ImGui::GetWindowPos();

   // ImGui::BeginChild("3u123813dsa", {}, true);
   // {
   //    for (auto& elem : elems)
   //    {
   //       std::string string_id =
   //           std::format("##{}{}BfGuiLuaDragDropSingleElement",
   //                       elem.type(),
   //                       elem.id());
   //
   //       if ((oldwsize.x != wsize.x && oldwsize.y != wsize.y) ||
   //           (oldwpos.x != wpos.x && oldwpos.y != wpos.y))
   //       {
   //          // ImVec2 new_pos = {
   //          // elem.pos().x + (oldwpos.x - wpos.x) + (oldwsize.x -
   //          wsize.x),
   //          // elem.pos().y + (oldwpos.y - wpos.y) + (oldwsize.y -
   //          wsize.y)};
   //
   //          // ImGui::SetNextWindowPos(new_pos);
   //          // ImGui::SetNextWindowSize(wsize);
   //
   //          // oldwsize = wsize;
   //          // oldwpos  = wpos;
   //       }
   //
   //       elem.draw(wpos, wsize);
   //    }
   // }
   // ImGui::EndChild();
}

void BfGuiLuaInteraction::__runSelectedScript()
{
   std::string script_name = __selected_script->path.stem().string();
   BfConfigManager::loadRequireScript(fs::absolute(__selected_script->path),
                                      script_name);
   __selected_script->table      = BfConfigManager::getLuaObj(script_name);
   __selected_script->read_table = std::make_shared<BfLuaTable>(
       std::move(BfConfigManager::convertLuaTable(&__selected_script->table)));
}

void BfGuiLuaInteraction::draw() { __render(); }

void BfGuiLuaInteraction::pollEvents()
{
   if (!(BfGuiFileDialog::instance()->isActive()) && __is_file_exploring)
   {
      if (fs::is_directory(__new_script))
      {
         for (const auto& item : fs::directory_iterator(__new_script))
         {
            __scripts.push_back({item, ""});
            readFileText(__scripts.rbegin()->path, &__scripts.rbegin()->text);
         }
      }
      else if (fs::is_regular_file(__new_script))
      {
         __scripts.push_back({__new_script, ""});
         readFileText(__scripts.rbegin()->path, &__scripts.rbegin()->text);
      }

      __new_script        = "";
      __is_file_exploring = false;
   }
}

BfEvent BfGuiLuaInteraction::readFileText(fs::path path, std::string* text)
{
   BfSingleEvent event{};
   event.type = BF_SINGLE_EVENT_TYPE_USER_EVENT;
   event.info = " " + path.string();
   std::ifstream file(path);
   if (!file.is_open())
   {
      event.action = BF_ACTION_TYPE_READ_FILE_FAILURE;
      return event;
   }
   else
   {
      event.action = BF_ACTION_TYPE_READ_FILE_FAILURE;
   }
   *text = std::string(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
   return event;
}
