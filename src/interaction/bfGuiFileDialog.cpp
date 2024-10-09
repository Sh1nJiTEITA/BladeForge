#include "bfGuiFileDialog.h"

#include <algorithm>
#include <exception>
#include <filesystem>
#include <iterator>
#include <string>

#include "bfGui.h"
#include "bfIconsFontAwesome6.h"
#include "imgui.h"
#include "imgui_internal.h"

const char* bfGetFileDialogElementTypeEmoji(BfFileDialogElementType_ e)
{
   static std::map<BfFileDialogElementType_, const char*> s{
       {BfFileDialogElementType_None, "\uefa4"},
       {BfFileDialogElementType_Directory, ICON_FA_FOLDER},
       {BfFileDialogElementType_DirectoryEmpty, ICON_FA_FOLDER_MINUS},
       {BfFileDialogElementType_RegularFile, ICON_FA_FILE},
       {BfFileDialogElementType_LuaFile, ICON_FA_CODE},
       // {BfFileDialogElementType_BackDirectory, ICON_FA_ARROW_ROTATE_LEFT},
       {BfFileDialogElementType_BackDirectory, ICON_FA_REPLY},
   };
   return s.at(e);
}

const ImVec4 bfGetFileDialogElementTypeColor(BfFileDialogElementType_ e)
{
   static std::map<BfFileDialogElementType_, const ImVec4> s{
       {BfFileDialogElementType_None, {0.0f, 0.0f, 0.0f, 1.0f}},
       {BfFileDialogElementType_Directory, {255.0f, 165.0f, 0.0f, 1.0f}},
       {BfFileDialogElementType_DirectoryEmpty, {255.0f, 200.0f, 100.0f, 1.0f}},
       {BfFileDialogElementType_RegularFile, {255.0f, 255.0f, 255.0f, 1.0f}},
       {BfFileDialogElementType_LuaFile, {10.0f, 20.0f, 255.0f, 1.0f}},
       {BfFileDialogElementType_BackDirectory, {255.0f, 255.0f, 255.0f, 1.0f}},
   };
   return s.at(e);
}

BfGuiFileDialog::BfGuiFileDialog() {}

void BfGuiFileDialog::setRoot(fs::path root) noexcept
{
   __root = (fs::canonical(root));
   update();
}

void BfGuiFileDialog::setExtension(std::initializer_list<std::string> list)
{
   __extensions = list;
   if (!__extensions.empty()) __chosen_ext = 0;
}

void BfGuiFileDialog::__render()
{
   if (__is_render)
   {
      if (ImGui::Begin("File Dialog", &__is_render))
      {
         if (ImGui::Button(ICON_FA_ARROW_LEFT))
         {
            goBack();
         }
         ImGui::SameLine();
         if (ImGui::Button(ICON_FA_REPEAT))
         {
            goTo(__root);
         }
         ImGui::SameLine();
         if (ImGui::Button(ICON_FA_ARROW_RIGHT))
         {
            goForward();
         }
         ImGui::SameLine();
         __renderPath();
         __renderTable();
         __renderWarning();
         ImGui::SeparatorText("Choose Extension");
         ImGui::Dummy({0, 20});
         __renderChosenFiles();
         ImGui::SameLine();
         __renderFileExtensionPicker();
      }
      ImGui::End();
   }
}

void BfGuiFileDialog::__renderPath()
{
   bool     found_hovered = false;
   bool     is_update     = false;
   fs::path new_path;
   for (auto it = __root.begin(); it != __root.end(); ++it)
   {
      if (!is_update) new_path /= *it;
      size_t      distance = std::distance(__root.begin(), it);
      std::string strid =
          std::move(it->string() + "##" + std::to_string(distance));

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
      std::string path_item;
#if defined(__linux__)
      if (distance)
         path_item = std::string("/") + strid;
      else
         path_item = strid;
#endif
      bool is_hovered = __hovered_item != -1 && __hovered_item >= distance;
      if (is_hovered)
      {
         ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                               ImVec4(0.26f, 0.59f, 0.98f, 1.0f));
         ImGui::PushStyleColor(ImGuiCol_Button,
                               ImVec4(0.26f, 0.59f, 0.98f, 1.0f));
      }
      if (ImGui::Button(path_item.c_str()))
      {
         is_update = true;
      }
      ImGui::PopStyleColor(1 + (is_hovered ? 2 : 0));
      if (ImGui::IsItemHovered() && !found_hovered)
      {
         __hovered_item = distance;
         found_hovered  = true;
      }
      ImGui::SameLine();
   }
   ImGui::Dummy({0, 0});
   if (!found_hovered) __hovered_item = -1;
   if (is_update)
   {
      goTo(new_path);
   }
}

void BfGuiFileDialog::__renderTable()
{
   bool     is_update = false;
   fs::path new_path;
   if (ImGui::BeginTable("##BfGuiFileDialogTable",
                         3,
                         /*ImGuiTableFlags_RowBg | */
                         ImGuiTableFlags_Sortable))
   {
      float table_w = ImGui::GetContentRegionAvail().x;
      float name_w  = 60.0f;
      float date_w  = 140.0f;

      ImGui::TableSetupColumn("Name",
                              ImGuiTableColumnFlags_WidthFixed,
                              table_w - name_w - date_w);
      ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, name_w);
      ImGui::TableSetupColumn("Last modified",
                              ImGuiTableColumnFlags_WidthFixed,
                              date_w);

      ImGui::TableHeadersRow();
      ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs();
      if (sort_specs) sort(sort_specs);
      int row_index = 0;
      for (auto& element : __elements)
      {
         ImGui::TableNextRow();
         ImGui::TableSetColumnIndex(0);

         ImGui::Selectable(("##" + element.path.string()).c_str(),
                           element.is_selected,
                           ImGuiSelectableFlags_SpanAllColumns);

         if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
         {
            if (element.type == BfFileDialogElementType_Directory ||
                element.type == BfFileDialogElementType_DirectoryEmpty)
            {
               new_path  = fs::absolute(__root / element.path);
               is_update = true;
            }
            else if (element.type == BfFileDialogElementType_BackDirectory)
            {
               new_path  = __root.parent_path();
               is_update = true;
            }
         }

         if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
         {
            element.is_selected = !element.is_selected;
         }

         ImGui::SameLine();
         ImGui::TextColored(bfGetFileDialogElementTypeColor(element.type),
                            "%s",
                            bfGetFileDialogElementTypeEmoji(element.type));
         ImGui::SameLine();
         ImGui::SetCursorPosX(40.0);
         ImGui::Text("%s", element.path.filename().c_str());
         ImGui::TableSetColumnIndex(1);
         ImGui::Text("%s",
                     (element.size ? std::to_string(element.size).c_str()
                                   : std::string("").c_str()));
         ImGui::TableSetColumnIndex(2);
         ImGui::Text("%s",
                     (element.date != std::time_t()
                          ? getLastWriteFileTime_string(element.date).c_str()
                          : std::string("").c_str()));
         row_index += 1;
      }
   }
   ImGui::EndTable();

   if (is_update) goTo(new_path);
}

void BfGuiFileDialog::__renderWarning()
{
   if (ImGui::BeginPopupModal("File dialog warning",
                              NULL,
                              ImGuiWindowFlags_AlwaysAutoResize))
   {
      ImGui::Text("%s", __warning_msg.c_str());
      if (ImGui::Button("Close"))
      {
         ImGui::CloseCurrentPopup();
         __warning_msg = "";
      }
      ImGui::EndPopup();
   }
}

void BfGuiFileDialog::__renderFileExtensionPicker()
{
   float window_w         = ImGui::GetContentRegionAvail().x;
   float window_padding_x = ImGui::GetStyle().WindowPadding.x * 0.5;
   float combo_w          = 80.0f;
   float combo_x          = window_w - combo_w + window_padding_x;
   ImGui::SetCursorPosX(combo_x);
   ImGui::SetNextItemWidth(combo_w);

   std::string chosen_ext =
       __chosen_ext == -1 ? "" : __extensions[__chosen_ext];
   if (ImGui::BeginCombo("##BfGuiFileDialogExtensionId", chosen_ext.c_str()))
   {
      for (size_t i = 0; i < __extensions.size(); ++i)
      {
         bool is_selected = (__chosen_ext == i);
         if (ImGui::Selectable(__extensions[i].c_str(), is_selected))
         {
            __chosen_ext = i;
         }
         if (is_selected)
         {
            ImGui::SetItemDefaultFocus();
         }
      }

      ImGui::EndCombo();
   }
}

void BfGuiFileDialog::__renderChosenFiles()
{
   std::ostringstream oss;
   for (auto element = __elements.begin(); element != __elements.end();
        element++)
   {
      size_t distance = std::distance(__elements.begin(), element);
      if (element->is_selected)
      {
         oss << element->path.filename();

         if (distance < __elements.size()) oss << ", ";
      }
   }
   std::string res = oss.str();
   ImGui::InputText("##BfGuiFileDialogRenderChosenFilesId",
                    const_cast<char*>(res.c_str()),
                    100000);
}

std::time_t BfGuiFileDialog::getLastWriteFileTime_time_t(const fs::path& path)
{
   fs::file_time_type file_time = fs::last_write_time(path);
   return bfUtilsToTimeT(file_time);
}

std::string BfGuiFileDialog::getLastWriteFileTime_string(const fs::path& path)
{
   // External solution
   std::time_t       tt  = getLastWriteFileTime_time_t(path);
   std::tm*          gmt = std::gmtime(&tt);
   std::stringstream buffer;
   buffer << std::put_time(gmt, "%d.%m.%Y %H:%M");
   return buffer.str();
}
std::string BfGuiFileDialog::getLastWriteFileTime_string(std::time_t time)
{
   std::tm*          gmt = std::gmtime(&time);
   std::stringstream buffer;
   buffer << std::put_time(gmt, "%d.%m.%Y %H:%M");
   return buffer.str();
}

size_t BfGuiFileDialog::getElementSize(const fs::path& root)
{
   if (fs::is_directory(root))
   {
      /*return getDirectorySize(root);*/
      return 0;
   }
   else if (fs::is_regular_file(root))
   {
      return getFileSize(root);
   }
   return 0;
}

size_t BfGuiFileDialog::getDirectorySize(const fs::path& root)
{
   size_t total_size = 0;
   for (const auto& item : fs::directory_iterator(root))
   {
      fs::path abs = fs::canonical(fs::absolute(root / item));
      if (!item.exists())
      {
         std::cerr << "File " << abs << " does not exist" << std::endl;
         abort();
      }
      if (item.is_directory())
      {
         total_size += getElementSize(abs);
      }
      else if (item.is_regular_file())
      {
         total_size += getFileSize(abs);
      }
   }
   return total_size;
}

size_t BfGuiFileDialog::getFileSize(const fs::path& path)
{
   return fs::file_size(path);
}

bool BfGuiFileDialog::isDirectoryEmpty(const fs::path& path)
{
   return std::filesystem::directory_iterator(path) ==
          std::filesystem::end(std::filesystem::directory_iterator());
}

void BfGuiFileDialog::__sortByTime(bool inverse)
{
   if (!inverse)
      __elements.sort([&](const BfFileDialogElement& a, BfFileDialogElement b) {
         if (a.type == BfFileDialogElementType_BackDirectory) return true;
         if (b.type == BfFileDialogElementType_BackDirectory) return false;

         return a.date < b.date;
      });
   else
      __elements.sort([&](const BfFileDialogElement& a, BfFileDialogElement b) {
         if (a.type == BfFileDialogElementType_BackDirectory) return true;
         if (b.type == BfFileDialogElementType_BackDirectory) return false;

         return a.date > b.date;
      });
}

void BfGuiFileDialog::__sortBySize(bool inverse)
{
   if (!inverse)
      __elements.sort([&](const BfFileDialogElement& a, BfFileDialogElement b) {
         if (a.type == BfFileDialogElementType_BackDirectory) return true;
         if (b.type == BfFileDialogElementType_BackDirectory) return false;
         return a.size < b.size;
      });
   else
      __elements.sort([&](const BfFileDialogElement& a, BfFileDialogElement b) {
         if (a.type == BfFileDialogElementType_BackDirectory) return true;
         if (b.type == BfFileDialogElementType_BackDirectory) return false;
         return a.size > b.size;
      });
}
void BfGuiFileDialog::__sortByName(bool inverse)
{
   __elements.sort(
       [&](const BfFileDialogElement& a, const BfFileDialogElement& b) {
          if (a.type == BfFileDialogElementType_BackDirectory) return true;
          if (b.type == BfFileDialogElementType_BackDirectory) return false;

          if (a.type != b.type)
          {
             if (a.type == BfFileDialogElementType_Directory) return true;
             if (b.type == BfFileDialogElementType_Directory) return false;

             if (a.type == BfFileDialogElementType_DirectoryEmpty) return true;
             if (b.type == BfFileDialogElementType_DirectoryEmpty) return false;

             return a.type == BfFileDialogElementType_RegularFile;
          }

          std::string a_str = a.path.filename().string();
          std::string b_str = b.path.filename().string();

          return inverse ? a_str.compare(b_str) > 0 : a_str.compare(b_str) < 0;
       });
}
void BfGuiFileDialog::show() noexcept { __is_render = true; }
void BfGuiFileDialog::hide() noexcept { __is_render = false; }

void BfGuiFileDialog::draw() { __render(); }

void BfGuiFileDialog::sort(const ImGuiTableSortSpecs* sort_specs)
{
   if (sort_specs->SpecsDirty)
   {
      if (sort_specs->SpecsCount > 0)
      {
         const ImGuiTableColumnSortSpecs* column_sort = &sort_specs->Specs[0];

         if (column_sort->ColumnIndex == 0)
         {
            if (column_sort->SortDirection == ImGuiSortDirection_Ascending)
               __sortByName(false);
            else
               __sortByName(true);
         }
         else if (column_sort->ColumnIndex == 1)
         {
            if (column_sort->SortDirection == ImGuiSortDirection_Ascending)
               __sortBySize(false);
            else
               __sortBySize(true);
         }
         else if (column_sort->ColumnIndex == 2)
         {
            if (column_sort->SortDirection == ImGuiSortDirection_Ascending)
               __sortByTime(true);
            else
               __sortByTime(false);
         }
      }
      // sort_specs->SpecsDirty = false;
   }
}

void BfGuiFileDialog::update()
{
   __root = fs::absolute(__root);
   try
   {
      std::list<BfFileDialogElement> tmp;
      tmp.push_back(
          {__root.root_name(), 0, 0, BfFileDialogElementType_BackDirectory});
      for (const auto& item : fs::directory_iterator(__root))
      {
         fs::path abs = fs::absolute(__root / item);
         if (abs.empty())
         {
            continue;
         }
         if (fs::exists(abs))
         {
            abs = fs::canonical(abs);
            BfFileDialogElement element{
                .path = abs,
                .date = getLastWriteFileTime_time_t(abs),
                .size = getElementSize(abs),
                .type = BfFileDialogElementType_None};
            if (fs::is_directory(abs))
            {
               if (isDirectoryEmpty(abs))
               {
                  element.type = BfFileDialogElementType_DirectoryEmpty;
               }
               else
                  element.type = BfFileDialogElementType_Directory;
            }
            else if (fs::is_regular_file(abs))
            {
               element.type = BfFileDialogElementType_RegularFile;
            }
            else
            {
               element.type = BfFileDialogElementType_None;
            }
            tmp.emplace_back(std::move(element));
         }
      }
      __elements = std::move(tmp);
   }
   catch (const fs::filesystem_error& e)
   {
      __warning_msg = e.what();
      ImGui::OpenPopup("File dialog warning");
   }
}

void BfGuiFileDialog::goTo(const fs::path& path)
{
   if (path != __root)
   {
      __back_stack.push(__root);
      __root          = path;
      __forward_stack = std::stack<fs::path>();
      this->update();
   }
}

void BfGuiFileDialog::goBack()
{
   if (!__back_stack.empty())
   {
      __forward_stack.push(__root);
      __root = __back_stack.top();
      __back_stack.pop();
      update();
   }
   else
   {
   }
}

void BfGuiFileDialog::goForward()
{
   if (!__forward_stack.empty())
   {
      __back_stack.push(__root);
      __root = __forward_stack.top();
      __forward_stack.pop();
      update();
   }
   else
   {
   }
}

void bfGetDirectoryItemsList(fs::path               root,
                             std::vector<fs::path>& dirs,
                             std::vector<fs::path>& files)
{
   if (!dirs.empty())
   {
      dirs.clear();
      dirs.reserve(100);
   }
   if (!files.empty())
   {
      files.clear();
      files.reserve(100);
   }

   for (const auto& item : fs::directory_iterator(root))
   {
      if (item.is_directory())
      {
         dirs.emplace_back(std::move(item));
      }
      else if (item.is_regular_file())
      {
         files.emplace_back(std::move(item));
      }
   }
}
