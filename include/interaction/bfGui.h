#ifndef BF_GUI_H
#define BF_GUI_H

#include <fstream>
#include <functional>
#include <map>
#include <queue>
#include <string>

#include "BfGuiLuaInteraction.h"
#include "bfBase.h"
#include "bfBladeSection.h"
#include "bfConfigManager.h"
#include "bfCurves3.h"
#include "bfEvent.h"
#include "bfGuiFileDialog.h"
#include "bfHolder.h"
#include "bfIconsFontAwesome6.h"
#include "bfLayerHandler.h"
#include "config_forms/bfFormGui.h"
#include "implot.h"

class BfGui
{
   BfBase*   __ptr_base   = nullptr;
   BfHolder* __ptr_holder = nullptr;

   BfGuiFileDialog     __file_dialog;
   BfGuiLuaInteraction __lua_interaction;

   ImFont* __default_font         = nullptr;
   ImFont* __icon_font            = nullptr;

   bool __is_info                 = true;
   bool __is_event_log            = false;
   bool __is_camera_info          = true;
   bool __is_settings             = false;
   bool __is_ortho_settings       = false;
   bool __is_file_dialog          = true;

   bool __is_left_dock_space_name = false;

   uint32_t __current_id;

   BfFormFontSettings __settings_form;

   std::queue<std::function<void()>> __queue_after_render;

public:
   BfGui();

   void pollEvents();

   BfEvent bindBase(BfBase* base);
   BfEvent bindHolder(BfHolder* base);
   BfEvent bindSettings(std::filesystem::path path);
   BfEvent bindDefaultFont();
   BfEvent bindIconFont();

   void updateFonts();

   std::string getMenuInfoStr();
   std::string getMenuEventLogStr();
   std::string getMenuCameraInfoStr();
   std::string getMenuSettingsInfoStr();
   std::string getMenuIsLeftDockTitleInfoStr();

   void presentLayerHandler();
   void presentMenuBar();
   void presentCamera();
   void presentEventLog();
   void presentToolType();
   void presentLeftDock();
   void presentTopDock();
   void presentSettings();
   void presentLuaInteraction();
   void presentFileDialog();

   void __presentOrthoSettings();
};

enum BfEnMenuStatus
{
   BF_MENU_STATUS_INFO_ENABLED             = 0x1,
   BF_MENU_STATUS_INFO_DISABLED            = -0x1,

   BF_MENU_STATUS_EVENT_LOG_ENABLED        = 0x2,
   BF_MENU_STATUS_EVENT_LOG_DISABLED       = -0x2,

   BF_MENU_STATUS_CAMERA_INFO_ENABLED      = 0x3,
   BF_MENU_STATUS_CAMERA_INFO_DISABLED     = -0x3,

   BF_MENU_STATUS_SETTINGS_ENABLED         = 0x4,
   BF_MENU_STATUS_SETTINGS_DISABLED        = -0x4,

   BF_MENU_STATUS_LEFT_DOCK_TITLE_ENABLED  = 0x5,
   BF_MENU_STATUS_LEFT_DOCK_TITLE_DISABLED = -0x5,
};

const std::map<int, std::string> bfSetMenuStr{
    {0x1, "Hide perfomance/view pannel"},
    {-0x1, "Show perfomance/view pannel"},

    {0x2, "Hide console log window"},
    {-0x2, "Show console log window"},

    {0x3, "Hide camera info window"},
    {-0x3, "Show camera info window"},

    {0x4, "Hide settings window"},
    {-0x4, "Show settings window"},

    {0x5, "Hide left-dock name window"},
    {-0x5, "Show left-dock name window"},

};

void bfPresentLayerHandler(BfLayerHandler&);
void bfPresentBladeSectionInside(BfBladeBase*              layer,
                                 BfBladeSectionCreateInfo* info,
                                 BfBladeSectionCreateInfo* old);
void bfShowNestedLayersRecursive(std::shared_ptr<BfDrawLayer> l);

void ShowTestPlot();

#endif  // BF_GUI_H
