#ifndef BF_HOLDER_H
#define BF_HOLDER_H

#include <vector>
#include <memory>

#include "bfEvent.h"
#include "bfWindow.h"
#include "bfPhysicalDevice.h"


struct BfHolder {
	//std::vector<std::shared_ptr<BfWindow>> windows;
	//std::vector<std::shared_ptr<BfPhysicalDevice>> physical_devices;
	std::vector<BfWindow> windows;
	std::vector<BfPhysicalDevice> physical_devices;

	std::vector<VkImage> images;
	std::vector<VkImageView> image_views;
	std::vector<VkFramebuffer> standart_framebuffers;
	std::vector<VkFramebuffer> gui_framebuffers;

	static BfHolder* __bfpHolder;

	BfHolder();
};

BfEvent BfBindHolder(BfHolder* holder);
BfEvent bfHoldWindow(BfWindow*& window);
BfEvent bfHoldPhysicalDevice(BfPhysicalDevice*& window);

//void bfAddToHolder(BfWindow window, int& index);
//void bfAddToHolder(BfPhysicalDevice physical_device, int& index);

BfHolder* bfGetpHolder();


#endif 