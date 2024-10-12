#ifndef BF_GUI_FILE_DIALOG_H
#define BF_GUI_FILE_DIALOG_H

#include <imgui.h>

#include <filesystem>
#include <iostream>
#include <list>
#include <set>
#include <stack>
#include <variant>
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
   BfFileDialogElementType_BackDirectory
};
const char*  bfGetFileDialogElementTypeEmoji(BfFileDialogElementType_ e);
const ImVec4 bfGetFileDialogElementTypeColor(BfFileDialogElementType_ e);

struct BfFileDialogElement
{
   fs::path                 path;
   std::time_t              date;
   size_t                   size;
   BfFileDialogElementType_ type;
   bool                     is_selected;
   bool                     is_extension;
};

enum BfFileDialogOpenMode_
{
   BfFileDialogOpenMode_File,
   BfFileDialogOpenMode_Files,
   BfFileDialogOpenMode_Directory,
};

class BfGuiFileDialog
{
   static BfGuiFileDialog* __instance;

   bool     __is_render                  = false;
   bool     __is_warning_window          = false;
   bool     __is_check_extension_in_dirs = true;
   fs::path __root;

   BfFileDialogOpenMode_ __mode;
   //
   std::variant<fs::path*, std::vector<fs::path>*> __out;
   //
   std::vector<std::string> __extensions = {".*"};
   //
   int __chosen_ext = -1;

   std::list<BfFileDialogElement> __elements;
   //
   std::string          __warning_msg;
   std::stack<fs::path> __forward_stack;
   std::stack<fs::path> __back_stack;

   int __hovered_item = -1;

   void __render();
   void __renderSettings();
   void __renderArrows();
   void __renderPath();
   void __renderTable();
   void __renderWarning();
   void __renderPicker();
   void __renderChosenFiles();

   void __pick();

   void __sortByTime(bool inverse = false);
   void __sortBySize(bool inverse = false);
   void __sortByName(bool inverse = false);

   void __update();
   void __updateElementExtensions();

public:
   BfGuiFileDialog();

   void setRoot(fs::path root) noexcept;
   void setExtension(std::initializer_list<std::string> list) noexcept;
   //
   static void             bindInstance(BfGuiFileDialog* ptr) noexcept;
   static BfGuiFileDialog* instance() noexcept;

   void openFile(fs::path* path, std::initializer_list<std::string> ext);
   void openFiles(std::vector<fs::path>*             path,
                  std::initializer_list<std::string> ext);
   void openDir(fs::path* path);

   void show() noexcept;
   void hide() noexcept;
   void draw();
   void sort(const ImGuiTableSortSpecs* sort_specs);
   void goTo(const fs::path&);
   void goBack();
   void goForward();

   bool isActive();

   static std::time_t getLastWriteFileTime_time_t(const fs::path&);
   static std::string getLastWriteFileTime_string(const fs::path&);
   static std::string getLastWriteFileTime_string(std::time_t);
   //
   static size_t getElementSize(const fs::path&);
   static size_t getDirectorySize(const fs::path&);
   static size_t getFileSize(const fs::path&);

   static bool isDirectoryEmpty(const fs::path&);
   static bool isDirectoryContainsExt(const fs::path&, std::string ext);
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
