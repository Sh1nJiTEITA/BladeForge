#ifndef BF_UNIFORMS_H
#define BF_UNIFORMS_H

#include "bfVariative.hpp"

struct BfUniformView
{
   alignas(16) glm::mat4 model;
   alignas(16) glm::mat4 view;
   alignas(16) glm::mat4 proj;
   alignas(8) glm::vec2 cursor_pos;
   alignas(16) glm::vec3 camera_pos;
   alignas(4) uint32_t id_on_cursor;
};

struct BfObjectData
{
   alignas(16) glm::mat4 model_matrix;
   alignas(16) glm::vec3 select_color;
   alignas(4) uint32_t index;
   alignas(4) uint32_t id = 999999;
   alignas(4) float line_thickness;
};
#endif
