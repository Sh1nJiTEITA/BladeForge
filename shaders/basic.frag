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
    uint id_on_cursor;
} ubo;


layout(std140, set = 1, binding = 0) buffer ObjectDataBuffer {
    ObjectData obj_data[];
} obj_data_buffer;

layout(set = 0, binding = 1, r32ui) uniform  uimage2D id_map;
//layout(set = 0, binding = 1, r32ui) uniform  uimageBuffer id_map;

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

void write_id_map(ivec2 texCoord, uint id) {
    imageStore(id_map, texCoord, uvec4(id));
};



void main() {
    
    //debugPrintfEXT("i: %i", read_id_map(ivec2(ubo.cursor_pos)));

    uint zIndex = uint(gl_FragCoord.z * DEPTH_ARRAY_SCALE);

    ivec2 texCoord = ivec2(gl_FragCoord.xy); // ѕример текстурных координат (пиксельные координаты)

    // «апись значени€ ID в изображение
    write_id_map(texCoord, obj_data_buffer.obj_data[obj_index].id);

    if(length(ubo.cursor_pos - gl_FragCoord.xy) <= 1) {
        //fill_cursor_pointer_array(obj_data_buffer.obj_data[obj_index].id);
        cp.data[zIndex] = obj_data_buffer.obj_data[obj_index].id;      
        //debugPrintfEXT("z: %f, i: %i", gl_FragCoord.z, zIndex);
    }
    
    outColor = vec4(fragColor * obj_data_buffer.obj_data[obj_index].select_color, 1.0);
}