#include "bfGuiFileDialog.h"

#include <algorithm>
#include <exception>
#include <filesystem>

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
   };
   return s.at(e);
}

BfGuiFileDialog::BfGuiFileDialog() {}

void BfGuiFileDialog::setRoot(fs::path root) noexcept { __root = root; }

void BfGuiFileDialog::__render()
{
   if (__is_render)
   {
      if (ImGui::Begin("File Dialog", &__is_render))
      {
         if (ImGui::Button(ICON_FA_ARROW_LEFT))
         {
            __root = __root.parent_path();
            this->update();
         }
         ImGui::SameLine();
         if (ImGui::Button("Update list"))
         {
            this->update();
         }
         __render_table();
      }
      ImGui::End();
   }
}

void BfGuiFileDialog::__render_table()
{
   static int selected_row = -1;
   if (ImGui::BeginTable("##BfGuiFileDialogTable",
                         3,
                         /*ImGuiTableFlags_RowBg | */
                         ImGuiTableFlags_Sortable))
   {
      try
      {
         float table_w = ImGui::GetContentRegionAvail().x;
         float name_w  = 60.0f;
         float date_w  = 140.0f;

         ImGui::TableSetupColumn("Name",
                                 ImGuiTableColumnFlags_WidthFixed,
                                 table_w - name_w - date_w);
         ImGui::TableSetupColumn("Size",
                                 ImGuiTableColumnFlags_WidthFixed,
                                 name_w);
         ImGui::TableSetupColumn("Last modified",
                                 ImGuiTableColumnFlags_WidthFixed,
                                 date_w);

         ImGui::TableHeadersRow();
         ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs();
         if (sort_specs) sort(sort_specs);
         int row_index = 0;
         for (const auto& element : __elements)
         {
            std::cout << element.str << "\n";
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            bool row_selected = (selected_row == row_index);

            ImGui::Selectable(("##" + element.str).c_str(),
                              row_selected,
                              ImGuiSelectableFlags_SpanAllColumns);

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
               this->__root = fs::absolute(__root / element.path);
               update();
               /*return;*/
            }

            /*if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))*/
            /*{*/
            /*   selected_row = row_index;*/
            /*   std::cout << element.path << " " << element.type << " "*/
            /*             << element.date << " " << element.size << "\n";*/
            /*}*/

            ImGui::SameLine();
            ImGui::TextColored(bfGetFileDialogElementTypeColor(element.type),
                               "%s",
                               bfGetFileDialogElementTypeEmoji(element.type));
            ImGui::SameLine();
            ImGui::SetCursorPosX(40.0);
            ImGui::Text("%s", element.str.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%d", static_cast<int>(element.size));
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s",
                        getLastWriteFileTime_string(element.date).c_str());
            row_index += 1;
         }
      }
      catch (const std::exception& e)
      {
         std::cerr << "Filesystem error: " << e.what() << "\n";
      }
   }
   ImGui::EndTable();
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
         return a.date < b.date;
      });
   else
      __elements.sort([&](const BfFileDialogElement& a, BfFileDialogElement b) {
         return a.date > b.date;
      });
}

void BfGuiFileDialog::__sortBySize(bool inverse)
{
   if (!inverse)
      __elements.sort([&](const BfFileDialogElement& a, BfFileDialogElement b) {
         return a.size < b.size;
      });
   else
      __elements.sort([&](const BfFileDialogElement& a, BfFileDialogElement b) {
         return a.size > b.size;
      });
}
void BfGuiFileDialog::__sortByName(bool inverse)
{
   if (!inverse)
      __elements.sort(
          [&](const BfFileDialogElement& a, const BfFileDialogElement& b) {
             if (a.type != b.type)
                return a.type == BfFileDialogElementType_Directory;

             std::string a_str = a.path.filename().string();
             std::string b_str = b.path.filename().string();
             return a_str.compare(b_str) < 0;
          });
   else
      __elements.sort(
          [&](const BfFileDialogElement& a, const BfFileDialogElement& b) {
             if (a.type != b.type)
                return a.type == BfFileDialogElementType_Directory;

             std::string a_str = a.path.filename().string();
             std::string b_str = b.path.filename().string();
             return a_str.compare(b_str) > 0;
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
   if (!__elements.empty())
   {
      __elements.clear();
   }
   try
   {
      std::cout << "Entering " << fs::absolute(__root) << "\n";
      for (const auto& item : fs::directory_iterator(__root))
      {
         /*fs::path abs = fs::canonical(fs::absolute(__root / item));*/
         fs::path abs = fs::absolute(__root / item);
         std::cout << "Found " << abs << "\n";
         if (abs.empty())
         {
            continue;
         }
         if (fs::exists(abs))
         {
            abs = fs::canonical(abs);
            BfFileDialogElement element{
                .path = abs,
                .str = abs.filename().string(),
                .date = getLastWriteFileTime_time_t(abs),
                .size = getElementSize(abs),
                .type = BfFileDialogElementType_None};
            if (fs::is_directory(abs))
            {
               if (isDirectoryEmpty(abs))
                  element.type = BfFileDialogElementType_DirectoryEmpty;
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

            __elements.emplace_back(std::move(element));
         }
      }
   }
   catch (const fs::filesystem_error& e)
   {
      std::cerr << "Filesystem error: " << e.what() << "\n";
      update();
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
