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

layout(set = 2, binding = 0) uniform sampler2D texSampler[1];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTex;

layout(location = 0) out vec4 outColor;
layout(location = 1) out uint outId;

layout(location = 4) flat in uint obj_index;

void main() {
    outColor = vec4(fragColor, 1.0);
    outColor = texture(texSampler[0], fragTex);
}
