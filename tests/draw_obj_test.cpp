#include <catch2/catch_test_macros.hpp>

#include "bfDrawObject.h"
#include "bfBase.h"
#include "bfDescriptor.h"
#include "bfConsole.h"
#include <random>

unsigned long long int ran_int() {
	static auto gen = std::mt19937_64(time(nullptr));
	return gen();
}


std::vector<BfVertex3> gen_vertices() {

	size_t len = ran_int() % 100;


	auto vec = std::vector<BfVertex3>();
	vec.reserve(len);

	for (size_t i = 0; i < len; i++) {
		BfVertex3 vert{};
		auto r = ran_int() % 10;/// ran_int() % 10;
		auto g = ran_int() % 10;/// ran_int() % 10;
		auto b = ran_int() % 10;/// ran_int() % 10;
		auto x = ran_int() % 10;/// ran_int() % 10;
		auto y = ran_int() % 10;/// ran_int() % 10;
		auto z = ran_int() % 10;/// ran_int() % 10;
		auto xn = ran_int() % 10;// / ran_int() % 10;
		auto yn = ran_int() % 10;// / ran_int() % 10;
		auto zn = ran_int() % 10;// / ran_int() % 10;

		vert.color = glm::vec3{ r,g,b };
		vert.pos = glm::vec3{ x,y,z };
		vert.normals = glm::vec3{ xn,yn,zn };
		vec.emplace_back(vert);
	}

	return vec;
}

class class_1 : public BfDrawObj {
public:
	class_1() : BfDrawObj()
	{
		
		__vertices = gen_vertices();
		__dvertices.resize(__vertices.size(), {{0,0,0},{0,0,0} ,{0,0,0} });
		__indices.reserve(__vertices.size() * 2);
	}

	void create_indices() override {
		for (int i = 0; i < __vertices.size(); i++) {
			__indices.emplace_back(i);
		}
	}

};

class class_2 : public BfDrawObj {
public:
	class_2(const size_t n) : BfDrawObj() {
		__vertices.reserve(n);
		for (int i = 0; i < n; i++) {
			__vertices.emplace_back(BfVertex3{ {i,i,i}, {i,i,i}, {i,i,i} });
		}
	}

};



void def_vulkan(BfBase& base, BfHolder& holder) {
	
	bfBindHolder(&holder);
	bfHoldWindow(base.window);
	bfHoldPhysicalDevice(base.physical_device);

	bfSetWindowSize(base.window, 1080, 1080);
	bfSetWindowName(base.window, "BF_APP_NAME");
	bfCreateWindow(base.window);

	bfCreateInstance(base);
	bfCreateDebugMessenger(base);
	bfCreateSurface(base);
	bfCreatePhysicalDevice(base);
	bfCreateLogicalDevice(base);
	bfCreateAllocator(base);
}





TEST_CASE("bdDrawObject-x2") {
	BfEventHandler::funcPtr = &BfConsole::print_single_single_event;
	
	BfBase base{};
	BfHolder holder{};
	def_vulkan(base, holder);
	

	BfDrawLayer layer(base.allocator, sizeof(BfVertex3), 100, 100);
	
	BfVertex3 v1 = { {10.0f, 10.0f, 10.0f} ,{10.0f, 10.0f, 10.0f} ,{10.0f, 10.0f, 10.0f} };
	BfVertex3 v2 = { {12.0f, 12.0f, 12.0f} ,{12.0f, 12.0f, 12.0f} ,{12.0f, 12.0f, 12.0f} };

	auto s1 = std::make_shared<class_2>(class_2(10));
	layer.add(s1);
	auto s2 = std::make_shared<class_2>(class_2(10));
	layer.add(s2);
	/*auto s1 = std::make_shared<class_1>(class_1());
	s1->create_indices();
	layer.add(s1);
	auto s2 = std::make_shared<BfDrawObj>(class_1());
	layer.add(s2);*/

	layer.update_vertex_offset();
	layer.update_index_offset();
	//layer.check_element_ready(190);
	auto d = gen_vertices();
	
	layer.update_buffer();

	std::array<BfVertex3, 20> ddata;

	void* pdata = layer.__buffer.get_vertex_allocation_info().pMappedData;//__base.descriptor.get_buffer(BfDescriptorPosPickUsage, __base.current_frame)->allocation_info.pMappedData;
	memcpy(ddata.data(), pdata, sizeof(BfVertex3) * 20);
	//////////////////////////////////////////////////////////////////////////////////////

	
}
/*
	BfDescriptor desc{};
	desc.set_frames_in_flight(MAX_FRAMES_IN_FLIGHT);
	
	BfDescriptorCreateInfo info_mtx{};

	info_mtx.usage = BfDescriptorModelMtxUsage;
	info_mtx.vma_allocator = base.allocator;
	info_mtx.single_buffer_element_size = sizeof(BfObjectData);
	info_mtx.elements_count = 1;
	info_mtx.vk_buffer_usage_flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	info_mtx.vma_memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	info_mtx.vma_alloc_flags = 0;
	
	info_mtx.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	info_mtx.shader_stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	info_mtx.binding = 1;

	info_mtx.layout_binding = BfDescriptorSetGlobal;


	BfDescriptorCreateInfo info_view{};

	info_view.usage = BfDescriptorModelMtxUsage;//BfDescriptorViewDataUsage;
	info_view.vma_allocator = base.allocator;
	info_view.single_buffer_element_size = sizeof(BfUniformView);
	info_view.elements_count = 1;
	info_view.vk_buffer_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	info_view.vma_memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	info_view.vma_alloc_flags = 0;

	info_view.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	info_view.shader_stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	info_view.binding = 0;

	info_view.layout_binding = BfDescriptorSetGlobal;

	std::vector<VkDescriptorPoolSize> sizes = {
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10}
	};

	desc.add_descriptor_create_info(info_mtx);
	desc.add_descriptor_create_info(info_view);

	desc.allocate_desc_buffers();
	desc.create_desc_pool(base.device, sizes);
	desc.create_desc_set_layouts(base.device);
	desc.allocate_desc_sets(base.device);
	desc.update_desc_sets(base.device);


*/