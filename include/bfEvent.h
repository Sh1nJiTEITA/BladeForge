#ifndef BF_EVENT_H
#define BF_EVENT_H

#include <vector>
#include <list>
#include <forward_list>
#include <chrono>
#include <ctime>
#include <string>
#include <optional>
#include <set>
#include <map>


enum class BfEnActionType {
	// Initializations
	BF_ACTION_TYPE_INIT_INSTANCE_SUCCESS					 =  0x2,
	BF_ACTION_TYPE_INIT_INSTANCE_FAILURE					 = -0x2,
															 
	BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_SUCCESS				 =  0x3,
	BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_FAILURE				 = -0x3,
	BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_NO_INIT				 =  0x3A,
															 
	BF_ACTION_TYPE_INIT_SURFACE_SUCCESS						 =  0x4,
	BF_ACTION_TYPE_INIT_SURFACE_FAILURE						 = -0x4,
															 
	BF_ACTION_TYPE_INIT_GLFW_WINDOW_SUCCESS					 =  0x5,
	BF_ACTION_TYPE_INIT_GLFW_WINDOW_FAILURE					 = -0x5,
	
	BF_ACTION_TYPE_INIT_NEW_BF_WINDOW						 =  0x5A,

	BF_ACTION_TYPE_SET_WINDOW_SIZE_SUCCESS					 =  0x6,
	BF_ACTION_TYPE_SET_WINDOW_SIZE_FAILURE					 = -0x6,
	BF_ACTION_TYPE_SET_WINDOW_NAME							 =  0x6A,
							 
	BF_ACTION_TYPE_GET_SWAPCHAIN_SUPPORT					 =  0x7,
															 
	BF_ACTION_TYPE_CHECK_EXTENSION_SUPPORT_SUCCESS			 =  0x8,
	BF_ACTION_TYPE_CHECK_EXTENSION_SUPPORT_FAILURE			 = -0x8,
															 
	BF_ACTION_TYPE_CHECK_QUEUE_SUPPORT_SUCCESS				 =  0x9,
	BF_ACTION_TYPE_CHECK_QUEUE_SUPPORT_FAILURE				 = -0x9,
															 
	BF_ACTION_TYPE_SEARCH_NEEDED_QUEUES_SUCCESS				 =  0x10,
	BF_ACTION_TYPE_SEARCH_NEEDED_QUEUES_FAILURE				 = -0x10,

	BF_ACTION_TYPE_CHECK_PHYSICAL_DEVICE_SUITABILITY_SUCCESS =  0x11,
	BF_ACTION_TYPE_CHECK_PHYSICAL_DEVICE_SUITABILITY_FAILURE = -0x11,

	BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_SUCCESS				 =  0x12,
	BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_FAILURE				 = -0x12,
	BF_ACTION_TYPE_INIT_PHYSICAL_DEVICE_NO_GPU				 =  0x12A,

	BF_ACTION_TYPE_HOLDER_ADD_BF_WINDOW						 =  0x13,
	BF_ACTION_TYPE_HOLDER_ADD_BF_PHYSICAL_DEVICE			 =  0x14,

	BF_ACTION_TYPE_INIT_LOGICAL_DEVICE_SUCCESS				 =  0x15,
	BF_ACTION_TYPE_INIT_LOGICAL_DEVICE_FAILURE				 = -0x15,

	BF_ACTION_TYPE_GET_SWAPCHAIN_SURFACE_FORMAT				 =  0x16,
	BF_ACTION_TYPE_GET_SWAPCHAIN_PRESENT_MODE				 =  0x17,
	BF_ACTION_TYPE_GET_SWAPCHAIN_EXTENT						 =  0x18,

	BF_ACTION_TYPE_INIT_SWAPCHAIN_SUCCESS					 =  0x19,
	BF_ACTION_TYPE_INIT_SWAPCHAIN_FAILURE					 = -0x19,

	BF_ACTION_TYPE_INIT_IMAGE_VIEWS_SUCCESS					 =  0x20,
	BF_ACTION_TYPE_INIT_IMAGE_VIEWS_FAILURE					 = -0x20,

	BF_ACTION_TYPE_INIT_RENDER_PASS_SUCCESS					 =  0x21,
	BF_ACTION_TYPE_INIT_RENDER_PASS_FAILURE					 = -0x21,
	
	BF_ACTION_TYPE_INIT_DESCRIPTOR_SET_LAYOUT_SUCCESS		 =  0x22,
	BF_ACTION_TYPE_INIT_DESCRIPTOR_SET_LAYOUT_FAILURE		 = -0x22,

	BF_ACTION_TYPE_READ_SHADER_FILE_SUCCESS					 =  0x23,
	BF_ACTION_TYPE_READ_SHADER_FILE_FAILURE					 = -0x23,

	BF_ACTION_TYPE_CREATE_SHADER_MODULE_SUCCESS				 =  0x24,
	BF_ACTION_TYPE_CREATE_SHADER_MODULE_FAILURE				 = -0x24,

	BF_ACTION_TYPE_CREATE_PIPELINE_LAYOUT_SUCCESS			 =  0x25,
	BF_ACTION_TYPE_CREATE_PIPELINE_LAYOUT_FAILURE			 = -0x25,

	BF_ACTION_TYPE_CREATE_GRAPHICS_PIPELINE_SUCCESS			 =  0x26,
	BF_ACTION_TYPE_CREATE_GRAPHICS_PIPELINE_FAILURE			 = -0x26,

	BF_ACTION_TYPE_CREATE_FRAME_BUFFER_SUCCESS				 =  0x27,
	BF_ACTION_TYPE_CREATE_FRAME_BUFFER_FAILURE				 = -0x27,

	BF_ACTION_TYPE_CREATE_COMMAND_POOL_SUCCESS				 =  0x28,
	BF_ACTION_TYPE_CREATE_COMMAND_POOL_FAILURE				 = -0x28,

	BF_ACTION_TYPE_ALLOC_BUFFER_SUCCESS						 =  0x29,
	BF_ACTION_TYPE_ALLOC_BUFFER_FAILURE						 = -0x29,

	BF_ACTION_INIT_WHOLE_DISCRIPTORS_SUCCESS				 =  0x30,
	BF_ACTION_INIT_WHOLE_DISCRIPTORS_FAILURE				 = -0x30,

	BF_ACTION_ALLOCATE_MESH_SUCCESS							 =  0x31,
	BF_ACTION_ALLOCATE_MESH_FAILURE							 = -0x31,

	BF_ACTION_UPLOAD_MESH_SUCCESS							 =  0x32,
	BF_ACTION_UPLOAD_MESH_FAILURE							 = -0x32,

	BF_CREATE_DEPTH_IMAGE_VIEW_SUCCESS						 =  0x33,
	BF_CREATE_DEPTH_IMAGE_VIEW_FAILURE						 = -0x33,

	BF_ACTION_TYPE_ALLOC_CURVE_SET_SUCCESS					 =  0x34,
	BF_ACTION_TYPE_ALLOC_CURVE_SET_FAILURE					 = -0x34,

	BF_ACTION_TYPE_BIND_HOLDER								 =  0x35,
	BF_ACTION_TYPE_BIND_CURVE_HOLDER						 =  0x36,

	BF_ACTION_TYPE_DEALLOC_CURVE_SET						 =  0x37,

	BF_ACTION_TYPE_ADD_CURVE_SET_TO_CURVE_HOLDER			 =  0x38,

	BF_ACTION_TYPE_BIND_OUTSIDE_ALLOCATOR_FOR_CURVE_HOLDER   =  0x39,

	BF_ACTION_TYPE_ALLOC_GEOMETRY_SET_SUCCESS				 =  0x40,
	BF_ACTION_TYPE_ALLOC_GEOMETRY_SET_FAILURE				 = -0x40,

	BF_ACTION_TYPE_BIND_GEOMETRY_HOLDER						 =  0x41,

	BF_ACTION_TYPE_DEALLOC_GEOMETRY_SET						 =  0x42,

	BF_ACTION_TYPE_ADD_GEOMETRY_SET_TO_GEOMETRY_HOLDER		 =  0x43,

	BF_ACTION_TYPE_BIND_OUTSIDE_ALLOCATOR_FOR_GEOMETRY_HOLDER = 0x44,

	BF_ACTION_TYPE_ADD_GEOMETRY_TO_SET_SUCCESS				 =  0x45,
	BF_ACTION_TYPE_ADD_GEOMETRY_TO_SET_FAILURE				 = -0x45,

	BF_ACTION_TYPE_BIND_GRAPHICS_PIPELINE_TO_GEOMETRY_SET	 =  0x46,

	BF_ACTION_TYPE_BIND_GRAPHICS_PIPELINE_TO_BFLAYER		 =  0x47,
	BF_ACTION_TYPE_BIND_ALLOCATOR_TO_BFLAYER				 =  0x48,

	BF_ACTION_TYPE_ALLOC_BFLAYER_SUCCESS					 =  0x49,
	BF_ACTION_TYPE_ALLOC_BFLAYER_FAILURE					 = -0x49
};

const std::map<int, std::string> bfSetActionsStr{
	// Initializations
	{ 0x2,   "Vulkan Instance was created successfully"},
	{-0x2,   "Vulkan Instance wasn't created"},

	{ 0x3,   "Vulkan Debug messenger was created successfully"},
	{-0x3,   "Vulkan Debug messenger wasn't created"},
	{ 0x3A,  "Vulkan Debug messenger wasn't created due to disabled validation layers"},

	{ 0x4,   "Vulkan Surface was created successfully"},
	{-0x4,   "Vulkan Surface wasn't created"},

	{ 0x5,   "GLFW window was created successfully"},
	{-0x5,   "GLFW window wasn't created"},
	{ 0x5A,  "BfWindow was initialized"},

	{ 0x6,   "Window size was successfully resized"},
	{-0x6,   "Window size wasn't resized: w <= 0 or h <= 0"},
	{ 0x6A,  "Window name was set"},
		     
	{ 0x7,   "Swapchain support was checked"},
		     
	{ 0x8,   "Physical device supports all needed extensions"},
	{-0x8,   "Physical device doesn't support all needed extensions"},
		     
	{ 0x9,   "Physical device contains all needed queue-families for work"},
	{-0x9,   "Physical device doesn't contain all needed queue-families for work"},
			 
	{ 0x10,  "All queue-families were found correctly"},
	{-0x10,  "Not all queue-families were found correctly"},
			 
	{ 0x11,  "Chosen physical device is suitable"},
	{-0x11,  "Chosen physical device isn't suitable"},
			 
	{ 0x12,  "VkPhysicalDevice was created successfully"},
	{-0x12,  "VkPhysicalDevice wasn't created"},
	{ 0x12A, "VkPHysicalDevice wasn't created, bacause there are no GPU"},

	{ 0x13,  "New bfWindow was added to holder"},
	{ 0x14,  "New BfPhysicalDevice was added to holder"},

	{ 0x15,  "VkLogicalDevice was created sucessfully"},
	{-0x15,  "VkLogicalDevice wasn't created"},

	{ 0x16,  "Swapchain surface format was chosen:"},
	{ 0x17,  "Swapchain surface present mode was chosen:"},
	{ 0x18,  "Swapchain surface extent was chosen:"},

	{ 0x19,  "Swapchain was created successfully"},
	{-0x19,  "Swapchain wasn't created"},

	{ 0x20,  "VkImageView was created sucessfully:"},
	{-0x20,  "VkImageView wasn't created:"},

	{ 0x21,  "VkRenderpass was created successfully:"},
	{-0x21,  "VkRenderpass wasn't created:"},

	{ 0x22,  "VkDescriptorSetLayout was created successfully"},
	{-0x22,  "VkDescriptorSetLayout wasn't created sucessfully"},

	{ 0x23,  "Shader file was read successfully:"},
	{-0x23,  "Shader file wasn't read successfully:"},

	{ 0x24,  "VkShaderModule was created successfully:"},
	{-0x24,  "VkShaderModule wasn't created:"},

	{ 0x25,  "VkPipelineLayout was created successfully"},
	{-0x25,  "VkPipelineLayout wasn't created"},

	{ 0x26,  "VkPipeline's was created sucessfully:"},
	{-0x26,  "VkPipeline's wasn't created:"},

	{ 0x27,  "VkFramebuffer was created successfully:"},
	{-0x27,  "VkFramebuffer wasn't created:"},

	{ 0x28,  "VkCommandPool was created successfully"},
	{-0x28,  "VkCommandPool wasn't created"},

	{ 0x29,  "VkBuffer was created succesfully"},
	{-0x29,  "VkBuffer wasn't created"},

	{ 0x30,  "Whole descriptors stuff was made successfully:"},
	{-0x30,  "Not whole descriptors stuff was made successfully:"},

	{ 0x31,  "Memory for mesh was allocated successfully:"},
	{-0x31,  "Memory for mesh wasn't allocated:"},

	{ 0x32,  "Mesh was uploaded successfully:"},
	{-0x32,  "Mesh wasn't uploaded"},

	{ 0x33,  "VkImageView for depth buffer was created successfully"},
	{-0x33,  "VkImageView for depth buffer wasn't created"},

	{ 0x34,  "BfCurveSet was allocated successfully:"},
	{-0x34,  "BfCurveSet wasn't allocated:"},
	
	{ 0x35,  "BfHolder was bound"},
	{ 0x36,  "BfCurveHolder was bound"},

	{ 0x37,  "BfCurveSet was deallocated:"},
	
	{ 0x38,  "BfCurveSet was added to BfCurveHolder:"},
	{ 0x39,  "VmaAllocator was bound to bound BfCurveHolder"},

	{ 0x40,  "BfGeometrySet was allocated successfully:" },
	{-0x40,  "BfGeometrySet wasn't allocated:" },
			 
	{ 0x41,  "BfGeometryHolder was bound"},
			 
	{ 0x42,  "BfGeometrySet was deallocated:"},
			 
	{ 0x43,  "BfGeometrySet was added to BfGeometryHolder:"},
			 
	{ 0x44,  "VmaAllocator was bound to bound BfGeometryHolder"},

	{ 0x45, "Vertices/Indices/BfGeometryData was added to set:"},
	{-0x45, "Vertices/Indices/BfGeometryData wasn't added to set:"},

	{ 0x46, "Graphics pipeline was bound to BfGeometrySet:"},

	{ 0x47, "Graphics pipeline was bound to BfLayer:"},
	
	{ 0x48, "VmaAllocator was bound to BfLayer:"},

	{ 0x49, "Buffers for BfLayer was allocated succefully:"},
	{-0x49, "Buffers for BfLater wasn't allocated:"}

};


enum class BfEnSingleEventType {
	BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT = 0x1,
	BF_SINGLE_EVENT_TYPE_HOLDER_EVENT = 0x2,
	BF_SINGLE_EVENT_TYPE_USER_EVENT = 0x4,
	BF_SINGLE_EVENT_TYPE_CHECK_EVENT = 0x8,
	BF_SINGLE_EVENT_TYPE_READ_DATA_EVENT = 0x10,
	BF_SINGLE_EVENT_TYPE_CURVE_HOLDER_EVENT = 0x20,
	BF_SINGLE_EVENT_TYPE_GEOMETRY_HOLDER_EVENT = 0x40,
	BF_SINGLE_EVENT_TYPE_GEOMETRY_SET_EVENT = 0x80,
	BF_SINGLE_EVENT_TYPE_LAYER_EVENT = 0x100,
};

enum class BfEnMultipleEventType {
	BF_MULTIPLE_EVENT_TYPE_RENDER_FRAME = 0,
};


struct BfSingleEvent {
	BfEnSingleEventType		type;
	BfEnActionType			action;
	std::string				info;
	std::time_t				time;
};

struct BfMultipleEvent {
	BfEnMultipleEventType type;
	uint32_t repet;

	bool operator<(const BfMultipleEvent& other) const;
};

struct BfEvent {
	BfSingleEvent single_event;
	BfMultipleEvent multiple_event;

	BfEvent();
	BfEvent(BfSingleEvent single_event_);
	BfEvent(BfMultipleEvent multiple_event_);

	void record_event();

private:
	enum class BfEnEventType {
		BF_EVENT_TYPE_INVALID = 0,
		BF_EVENT_TYPE_SINGLE = 1,
		BF_EVENT_TYPE_MULTIPLE = 2
	};
	BfEnEventType current_event_type;
};

struct BfEventHandler {
	static std::forward_list<BfSingleEvent> single_events;
	//std::set<BfMultipleEvent> multiple_events;

	static void add_single_event(BfSingleEvent event);
	static void add_multiple_event(BfMultipleEvent event);
	
	static const std::forward_list<BfSingleEvent>& get_single_events();
	static std::time_t get_time();

	static bool is_all_ok();

	typedef void (*FunctionPointer)(BfSingleEvent);

	static FunctionPointer funcPtr;
};



#endif,