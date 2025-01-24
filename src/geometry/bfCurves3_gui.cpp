#include <glm/ext/quaternion_common.hpp>
#include <glm/matrix.hpp>

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

         // Получаем текущую камеру
         auto camera = BfCamera::instance();

         // Получаем нормализованные координаты мыши в пространстве NDC
         glm::vec2 worldMousePos;
         worldMousePos.x = (2.0f * mousePos.x) / extent.x - 1.0f;
         worldMousePos.y = 1.0f - (2.0f * mousePos.y) / extent.y;

         // Преобразуем вектора в 4D (для использования в матрицах)
         glm::vec4 ndcPos(worldMousePos.x, worldMousePos.y, 0.0f, 1.0f);

         // Инвертируем матрицы проекции и вида
         glm::mat4 invProj = glm::inverse(camera->projection());
         glm::mat4 invView = glm::inverse(camera->view());

         // Преобразуем в мировые координаты
         glm::vec4 worldPos = invProj * invView * ndcPos;

         // Если нужно, преобразуем в 2D координаты
         glm::vec2 finalWorldPos(worldPos.x, worldPos.y);

         std::cout << "m[" << worldMousePos.x << " " << worldMousePos.y << "] "
                   << __dvertices[0].pos.x << " " << __dvertices[0].pos.y
                   << " ==> "

                      "["
                   << finalWorldPos.x << " " << finalWorldPos.y << "]\n";
         // __dvertices[0] = glm::vec3(
         //     scaledMousePos,
         //     0.0f
         // );  // Новая позиция в мировых координатах
         // createVertices();
         // createIndices();
         // notNestedRoot->update_buffer();
         // }

         if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
         {
            is_dragged = false;
         }
      }
   }
}

#endif
