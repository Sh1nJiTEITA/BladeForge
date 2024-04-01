#ifndef BF_DRAWOBJECT_H
#define BF_DRAWOBJECT_H

#include <vector>
#include <unordered_set>
#include <memory>

#include "bfBuffer.h"
#include "bfVertex2.hpp"
#include "bfUniforms.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class BfObjID {
	uint32_t __value;
	uint32_t __type;
	static std::unordered_set<uint32_t> __existing_values;
	static std::map<uint32_t, uint32_t> __existing_pairs;

public:
	BfObjID();
	BfObjID(uint32_t type);
	~BfObjID();
	const uint32_t get() const;
	const uint32_t get_type() const;
	static bool is_id_exists(uint32_t id);
	static bool is_id_exists(BfObjID& id);
	static uint32_t find_type(uint32_t);
};
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class BfDrawObj;
class BfLayerHandler;

struct BfDrawLayerCreateInfo {
	VmaAllocator allocator;
	size_t vertex_size;
	size_t max_vertex_count;
	size_t max_reserved_count;
};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class BfDrawLayer {
	uint32_t __reserved_n;
	std::vector<std::shared_ptr<BfDrawObj>> __objects;
	std::vector<std::shared_ptr<BfDrawLayer>> __layers;

	std::vector<int32_t> __vertex_offsets; // offset of index of vertex in vertex buffer
	std::vector<int32_t> __index_offsets; // offset of index of index in index buffer

public:

	BfObjID id;

	BfLayerBuffer __buffer;
	BfDrawLayer(VmaAllocator allocator, 
				size_t vertex_size, 
				size_t max_vertex_count, 
				size_t max_reserved_count);
	
	BfDrawLayer(const BfDrawLayerCreateInfo& info);

	std::vector<std::shared_ptr<BfDrawObj>>::iterator begin();
	std::vector<std::shared_ptr<BfDrawObj>>::iterator end();

	const size_t get_whole_vertex_count() const noexcept;
	const size_t get_whole_index_count() const noexcept;
	const size_t get_obj_count() const noexcept;
	const std::vector<BfObjectData> get_obj_model_matrices() const noexcept;

	void add(std::shared_ptr<BfDrawObj> obj);
	void add(std::shared_ptr<BfDrawLayer> layer);
	void add_l(std::shared_ptr<BfDrawObj> obj);


	void del(uint32_t id);
	void del(const std::vector<uint32_t>& id);

	void generate_draw_data();

	void update_vertex_offset();
	void update_index_offset();
	void update_buffer();
	// TODO: global model-matrix descriptor
	void draw(VkCommandBuffer combuffer, size_t& offset);
	void map_model_matrices(size_t frame_index, size_t& offset, void* data);


	std::shared_ptr<BfDrawObj> get_object_by_index(size_t index);
	friend BfLayerHandler;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



class BfGuiIntegration {
protected:
	bool __is_selected = false;

public:
	bool* get_pSelection();
};

class BfDrawObj : public BfGuiIntegration {

protected:
	std::vector<BfVertex3> __vertices;
	std::vector<BfVertex3> __dvertices;
	std::vector<uint16_t> __indices;

	VkPipeline* __pPipeline = nullptr;
	glm::mat4 __model_matrix = glm::mat4(1.0f);
	glm::vec3 __main_color = glm::vec3(1.0f);
	float __line_thickness = 0.005;

public:

	BfDrawObj();
	BfDrawObj(uint32_t type);
	BfDrawObj(const std::vector<BfVertex3>& dvert, uint32_t type = 0);

	BfObjID id;

	const std::vector<BfVertex3>& get_rVertices() const ;
	const std::vector<BfVertex3>& get_rdVertices() const ;
	const std::vector<uint16_t>& get_rIndices() const ;

	BfVertex3* get_pVertices();
	BfVertex3* get_pdVertices();
	uint16_t* get_pIndices();

	BfObjectData get_obj_data();

	const size_t get_vertices_count() const ;
	const size_t get_dvertices_count() const ;
	const size_t get_indices_count() const ;

	size_t get_vertex_data_size();
	size_t get_index_data_size();

	VkPipeline* get_bound_pPipeline();

	glm::mat4& get_model_matrix();
	void bind_pipeline(VkPipeline* pPipeline);
	void set_color(glm::vec3 c);

	virtual bool is_ok();
	virtual void create_indices();
	virtual void create_vertices();
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~








#endif 