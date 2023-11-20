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
#include <map>
#include <set>
// External
#include <glm/glm.hpp>

#include "bfBuffer.h"
#include "bfUniforms.h"

struct bfVertex {
	glm::vec3 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(bfVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(bfVertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(bfVertex, color);

		return attributeDescriptions;
	}

};

enum BfeMeshType {
	BF_MESH_TYPE_TRIANGLE = 0,
	BF_MESH_TYPE_RECTANGLE = 1,
	BF_MESH_TYPE_CIRCLE = 2,
	BF_MESH_TYPE_CURVE = 3,
	BF_MESH_TYPE_CUBE = 4
};

static inline const std::map<BfeMeshType, size_t> BfsMeshTypeMaxDotsCount{
	{BF_MESH_TYPE_TRIANGLE,	 3},
	{BF_MESH_TYPE_RECTANGLE, 4},
	{BF_MESH_TYPE_CIRCLE,	 80},
	{BF_MESH_TYPE_CURVE,	 10000},
	{BF_MESH_TYPE_CUBE,      36}
};


struct BfMesh : BfObjectData {
	uint32_t				id;
	BfeMeshType				type;
	bool					is_fully_allocated;

	std::vector<bfVertex>	vertices;
	std::vector<uint16_t>	indices;

	BfAllocatedBuffer		vertex_buffer;
	BfAllocatedBuffer		index_buffer;
};

inline size_t bfGetMeshMaxSize(BfeMeshType type, size_t single_point_size) {
	return BfsMeshTypeMaxDotsCount.at(type) * single_point_size;
}


class BfMeshHandler {
private:

	std::vector<BfMesh> __meshes;
	

	const uint32_t __max_mesh_count;

	

	uint32_t __assign_id() {
		static uint32_t next_id = 0;
		next_id++;
		return next_id;
	}
	

public:
	
	inline static BfMeshHandler* pBound_mesh_handler = nullptr;
	inline static void bind_mesh_handler(BfMeshHandler* handler) {
		BfMeshHandler::pBound_mesh_handler = handler;
	}
	inline static BfMeshHandler* get_bound_handler() {
		return BfMeshHandler::pBound_mesh_handler;
	}


	BfMeshHandler(uint32_t max_mesh_count) : 
		__max_mesh_count{max_mesh_count}
	{
		__meshes.resize(max_mesh_count);
	}

	

	BfMesh& get_rMesh(const uint32_t mesh_index) {
		return __meshes.at(mesh_index);
	}

	BfMesh* get_pMesh(const uint32_t mesh_index) {
		return &__meshes.at(mesh_index);
	}

	uint32_t get_max_meshes_count() {
		return __max_mesh_count;
	}

	uint32_t get_allocated_meshes_count() {
		uint32_t active_meshes_count = 0;
		for (size_t i = 0; i < __max_mesh_count; i++) {
			BfMesh* pMesh = get_pMesh(i);
			if (pMesh->is_fully_allocated) {
				active_meshes_count++;
			}
		}
		return active_meshes_count;
	}

	BfEvent allocate_mesh(VmaAllocator allocator, const uint32_t mesh_index, BfeMeshType mesh_type) {
		BfMesh* pMesh = this->get_pMesh(mesh_index);
		
		pMesh->id = __assign_id();

		// Create vertex buffer
		size_t vertex_size = bfGetMeshMaxSize(mesh_type, sizeof(bfVertex));

		bfCreateBuffer(&pMesh->vertex_buffer, allocator, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		// Create index buffer
		size_t index_size = bfGetMeshMaxSize(mesh_type, sizeof(uint32_t)); // ? 

		bfCreateBuffer(&pMesh->index_buffer, allocator, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		BfSingleEvent event{};
		{
			event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

			std::stringstream ss; ss << "Mesh(id = " << pMesh->id << "): Index buffer status:" << pMesh->index_buffer.is_allocated
				<< "; Vertex buffer status:" << pMesh->vertex_buffer.is_allocated;

			event.info = ss.str();

			if (pMesh->index_buffer.is_allocated && pMesh->vertex_buffer.is_allocated) {
				pMesh->is_fully_allocated = true;
				event.action = BfEnActionType::BF_ACTION_ALLOCATE_MESH_SUCCESS;
			}
			else {
				pMesh->is_fully_allocated = false;
				event.action = BfEnActionType::BF_ACTION_ALLOCATE_MESH_FAILURE;
			}
		}
		return BfEvent(event);
	}

	BfEvent upload_mesh(const uint32_t mesh_index, BfMesh& input_mesh) {
		BfMesh* pMesh = this->get_pMesh(mesh_index);

		pMesh->indices = input_mesh.indices;
		pMesh->vertices = input_mesh.vertices;

		BfSingleEvent event{};
		{
			event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

			std::stringstream ss; ss << "Mesh(id = " << pMesh->id << ") Vertex buffer is correct;";
			event.info = ss.str();
			if (pMesh->vertices.size() <= bfGetMeshMaxSize(pMesh->type, sizeof(bfVertex))) {
				event.action = BfEnActionType::BF_ACTION_UPLOAD_MESH_SUCCESS;
			}
			else {
				event.action = BfEnActionType::BF_ACTION_UPLOAD_MESH_FAILURE;
			}
		}
		return BfEvent(event);
	}

	BfEvent set_up_mesh(const uint32_t mesh_index, BfMesh& input_mesh);

	void load_mesh_to_buffers(VmaAllocator allocator, const uint32_t mesh_index) {
		BfMesh* pMesh = get_pMesh(mesh_index);
		
		void* vertex_data;
		vmaMapMemory(allocator, pMesh->vertex_buffer.allocation, &vertex_data);
		{
			memcpy(vertex_data, pMesh->vertices.data(), sizeof(bfVertex) * pMesh->vertices.size());
		}
		vmaUnmapMemory(allocator, pMesh->vertex_buffer.allocation);

		void* index_data;
		vmaMapMemory(allocator, pMesh->index_buffer.allocation, &index_data); 
		{
			memcpy(index_data, pMesh->indices.data(), sizeof(uint32_t) * pMesh->indices.size());
		}
		vmaUnmapMemory(allocator, pMesh->index_buffer.allocation);
	}

	void bind_mesh(VkCommandBuffer command_buffer, const uint32_t mesh_index) {
		BfMesh* pMesh = get_pMesh(mesh_index);
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(command_buffer, 0, 1, &pMesh->vertex_buffer.buffer, offsets);
		vkCmdBindIndexBuffer(command_buffer, pMesh->index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
	}

	void draw_indexed(VkCommandBuffer command_buffer, const uint32_t mesh_index) {
		BfMesh* pMesh = get_pMesh(mesh_index);

		vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(pMesh->indices.size()), 1, 0, 0, mesh_index);
	}


};




#endif // !BF_VERTEX_H
