#ifndef BF_GUI_H
#define BF_GUI_H

#include <fstream>
#include <functional>
#include <map>
#include <string>

#include "bfBase.h"
#include "bfBladeSection.h"
#include "bfConfigManager.h"
#include "bfCurves3.h"
#include "bfEvent.h"
#include "bfHolder.h"
#include "bfIconsFontAwesome6.h"
#include "bfLayerHandler.h"
#include "implot.h"

class BfGui
{
   BfBase*   __ptr_base   = nullptr;
   BfHolder* __ptr_holder = nullptr;

   ImFont* __default_font = nullptr;
   ImFont* __icon_font    = nullptr;

   bool __is_info         = true;
   bool __is_event_log    = false;
   bool __is_camera_info  = false;

   uint32_t __current_id;

   BfBladeSectionCreateInfo blade_section_info;

public:
   BfGui();

   BfEvent bindBase(BfBase* base);
   BfEvent bindHolder(BfHolder* base);
   BfEvent bindDefaultFont(std::string path);
   BfEvent bindDefaultFont(sol::table table);
   BfEvent bindIconFont(std::string path);

   std::string getMenueInfoStr();
   std::string getMenueEventLogStr();
   std::string getMenueCameraInfoStr();

   void presentLayerHandler();
   void presentMenueBar();
   void presentCamera();
   void presentEventLog();
   void presentToolType();
   void presentLeftDock();
};

enum BfEnMenueStatus
{
   BF_MENUE_STATUS_INFO_ENABLED         = 0x1,
   BF_MENUE_STATUS_INFO_DISABLED        = -0x1,

   BF_MENUE_STATUS_EVENT_LOG_ENABLED    = 0x2,
   BF_MENUE_STATUS_EVENT_LOG_DISABLED   = -0x2,

   BF_MENUE_STATUS_CAMERA_INFO_ENABLED  = 0x3,
   BF_MENUE_STATUS_CAMERA_INFO_DISABLED = -0x3,
};

const std::map<int, std::string> bfSetMenueStr{
    {0x1, "Hide perfomance/view pannel"},
    {-0x1, "Show perfomance/view pannel"},

    {0x2, "Hide console log window"},
    {-0x2, "Show console log window"},

    {0x3, "Hide camera info window"},
    {-0x3, "Show camera info window"}};

void bfPresentLayerHandler(BfLayerHandler&);
void bfPresentBladeSectionInside(BfBladeBase*              layer,
                                 BfBladeSectionCreateInfo* info,
                                 BfBladeSectionCreateInfo* old);
void bfShowNestedLayersRecursive(std::shared_ptr<BfDrawLayer> l);

void ShowTestPlot();

#endif  // BF_GUI_H
