#version 460
#extension GL_EXT_debug_printf : enable

layout(push_constant) uniform PushConstants {
    mat4 scale;
    mat4 proj;
} pc;

layout(push_constant) uniform ViewHandlesPC {
    mat4 scale;
    mat4 invScale;
} ViewPC;


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

layout(location = 2) out vec3 outNormals;
layout(location = 0) out vec3 fragColor;
layout(location = 3) out vec3 fragPos;

layout(location = 4) flat out uint obj_index;

void main() {
    // debugPrintfEXT("Vertex Position: (%.2f, %.2f, %.2f)\n", inPosition.x, inPosition.y, inPosition.z);

    vec4 coo = pc.scale * 
               pc.proj * 
               ubo.view * 
               // ViewPC.scale * 
    obj_data_buffer.obj_data[gl_BaseInstance].model_matrix * 
    vec4(inPosition, 1.0);


    outNormals = mat3(transpose(inverse(obj_data_buffer.obj_data[gl_BaseInstance].model_matrix))) * inNormals; 
    gl_Position = coo;
    obj_index = gl_BaseInstance;
 
    if (obj_data_buffer.obj_data[gl_BaseInstance].id == ubo.id_on_cursor) {                
        fragColor = vec3(0.0, 1.0, 0.5);
    }
    else {
        fragColor = inColor;
    }
}
