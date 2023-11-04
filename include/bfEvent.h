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
	BF_ACTION_TYPE_INIT_INSTANCE_SUCCESS		=  0x2,
	BF_ACTION_TYPE_INIT_INSTANCE_FAILURE		= -0x2,

	BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_SUCCESS =  0x3,
	BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_FAILURE = -0x3,
	BF_ACTION_TYPE_INIT_DEBUG_MESSENGER_NO_INIT =  0x3A,
	
	BF_ACTION_TYPE_INIT_SURFACE_SUCCESS			=  0x4,
	BF_ACTION_TYPE_INIT_SURFACE_FAILURE			= -0x4,

	BF_ACTION_TYPE_INIT_GLFW_WINDOW_SUCCESS		=  0x5,
	BF_ACTION_TYPE_INIT_GLFW_WINDOW_FAILURE		= -0x5,

	BF_ACTION_TYPE_SET_WINDOW_SIZE_SUCCESS		=  0x6,
	BF_ACTION_TYPE_SET_WINDOW_SIZE_FAILURE		= -0x6
};

const std::map<int, std::string> bfSetActionsStr{
	// Initializations
	{ 0x2, "Vulkan Instance was created successfully"},
	{-0x2, "Vulkan Instance wasn't created"},

	{ 0x3, "Vulkan Debug messenger was created successfully"},
	{-0x3, "Vulkan Debug messenger wasn't created"},
	{ 0x3A,"Vulkan Debug messenger wasn't created due to disabled validation layers"},

	{ 0x4, "Vulkan Surface was created successfully"},
	{-0x4, "Vulkan Surface wasn't created"},

	{ 0x5, "GLFW window was created successfully"},
	{-0x5, "GLFW window wasn't created"},

	{ 0x6, "Window size was successfully resized"},
	{-0x6, "Window size wasn't resized: w <= 0 or h <= 0"}
};


enum class BfEnSingleEventType {
	BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT = 0,
	BF_SINGLE_EVENT_TYPE_USER_EVENT = 1,
	BF_SINGLE_EVENT_TYPE_CHECK_EVENT = 2
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
};



#endif