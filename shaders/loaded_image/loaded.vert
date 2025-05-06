#version 460
#extension GL_EXT_debug_printf : enable

struct ObjectData {
    mat4 model_matrix;
    vec3 select_color;
    int index;
    int id;
    float line_thickness;
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

layout(std140, set = 1, binding = 0) buffer ObjectDataBuffer {
    ObjectData obj_data[];
} obj_data_buffer;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormals;

// layout(location = 2) out vec3 outNormals;
layout(location = 1) out vec2 fragTex;
layout(location = 2) flat out uint objectIndex;

void main() {
    vec4 coo = ubo.model * 
	       ubo.proj * 
	       ubo.view * 
	       obj_data_buffer.obj_data[gl_BaseInstance].model_matrix * 
	       vec4(inPosition, 1.0);

    gl_Position = coo;
    objectIndex = gl_BaseInstance;
    fragTex = vec2(inColor.x, inColor.y); 
}
