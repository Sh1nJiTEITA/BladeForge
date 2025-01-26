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
         auto pLayerHandler = BfLayerHandler::instance();
         auto extent = BfCamera::instance()->m_extent;
         ImVec2 mousePos = ImGui::GetMousePos();

         auto notNestedRoot = pLayerHandler->findNotNestedRoot(m_root);

         auto camera = BfCamera::instance();
         glm::vec2 worldMousePos;
         worldMousePos.x = (2.0f * mousePos.x) / extent.x - 1.0f;
         worldMousePos.y = (2.0f * mousePos.y) / extent.y - 1.0f;
         glm::vec4 ndcPos(worldMousePos.x, worldMousePos.y, 0.0f, 1.0f);
         glm::mat4 invProj = glm::inverse(camera->projection());
         glm::mat4 invView = glm::inverse(camera->view());
         glm::vec4 worldPos = invView * invProj * ndcPos;
         glm::vec2 finalWorldPos(worldPos.x, worldPos.y);
         // std::cout << "m[" << worldMousePos.x << " " << worldMousePos.y << "]
         // "
         //           << __dvertices[0].pos.x << " " << __dvertices[0].pos.y
         //           << " ==> [" << finalWorldPos.x << " " << finalWorldPos.y
         //           << "]\n";

         // clang-format off
         if (auto bezierFrame = dynamic_cast<BfBezierCurveFrame*>(m_root))
         {
            for (auto& dvert : bezierFrame->attachedCurve()->dVertices())
            {
               if (dvert.pos == __dvertices[0].pos)
               {
                  dvert = __dvertices[0] = {
                      glm::vec3(finalWorldPos, 0.0f),
                      __dvertices[0].color,
                      __dvertices[0].normals
                  };
                  bezierFrame->attachedCurve()->createVertices();
                  bezierFrame->attachedCurve()->createIndices();
                  createVertices();
                  createIndices();
                  notNestedRoot->update_buffer();
                  break;
               }
            }
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
