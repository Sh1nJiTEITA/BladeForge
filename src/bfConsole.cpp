#include "bfConsole.h"

std::string BfConsole::make_str()
{
	
}

std::string BfConsole::make_str(VkExtent2D extent2d)
{
	std::stringstream ss;
	ss << "VkExtent2D(w = " << extent2d.width << ", h = " << extent2d.height << ") ";
	return ss.str();
}
