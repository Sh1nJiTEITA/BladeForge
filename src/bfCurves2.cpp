#include "bfCurves2.h"

BfID::BfID()  
{
	static uint16_t id = 0;
	this->value = id;
	id++;
}

const uint16_t BfID::get() const {
	return this->value;
}

//bool BfDrawObject::__check_is_3D(pVecVert3 vert) const
//{	
//	for (const auto& it : *vert) {
//		if (it.pos.z != 0.0f) {
//			return true;
//		}
//	}
//	return false;
//}
//
//bool BfDrawObject::__check_is_3D(std::initializer_list<BfVertex3> list) const {
//	for (const auto& it : list) {
//		if (it.pos.z != 0.0f) {
//			return true;
//		}
//	}
//	return false;
//}

glm::vec3 BfDrawObject::__get_average_color(pVecVert3 vert) const {
	glm::vec3 color{};
	for (const auto& it : *vert) {
		color += it.color;
	}
	return color / (float)vert->size();
}

glm::vec3 BfDrawObject::__get_average_color(std::initializer_list<BfVertex3> list) const {
	glm::vec3 color{};
	for (const auto& it : list) {
		color += it.color;
	}
	return color / (float)list.size();
}

uint16_t BfDrawObject::get_id()
{
	return __id.get();
}

void BfDrawObject::bind_pipeline(std::shared_ptr<VkPipeline> pPipeline)
{
	this->__pPipeline = std::move(pPipeline);
}

void BfDrawObject::bind_pipeline(std::unique_ptr<VkPipeline> pPipeline)
{
	this->__pPipeline = std::move(pPipeline);
}

BfRectangle::BfRectangle(const BfVertex3& left_bot, const BfVertex3& right_top) {
	
	glm::vec3 color = this->__get_average_color({ left_bot, right_top });

	this->__vertices = std::make_shared<std::vector<BfVertex3>>();
	this->__indices = std::make_shared<std::vector<uint16_t>>();
	this->__is_3D = false;
	this->__def_vertices = std::make_shared<std::vector<BfVertex3>>();
	this->__def_vertices->resize(BF_RECTANGLE_DEF_VERTICES_COUNT);

	/*float diagonal = sqrtf(
		(right_top.pos.x - left_bot.pos.x) * (right_top.pos.x - left_bot.pos.x) +
		(right_top.pos.y - left_bot.pos.y) * (right_top.pos.y - left_bot.pos.y) +
		(right_top.pos.z - left_bot.pos.z) * (right_top.pos.z - left_bot.pos.z)
	);*/

	glm::vec3 center = (left_bot.pos + right_top.pos) / 2.0f;

	glm::vec3 dia_direction = left_bot.pos - center;
	glm::vec3 dia_direction_2(-dia_direction.y, dia_direction.x, 0.0f);

	glm::vec3 left_top = center + dia_direction_2;
	glm::vec3 right_bot = center - dia_direction_2;

	glm::vec3 normal = glm::cross(dia_direction, dia_direction_2);

	this->__def_vertices->at(0) = BfVertex3(
		left_bot.pos,
		color,
		normal
	);
	this->__def_vertices->at(1) = BfVertex3(
		left_top,
		color,
		normal
	);
	this->__def_vertices->at(2) = BfVertex3(
		right_top.pos,
		color,
		normal
	);
	this->__def_vertices->at(3) = BfVertex3(
		right_bot,
		color,
		normal
	);
}

void BfRectangle::calculate_vertices(uint16_t flags)
{
	if (flags & BF_DRAW_OBJECT_LINE_MODE_BIT) {
		__vertices->resize(BF_RECTANGLE_DEF_VERTICES_COUNT);
		std::copy(__def_vertices->begin(), __def_vertices->end(), __vertices->data());
	}
	else if (flags & BF_DRAW_OBJECT_TRIANGLE_MODE_BIT) {
		__vertices->resize(BF_RECTANGLE_TRIANGLE_MODE_VERTICES_COUNT);
		__vertices->at(0) = __def_vertices->at(0);
		__vertices->at(1) = __def_vertices->at(1);
		__vertices->at(2) = __def_vertices->at(2);
		__vertices->at(3) = __def_vertices->at(2);
		__vertices->at(4) = __def_vertices->at(3);
		__vertices->at(5) = __def_vertices->at(0);
	}
	else {
		throw std::runtime_error("Incorrect input flags");
	}
}

void BfRectangle::calculate_indices(uint16_t flags)
{
	if (flags & BF_DRAW_OBJECT_LINE_MODE_BIT) {
		__indices->resize(BF_RECTANGLE_DEF_VERTICES_COUNT);
		__indices->at(0) = 0;
		__indices->at(1) = 1;
		__indices->at(2) = 2;
		__indices->at(3) = 3;
	}
	else if (flags & BF_DRAW_OBJECT_TRIANGLE_MODE_BIT) {
		__indices->resize(BF_RECTANGLE_TRIANGLE_MODE_VERTICES_COUNT);
		__indices->at(0) = 0;
		__indices->at(1) = 1;
		__indices->at(2) = 2;
		__indices->at(3) = 3;
		__indices->at(4) = 4;
		__indices->at(5) = 5;
	}
	else {
		throw std::runtime_error("Incorrect input flags");
	}
}

bool BfRectangle::__check_collineare(const pVecVert3 vert) const {
	if ((vert->size() == 1) || (vert->size() == 0)) {
		return true;
	}
	for (auto it = vert->begin() + 1; it != vert->end(); it++) {
		if (vert->begin()->normals != it->normals) {
			return false;
		}
	}
	return true;
}

bool BfRectangle::__check_collineare(std::initializer_list<BfVertex3> list) const {
	if ((list.size() == 1) || (list.size() == 0)) {
		return true;
	}
	for (auto it = list.begin() + 1; it != list.end(); it++) {
		if (list.begin()->normals != it->normals) {
			return false;
		}
	}
	return true;
}


bool Bf2DObject::__check_if_all_vertices_in_same_plane(pVecVert3 vert) const
{
	if (vert->size() <= 3) {
		return true;
	}
	
	glm::mat3 plane;

	plane[1][0] = (vert->end() - 1)->pos.x - vert->begin()->pos.x;
	plane[1][1] = (vert->end() - 1)->pos.y - vert->begin()->pos.y;
	plane[1][2] = (vert->end() - 1)->pos.z - vert->begin()->pos.z;

	plane[2][0] = (vert->end() - 2)->pos.x - vert->begin()->pos.x;
	plane[2][1] = (vert->end() - 2)->pos.y - vert->begin()->pos.y;
	plane[2][2] = (vert->end() - 2)->pos.z - vert->begin()->pos.z;

	for (auto vert_it = vert->begin(); vert_it != vert->end() - 2; vert_it++) {
		plane[0][0] = vert_it->pos.x - vert->begin()->pos.x;
		plane[0][1] = vert_it->pos.y - vert->begin()->pos.y;
		plane[0][2] = vert_it->pos.z - vert->begin()->pos.z;

		if (glm::determinant(plane) != 0) {
			return false;
		}
	}
	return true;
}

bool Bf2DObject::__check_if_all_vertices_in_same_plane(std::initializer_list<BfVertex3> vert) const
{
	if (vert.size() <= 3) {
		return true;
	}

	glm::mat3 plane;

	plane[1][0] = (vert.end() - 1)->pos.x - vert.begin()->pos.x;
	plane[1][1] = (vert.end() - 1)->pos.y - vert.begin()->pos.y;
	plane[1][2] = (vert.end() - 1)->pos.z - vert.begin()->pos.z;
					   
	plane[2][0] = (vert.end() - 2)->pos.x - vert.begin()->pos.x;
	plane[2][1] = (vert.end() - 2)->pos.y - vert.begin()->pos.y;
	plane[2][2] = (vert.end() - 2)->pos.z - vert.begin()->pos.z;

	for (auto vert_it = vert.begin(); vert_it != vert.end() - 2; vert_it++) {
		plane[0][0] = vert_it->pos.x - vert.begin()->pos.x;
		plane[0][1] = vert_it->pos.y - vert.begin()->pos.y;
		plane[0][2] = vert_it->pos.z - vert.begin()->pos.z;

		if (glm::determinant(plane) != 0) {
			return false;
		}
	}
	return true;
}

void Bf2DObject::calculate_geometry(uint16_t flags)
{
	this->calculate_vertices(flags);
	this->calculate_indices(flags);
}

const pVecVert3 Bf2DObject::get_pVertices()
{
	return this->__vertices;
}

const pVecUint Bf2DObject::get_pIndices()
{
	return this->__indices;
}


BfLayer::BfLayer(const glm::vec3& d, float offset, size_t max_obj)
	: __direction{d}
	, __center_offset{offset}
	, __reserved_objects_count{max_obj}
	, __vertex_buffer{}
	, __index_buffer{}
	, __id{}
{
	__objects.reserve(max_obj);
}

BfLayer::BfLayer(const BfVertex3& f, const BfVertex3& s, const BfVertex3& t, size_t max_obj)
	: BfLayer{ f.pos, s.pos, t.pos, max_obj }
{}

BfLayer::BfLayer(const glm::vec3& f, const glm::vec3& s, const glm::vec3& t, size_t max_obj)
	: __reserved_objects_count{max_obj}
	, __vertex_buffer{}
	, __index_buffer{}
	, __id{}
{
	__objects.reserve(max_obj);
	/*__direction = (glm::cross(s - f, t - f));
	__center_offset = -glm::dot(__direction, f)*/;

	glm::mat3 xd = {
		f.y, f.z, 1,
		s.y, s.z, 1,
		t.y, t.z, 1
	};

	glm::mat3 yd = {
		f.x, f.z, 1,
		s.x, s.z, 1,
		t.x, t.z, 1
	};

	glm::mat3 zd = {
		f.x, f.y, 1,
		s.x, s.y, 1,
		t.x, t.y, 1
	};

	__direction = glm::vec3(
		glm::determinant(xd),
	   -glm::determinant(yd),
		glm::determinant(zd)
	);

	glm::mat3 D = {
		f.x, f.y, f.z,
		s.x, s.y, s.z,
		t.x, t.y, t.z
	};

	__center_offset = -glm::determinant(D);
}

BfEvent BfLayer::bind_allocator(std::shared_ptr<VmaAllocator> allocator) {
	__pAllocator = std::move(allocator);
	
	BfSingleEvent event{};
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_LAYER_EVENT;
		event.action = BfEnActionType::BF_ACTION_TYPE_BIND_ALLOCATOR_TO_BFLAYER;
		std::stringstream ss;
		ss << " Allocator = " << allocator.get() << " was bound to "
			<< "BfLayer with id = " << __id.get();
		event.info = ss.str();
	}

	return BfEvent(event);
}
BfEvent BfLayer::bind_allocator(std::unique_ptr<VmaAllocator> allocator) {
	return this->bind_allocator(std::move(allocator));
}
BfEvent BfLayer::bind_allocator(VmaAllocator* allocator) {
	return this->bind_allocator(std::make_shared<VmaAllocator>(*allocator));
}

BfEvent BfLayer::allocate_buffers(size_t max_vertices)
{
	size_t max_vertices_size {
		sizeof(BfVertex3) * // size of 1 vertex
		max_vertices * // max number of vertices in 1 object
		__reserved_objects_count // max number of objects
	};
		
	bfCreateBuffer(&__vertex_buffer,
				   *__pAllocator.get(),
				   max_vertices_size,
				   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				   VMA_MEMORY_USAGE_CPU_TO_GPU);
	
	size_t max_indices_size {
		sizeof(uint16_t) *
		max_vertices *  
		__reserved_objects_count
	};
	

	bfCreateBuffer(&__index_buffer,
				   *__pAllocator.get(),
				   max_indices_size,
				   VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				   VMA_MEMORY_USAGE_CPU_TO_GPU);

	BfSingleEvent event{};
	{
		event.type = BfEnSingleEventType::BF_SINGLE_EVENT_TYPE_LAYER_EVENT;

		std::stringstream ss; ss << "BfLayer with id = " << __id.get()
			<< ", Index buffer status: " << __index_buffer.is_allocated
			<< "; Vertex buffer status: " << __vertex_buffer.is_allocated;

		event.info = ss.str();

		if (__index_buffer.is_allocated && __vertex_buffer.is_allocated) {
			__is_allocated = true;
			event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_BFLAYER_SUCCESS;
			__allocated_objects_count = __reserved_objects_count;
		}
		else {
			__is_allocated = false;
			event.action = BfEnActionType::BF_ACTION_TYPE_ALLOC_BFLAYER_FAILURE;
		}
	}
	return BfEvent(event);
}

void* BfLayer::write_to_buffers()
{

	size_t full_size = 0;
	for (auto& it : __objects) {
		full_size += it->get_pVertices()->size() * sizeof(BfVertex3);
	}




	void* vertex_data;// = operator new(full_size);
	vmaMapMemory(*__pAllocator.get(), __vertex_buffer.allocation, &vertex_data);
	{
		char* current_ptr = static_cast<char*>(vertex_data);
		for (const auto& ptr : __objects) {
			std::memcpy(current_ptr, ptr->get_pVertices()->data(), ptr->get_pVertices()->size() * sizeof(BfVertex3));
			current_ptr += ptr->get_pVertices()->size() * sizeof(BfVertex3);
		}
		//memcpy(vertex_data, vertices.data(), sizeof(BfVertex3) * vertices.size());
	}
	vmaUnmapMemory(*__pAllocator.get(), __vertex_buffer.allocation);

	void* index_data;
	vmaMapMemory(*__pAllocator.get(), __index_buffer.allocation, &index_data);
	{
		char* current_ptr_index = static_cast<char*>(index_data);
		for (const auto& ptr : __objects) {
			const auto& indices = ptr->get_pIndices();


		}

	}
	vmaUnmapMemory(*__pAllocator.get(), __index_buffer.allocation);

	return vertex_data;
}

uint16_t BfLayer::add_obj(std::unique_ptr<Bf2DObject> obj)
{
	uint16_t id = obj->get_id();
	__objects.push_back(std::move(obj));
	return id;
}

uint16_t BfLayer::add_obj(std::shared_ptr<Bf2DObject> obj)
{
	if (obj == nullptr) {
		throw std::runtime_error("Input object ptr is nullptr");
	}
	
	uint16_t id = obj->get_id();
	__objects.push_back(std::move(obj));
	return id;
}

void BfLayer::delete_obj(uint16_t id)
{
	for (auto it = __objects.begin(); it != __objects.end(); it++) {
		if (it->get()->get_id() == id) {
			it->reset();
			__objects.erase(it);
			break;
		}
	}
}

const uint16_t BfLayer::get_id() const
{
	return __id.get();
}

const uint16_t BfLayer::get_total_vertex_size() const
{
	uint16_t len = 0;
	for (const auto& it : __objects) {
		len += it->get_pVertices()->size();
	}
	return len;
}

std::shared_ptr<Bf2DObject> BfLayer::get_obj(uint16_t id)
{
	for (auto& it : __objects) {
		if (it == nullptr) continue;
		if (it->get_id() == id) {
			return it;
		}
	}
	return nullptr;
}

bool BfLayer::is_layers_equal(const BfLayer& f, const BfLayer& s)
{
	if ((f.__direction == s.__direction) && (f.__center_offset == s.__center_offset))
		return true;
	else if (((f.__direction.x / s.__direction.x) == (f.__direction.y / s.__direction.y)) &&
			((f.__direction.y / s.__direction.y) == (f.__direction.z / s.__direction.z)) &&
			((f.__direction.z / s.__direction.z) == (f.__center_offset / s.__center_offset)))
			return true;
	else
		return false;
}
