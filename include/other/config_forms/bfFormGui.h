#ifndef BF_FORMS_GUI_H
#define BF_FORMS_GUI_H

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

struct BfFormFont
{
   std::vector<std::string> name;
   fs::path                 path;
   int                      current;
   int                      size;
   std::pair<float, float>  glypth_offset;
   float                    glypth_min_advance_x;
};

struct BfFormFontSettings
{
   std::vector<fs::path> font_directory_paths;
   BfFormFont            standart_font;
   BfFormFont            icon_font;
   BfFormFont            greek_font;
};

#endif
