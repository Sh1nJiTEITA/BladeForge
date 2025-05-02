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

layout(location = 1) in vec2 fragTex;
layout(location = 2) flat in uint objectIndex;

layout(location = 0) out vec4 outColor;
layout(location = 1) out uint outId;


void main() {
    float transparency = obj_data_buffer.obj_data[objectIndex].select_color.x;

    vec4 texColor = texture(texSampler[0], fragTex);
    texColor.w = transparency;

    float borderWidth = 0.005;

    bool isCursorSelected = obj_data_buffer.obj_data[objectIndex].id == 
                            ubo.id_on_cursor;
    bool isBorder =
        (fragTex.x < borderWidth || fragTex.x > 1.0 - borderWidth ||
        fragTex.y < borderWidth || fragTex.y > 1.0 - borderWidth) &&
        isCursorSelected && (obj_data_buffer.obj_data[objectIndex].index == 0);


    vec4 borderColor = vec4(0.6, 0.6, 0.6, transparency); // red border
    
    outColor = isBorder ? borderColor : texColor;
    outId = obj_data_buffer.obj_data[objectIndex].id;
}














