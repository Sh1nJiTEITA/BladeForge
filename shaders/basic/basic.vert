#version 460
#extension GL_EXT_debug_printf : enable
// Structs 

struct BezierPoints {
	vec2 coo;
};

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



//layout(set = 0, binding = 1, r32ui) uniform uimage2D id_map;
//layout(set = 0, binding = 1, r32f) uniform image2DArray id_map;
//layout(set = 0, binding = 1, r32ui) uniform  uimageBuffer id_map;

layout(std140, set = 1, binding = 0) buffer ObjectDataBuffer {
    ObjectData obj_data[];
} obj_data_buffer;

//#define DEPTH_ARRAY_SCALE 2048
//layout(set = 1, binding = 1) buffer cursor_picking {
//    uint data[DEPTH_ARRAY_SCALE];
//} cp;



layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormals;

layout(location = 2) out vec3 outNormals;
layout(location = 0) out vec3 fragColor;
layout(location = 4) flat out uint obj_index;


void print_mat4(mat4 matrix) {
    debugPrintfEXT("matrix[0] = %f, %f, %f, %f", matrix[0][0],matrix[0][1],matrix[0][2],matrix[0][3]);
    debugPrintfEXT("matrix[1] = %f, %f, %f, %f", matrix[1][0],matrix[1][1],matrix[1][2],matrix[1][3]);
    debugPrintfEXT("matrix[2] = %f, %f, %f, %f", matrix[2][0],matrix[2][1],matrix[2][2],matrix[2][3]);
    debugPrintfEXT("matrix[3] = %f, %f, %f, %f", matrix[3][0],matrix[3][1],matrix[3][2],matrix[3][3]);
};

//uint read_id_map(ivec2 texCoord) { 
//    uvec4 value = imageLoad(id_map, texCoord);
//    return value.x;
//}
//
//void write_id_map(ivec2 texCoord, uint id)  {
//    imageStore(id_map, texCoord, uvec4(id));
//};


void main() {
    
//    //print_mat4(obj_data_buffer.obj_data[gl_BaseInstance].model_matrix );
//    debugPrintfEXT("pos = %f, %f. %f", inPosition[0], inPosition[1], inPosition[2]);
//    debugPrintfEXT("col = %f, %f. %f", inColor[0], inColor[1], inColor[2]);
//    debugPrintfEXT("nor = %f, %f. %f", inNormals[0], inNormals[1], inNormals[2]);

    vec4 coo = ubo.proj * ubo.view * obj_data_buffer.obj_data[gl_BaseInstance].model_matrix * vec4(inPosition, 1.0);


    outNormals = inNormals;
    gl_Position = coo;
    obj_index = gl_BaseInstance;
 
    //pos_distance = length(inPosition);
    //for (int i = 0; i < DEPTH_ARRAY_SCALE+1; i++) {

//    if (obj_data_buffer.obj_data[gl_BaseInstance].id == cp.data[DEPTH_ARRAY_SCALE-1]) {
//        cp.data[DEPTH_ARRAY_SCALE-1] = 0;
//        fragColor = vec3(0.0, 1.0, 0.5);
//    }
//    else {
//        fragColor = inColor;
//    }
    //debugPrintfEXT("%f %f", ubo.cursor_pos.x, ubo.cursor_pos.y);
    //uvec4 pixel_value = imageLoad(id_map, ivec2(ubo.cursor_pos));
    //debugPrintfEXT("%i, %i, %i, %i", pixel_value.x, pixel_value.y, pixel_value.z, pixel_value.w);
    if (obj_data_buffer.obj_data[gl_BaseInstance].id == ubo.id_on_cursor) {                
        fragColor = vec3(0.0, 1.0, 0.5);
    }
    else {
        fragColor = inColor;
    }


//    for (int i = DEPTH_ARRAY_SCALE-1; i >= 0; i--) {
//        if (obj_data_buffer.obj_data[gl_BaseInstance].id == cp.data[i]) {
//            //cp.data[i] = 0;
//            fragColor = vec3(0.0, 1.0, 0.5);
//            break;
//        }
//        else {
//            fragColor = inColor;
//        }
//    }
//    for (int i = 0; i < DEPTH_ARRAY_SCALE; i++) {
//        cp.data[i] = 0;
//    }
}