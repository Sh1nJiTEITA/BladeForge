#ifndef BF_CONSOLE_H
#define BF_CONSOLE_H

#include "bfVariative.hpp"
#include "bfEvent.h"
#include <string>


struct BfConsole {
	static void print_all_single_events(int types);
	static void print_single_single_event(BfSingleEvent event);
	static std::string make_str(VkExtent2D extent2d);
	static std::string make_str(VkSurfaceTransformFlagBitsKHR in);
	static std::string make_str(VkCompositeAlphaFlagBitsKHR in);
	static std::string make_str(VkImageUsageFlagBits in);
	static std::string make_str(VkSurfaceCapabilitiesKHR in);
};



#endif