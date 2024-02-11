#ifndef BF_DRAWOBJECT_H
#define BF_DRAWOBJECT_H

#include <vector>
#include <unordered_set>
#include <memory>

#include "bfBuffer.h"
#include "bfVertex2.hpp"

class BfObjID {
	uint32_t __value;
	static std::unordered_set<uint32_t> __existing_values;

public:
	BfObjID();
	~BfObjID();
	const uint32_t get() const;
	static bool is_id_exists(uint32_t id);
	static bool is_id_exists(BfObjID id);
};

class BfDrawObj;

struct BfDrawVar {
	uint32_t index_count;// = __objects[element_index]->get_indices_count();
	uint32_t instance_count;// = 1;
	uint32_t first_index;// = __index_offsets[element_index];
	uint32_t vertex_offset;// = __vertex_offsets[element_index];
	uint32_t first_instance;// = 1;

	friend std::ostream& operator<<(std::ostream& s, BfDrawVar v) {
		s << "("
			<< "index_count = " << v.index_count
			<< " instance_count = " << v.instance_count
			<< " first_index = " << v.first_index
			<< " vertex_offset = " << v.vertex_offset
			<< " first_instance = " << v.first_instance << ")";
		return s;
	}

};

class BfDrawLayer {
	BfLayerBuffer __buffer;

	uint32_t __reserved_n;
	std::vector<std::shared_ptr<BfDrawObj>> __objects;
	
	std::vector<int32_t> __vertex_offsets; // offset of index of vertex in vertex buffer
	std::vector<int32_t> __index_offsets; // offset of index of index in index buffer

public:
	BfDrawLayer(VmaAllocator allocator, 
				size_t vertex_size, 
				size_t max_vertex_count, 
				size_t max_reserved_count);
	
	size_t get_whole_vertex_count();
	size_t get_whole_index_count();

	void add(std::shared_ptr<BfDrawObj> obj);
	void update_vertex_offset();
	void update_index_offset();
	
	// TODO: global model-matrix descriptor
	void draw(VkCommandBuffer combuffer, VkPipeline pipeline);

	void check_element_ready(size_t element_index);
};

class BfDrawObj {

protected:
	std::vector<BfVertex3> __vertices;
	std::vector<BfVertex3> __dvertices;
	std::vector<uint16_t> __indices;

	glm::mat4 __model_matrix;

public:

	BfObjID id;

	const std::vector<BfVertex3>& get_rVertices() const ;
	const std::vector<BfVertex3>& get_rdVertices() const ;
	const std::vector<uint16_t>& get_rIndices() const ;

	BfVertex3* get_pVertices();
	BfVertex3* get_pdVertices();
	uint16_t* get_pIndices();

	const size_t get_vertices_count() const ;
	const size_t get_dvertices_count() const ;
	const size_t get_indices_count() const ;

	size_t get_vertex_data_size();
	size_t get_index_data_size();

	virtual bool is_ok();
	void check_ok();
	virtual void create_indices();
};


#endif 