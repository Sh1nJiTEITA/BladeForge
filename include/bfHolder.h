#ifndef BF_HOLDER_H
#define BF_HOLDER_H

#include <vector>
#include <memory>

#include "bfEvent.h"
#include "bfWindow.h"
#include "bfPhysicalDevice.h"
#include "bfBuffer.h"
#include "bfVertex2.hpp"
#include "bfCurves.hpp"
#include "bfExecutionTime.hpp"
#include "bfDescriptor.h"


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
	std::vector<BfAllocatedBuffer> pos_pick_buffers;

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

struct BfGeometryHolder {
	size_t allocated_geometries;
	VmaAllocator outside_allocator;

	std::vector<BfGeometrySet> geometry_sets;



	BfGeometryHolder();
	BfGeometryHolder(VmaAllocator _outside_allocator);
	

	BfGeometrySet* get_geometry_set(BfeGeometrySetType type);
	BfGeometrySet* get_geometry_set_by_index(size_t i);
	void update_obj_data(VmaAllocation allocation);
	void update_obj_data_desc(BfDescriptor& desc, uint32_t frame_index);
	void draw_indexed(VkCommandBuffer command_buffer);


	static BfGeometryHolder* __bfpGeometryHolder;
};


BfEvent bfBindGeometryHolderOutsideAllocator(VmaAllocator _outside_allocator);
BfEvent bfBindGeometryHolder(BfGeometryHolder* curve_holder);
BfEvent bfAllocateGeometrySet(BfeGeometrySetType type, size_t elements_count);
BfEvent bfBindGraphicsPipeline(BfeGeometrySetType type, VkPipeline* pipeline, BfePipelineType ptype);
BfGeometryHolder* bfGetpGeometryHolder();

void bfAddToHolder(const BfLine& o, const BfObjectData& obj_data);
void bfAddToHolder(const BfBezier& o, const BfObjectData& obj_data);


#endif 