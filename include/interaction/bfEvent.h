#ifndef BF_EVENT_H
#define BF_EVENT_H

#include <chrono>
#include <ctime>
#include <forward_list>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

enum BfEnActionType
{
   // Destroy

   BF_ACTION_TYPE_DESTROY_IMGUI = 0x1A0,
   BF_ACTION_TYPE_DESTROY_SYNC_OBJECTS = 0x1A1,
   BF_ACTION_TYPE_DESTROY_IMGUI_COMMAND_BUFFERS = 0x1A2,
   BF_ACTION_TYPE_DESTROY_MAIN_COMMAND_BUFFERS = 0x1A3,
   BF_ACTION_TYPE_DESTROY_IMGUI_DESCRIPTOR_POOL = 0x1A4,
   // BF_ACTION_TYPE_DESTROY_MAIN_DESCRIPTOR_POOL =  0x1A5,
   BF_ACTION_TYPE_DESTROY_COMMAND_POOL = 0x1A6,
   BF_ACTION_TYPE_DESTROY_MAIN_FRAMEBUFFERS = 0x1A7,
   BF_ACTION_TYPE_DESTROY_IMGUI_FRAMEBUFFERS = 0x1A8,
   BF_ACTION_TYPE_DESTROY_DEPTH_BUFFER = 0x1A9,
   BF_ACTION_TYPE_DESTROY_PIPELINES = 0x1B0,
   BF_ACTION_TYPE_DESTROY_IDMAP = 0x1B1,
   BF_ACTION_TYPE_DESTROY_IMGUI_RENDER_PASS = 0x1B2,
   BF_ACTION_TYPE_DESTROY_MAIN_RENDER_PASS = 0x1B2,
   BF_ACTION_TYPE_DESTROY_IMAGE_VIEWS = 0x1B3,
   BF_ACTION_TYPE_DESTROY_SWAPCHAIN = 0x1B4,
   BF_ACTION_TYPE_DESTROY_SWAPCHAIN_PARTS = 0x1B5,
   BF_ACTION_TYPE_DESTROY_SURFACE = 0x1B6,
   BF_ACTION_TYPE_DESTROY_INSTANCE = 0x1B7,
   BF_ACTION_TYPE_DESTROY_VMA_ALLOCATOR = 0x1B8,
   BF_ACTION_TYPE_DESTROY_DEVICE = 0x1B9,

   // Initializations
   BF_ACTION_TYPE_INIT_INSTANCE_SUCCESS = 0x2,
   BF_ACTION_TYPE_INIT_INSTANCE_FAILURE = -0x2,

   BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_SUCCESS = 0x3,
   BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_FAILURE = -0x3,
   BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_NO_INIT = 0x3A,
   BF_ACTION_TYPE_DESTROY_DEBUG_MESSENGER_SUCCESS = 0x3B,
   BF_ACTION_TYPE_DESTROY_DEBUG_MESSENGER_FAILURE = -0x3B,
   BF_ACTION_TYPE_DESTROY_DEBUG_MESSENGER_NO_INIT = 0x3BA,

   BF_ACTION_TYPE_INIT_SURFACE_SUCCESS = 0x4,
   BF_ACTION_TYPE_INIT_SURFACE_FAILURE = -0x4,

   BF_ACTION_TYPE_INIT_GLFW_WINDOW_SUCCESS = 0x5,
   BF_ACTION_TYPE_INIT_GLFW_WINDOW_FAILURE = -0x5,

   BF_ACTION_TYPE_INIT_NEW_BF_WINDOW = 0x5A,

   BF_ACTION_TYPE_SET_WINDOW_SIZE_SUCCESS = 0x6,
   BF_ACTION_TYPE_SET_WINDOW_SIZE_FAILURE = -0x6,
   BF_ACTION_TYPE_SET_WINDOW_NAME = 0x6A,

   BF_ACTION_TYPE_GET_SWAPCHAIN_SUPPORT = 0x7,

   BF_ACTION_TYPE_CHECK_EXTENSION_SUPPORT_SUCCESS = 0x8,
   BF_ACTION_TYPE_CHECK_EXTENSION_SUPPORT_FAILURE = -0x8,

   BF_ACTION_TYPE_CHECK_QUEUE_SUPPORT_SUCCESS = 0x9,
   BF_ACTION_TYPE_CHECK_QUEUE_SUPPORT_FAILURE = -0x9,

   BF_ACTION_TYPE_SEARCH_NEEDED_QUEUES_SUCCESS = 0x10,
   BF_ACTION_TYPE_SEARCH_NEEDED_QUEUES_FAILURE = -0x10,

   BF_ACTION_TYPE_CHECK_PHYSICAL_DEVICE_SUITABILITY_SUCCESS = 0x11,
   BF_ACTION_TYPE_CHECK_PHYSICAL_DEVICE_SUITABILITY_FAILURE = -0x11,

   BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_SUCCESS = 0x12,
   BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_FAILURE = -0x12,
   BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_NO_GPU = 0x12A,

   BF_ACTION_TYPE_HOLDER_ADD_BF_WINDOW = 0x13,
   BF_ACTION_TYPE_HOLDER_ADD_BF_PHYSICAL_DEVICE = 0x14,

   BF_ACTION_TYPE_INIT_LOGICAL_DEVICE_SUCCESS = 0x15,
   BF_ACTION_TYPE_INIT_LOGICAL_DEVICE_FAILURE = -0x15,

   BF_ACTION_TYPE_GET_SWAPCHAIN_SURFACE_FORMAT = 0x16,
   BF_ACTION_TYPE_GET_SWAPCHAIN_PRESENT_MODE = 0x17,
   BF_ACTION_TYPE_GET_SWAPCHAIN_EXTENT = 0x18,

   BF_ACTION_TYPE_INIT_SWAPCHAIN_SUCCESS = 0x19,
   BF_ACTION_TYPE_INIT_SWAPCHAIN_FAILURE = -0x19,

   BF_ACTION_TYPE_INIT_IMAGE_VIEWS_SUCCESS = 0x20,
   BF_ACTION_TYPE_INIT_IMAGE_VIEWS_FAILURE = -0x20,

   BF_ACTION_TYPE_INIT_RENDER_PASS_SUCCESS = 0x21,
   BF_ACTION_TYPE_INIT_RENDER_PASS_FAILURE = -0x21,

   BF_ACTION_TYPE_INIT_DESCRIPTOR_SET_LAYOUT_SUCCESS = 0x22,
   BF_ACTION_TYPE_INIT_DESCRIPTOR_SET_LAYOUT_FAILURE = -0x22,
   BF_ACTION_TYPE_CREATE_DESCRIPTOR_POOL_SUCSESS = 0x22A,
   BF_ACTION_TYPE_CREATE_DESCRIPTOR_POOL_FAILURE = -0x22A,
   BF_ACTION_TYPE_ALLOCATE_DESCRIPTOR_BUFFERS_SUCCESS = 0x22B,
   BF_ACTION_TYPE_ALLOCATE_DESCRIPTOR_BUFFERS_FAILURE = -0x22B,
   BF_ACTION_TYPE_ADD_DESCRIPTOR_CREATE_INFO_SUCCESS = 0x22C,
   BF_ACTION_TYPE_ADD_DESCRIPTOR_CREATE_INFO_FAILURE = 0x22C,
   BF_ACTION_TYPE_DESTROY_BF_DESCRIPTOR_BUFFERS = 0x22D,
   BF_ACTION_TYPE_ALLOCATE_DESCRIPTOR_SETS_SUCCESS = 0x22E,
   BF_ACTION_TYPE_ALLOCATE_DESCRIPTOR_SETS_FAILURE = -0x22E,
   BF_ACTION_TYPE_UPDATE_DESCRIPTOR_SETS = 0x22F,
   BF_ACTION_TYPE_DESTROY_DESCRIPTOR_POOL = 0x22A0,
   BF_ACTION_TYPE_DESTROY_DESCRIPTOR_SET_LAYOUTS = 0x22A1,
   BF_ACTION_TYPE_DESTORY_DESCRIPTOR_IMAGES = 0x22A2,

   BF_ACTION_TYPE_READ_SHADER_FILE_SUCCESS = 0x23,
   BF_ACTION_TYPE_READ_SHADER_FILE_FAILURE = -0x23,

   BF_ACTION_TYPE_CREATE_SHADER_MODULE_SUCCESS = 0x24,
   BF_ACTION_TYPE_CREATE_SHADER_MODULE_FAILURE = -0x24,

   BF_ACTION_TYPE_CREATE_PIPELINE_LAYOUT_SUCCESS = 0x25,
   BF_ACTION_TYPE_CREATE_PIPELINE_LAYOUT_FAILURE = -0x25,

   BF_ACTION_TYPE_DESTROY_PIPELINE_LAYOUT = 0x25A,

   BF_ACTION_TYPE_CREATE_GRAPHICS_PIPELINE_SUCCESS = 0x26,
   BF_ACTION_TYPE_CREATE_GRAPHICS_PIPELINE_FAILURE = -0x26,

   BF_ACTION_TYPE_CREATE_FRAME_BUFFER_SUCCESS = 0x27,
   BF_ACTION_TYPE_CREATE_FRAME_BUFFER_FAILURE = -0x27,

   BF_ACTION_TYPE_CREATE_COMMAND_POOL_SUCCESS = 0x28,
   BF_ACTION_TYPE_CREATE_COMMAND_POOL_FAILURE = -0x28,

   BF_ACTION_TYPE_ALLOC_BUFFER_SUCCESS = 0x29,
   BF_ACTION_TYPE_ALLOC_BUFFER_FAILURE = -0x29,
   BF_ACTION_TYPE_DESTROY_BUFFER = 0x29A,

   BF_ACTION_INIT_WHOLE_DISCRIPTORS_SUCCESS = 0x30,
   BF_ACTION_INIT_WHOLE_DISCRIPTORS_FAILURE = -0x30,

   BF_ACTION_ALLOCATE_MESH_SUCCESS = 0x31,
   BF_ACTION_ALLOCATE_MESH_FAILURE = -0x31,

   BF_ACTION_UPLOAD_MESH_SUCCESS = 0x32,
   BF_ACTION_UPLOAD_MESH_FAILURE = -0x32,

   BF_CREATE_DEPTH_IMAGE_VIEW_SUCCESS = 0x33,
   BF_CREATE_DEPTH_IMAGE_VIEW_FAILURE = -0x33,

   BF_ACTION_TYPE_ALLOC_CURVE_SET_SUCCESS = 0x34,
   BF_ACTION_TYPE_ALLOC_CURVE_SET_FAILURE = -0x34,

   BF_ACTION_TYPE_BIND_HOLDER = 0x35,
   BF_ACTION_TYPE_BIND_CURVE_HOLDER = 0x36,

   BF_ACTION_TYPE_DEALLOC_CURVE_SET = 0x37,

   BF_ACTION_TYPE_ADD_CURVE_SET_TO_CURVE_HOLDER = 0x38,

   BF_ACTION_TYPE_BIND_OUTSIDE_ALLOCATOR_FOR_CURVE_HOLDER = 0x39,

   BF_ACTION_TYPE_ALLOC_GEOMETRY_SET_SUCCESS = 0x40,
   BF_ACTION_TYPE_ALLOC_GEOMETRY_SET_FAILURE = -0x40,

   BF_ACTION_TYPE_BIND_GEOMETRY_HOLDER = 0x41,

   BF_ACTION_TYPE_DEALLOC_GEOMETRY_SET = 0x42,

   BF_ACTION_TYPE_ADD_GEOMETRY_SET_TO_GEOMETRY_HOLDER = 0x43,

   BF_ACTION_TYPE_BIND_OUTSIDE_ALLOCATOR_FOR_GEOMETRY_HOLDER = 0x44,

   BF_ACTION_TYPE_ADD_GEOMETRY_TO_SET_SUCCESS = 0x45,
   BF_ACTION_TYPE_ADD_GEOMETRY_TO_SET_FAILURE = -0x45,

   BF_ACTION_TYPE_BIND_GRAPHICS_PIPELINE_TO_GEOMETRY_SET = 0x46,

   BF_ACTION_TYPE_BIND_GRAPHICS_PIPELINE_TO_BFLAYER = 0x47,
   BF_ACTION_TYPE_BIND_ALLOCATOR_TO_BFLAYER = 0x48,

   BF_ACTION_TYPE_ALLOC_BFLAYER_SUCCESS = 0x49,
   BF_ACTION_TYPE_ALLOC_BFLAYER_FAILURE = -0x49,

   BF_ACTION_TYPE_ADD_LAYER_TO_LAYER_HANDLER_SUCCESS = 0x50,
   BF_ACTION_TYPE_ADD_LAYER_TO_LAYER_HANDLER_FAILURE = -0x50,
   BF_ACTION_TYPE_BIND_BFDESCRIPTOR_TO_LAYER_HANDLER_SUCCESS = 0x50A,
   BF_ACTION_TYPE_BIND_BFDESCRIPTOR_TO_LAYER_HANDLER_FAILURE = -0x50A,
   BF_ACTION_TYPE_DEL_LAYER_FROM_LAYER_HANDLER_SUCCESS = 0x50B,
   BF_ACTION_TYPE_DEL_LAYER_FROM_LAYER_HANDLER_FAILURE = -0x50B,

   BF_ACTION_TYPE_CREATE_IMAGE_SUCCESS = 0x51,
   BF_ACTION_TYPE_CREATE_IMAGE_FAILURE = -0x51,
   BF_ACTION_TYPE_DESTROY_IMAGE = 0x52,

   BF_ACTION_TYPE_CREATE_IMAGE_VIEW_SUCCESS = 0x53,
   BF_ACTION_TYPE_CREATE_IMAGE_VIEW_FAILURE = -0x53,
   BF_ACTION_TYPE_DESTROY_IMAGE_VIEW = 0x54,

   BF_ACTION_TYPE_CREATE_IDMAP_SUCCESS = 0x55,
   BF_ACTION_TYPE_CREATE_IDMAP_FAILURE = -0x55,

   BF_ACTION_TYPE_CREATE_VMA_ALLOCATOR_SUCCESS = 0x56,
   BF_ACTION_TYPE_CREATE_VMA_ALLOCATOR_FAILURE = -0x56,

   BF_ACTION_TYPE_CREATE_SHADER_MODULE_PACK_SUCCESS = 0x57,
   BF_ACTION_TYPE_CREATE_SHADER_MODULE_PACK_FAILURE = -0x57,

   BF_ACTION_TYPE_CREATE_TEXTURE_DESCRIPTOR_SUCCESS = 0x58,
   BF_ACTION_TYPE_CREATE_TEXTURE_DESCRIPTOR_FAILURE = -0x58,

   BF_ACTION_TYPE_CREATE_TEXTURE_LOADER_SUCCESS = 0x59,
   BF_ACTION_TYPE_CREATE_TEXTURE_LOADER_FAILURE = -0x59,

   BF_ACTION_TYPE_BIND_BASE_TO_GUI_SUCCESS = 0x60,
   BF_ACTION_TYPE_BIND_BASE_TO_GUI_FAILURE = -0x60,

   BF_ACTION_TYPE_BIND_HOLDER_TO_GUI_SUCCESS = 0x61,
   BF_ACTION_TYPE_BIND_HOLDER_TO_GUI_FAILURE = -0x61,

   BF_ACTION_TYPE_LOAD_LUA_SCRIPT_SUCCESS = 0x62,
   BF_ACTION_TYPE_LOAD_LUA_SCRIPT_FAILURE = -0x62,

   BF_ACTION_TYPE_LOAD_STD_LUA_LIBRARY_SUCCESS = 0x63,
   BF_ACTION_TYPE_LOAD_STD_LUA_LIBRARY_FAILURE = -0x63,

   BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_SUCCESS = 0x64,
   BF_ACTION_TYPE_ADD_LUA_PACKAGE_PATH_FAILURE = -0x64,

   BF_ACTION_TYPE_FILL_FORM_SUCCESS = 0x65,
   BF_ACTION_TYPE_FILL_FORM_FAILURE = -0x65,

   BF_ACTION_TYPE_BIND_GUI_SETTINGS_SUCCESS = 0x66,
   BF_ACTION_TYPE_BIND_GUI_SETTINGS_FAILURE = -0x66,

   BF_ACTION_TYPE_FILL_FORM_FONT_NAME_INVALID_NAME_FAILURE = -0x67,
   BF_ACTION_TYPE_FILL_FORM_FONT_NAME_INVALID_TYPE_FAILURE = -0x68,

   BF_ACTION_TYPE_READ_FILE_SUCCESS = 0x69,
   BF_ACTION_TYPE_READ_FILE_FAILURE = -0x69,

   BF_ACTION_TYPE_BIND_ALLOCATOR_TO_LAYER_HANDLER_SUCCESS = 0x70,
   BF_ACTION_TYPE_BIND_ALLOCATOR_TO_LAYER_HANDLER_FAILURE = -0x70,

   BF_ACTION_TYPE_BIND_TRIANGLE_PIPELINE_TO_LAYER_HANDLER_SUCCESS = 0x71,
   BF_ACTION_TYPE_BIND_TRIANGLE_PIPELINE_TO_LAYER_HANDLER_FAILURE = -0x71,

   BF_ACTION_TYPE_BIND_LINE_PIPELINE_TO_LAYER_HANDLER_SUCCESS = 0x72,
   BF_ACTION_TYPE_BIND_LINE_PIPELINE_TO_LAYER_HANDLER_FAILURE = -0x71,

   BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_LAYER_OR_OBJ_NOT_EXISTS = -0x73,
   BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_LAYER_NOT_NESTED = -0x73A,
   BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_MOVING_INSIDE_OBJ = -0x73B,
   BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_MOVING_LAYER_TO_LAYER = 0x73A,
   BF_ACTION_TYPE_LAYER_HANDLER_TRANSACTION_MOVING_OBJ_TO_LAYER = 0x73B,
};

const std::map<int, std::string> bfSetActionsStr{
    // Destroy
    {0x1A0, "ImGUI were destroyed"},
    {0x1A1, "Sync objects were destroyed"},
    {0x1A2, "ImGUI command buffers were freed"},
    {0x1A3, "Main command buffers were freed"},
    {0x1A4, "ImGUI descriptor pool was destroyed"},
    //{ 0x1A5, "Main descriptor pool was destroyed"},
    {0x1A6, "VkCommandPool was destroyed"},
    {0x1A7, "ImGUI Framebuffers were destroyed"},
    {0x1A8, "Main Framebuffers were destroyed"},
    {0x1A9, "Depth buffer parts were destroyed"},
    {0x1B0, "VkPipeline's were destroyed"},
    {0x1B1, "IdMap parts were destroyed"},
    {0x1B2, "ImGUI Render pass was destroyed"},
    {0x1B3, "Image views of SwapChain images were destroyed"},
    {0x1B4, "VkSwapchain was destroyed"},
    {0x1B5, "VkSwapchain parts were destroyed"},
    {0x1B6, "VkSurfaceKHR was destroyed"},
    {0x1B7, "VkInstance was destroyed"},
    {0x1B8, "VmaAllocator was destroyed"},
    {0x1B9, "VkDevice was destroyed"},

    // Initializations
    {0x2, "Vulkan Instance was created successfully"},
    {-0x2, "Vulkan Instance wasn't created"},

    {0x3, "Vulkan Debug messenger was created successfully"},
    {-0x3, "Vulkan Debug messenger wasn't created"},
    {0x3A,
     "Vulkan Debug messenger wasn't created due to disabled validation layers"},
    {0x3B, "Vulkan Debug messenger was destroyed"},
    {-0x3B, "Vulkan Debug messenger was not destroyed"},
    {0x3BA, "Vulkan Debut messenger was not destroyed -> was not created before"
    },

    {0x4, "Vulkan Surface was created successfully"},
    {-0x4, "Vulkan Surface wasn't created"},

    {0x5, "GLFW window was created successfully"},
    {-0x5, "GLFW window wasn't created"},
    {0x5A, "BfWindow was initialized"},

    {0x6, "Window size was successfully resized"},
    {-0x6, "Window size wasn't resized: w <= 0 or h <= 0"},
    {0x6A, "Window name was set"},

    {0x7, "Swapchain support was checked"},

    {0x8, "Physical device supports all needed extensions"},
    {-0x8, "Physical device doesn't support all needed extensions"},

    {0x9, "Physical device contains all needed queue-families for work"},
    {-0x9, "Physical device doesn't contain all needed queue-families for work"
    },

    {0x10, "All queue-families were found correctly"},
    {-0x10, "Not all queue-families were found correctly"},

    {0x11, "Chosen physical device is suitable"},
    {-0x11, "Chosen physical device isn't suitable"},

    {0x12, "VkPhysicalDevice was created successfully"},
    {-0x12, "VkPhysicalDevice wasn't created"},
    {0x12A, "VkPHysicalDevice wasn't created, bacause there are no GPU"},

    {0x13, "New bfWindow was added to holder"},
    {0x14, "New BfPhysicalDevice was added to holder"},

    {0x15, "VkLogicalDevice was created sucessfully"},
    {-0x15, "VkLogicalDevice wasn't created"},

    {0x16, "Swapchain surface format was chosen:"},
    {0x17, "Swapchain surface present mode was chosen:"},
    {0x18, "Swapchain surface extent was chosen:"},

    {0x19, "Swapchain was created successfully"},
    {-0x19, "Swapchain wasn't created"},

    {0x20, "VkImageView was created sucessfully:"},
    {-0x20, "VkImageView wasn't created:"},

    {0x21, "VkRenderpass was created successfully:"},
    {-0x21, "VkRenderpass wasn't created:"},

    {0x22, "VkDescriptorSetLayout was created successfully"},
    {-0x22, "VkDescriptorSetLayout wasn't created sucessfully"},
    {0x22A, "VkDescriptorPool was created successfully"},
    {-0x22A, "VkDEscriptorPoll wasn't created"},
    {0x22B, "BfAllocated buffers for BfDescriptor was successfully allocated"},
    {-0x22B, "BfAllocated buffers for BfDescriptor wasn't allocated:"},
    {0x22C, "BfDescriptorCreateInfo was added to BfDescriptor"},
    {-0x22C, "BfDescriptorCreateInfo wasn't added to BfDescriptor"},
    {0x22D, "Buffers in BfDescriptor were destroyed"},
    {0x22E, "BfDescriptor sets were successfully allocated:"},
    {-0x22E, "BfDescriptor sets were not allocated:"},
    {0x22F, "BfDescriptor sets were updated"},
    {0x22A0, "VkDescriptorPool was destroyed in BfDescriptor"},
    {0x22A1, "VkDescriptorSetLayouts were destoyed in BfDescriptor"},
    {0x22A2, "VkImage's & VkImageView's were destroyed in BfDescriptor"},

    {0x23, "Shader file was read successfully:"},
    {-0x23, "Shader file wasn't read successfully:"},

    {0x24, "VkShaderModule was created successfully:"},
    {-0x24, "VkShaderModule wasn't created:"},

    {0x25, "VkPipelineLayout was created successfully"},
    {-0x25, "VkPipelineLayout wasn't created"},
    {0x25A, "VkPipelineLayout was destroyed:"},

    {0x26, "VkPipeline's was created sucessfully:"},
    {-0x26, "VkPipeline's wasn't created:"},

    {0x27, "VkFramebuffer was created successfully:"},
    {-0x27, "VkFramebuffer wasn't created:"},

    {0x28, "VkCommandPool was created successfully"},
    {-0x28, "VkCommandPool wasn't created"},

    {0x29, "VkBuffer was created succesfully"},
    {-0x29, "VkBuffer wasn't created"},
    {0x29A, "VkBuffer was destroyed"},

    {0x30, "Whole descriptors stuff was made successfully:"},
    {-0x30, "Not whole descriptors stuff was made successfully:"},

    {0x31, "Memory for mesh was allocated successfully:"},
    {-0x31, "Memory for mesh wasn't allocated:"},

    {0x32, "Mesh was uploaded successfully:"},
    {-0x32, "Mesh wasn't uploaded"},

    {0x33, "VkImageView for depth buffer was created successfully"},
    {-0x33, "VkImageView for depth buffer wasn't created"},

    {0x34, "BfCurveSet was allocated successfully:"},
    {-0x34, "BfCurveSet wasn't allocated:"},

    {0x35, "BfHolder was bound"},
    {0x36, "BfCurveHolder was bound"},

    {0x37, "BfCurveSet was deallocated:"},

    {0x38, "BfCurveSet was added to BfCurveHolder:"},
    {0x39, "VmaAllocator was bound to bound BfCurveHolder"},

    {0x40, "BfGeometrySet was allocated successfully:"},
    {-0x40, "BfGeometrySet wasn't allocated:"},

    {0x41, "BfGeometryHolder was bound"},

    {0x42, "BfGeometrySet was deallocated:"},

    {0x43, "BfGeometrySet was added to BfGeometryHolder:"},

    {0x44, "VmaAllocator was bound to bound BfGeometryHolder"},

    {0x45, "Vertices/Indices/BfGeometryData was added to set:"},
    {-0x45, "Vertices/Indices/BfGeometryData wasn't added to set:"},

    {0x46, "Graphics pipeline was bound to BfGeometrySet:"},

    {0x47, "Graphics pipeline was bound to BfLayer:"},

    {0x48, "VmaAllocator was bound to BfLayer:"},

    {0x49, "Buffers for BfLayer was allocated succefully:"},
    {-0x49, "Buffers for BfLater wasn't allocated:"},

    {0x50, "BfLayer was added to BfLayerHandler successfully:"},
    {-0x50, "BfLayer wasn't added to BfLayerHandler:"},
    {0x50A, "BfDescriptor was bound to BfLayerHandler"},
    {-0x50A, "BfDescriptor wasn't bound to BfLayerHandler: pointer is nullptr"},
    {0x50B, "BfLayer was deleted from BfLayerHandler"},
    {-0x50B, "BfLayer wasn't deleted from BfLayerHandler"},

    {0x51, "VkImage was created"},
    {-0x51, "VkImage wasn't created"},
    {0x52, "VkImage was destroyed"},

    {0x53, "VkImageView was created"},
    {-0x53, "VkImageView wasm't created"},
    {0x54, "VkImageView was destroyed"},

    {0x55, "IdMap parts were created successfully"},
    {-0x55, "IdMap parts weren't created"},

    {0x56, "VmaAllocator was created"},
    {-0x56, "VmaALlocator wasn't created"},

    {0x57, "Shader Module pack was created:"},
    {-0x57, "Shader Module pack wasn't created:"},
    {0x58, "Adding texture to descriptor was successfull"},
    {-0x58, "Adding texture to descriptor handler was not successfull:"},

    {0x59, "BfTextureLoader was successfully created"},
    {-0x59, "BfTextureLoader was not created"},

    {0x60, "BfBase was succesfully bound to gui-class"},
    {-0x60, "BfBase was not bind to gui-class due to null ptr"},

    {0x61, "BfHolder was succesfully bound to gui-class"},
    {-0x61, "BfHolder was not bind to gui-class due to null ptr"},

    {0x62, "Lua Script was loaded:"},
    {-0x62, "Lua Script wasn't loaded due to:"},

    {0x63, "Standart lua library was loaded, input lib name:"},
    {-0x63, "Standart lua library wasn't loaded, input lib name:"},

    {0x64, "Added package path to lua-state:"},
    {-0x64, "Cant add package path to lua-state:"},

    {0x65, "Form loaded:"},
    {-0x65, "Form did not load:"},

    {0x66, "Gui settings were bound successfully"},
    {-0x66, "Gui settings were not bound:"},

    {-0x67,
     "No name inside font table provided "
     "might be lua-table names or string of name"},

    {-0x68,
     "Invalid settings->fonts lua type or no standart font provided in "
     "standart directory './resources/fonts/Cousine-Regular.ttf"},

    {0x69, "Text file was read sucessfully, path:"},
    {-0x69, "Text file was not read, path:"},

    {0x70, "BfAllocator was bound to BfLayerHandler"},
    {-0x70, "BfAllocator wasn't bound to BfLayerHandler"},

    {0x71, "Triangle pipeline was bound to BfLayerHandler"},
    {-0x71, "Triangle pipeline wasn't bound to BfLayerHandler"},

    {0x72, "Line pipeline was bound to BfLayerHandler"},
    {-0x72, "Line pipeline wasn't bound to BfLayerHandler"},

    {-0x73, "Moving layer or object do not exist"},
    {-0x73A, "Moving not nested layer"},
    {-0x73B, "Moving smt inside obj"},
    {0x73A, "Moving layer inside layer"},
    {0x73B, "Moving obj inside layer"},
};

enum BfEnSingleEventType
{
   BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT = 0x1,
   BF_SINGLE_EVENT_TYPE_HOLDER_EVENT = 0x2,
   BF_SINGLE_EVENT_TYPE_USER_EVENT = 0x4,
   BF_SINGLE_EVENT_TYPE_CHECK_EVENT = 0x8,
   BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT = 0x10,
   BF_SINGLE_EVENT_TYPE_CURVE_HOLDER_EVENT = 0x20,
   BF_SINGLE_EVENT_TYPE_GEOMETRY_HOLDER_EVENT = 0x40,
   BF_SINGLE_EVENT_TYPE_GEOMETRY_SET_EVENT = 0x80,
   BF_SINGLE_EVENT_TYPE_LAYER_EVENT = 0x100,
   BF_SINGLE_EVENT_TYPE_DESTROY_EVENT = 0x200

};

enum BfEnMultipleEventType
{
   BF_MULTIPLE_EVENT_TYPE_RENDER_FRAME = 0,
};

struct BfSingleEvent
{
   BfEnSingleEventType type;
   BfEnActionType action;
   std::string info;
   std::time_t time;
   bool success = true;
};

struct BfMultipleEvent
{
   BfEnMultipleEventType type;
   uint32_t repet;

   bool operator<(const BfMultipleEvent& other) const;
};

struct BfEvent
{
   BfSingleEvent single_event;
   BfMultipleEvent multiple_event;

   BfEvent();
   BfEvent(BfSingleEvent single_event_);
   BfEvent(BfMultipleEvent multiple_event_);

   void record_event();

private:
   enum class BfEnEventType
   {
      BF_EVENT_TYPE_INVALID = 0,
      BF_EVENT_TYPE_SINGLE = 1,
      BF_EVENT_TYPE_MULTIPLE = 2
   };
   BfEnEventType current_event_type;
};

struct BfEventHandler
{
   static std::list<BfSingleEvent> single_events;
   static std::list<std::string> single_event_time;
   static std::list<std::string> single_event_message;
   // std::set<BfMultipleEvent> multiple_events;

   static void add_single_event(BfSingleEvent event);
   static void add_multiple_event(BfMultipleEvent event);

   static const std::list<BfSingleEvent>& get_single_events();
   static std::time_t get_time();

   static bool is_all_ok();

   typedef void (*FunctionPointer)(BfSingleEvent);
   typedef std::string (*FunctionPointerStr)(BfSingleEvent);
   // using FunctionPointerStr = std::string(*)(BfSingleEvent);

   static FunctionPointer funcPtr;
   static FunctionPointerStr funcPtrStr;

   static std::string whole_log;
};

std::string time_t_to_text(const time_t& time);

#endif
