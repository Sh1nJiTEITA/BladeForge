#ifndef BF_VERTEX2_H
#define BF_VERTEX2_H

// Graphics Libs
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

// STL
#include <array>
#include <vector>
// External
#include <glm/glm.hpp>

#include "bfBuffer.h"

struct bfVertex {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

};

struct BfMesh {
	std::vector<bfVertex> vertices;
	std::vector<uint16_t> indices;

	BfAllocatedBuffer vertex_buffer;
	BfAllocatedBuffer index_buffer;
};




#endif // !BF_VERTEX_H
