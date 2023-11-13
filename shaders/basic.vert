#version 450
#extension GL_EXT_debug_printf : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);


void print_mat4(mat4 matrix) {
    debugPrintfEXT("matrix[0] = %f, %f, %f, %f", matrix[0][0],matrix[0][1],matrix[0][2],matrix[0][3]);
    debugPrintfEXT("matrix[1] = %f, %f, %f, %f", matrix[1][0],matrix[1][1],matrix[1][2],matrix[1][3]);
    debugPrintfEXT("matrix[2] = %f, %f, %f, %f", matrix[2][0],matrix[2][1],matrix[2][2],matrix[2][3]);
    debugPrintfEXT("matrix[3] = %f, %f, %f, %f", matrix[3][0],matrix[3][1],matrix[3][2],matrix[3][3]);
}

void main() {
    
    
    //debugPrintfEXT("inPosition: (%f, %f), (%f, %f, %f)", inPosition[0], inPosition[1], inColor[0], inColor[1], inColor[2]);
    //debugPrintfEXT("model = %", ubo.model[0]);
    print_mat4(ubo.model);
    //gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
    //gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
    //gl_Position = vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}