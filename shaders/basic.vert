#version 460
#extension GL_EXT_debug_printf : enable

// Structs 

struct BezierPoints {
	vec2 coo;
};


// Layouts 
layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(set = 0, binding = 1) uniform BezierProperties {
    int points_count;
} bez;


layout(std140, set = 1, binding = 0) readonly buffer BezierPointsBuffer {
    vec2 point_i[];
} bezp;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;



void print_mat4(mat4 matrix) {
    debugPrintfEXT("matrix[0] = %f, %f, %f, %f", matrix[0][0],matrix[0][1],matrix[0][2],matrix[0][3]);
    debugPrintfEXT("matrix[1] = %f, %f, %f, %f", matrix[1][0],matrix[1][1],matrix[1][2],matrix[1][3]);
    debugPrintfEXT("matrix[2] = %f, %f, %f, %f", matrix[2][0],matrix[2][1],matrix[2][2],matrix[2][3]);
    debugPrintfEXT("matrix[3] = %f, %f, %f, %f", matrix[3][0],matrix[3][1],matrix[3][2],matrix[3][3]);
};

//void print_bezier_points(BezierPoints bp) {
//    
//};

bool is_print = true;
void main() {
    
    
    //debugPrintfEXT("inPosition: [%i](%f, %f)", gl_BaseInstance, bezp.point_i[gl_BaseInstance][0], bezp.point_i[gl_BaseInstance][1]);
    //debugPrintfEXT("model = %", ubo.model[0]);
    
    //debugPrintfEXT("BezierProp: %i", bez.points_count);
//    if (is_print == true) {
//        for (int i=0;i<bez.points_count;i++) {
//            debugPrintfEXT("bezier_point[%i] = %f, %f", i, bezp.point_i[i][0], bezp.point_i[i][1]);    
//        }
//        is_print = false;
//    }
    
    //!vec2 position_from_storage = bezp.point_i[gl_VertexIndex];
    //!vec2 position_from_storage1 = bezp.point_i[gl_VertexIndex] + inPosition;
    
    //debugPrintfEXT("%f, %f", position_from_storage1[0],position_from_storage1[1]);
    //debugPrintfEXT("[%i] %f, %f", gl_VertexIndex,inPosition[0],inPosition[1]);


    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    //!vec4 out_coo = ubo.proj * ubo.view * ubo.model * vec4(inPosition + bezp.point_i[gl_VertexIndex], 0.0, 1.0);
    //!debugPrintfEXT("%f, %f", out_coo[0],out_coo[1], out_coo[2], out_coo[3]);
    //gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vec2(0.0,0.0) +  bezp.point_i[gl_VertexIndex], 0.0, 1.0);

    //debugPrintfEXT("%f, %f", position_from_storage1[0],position_from_storage1[1]);


    //gl_Position = vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
    //!fragColor = vec3(1.0,1.0,1.0);
}