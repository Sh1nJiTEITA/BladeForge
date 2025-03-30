#include <cfloat>
#include <glm/ext/quaternion_common.hpp>
#include <glm/geometric.hpp>
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
            // bezierFrame->remake();
            notNestedRoot->remake();
            notNestedRoot->update_buffer();
         }
         if (auto circle = dynamic_cast<BfCircleFilledWithHandles*>(m_root))
         {
            if (__pdvertices.at(0)->pos == circle->circle()->get_center().pos) { 
               circle->circle()->get_center().pos = BfCamera::instance()->mouseWorldCoordinates();

               size_t index = 0;
               // for (auto vert : circle->circle()->vertices()) { 
               for (size_t i = 0; i < circle->circle()->vertices().size(); ++i) { 
                  if (circle->circle()->vertices().at(i).pos == 
                      circle->radiusHandle()->_center().pos) 
                  { 
                     index = i;
                     break;
                  }
               }
               circle->circle()->createVertices();
               circle->radiusHandle()->_center() = circle->circle()->vertices().at(index);
            }
            else { 
               BfVertex3 min_v = this->_center();
               float min_distance = FLT_MAX;
               for (auto vert : circle->circle()->vertices()) { 
                  float new_distance = glm::distance(vert.pos, BfCamera::instance()->mouseWorldCoordinates());
                  if (new_distance < min_distance) { 
                     min_distance = new_distance;
                     min_v = vert;
                  }
               }
               this->_center() = min_v;


               circle->circle()->__radius = glm::distance(BfCamera::instance()->mouseWorldCoordinates(),
                                                          circle->circle()->get_center().pos);
               // circle->circle()->createVertices();
               // circle->radiusHandle()->_center() = &circle->circle()->vertices().at(0);
            }
            notNestedRoot->remake();
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
