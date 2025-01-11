#version 460
#extension GL_EXT_debug_printf : enable

struct ObjectData {
    mat4 model_matrix;
    vec3 select_color;
    uint index;
    uint id;
};

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model; 
    mat4 view;
    mat4 proj;
    vec2 cursor_pos;
    vec3 camera_pos;
    uint id_on_cursor;
} ubo;


layout(std140, set = 1, binding = 0) buffer ObjectDataBuffer {
    ObjectData obj_data[];
} obj_data_buffer;


layout(location = 0) in vec3 fragColor;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPos;
layout(location = 0) out vec4 outColor;
layout(location = 1) out uint outId;

layout(location = 4) flat in uint obj_index;

vec3 uLightPosition = vec3(0.0, 1.0, 2.0); // Позиция источника света
vec3 uLightColor = vec3(1.0, 1.0, 1.0);    // Цвет источника света

void main() 
{
    outId = obj_data_buffer.obj_data[obj_index].id;
    outColor = vec4(fragColor * obj_data_buffer.obj_data[obj_index].select_color, 1.0);
}
