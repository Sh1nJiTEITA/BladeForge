#include <glm/ext/quaternion_common.hpp>
#include <glm/matrix.hpp>
#include <type_traits>

#include "bfBase.h"
#include "bfCamera.h"
#include "bfCurves3.h"
#include "bfDrawObject.h"
#include "bfLayerHandler.h"
#include "imgui.h"

#ifdef BF_CURVES3_GUI

void
BfCircleFilled::update()
{
   {
      is_dragged = is_dragged ||
                   (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left));
      if (is_dragged)
      {
         if (__depMode != BfDrawObjDependencies_Mode_Ptr) return;
         // TODO: REMAKE if function `mouseWorldCoordinates` will be updated
         if (BfCamera::instance()->m_mode != BfCameraMode_Ortho) return;
         auto pLayerHandler = BfLayerHandler::instance();
         auto notNestedRoot = pLayerHandler->findNotNestedRoot(m_root);
         // clang-format off
         if (auto bezierFrame = dynamic_cast<BfBezierCurveWithHandles*>(m_root))
         {
            for (auto& dvert : bezierFrame->curve()->dVertices())
            {
               if (dvert.pos == __pdvertices[0]->pos)
               {
                  dvert.pos = BfCamera::instance()->mouseWorldCoordinates();
                  break;
               }
            }
            bezierFrame->remake();
            notNestedRoot->update_buffer();
         }
         // clang-format on

         if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
         {
            is_dragged = false;
         }
      }
   }
}

#endif
