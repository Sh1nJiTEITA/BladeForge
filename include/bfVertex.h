#ifndef BF_VERTEX_H
#define BF_VERTEX_H

// Graphics Libs
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

// STL
#include <array>

// External
#include <glm/glm.hpp>

struct bfVertex {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

};





#endif // !BF_VERTEX_H
