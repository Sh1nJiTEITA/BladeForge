#pragma once
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


}; // namespace gui

#endif
