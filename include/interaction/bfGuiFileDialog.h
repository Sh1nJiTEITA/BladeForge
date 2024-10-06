#ifndef BF_GUI_FILE_DIALOG_H
#define BF_GUI_FILE_DIALOG_H

#include <imgui.h>

#include <filesystem>
#include <iostream>
#include <list>
#include <vector>

#include "bfIconsFontAwesome6.h"

namespace fs = std::filesystem;

enum BfFileDialogElementType_
{
   BfFileDialogElementType_None,
   BfFileDialogElementType_Directory,
   BfFileDialogElementType_DirectoryEmpty,
   BfFileDialogElementType_RegularFile,
   BfFileDialogElementType_LuaFile,
};
const char* bfGetFileDialogElementTypeEmoji(BfFileDialogElementType_ e);

struct BfFileDialogElement
{
   fs::path                 path;
   std::time_t              date;
   size_t                   size;
   BfFileDialogElementType_ type;
};

class BfGuiFileDialog
{
   bool     __is_render = true;
   fs::path __root;

   std::list<BfFileDialogElement> __elements;

   void __render();
   void __render_table();

   void __sortByTime(bool inverse = false);
   void __sortBySize(bool inverse = false);
   void __sortByName(bool inverse = false);

public:
   BfGuiFileDialog();

   void setRoot(fs::path root) noexcept;
   void show() noexcept;
   void hide() noexcept;
   void draw();
   void sort(const ImGuiTableSortSpecs* sort_specs);
   void update();

   static std::time_t getLastWriteFileTime_time_t(const fs::path&);
   static std::string getLastWriteFileTime_string(const fs::path&);
   static std::string getLastWriteFileTime_string(std::time_t);
   //
   static size_t getElementSize(const fs::path&);
   static size_t getDirectorySize(const fs::path&);
   static size_t getFileSize(const fs::path&);

   static bool isDirectoryEmpty(const fs::path&);
};

void bfGetDirectoryItemsList(fs::path               root,
                             std::vector<fs::path>& dirs,
                             std::vector<fs::path>& files);

template <typename TP>
std::time_t bfUtilsToTimeT(TP tp)
{
   using namespace std::chrono;
   auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() +
                                                       system_clock::now());
   return system_clock::to_time_t(sctp);
}

#endif
