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
layout (triangle_strip, max_vertices = 4) out;

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




void main() {
//    float thickness = 0.05; // Пример толщины линии
//    //vec3 offset = normal * thickness;
//
//    // Пропустить первую вершину линии
//    gl_Position = gl_in[0].gl_Position + vec4(inNormals[0] * thickness / 2.0, 0.0);;
//    geomFragColor = fragColor[0];
//    geomObjIndex = obj_index[0];
//    EmitVertex();
//
//    // Пропустить вторую вершину линии
//    gl_Position = gl_in[1].gl_Position - vec4(inNormals[1] * thickness / 2.0, 0.0);;
//    geomFragColor = fragColor[1];
//    geomObjIndex = obj_index[1];
//    EmitVertex();
//    
//    gl_Position = gl_in[0].gl_Position + vec4(inNormals[0] * thickness / 2.0, 0.0);
//    geomFragColor = fragColor[0];
//    geomObjIndex = obj_index[0];
//    EmitVertex();
//
//    gl_Position = gl_in[1].gl_Position - vec4(inNormals[1] * thickness / 2.0, 0.0);
//    geomFragColor = fragColor[1];
//    geomObjIndex = obj_index[1];
//    EmitVertex();
//
//    EndPrimitive();
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    float w = obj_data_buffer.obj_data[obj_index[0]].line_thickness;



    vec3 dir = normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);

    vec3 side_1 = normalize(cross(dir, inNormals[0]));
    //vec3 side_2 = cross(dir, inNormals[1]);

    //ubo.proj * ubo.view * obj_data_buffer.obj_data[obj_index[0]].model_matrix * (gl_in[0].gl_Position + vec4(side_1 * w, 0.0));

    gl_Position = ubo.proj * ubo.view * 
                  ubo.model * 
                  //obj_data_buffer.obj_data[obj_index[0]].model_matrix * 
                  (gl_in[0].gl_Position + vec4(side_1 * w, 0.0));
    
    geomFragColor = fragColor[0];
    geomObjIndex = obj_index[0];
    EmitVertex();

    // Пропустить вторую вершину линии
    
    //gl_Position = gl_in[1].gl_Position + vec4(side_1 * w, 0.0);
    gl_Position = ubo.proj * ubo.view * 
                  ubo.model * 
                  //obj_data_buffer.obj_data[obj_index[1]].model_matrix * 
                  (gl_in[1].gl_Position + vec4(side_1 * w, 0.0));
    
    geomFragColor = fragColor[1];
    geomObjIndex = obj_index[1];
    EmitVertex();
    
    gl_Position = ubo.proj * ubo.view * 
                  ubo.model * 
                  //obj_data_buffer.obj_data[obj_index[0]].model_matrix * 
                  (gl_in[0].gl_Position - vec4(side_1 * w, 0.0));
    
    geomFragColor = fragColor[0];
    geomObjIndex = obj_index[0];
    EmitVertex();

    gl_Position = ubo.proj * ubo.view * 
                  ubo.model * 
                  //obj_data_buffer.obj_data[obj_index[1]].model_matrix * 
                  (gl_in[1].gl_Position - vec4(side_1 * w, 0.0));
    
    geomFragColor = fragColor[1];
    geomObjIndex = obj_index[1];
    EmitVertex();

    EndPrimitive();
}