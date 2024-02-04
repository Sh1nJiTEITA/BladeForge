#include <catch2/catch_test_macros.hpp>

#include "bfDrawObject.h"
#include "bfBase.h"


class class_1 : public BfDrawObj {
	unsigned int __n;
public:
	class_1() :BfDrawObj()
	{
		class_1(0, {{0,0,0},{0,0,0},{0,0,0}});
	}
	class_1(unsigned int n, BfVertex3 v_value) 
		: BfDrawObj(), __n { n }
	{
		__vertices.resize(__n, v_value);
		__dvertices.resize(__n, v_value);
		__indices.reserve(__n * 2);
	}

	void create_indices() override {
		for (int i = 0; i < __n; i++) {
			__indices.emplace_back(i);
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
	
	BfBase base{};
	BfHolder holder{};
	def_vulkan(base, holder);
	
	BfDrawLayer layer(base.allocator, sizeof(BfVertex3), 100, 100);
	
	BfVertex3 v1 = { {10.0f, 10.0f, 10.0f} ,{10.0f, 10.0f, 10.0f} ,{10.0f, 10.0f, 10.0f} };
	BfVertex3 v2 = { {12.0f, 12.0f, 12.0f} ,{12.0f, 12.0f, 12.0f} ,{12.0f, 12.0f, 12.0f} };

	auto s1 = std::make_shared<BfDrawObj>(BfDrawObj());
	layer.add(s1);
	auto s2 = std::make_shared<BfDrawObj>(BfDrawObj());
	layer.add(s2);

	layer.update_vertex_offset();
	layer.update_index_offset();

	//BfDrawLayer
}