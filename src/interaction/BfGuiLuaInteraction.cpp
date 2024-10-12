#include "BfGuiLuaInteraction.h"

#include <algorithm>
#include <filesystem>
#include <iterator>

#include "imgui.h"

void BfGuiLuaInteraction::__render()
{
   static ImVec2 list_text_size = ImGui::GetContentRegionAvail();
   if (__is_render)
   {
      if (ImGui::Begin("Lua Interaction", &__is_render))
      {
         ImVec2 window_size = ImGui::GetWindowSize();
         float  button_w    = 10.0f;
         float  button_h    = ImGui::GetContentRegionAvail().y;

         __renderImportScriptPannel();
         // ImGui::BeginChild("##BfGuiLuaInteractionListChild", {}, true);
         ImGui::BeginChild("##BfGuiLuaInteractionListChild",
                           {-button_w + list_text_size.x, 0.0f},
                           true);
         {
            __renderList();
         }
         ImGui::EndChild();
         ImGui::SameLine();
         ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
         ImGui::Button("##BfGuiLuaInteractionListAndTextResize",
                       {button_w, 100.0f});

         ImGui::IsItemHovered()
             ? ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW)
             : ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

         if (ImGui::IsItemActive())
         {
            list_text_size.x += ImGui::GetIO().MouseDelta.x;
            list_text_size.x = std::ranges::clamp(list_text_size.x,
                                                  50.0f,
                                                  window_size.x - button_w);
         }

         ImGui::SameLine();

         ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
         ImGui::BeginChild("##BfGuiLuaInteractionTextChild", {}, true);
         {
            __renderFileText();
         }
         ImGui::EndChild();
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
}

void BfGuiLuaInteraction::__renderList()
{
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
      ImGui::InputTextMultiline(
          "##BfGuiLuaInteractionFileTextExplorer",
          __selected_script->text.data(),
          __selected_script->text.size(),
          ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 20),
          ImGuiInputTextFlags_ReadOnly);
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
