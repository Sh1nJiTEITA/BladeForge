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
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>
#include "bfMath.hpp"
#include "bfBuffer.h"
#include "bfUniforms.h"
// #include "bfExecutionTime.hpp"
//#include "bfMatrix2.h"


struct bfVertex {
	glm::vec3 pos;
	glm::vec3 color;

	static inline VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(bfVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	static inline std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
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

struct BfVertex3 {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normals;

	BfVertex3()
		: pos{ 0.0f,0.0f,0.0f }
		, color{ 1.0f,1.0f,1.0f }
		, normals{0.0f, 0.0f, 0.0f} {}

	BfVertex3(glm::vec3 ipos)
			  //glm::vec3 icol = {1.0f, 1.0f, 1.0f}, 
			  //glm::vec3 inor = {0.0f, 0.0f, 0.0f})
		: pos{ipos}
		/*, color{icol}
		, normals{inor}*/
		, color{ 1.0f,1.0f,1.0f }
		, normals{ 0.0f,0.0f,0.0f }

	{}

	BfVertex3(glm::vec3 ipos,
		glm::vec3 icol, 
		glm::vec3 inor)
		: pos{ ipos }
		/*, color{icol}
		, normals{inor}*/
		, color{ icol }
		, normals{ inor }

	{}

//#ifdef BF_MATRIX2_H
//	BfVertex3(BfVec2 _pos) 
//		: pos(_pos.x, _pos.y, 0.0f)
//		, color{ 1.0f,1.0f,1.0f }
//		, normals{ 0.0f,0.0f,0.0f } 
//	{}
//#endif // BF_MATRIX2_H
	inline bool equal(const BfVertex3& o) const {
		return CHECK_FLOAT_EQUALITY(o.pos.x, pos.x) &&
			   CHECK_FLOAT_EQUALITY(o.pos.y, pos.y) &&
			   CHECK_FLOAT_EQUALITY(o.pos.z, pos.z);
	}

	static inline VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(BfVertex3);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	static inline std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(BfVertex3, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(BfVertex3, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(BfVertex3, normals);

		return attributeDescriptions;
	}

	friend std::ostream& operator<<(std::ostream& os, const BfVertex3& vert) {
		os << "BfVertex3( p(" << vert.pos.x << ", " << vert.pos.y << ", " << vert.pos.z << "), c("
			<< vert.color.r << ", " << vert.color.g << ", " << vert.color.b << "), n("
			<< vert.normals.x << ", " << vert.normals.y << ", " << vert.normals.z << ") )";
			
		return os;
	}
};

struct BfVertex4 {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 normal;
	glm::uint32_t obj_id;

	static inline VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(BfVertex4);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	static inline std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(BfVertex4, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(BfVertex4, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(BfVertex4, normal);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 2;
		attributeDescriptions[3].format = VK_FORMAT_R32_UINT;//VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(BfVertex4, obj_id);

		return attributeDescriptions;
	}

	friend std::ostream& operator<<(std::ostream& os, const BfVertex4& vert) {
		os << "BfVertex3( p(" << vert.pos.x << ", " << vert.pos.y << ", " << vert.pos.z << "), c("
			<< vert.color.r << ", " << vert.color.g << ", " << vert.color.b << "), n("
			<< vert.normal.x << ", " << vert.normal.y << ", " << vert.normal.z << ") obj_id("
			<< vert.obj_id << ')';

		return os;
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
	{BF_MESH_TYPE_RECTANGLE, 6},
	{BF_MESH_TYPE_CIRCLE,	 80},
	{BF_MESH_TYPE_CURVE,	 10000},
	{BF_MESH_TYPE_CUBE,      48}
};








struct BfMesh : BfObjectData {
	BfeMeshType				type;
	bool					is_fully_allocated;

	std::vector<bfVertex>	vertices;
	std::vector<uint32_t>	indices;

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
		pMesh->type = mesh_type;

		pMesh->id = pMesh->__assign_id();

		// Create vertex buffer
		size_t vertex_size = bfGetMeshMaxSize(mesh_type, sizeof(bfVertex));

		bfCreateBuffer(&pMesh->vertex_buffer, allocator, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		// Create index buffer
		size_t index_size = bfGetMeshMaxSize(mesh_type, sizeof(uint32_t)); // ? 

		bfCreateBuffer(&pMesh->index_buffer, allocator, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		BfSingleEvent event{};
		{
			event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

			std::stringstream ss; ss << "Mesh(id = " << pMesh->id 
				<< "): Index buffer status:" << pMesh->index_buffer.is_allocated
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

			std::stringstream ss; ss << "Mesh(id = " << pMesh->id 
				<< ") Vertex buffer is correct;";
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
		//VkBuffer vertexBuffers[] = { pMesh->vertex_buffer.buffer };

		vkCmdBindVertexBuffers(command_buffer, 0, 1, &pMesh->vertex_buffer.buffer, offsets);
		vkCmdBindIndexBuffer(command_buffer, pMesh->index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
	}

	void draw_indexed(VkCommandBuffer command_buffer, const uint32_t mesh_index) {
		BfMesh* pMesh = get_pMesh(mesh_index);

		//vkCmdDraw(command_buffer, 3, 0, 0, 0);
		vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(pMesh->indices.size()), 1, 0, 0, mesh_index);
	}


};
//

enum BfeGeometrySetType {
	BF_GEOMETRY_TYPE_UNDERFINED = -1,

	BF_GEOMETRY_TYPE_CURVE_BEZIER = 0,
	BF_GEOMETRY_TYPE_HANDLE_CURVE_BEZIER = 0x0A,
	BF_GEOMETRY_TYPE_CARCASS_CURVE_BEZIER = 0x0B,
	
	BF_GEOMETRY_TYPE_CURVE_LINEAR = 1

};


static inline const std::map<BfeGeometrySetType, size_t> BfmGeometrySetTypeMaxElementsCount{
	{BF_GEOMETRY_TYPE_CURVE_BEZIER, 1000},
	{BF_GEOMETRY_TYPE_HANDLE_CURVE_BEZIER, 1000},
	{BF_GEOMETRY_TYPE_CARCASS_CURVE_BEZIER, 1000},

	{BF_GEOMETRY_TYPE_CURVE_LINEAR, 10000}
};

static inline const std::map<BfeGeometrySetType, size_t> BfmGeometrySetTypeMaxNumberOfVertices{
	{BF_GEOMETRY_TYPE_CURVE_BEZIER, 100},
	{BF_GEOMETRY_TYPE_HANDLE_CURVE_BEZIER, 300},
	{BF_GEOMETRY_TYPE_CARCASS_CURVE_BEZIER, 400},

	{BF_GEOMETRY_TYPE_CURVE_LINEAR, 2}
};

enum BfePipelineType {
	BF_GRAPHICS_PIPELINE_LINES = 0,
	BF_GRAPHICS_PIPELINE_TRIANGLE = 1
};

struct BfGeometryData {
	VkDeviceSize index_offset;
	BfObjectData obj_data;

	size_t vertices_count;
	size_t indices_count;
};

struct BfGeometrySet {
// Variables
	VmaAllocator outside_allocator;

	bool	 is_fully_allocated;
	uint32_t allocated_elements_count;
	uint32_t ready_elements_count;

	BfeGeometrySetType type;

	std::vector<BfGeometryData>  datas;
	std::vector<BfVertex3>	  vertices;
	std::vector<uint32_t>	  indices;

	BfAllocatedBuffer vertices_buffer;
	BfAllocatedBuffer indices_buffer;

	VkPipeline* pPipeline;
	BfePipelineType pipeline_type;

// Methods
	BfGeometrySet() : BfGeometrySet(nullptr) {};
	BfGeometrySet(VmaAllocator _outside_allocator) {
		this->is_fully_allocated = false;
		this->outside_allocator = _outside_allocator;
		this->allocated_elements_count = 0;
		this->ready_elements_count = 0;
		this->type = BF_GEOMETRY_TYPE_UNDERFINED;
		this->pPipeline = nullptr;
	}

	void set_up(BfeGeometrySetType _type, size_t elements_count, VmaAllocator allocator) {
		outside_allocator = allocator;
		type = _type;
		is_fully_allocated = false;

		datas.resize(elements_count);

		this->allocate(type, elements_count);

		ready_elements_count = 0;

		vertices.resize(BfmGeometrySetTypeMaxNumberOfVertices.at(type) *
			//BfmCurveTypeMaxElementsCount.at(type)
			elements_count
		);

		indices.resize(BfmGeometrySetTypeMaxNumberOfVertices.at(type) *  // * max number of vertices in 1 curve
			//BfmCurveTypeMaxElementsCount.at(type)
			elements_count
		);
	}

	void write_to_buffers() {

		void* vertex_data;
		vmaMapMemory(outside_allocator, vertices_buffer.allocation, &vertex_data);
		{
			memcpy(vertex_data, vertices.data(), sizeof(BfVertex3) * vertices.size());
		}
		vmaUnmapMemory(outside_allocator, vertices_buffer.allocation);

		void* index_data;
		vmaMapMemory(outside_allocator, indices_buffer.allocation, &index_data);
		{
			memcpy(index_data, indices.data(), sizeof(uint32_t) * indices.size());
		}
		vmaUnmapMemory(outside_allocator, indices_buffer.allocation);
	}

	void draw_indexed(VkCommandBuffer command_buffer, uint32_t obj_data_index_offset = 0) {
		VkDeviceSize offsets[] = { 0 };

		//BfExecutionTime::BeginTimeCut("VkBind");

		vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertices_buffer.buffer, offsets);
		vkCmdBindIndexBuffer(command_buffer, indices_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
		
		//BfExecutionTime::EndTimeCut("VkBind");

		//BfExecutionTime::BeginTimeCut("FullDraw");
		for (size_t i = 0; i < this->ready_elements_count; i++) {
			
			//BfExecutionTime::BeginTimeCut("GeometryPtr");

			BfGeometryData* pData = &datas[i];
			
			//BfExecutionTime::EndTimeCut("GeometryPtr");


			//BfExecutionTime::BeginTimeCut("VkDraw");
			vkCmdDrawIndexed(command_buffer, (uint32_t)pData->indices_count, 1, (uint32_t)pData->index_offset, 0, i + obj_data_index_offset);
			//BfExecutionTime::EndTimeCut("VkDraw");

		}
		//BfExecutionTime::EndTimeCut("FullDraw");
	
	}

	void update_object_data(VmaAllocation allocation) {
		void* pobjects_data;

		std::vector<BfObjectData> objects_data(this->ready_elements_count);

		for (size_t i = 0; i < objects_data.size(); i++) {
			objects_data[i] = this->datas[i].obj_data;
		}

		vmaMapMemory(outside_allocator, allocation, &pobjects_data);
		{
			memcpy(pobjects_data, objects_data.data(), sizeof(BfObjectData) * objects_data.size());
		}
		vmaUnmapMemory(outside_allocator, allocation);
	}

	BfEvent add_data(const std::vector<BfVertex3>& _vertices, const std::vector<uint32_t>& _indices, const BfObjectData& obj_data) {
		return add_data(_vertices.data(), _vertices.size(), _indices.data(), _indices.size(), &obj_data);

		/*BfSingleEvent event{};
		if (++ready_elements_count > allocated_elements_count) {
			event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_GEOMETRY_SET_EVENT;
			event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_GEOMETRY_SET_FAILURE;
			event.info = ("All objects in geometry set with type = " + std::to_string(type) + "are ready, memory for additional object wasn't allocated");
			return event;
		}

		BfGeometryData* pCurve_data = &datas[ready_elements_count - 1];

		pCurve_data->vertices_count = _vertices.size();
		pCurve_data->indices_count = _indices.size();
		pCurve_data->obj_data = obj_data;

		size_t index_offset;
		size_t vertex_offset;

		if (ready_elements_count == 1) {
			pCurve_data->index_offset = 0;

			index_offset = 0;
			vertex_offset = 0;
		}
		else {
			BfGeometryData* pCurve_data_previous = &datas[ready_elements_count - 2];
			pCurve_data->index_offset = pCurve_data->indices_count * sizeof(BfVertex3) + pCurve_data_previous->index_offset;

			index_offset = 0;
			vertex_offset = 0;

			for (int i = 0; i < ready_elements_count - 1; i++) {
				vertex_offset += datas[i].vertices_count;
				index_offset += datas[i].indices_count;
			}
		}
		pCurve_data->index_offset = index_offset;


		if (_vertices.size() == _indices.size()) {
			for (size_t i = 0; i < _vertices.size(); i++) {
				vertices[i + vertex_offset] = _vertices[i];
				indices[i + index_offset] = _indices[i] + index_offset;
			}
		}
		else {


			for (size_t i = 0; i < _vertices.size(); i++) {
				vertices[i + vertex_offset] = _vertices[i];
			}

			for (size_t i = 0; i < _indices.size(); i++) {
				indices[i + index_offset] = _indices[i] + index_offset;
			}
		}

		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_GEOMETRY_SET_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_GEOMETRY_SET_SUCCESS;
		event.info = "New geometry vertices/indices/BfObjectData was added to BfGeometrySet = " + std::to_string(type);
		return event;*/
	}

	BfEvent add_data(const BfVertex3* pVertices, size_t countVertices, const uint32_t* pIndices, size_t countIndices, const BfObjectData* obj_data) {
		BfSingleEvent event{};
		if (++ready_elements_count > allocated_elements_count) {
			event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_GEOMETRY_SET_EVENT;
			event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_GEOMETRY_SET_FAILURE;
			event.info = ("All objects in geometry set with type = " + std::to_string(type) + "are ready, memory for additional object wasn't allocated");
			return event;
		}

		BfGeometryData* pCurve_data = &datas[ready_elements_count - 1];

		pCurve_data->vertices_count = countVertices;
		pCurve_data->indices_count = countIndices;
		pCurve_data->obj_data = *obj_data;

		size_t index_offset;
		size_t vertex_offset;

		if (ready_elements_count == 1) {
			pCurve_data->index_offset = 0;

			index_offset = 0;
			vertex_offset = 0;
		}
		else {
			BfGeometryData* pCurve_data_previous = &datas[ready_elements_count - 2];
			pCurve_data->index_offset = pCurve_data->indices_count * sizeof(BfVertex3) + pCurve_data_previous->index_offset;

			index_offset = 0;
			vertex_offset = 0;
			for (int i = 0; i < ready_elements_count - 1; i++) {
				vertex_offset += datas[i].vertices_count;
				index_offset += datas[i].indices_count;
			}
			
		}
		pCurve_data->index_offset = index_offset;


		if (countVertices == countIndices) {
			for (size_t i = 0; i < countVertices; i++) {
				vertices[i + vertex_offset] = pVertices[i];
				indices[i + index_offset] = pIndices[i] + index_offset;
			}
		}
		else {


			for (size_t i = 0; i < countVertices; i++) {
				vertices[i + vertex_offset] = pVertices[i];
			}

			if (this->pipeline_type == BF_GRAPHICS_PIPELINE_LINES) {
				for (size_t i = 0; i < countIndices; i++) {
					indices[i + index_offset] = pIndices[i] + index_offset;
				}
			}
			else if (this->pipeline_type == BF_GRAPHICS_PIPELINE_TRIANGLE) {
				for (size_t i = 0; i < countIndices; i++) {
					indices[i + index_offset] = pIndices[i] + (index_offset / 6 * 4);
				}
			}

			
		}

		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_GEOMETRY_SET_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_GEOMETRY_SET_SUCCESS;
		event.info = "New geometry vertices/indices/BfObjectData was added to BfGeometrySet = " + std::to_string(type);
		return event;
	}

	BfEvent allocate(BfeGeometrySetType type, size_t elements_count) {

		// Size for vertex buffer
		size_t max_vertices_size =						// Sizes:
			sizeof(BfVertex3) *							// Of 1 vertex 
			BfmGeometrySetTypeMaxNumberOfVertices.at(type) *  // * max number of vertices in 1 curve
			elements_count;
		//BfmCurveTypeMaxElementsCount.at(type);		// * max number of curves

	// Create vertex buffer for loading to GPU
		bfCreateBuffer(&vertices_buffer,
			outside_allocator,
			max_vertices_size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VMA_MEMORY_USAGE_CPU_TO_GPU);

		// Size for vertex buffer
		size_t max_indices_size =						// Sizes:
			sizeof(uint32_t) *							// Of 1 index
			BfmGeometrySetTypeMaxNumberOfVertices.at(type) *  // * max number of vertices in 1 curve
			elements_count;
		//BfmCurveTypeMaxElementsCount.at(type);		// * max number of curves

	// Create index buffer for loading to GPU
		bfCreateBuffer(&indices_buffer,
			outside_allocator,
			max_indices_size,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VMA_MEMORY_USAGE_CPU_TO_GPU);

		BfSingleEvent event{};
		{
			event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;

			std::stringstream ss; ss << "BfGeometrySet for type = " << this->type
				<< ", Index buffer status: " << this->vertices_buffer.is_allocated
				<< "; Vertex buffer status: " << this->indices_buffer.is_allocated;

			event.info = ss.str();

			if (this->indices_buffer.is_allocated && this->vertices_buffer.is_allocated) {
				is_fully_allocated = true;
				event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_GEOMETRY_SET_SUCCESS;
				allocated_elements_count = elements_count;
			}
			else {
				is_fully_allocated = false;
				event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_GEOMETRY_SET_FAILURE;
			}
		}
		return BfEvent(event);
	}

	BfEvent deallocate() {
		vmaDestroyBuffer(outside_allocator, vertices_buffer.buffer, vertices_buffer.allocation);
		vmaDestroyBuffer(outside_allocator, indices_buffer.buffer, indices_buffer.allocation);

		BfSingleEvent event{};
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_GEOMETRY_HOLDER_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_DEALLOC_GEOMETRY_SET;
		event.info = " BfGeometrySet for type = " + std::to_string(this->type);
		return BfEvent(event);
	}
};


////
//enum BfeCurveType {
//	BF_CURVE_TYPE_UNDERFINED = -1,
//	BF_CURVE_TYPE_BEZIER = 0,
//	BF_CURVE_TYPE_LINEAR = 1
//};
//
//
//static inline const std::map<BfeCurveType, size_t> BfmCurveTypeMaxElementsCount {
//	{BF_CURVE_TYPE_BEZIER, 1000},
//	{BF_CURVE_TYPE_LINEAR, 10000}
//};
//
//static inline const std::map<BfeCurveType, size_t> BfmCurveTypeMaxNumberOfVertices{
//	{BF_CURVE_TYPE_BEZIER, 50},
//	{BF_CURVE_TYPE_LINEAR, 2}
//};
//
//
//
//struct BfCurveData {
//	VkDeviceSize index_offset;
//	BfObjectData obj_data;
//
//	size_t vertices_count;
//	size_t indices_count;
//};
//
//
//
//struct BfCurveSet {
//	
//	VmaAllocator outside_allocator;
//	BfeCurveType type;
//	bool is_fully_allocated;
//	uint32_t allocated_elements_count;
//	uint32_t ready_elements_count;
//	
//	std::vector<BfCurveData> curve_datas;
//
//	std::vector<BfVertex3>	  vertices;
//	std::vector<uint32_t>	  indices;
//
//	BfAllocatedBuffer vertices_buffer;
//	BfAllocatedBuffer indices_buffer;
//
//
//	inline static BfCurveSet* pBound_curve_set = nullptr;
//	inline static void bind_curve_set(BfCurveSet* curve_set) {
//		BfCurveSet::pBound_curve_set = curve_set;
//	}
//	inline static BfCurveSet* get_pBound_curve_set() {
//		return BfCurveSet::pBound_curve_set;
//	}
//
//	BfCurveSet() : BfCurveSet(nullptr){};
//	BfCurveSet(VmaAllocator _outside_allocator) {
//		this->is_fully_allocated = false;
//		this->outside_allocator = _outside_allocator;
//		this->allocated_elements_count = 0;
//		this->ready_elements_count = 0;
//		this->type = BF_CURVE_TYPE_UNDERFINED;
//	}
//		
//
//	void set_up(BfeCurveType _type, size_t elements_count, VmaAllocator allocator) {
//		outside_allocator = allocator;
//		type = _type;
//		is_fully_allocated = false;
//
//		curve_datas.resize(elements_count);
//
//		this->allocate(type, elements_count);
//
//		ready_elements_count = 0;
//
//		vertices.resize(BfmCurveTypeMaxNumberOfVertices.at(type) *
//			//BfmCurveTypeMaxElementsCount.at(type)
//			elements_count
//		);
//
//		indices.resize(BfmCurveTypeMaxNumberOfVertices.at(type) *  // * max number of vertices in 1 curve
//			//BfmCurveTypeMaxElementsCount.at(type)
//			elements_count
//		);
//	}
//	
//
//	void add_curve(std::vector<BfVertex3>& _vertices, std::vector<uint32_t>& _indices, BfObjectData obj_data) {
//		if (++ready_elements_count > allocated_elements_count) {
//			throw std::runtime_error("All curves in curve set with type = " + std::to_string(type) + "are ready, memory for additional curve wasn't allocated");
//		}
//
//		BfCurveData* pCurve_data = &curve_datas[ready_elements_count - 1];
//
//		pCurve_data->vertices_count = _vertices.size();
//		pCurve_data->indices_count = _indices.size();
//		pCurve_data->obj_data = obj_data;
//
//		size_t index_offset;
//		size_t vertex_offset;
//
//		if (ready_elements_count == 1) {
//			pCurve_data->index_offset = 0;
//
//			index_offset = 0;
//			vertex_offset = 0;
//		}
//		else {
//			BfCurveData* pCurve_data_previous = &curve_datas[ready_elements_count - 2];
//			pCurve_data->index_offset = pCurve_data->indices_count * sizeof(BfVertex3) + pCurve_data_previous->index_offset;
//
//			index_offset = 0;
//			vertex_offset = 0;
//
//			for (int i = 0; i < ready_elements_count-1; i++) {
//				vertex_offset += curve_datas[i].vertices_count;
//				index_offset += curve_datas[i].indices_count;
//			}
//		}
//		pCurve_data->index_offset = index_offset;
//
//
//		if (_vertices.size() == _indices.size()) {
//			for (size_t i = 0; i < _vertices.size(); i++) {
//				vertices[i + vertex_offset] = _vertices[i];
//				indices[i + index_offset] = _indices[i] + index_offset;
//			}
//		}
//		else {
//		
//
//			for (size_t i = 0; i < _vertices.size(); i++) {
//				vertices[i + vertex_offset] = _vertices[i];
//			}
//
//			for (size_t i = 0; i < _indices.size(); i++) {
//				indices[i + index_offset] = _indices[i] + index_offset;
//			}
//		}
//	}
//	void write_to_buffers() {
//	
//		void* vertex_data;
//		vmaMapMemory(outside_allocator, vertices_buffer.allocation, &vertex_data);
//		{
//			memcpy(vertex_data, vertices.data(), sizeof(BfVertex3) * vertices.size());
//		}
//		vmaUnmapMemory(outside_allocator, vertices_buffer.allocation);
//
//		void* index_data;
//		vmaMapMemory(outside_allocator, indices_buffer.allocation, &index_data);
//		{
//			memcpy(index_data, indices.data(), sizeof(uint32_t) * indices.size());
//		}
//		vmaUnmapMemory(outside_allocator, indices_buffer.allocation);
//	}
//
//	void draw_indexed(VkCommandBuffer command_buffer, uint32_t obj_data_index_offset = 0) {
//		VkDeviceSize offsets[] = { 0 };
//		vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertices_buffer.buffer, offsets);
//		vkCmdBindIndexBuffer(command_buffer, indices_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
//
//		for (size_t i = 0; i < this->ready_elements_count; i++) {
//			BfCurveData* pData = &curve_datas[i];
//			vkCmdDrawIndexed(command_buffer, (uint32_t)pData->indices_count, 1, (uint32_t)pData->index_offset, 0, i + obj_data_index_offset);
//		}
//
//	}
//
//	void update_object_data(VmaAllocation allocation) {
//		void* pobjects_data;
//
//		std::vector<BfObjectData> objects_data(this->ready_elements_count);
//
//		for (size_t i = 0; i < objects_data.size(); i++) {
//			objects_data[i] = this->curve_datas[i].obj_data;
//		}
//
//		vmaMapMemory(outside_allocator, allocation, &pobjects_data);
//		{
//			memcpy(pobjects_data, objects_data.data(), sizeof(BfObjectData) * objects_data.size());
//		}
//		vmaUnmapMemory(outside_allocator, allocation);
//	}
//
//	BfEvent allocate(BfeCurveType type, size_t elements_count) {
//		
//		// Size for vertex buffer
//		size_t max_vertices_size =						// Sizes:
//			sizeof(BfVertex3) *							// Of 1 vertex 
//			BfmCurveTypeMaxNumberOfVertices.at(type) *  // * max number of vertices in 1 curve
//			elements_count;
//			//BfmCurveTypeMaxElementsCount.at(type);		// * max number of curves
//
//		// Create vertex buffer for loading to GPU
//		bfCreateBuffer(&vertices_buffer,
//						outside_allocator,
//						max_vertices_size,
//						VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//						VMA_MEMORY_USAGE_CPU_TO_GPU);
//
//		// Size for vertex buffer
//		size_t max_indices_size =						// Sizes:
//			sizeof(uint32_t) *							// Of 1 index
//			BfmCurveTypeMaxNumberOfVertices.at(type) *  // * max number of vertices in 1 curve
//			elements_count;
//			//BfmCurveTypeMaxElementsCount.at(type);		// * max number of curves
//
//		// Create index buffer for loading to GPU
//		bfCreateBuffer(&indices_buffer,
//						outside_allocator,
//						max_indices_size,
//						VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
//						VMA_MEMORY_USAGE_CPU_TO_GPU);
//
//		BfSingleEvent event{};
//		{
//			event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_INITIALIZATION_EVENT;
//
//			std::stringstream ss; ss << "BfCurveSet for type = " << this->type
//				<< ",Index buffer status:" << this->vertices_buffer.is_allocated
//				<< "; Vertex buffer status:" << this->indices_buffer.is_allocated;
//
//			event.info = ss.str();
//
//			if (this->indices_buffer.is_allocated && this->vertices_buffer.is_allocated) {
//				is_fully_allocated = true;
//				event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_CURVE_SET_SUCCESS;
//				allocated_elements_count = elements_count;
//			}
//			else {
//				is_fully_allocated = false;
//				event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_CURVE_SET_FAILURE;
//			}
//		}
//		return BfEvent(event);
//	}
//
//	BfEvent deallocate() {
//		vmaDestroyBuffer(outside_allocator, vertices_buffer.buffer, vertices_buffer.allocation);
//		vmaDestroyBuffer(outside_allocator, indices_buffer.buffer, indices_buffer.allocation);
//
//		BfSingleEvent event{};
//		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_CURVE_HOLDER_EVENT;
//		event.action = BfEnActionType::BF_ACTION_TYPE_DEALLOC_CURVE_SET;
//		event.info = " BfCurveSet for type = " + std::to_string(this->type);
//		return BfEvent(event);
//	}
//};
//
//





#endif // !BF_VERTEX_H
