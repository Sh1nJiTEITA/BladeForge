#version 460
#extension GL_EXT_debug_printf : enable
// Structs

layout(push_constant) uniform PushConstants {
    int viewport_index;
} pc;

struct ObjectData {
    mat4 model_matrix;
    vec4 select_color;
    vec4 center;
    int index;
    int id;
    float line_thickness;
    float _pad;
};

struct ViewData { 
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 scale;
    vec2 cursor_pos;
    vec3 camera_pos;
    uint id_on_cursor;
};

// Layouts
layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec2 cursor_pos;
    vec3 camera_pos;
    uint id_on_cursor;
} ubo;

layout(std140, set = 0, binding = 1) buffer MultiportViewUBO {
    ViewData view_data[];
} MVUBO;


layout(std140, set = 1, binding = 0) buffer ObjectDataBuffer {
    ObjectData obj_data[];
} obj_data_buffer;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormals;

layout(location = 2) out vec3 outNormals;
layout(location = 0) out vec3 fragColor;
layout(location = 4) flat out uint obj_index;


void main() {
    ViewData view_data = MVUBO.view_data[pc.viewport_index];

    vec4 coo = view_data.scale 
             * view_data.proj 
             * view_data.view 
             * obj_data_buffer.obj_data[gl_BaseInstance].model_matrix 
             * vec4(inPosition, 1.0);

    outNormals = inNormals;
    gl_Position = coo;
    obj_index = gl_BaseInstance;

    if (obj_data_buffer.obj_data[gl_BaseInstance].id == ubo.id_on_cursor) {
        fragColor = vec3(0.0, 1.0, 0.5);
    }
    else {
        fragColor = inColor;
    }
}

