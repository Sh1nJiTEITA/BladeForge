#ifndef BF_HOLDER_H
#define BF_HOLDER_H

#include <vector>
#include <memory>

#include "bfEvent.h"
#include "bfWindow.h"
#include "bfPhysicalDevice.h"
#include "bfBuffer.h"
#include "bfVertex2.hpp"

struct BfHolder {
	//std::vector<std::shared_ptr<BfWindow>> windows;
	//std::vector<std::shared_ptr<BfPhysicalDevice>> physical_devices;
	std::vector<BfWindow> windows;
	std::vector<BfPhysicalDevice> physical_devices;

	// Image-pack arrays
	std::vector<VkImage> images;
	std::vector<VkImageView> image_views;
	std::vector<VkFramebuffer> standart_framebuffers;
	std::vector<VkFramebuffer> gui_framebuffers;

	// Frame-pack arrays
		// Uniforms
	std::vector<BfAllocatedBuffer> uniform_view_buffers;
	std::vector<BfAllocatedBuffer> bezier_points_buffers;
	std::vector<BfAllocatedBuffer> model_buffers;

	std::vector<VkDescriptorSet> global_descriptor_sets;
	std::vector<VkDescriptorSet> main_descriptor_sets;
	
	std::vector<VkCommandBuffer> standart_command_buffers;
	std::vector<VkCommandBuffer> gui_command_buffers;
		// Sync
	std::vector<VkSemaphore> available_image_semaphores;
	std::vector<VkSemaphore> finish_render_image_semaphores;
	std::vector<VkFence> frame_in_flight_fences;
	
	

	static BfHolder* __bfpHolder;

	BfHolder();
};

BfEvent bfBindHolder(BfHolder* holder);
BfEvent bfHoldWindow(BfWindow*& window);
BfEvent bfHoldPhysicalDevice(BfPhysicalDevice*& window);
BfHolder* bfGetpHolder();

struct BfCurveHolder {
	size_t allocated_curves;
	VmaAllocator outside_allocator;

	std::vector<BfCurveSet> curve_sets;



	BfCurveHolder();
	BfCurveHolder(VmaAllocator _outside_allocator);
	

	BfCurveSet* get_curve_set(BfeCurveType type);
	BfCurveSet* get_curve_set_by_index(size_t i);
	void update_obj_data(VmaAllocation allocation);
	void draw_indexed(VkCommandBuffer command_buffer);

	static BfCurveHolder* __bfpCurveHolder;
};


BfEvent bfBindCurveHolderOutsideAllocator(VmaAllocator _outside_allocator);
BfEvent bfBindCurveHolder(BfCurveHolder* curve_holder);
BfEvent bfAllocateCurveSet(BfeCurveType type, size_t elements_count);
BfCurveHolder* bfGetpCurveHolder();


#endif 