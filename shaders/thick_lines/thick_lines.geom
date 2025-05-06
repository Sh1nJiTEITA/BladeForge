// #version 460
//
// struct ObjectData {
//     mat4 model_matrix;
//     vec3 select_color;
//     int index;
//     int id;
//     float line_thickness;
// };
//
// layout(std140, set = 1, binding = 0) buffer ObjectDataBuffer {
//     ObjectData obj_data[];
// } obj_data_buffer;
//
// layout (lines) in;
// layout (triangle_strip, max_vertices = 4) out;
//
// layout(location = 0) in vec3 fragColor[];
// layout(location = 2) in vec3 inNormals[];
// layout(location = 4) flat in uint obj_index[];
//
// layout(location = 0) out vec3 geomFragColor;
// layout(location = 4) flat out uint geomObjIndex;
//
// layout(set = 0, binding = 0) uniform UniformBufferObject {
//     mat4 model;
//     mat4 view;
//     mat4 proj;
//     vec2 cursor_pos;
//     vec3 camera_pos;
//     uint id_on_cursor;
// } ubo;
//
//
//
//
// void main() {
//     float w = obj_data_buffer.obj_data[obj_index[0]].line_thickness;
//
//     vec3 dir = normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);
//
//     vec3 side_1 = normalize(cross(dir, inNormals[0]));
//
//     gl_Position = ubo.model * ubo.proj * ubo.view * 
//                   obj_data_buffer.obj_data[obj_index[0]].model_matrix * 
//                   (gl_in[0].gl_Position + vec4(side_1 * w, 0.0));
//     
//     geomFragColor = fragColor[0];
//     geomObjIndex = obj_index[0];
//     EmitVertex();
//
//     
//     gl_Position = ubo.model * ubo.proj * ubo.view * 
//                   obj_data_buffer.obj_data[obj_index[1]].model_matrix * 
//                   (gl_in[1].gl_Position + vec4(side_1 * w, 0.0));
//     
//     geomFragColor = fragColor[1];
//     geomObjIndex = obj_index[1];
//     EmitVertex();
//     
//     gl_Position = ubo.model * ubo.proj * ubo.view * 
//                   obj_data_buffer.obj_data[obj_index[0]].model_matrix * 
//                   (gl_in[0].gl_Position - vec4(side_1 * w, 0.0));
//     
//     geomFragColor = fragColor[0];
//     geomObjIndex = obj_index[0];
//     EmitVertex();
//
//     gl_Position = ubo.model * ubo.proj * ubo.view * 
//                   obj_data_buffer.obj_data[obj_index[1]].model_matrix * 
//                   (gl_in[1].gl_Position - vec4(side_1 * w, 0.0));
//     
//     geomFragColor = fragColor[1];
//     geomObjIndex = obj_index[1];
//     EmitVertex();
//
//     EndPrimitive();
// }



#version 460

struct ObjectData {
    mat4 model_matrix;
    vec3 select_color;
    int index;
    int id;
    float line_thickness;
};

layout(std140, set = 1, binding = 0) buffer ObjectDataBuffer {
    ObjectData obj_data[];
} obj_data_buffer;

layout (lines) in;
layout (triangle_strip, max_vertices = 32) out;

layout(location = 0) in vec3 fragColor[];
layout(location = 2) in vec3 inNormals[];
layout(location = 4) flat in uint obj_index[];

layout(location = 0) out vec3 geomFragColor;
layout(location = 4) flat out uint geomObjIndex;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec2 cursor_pos;
    vec3 camera_pos;
    uint id_on_cursor;
} ubo;

// Number of segments in the semicircle
const int CIRCLE_SEGMENTS = 8;
const float PI = 3.14159265;

void emitOffsetVertex(vec4 pos, vec3 offset, vec3 color, uint index) {
    gl_Position = ubo.model * ubo.proj * ubo.view * obj_data_buffer.obj_data[index].model_matrix * (pos + vec4(offset, 0.0));
    geomFragColor = color;
    geomObjIndex = index;
    EmitVertex();
}

void main() {
    float w = obj_data_buffer.obj_data[obj_index[0]].line_thickness;
    
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    
    vec3 dir = normalize(p1.xyz - p0.xyz);
    vec3 side = normalize(cross(dir, inNormals[0]));

    // Rectangle
    emitOffsetVertex(p0,  side * w, fragColor[0], obj_index[0]);
    emitOffsetVertex(p1,  side * w, fragColor[1], obj_index[1]);
    emitOffsetVertex(p0, -side * w, fragColor[0], obj_index[0]);
    emitOffsetVertex(p1, -side * w, fragColor[1], obj_index[1]);
    EndPrimitive();

    // Start Cap (half-circle at p0)
    vec3 normal = inNormals[0];
    vec3 forward = dir;
    vec3 right = side;

    // Start Cap (p0 semicircle)
    for (int i = 0; i <= CIRCLE_SEGMENTS; ++i) {
        float theta = PI - (float(i) / CIRCLE_SEGMENTS) * PI * 2;
        vec3 offset = cos(theta) * right + sin(theta) * forward;
        emitOffsetVertex(p0, offset * w, fragColor[0], obj_index[0]);
        emitOffsetVertex(p0, vec3(0.0), fragColor[0], obj_index[0]);
    }
    EndPrimitive();

    // End Cap (p1 semicircle)
    for (int i = 0; i <= CIRCLE_SEGMENTS; ++i) {
        float theta = (float(i) / CIRCLE_SEGMENTS) * PI * 2;
        vec3 offset = cos(theta) * right + sin(theta) * forward;
        emitOffsetVertex(p1, offset * w, fragColor[1], obj_index[1]);
        emitOffsetVertex(p1, vec3(0.0), fragColor[1], obj_index[1]);
    }
    EndPrimitive();
}
