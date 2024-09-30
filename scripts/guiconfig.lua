local settings = {
   fonts = {
      fonts_paths = {
         "./resources/fonts",
      },

      standart_font = {
         name = "Cousine-Regular.ttf",
         size = 20,
         -- Смещения глифа (символа) по горизонтали/вертикали
         glypth_offset = { 0, 2.0 },
         glypth_min_advance_x = nil,
      },

      icon_font = {
         name = "fa-solid-900.ttf",
         size = 20,
         glypth_offset = { 0, 2.0 },
         glypth_min_advance_x = 13.0,
      },
   },
}
return settings
