#ifndef BF_FORMS_GUI_H
#define BF_FORMS_GUI_H

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

struct BfFormFont
{
   std::string             name;
   fs::path                path;
   uint8_t                 size;
   std::pair<float, float> glypth_offset;
   float                   glypth_min_advance_x;
};

inline std::string BfGetStrFormFont(BfFormFont* form)
{
   return "name = " + form->name + "; path = " + form->path.string() +
          "; size = " + std::to_string(form->size) + "; glypth_offset = { " +
          std::to_string(form->glypth_offset.first) + ", " +
          std::to_string(form->glypth_offset.second) + " }; " +
          "glypth_min_advance_x = " +
          std::to_string(form->glypth_min_advance_x);
};

struct BfFormFontSettings
{
   std::vector<fs::path> font_directory_paths;
   BfFormFont            standart_font;
   BfFormFont            icon_font;
};

#endif
