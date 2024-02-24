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
} ubo;

layout(std140, set = 1, binding = 0) buffer ObjectDataBuffer {
    ObjectData obj_data[];
} obj_data_buffer;

#define DEPTH_ARRAY_SCALE 2048
layout(set = 1, binding = 1) buffer cursor_picking {
    uint data[DEPTH_ARRAY_SCALE];
} cp;




layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

layout(location = 4) flat in uint obj_index;

void fill_cursor_pointer_array(uint iid) {
    for (int i = DEPTH_ARRAY_SCALE - 1; i >= 0; i--) {
        if (cp.data[i] == iid) return;
        if (cp.data[i] != 0) {
            continue;
        }
        else {
            cp.data[i] = iid;
            return;
        }
    }
};


void main() {
   
    uint zIndex = uint(gl_FragCoord.z * DEPTH_ARRAY_SCALE);


    if(length(ubo.cursor_pos - gl_FragCoord.xy) <= 1) {
        //fill_cursor_pointer_array(obj_data_buffer.obj_data[obj_index].id);
        cp.data[zIndex] = obj_data_buffer.obj_data[obj_index].id;      
        debugPrintfEXT("z: %f, i: %i", gl_FragCoord.z, zIndex);
    }
    
    outColor = vec4(fragColor * obj_data_buffer.obj_data[obj_index].select_color, 1.0);
}