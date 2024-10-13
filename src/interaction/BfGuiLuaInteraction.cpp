#include "BfGuiLuaInteraction.h"

#include <algorithm>
#include <filesystem>
#include <iterator>
#include <memory>

#include "bfConfigManager.h"
#include "imgui.h"
#include "imgui_internal.h"

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
         }
         ImGui::SameLine();
         ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
         {
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
         }
         ImGui::SameLine();
         ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
         {
            ImGui::BeginChild("##BfGuiLuaInteractionTableChild", {}, false);
            {
               ImGui::SeparatorText("Table preview");
               if (__selected_script)
                  __renderLuaTable(__selected_script->read_table);
            }
            ImGui::EndChild();
         }
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
         if (ImGui::Selectable(script->path.filename().c_str(), is_selected))
         {
            __selected_script = &(*script);
         }

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
