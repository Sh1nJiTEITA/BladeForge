#pragma once
#include <cmath>
#include <imgui.h>
#ifndef BF_GUI_BODY_H
#define BF_GUI_BODY_H

#include "bfCurves4.h"
#include <bfBladeBody.h>
#include <bfBladeSection2.h>
#include <variant>

namespace gui
{
// clang-format off



using inputTableField = std::variant< int*, float* >;
using inputTableFieldSpeed = std::variant< int, float >;
using pSection = std::shared_ptr< obj::section::BfBladeSection >;
using pImage = std::shared_ptr< obj::curves::BfTexturePlane >;
using pBody = std::shared_ptr< obj::body::BfBladeBody >;

/**
 * @brief Present table of main section parameters
 *
 * @param label Table name
 * @param values Values to modify
 * @param names Name of values to modify
 * @param count Number of values to modify
 * @return status: true  - if smt were modified
 *                 false - if smt were modified
 */
bool presentBladeSectionTable(
   const char* label, 
   inputTableField values[], 
   inputTableFieldSpeed speeds[],
   const char* names[], 
   int count
);

// ---------------------------------------------------------- // 

/** @brief Status of new center circle creation */
enum class CenterCirclePopupReturnStatus { OK, CANCEL, IDLE, };

/**
 * @brief Present creation popup for new centeral circle
 *
 * @param new_circle Circle to modify 
 * @return Status of creation, if IDLE -> nothing happend
 *                                OK -> creation done
 *                                CANCEL -> creation failed
 */
auto centerCirclesPopup(obj::section::CenterCircle& new_circle) ->CenterCirclePopupReturnStatus;


// ---------------------------------------------------------- // 

/**
 * @brief Presents window to modify & create new central circles
 *
 * @param circles Storage (list) to modify / add circles to
 * @return Status of creation / modification: if true -> smt happend
 *                                               false -> nothing happend
 */
bool presentCenterCirclesEditor(std::list< obj::section::CenterCircle >& circles);

// ---------------------------------------------------------- // 

/**
 * @brief Presents blade section create info window for modifing
 * section parameters.
 *
 * Includes central circle editor
 *
 * @param sec Section to modify
 * @return Stauts: if true -> smt were modified from GUI stage;
 *                    false -> idle
 */
bool presentBladeSectionCreateWindow(pSection sec);

// ---------------------------------------------------------- // 

/**
 * @brief Present WINDOW to control loaded to local vulkan descriptor
 * image to work with
 *
 * @param img Shared pointer to Texture plane object
 * @return Always true
 */
bool presentImageControlWindow(pImage img);

// ---------------------------------------------------------- // 

/**
 * @brief Presents body ...?
 *
 * @param body 
 * @return 
 */
bool presentBodyCreateWindow(pBody body);

// ---------------------------------------------------------- // 

struct SectionCreateInfoGui : public obj::body::SectionCreateInfoExtended { 
   bool isRender = true;
   bool isFormatting = false;
   bool isParametersDockBuild = false;
   std::optional<fs::path> savePath;
   float relZ = 0.0f;
};

bool processPopen(bool& handle, ImGuiKey down_key, ImGuiKey press_key);

using MainDockSignal = uint64_t;
enum MainDockSignalEnum : MainDockSignal { 
   RebuildDock = 1 << 1,
   Idle = 1 << 2
};

class MainDock { 
public:
   MainDock();
   MainDock(pBody body);
   void bindBody(pBody body);

   auto addSection() -> pSection;
   auto addSectionAndUpdateBuffer() -> pSection;

   /**
    * @brief Presents main window (dock) of interactin
    */
   void draw();
   void kill();

public:

   void buildMainDock(ImGuiID dock_id);
   auto presentMainDockCurrentExistingSections() -> MainDockSignal;
   auto presentSaveButtonSectionsSeparatly() -> void;

   auto presentMainDockMenuBar() -> MainDockSignal;

   auto presentCurrentFormattingSections() -> bool;
   auto presentSectionDock(pSection sec) -> void;
   auto presentSectionParameters(pSection sec) -> bool;
   auto presentSectionSaveButton(pSection sec) -> void;
   auto presentSectionToggleView(pSection sec) -> void; 
   auto presentSectionDeleteMenu(pSection sec) -> void;
   
   auto activeSections() -> std::vector< pSection >;
   auto formattingSections() -> std::vector< pSection >;
   auto genSectionWindowName(pSection sec) -> std::string;

private:
   uint64_t m_signal;

   std::optional<ImGuiID> m_leftPartID;
   std::optional<ImGuiID> m_rightPartID;

   pBody m_body;
   std::list< SectionCreateInfoGui > m_infos;
};








}; // namespace gui
#endif
