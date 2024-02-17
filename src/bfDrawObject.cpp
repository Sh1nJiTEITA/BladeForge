#include "BfDrawObject.h"

//BfDrawObj::BfDrawObj() : 
//	id()
//{
//
//}
//// 8(495) 377 10-86
//BfDrawObj::BfDrawObj(const BfDrawObj& obj)
//{
//	this->__vertices = obj.__vertices;
//	this->__dvertices = obj.__dvertices;
//	this->__indices = obj.__indices;
//	this->id = obj.id;
//}

bool BfDrawObj::is_ok()
{
	bool decision = !__vertices.empty() * !__indices.empty() * BfObjID::is_id_exists(id);


	return decision;
}

void BfDrawObj::check_ok()
{
	if (!is_ok())
	{
		std::stringstream ss;
		ss << "BdDrawObj with id = " << id.get() << " is not OK";
		throw std::runtime_error(ss.str());
	}
}

const std::vector<BfVertex3>& BfDrawObj::get_rVertices() const
{
	return __vertices;
}

const std::vector<BfVertex3>& BfDrawObj::get_rdVertices() const
{
	return __dvertices;
}

const std::vector<uint16_t>& BfDrawObj::get_rIndices() const
{
	return __indices;
}

BfVertex3* BfDrawObj::get_pVertices()
{
	return __vertices.data();
}

BfVertex3* BfDrawObj::get_pdVertices()
{
	return __dvertices.data();
}

uint16_t* BfDrawObj::get_pIndices()
{
	return __indices.data();
}

const BfObjectData& BfDrawObj::get_obj_data() const noexcept
{
	return __obj_data;
}

const size_t BfDrawObj::get_vertices_count() const
{
	return __vertices.size();
}

const size_t BfDrawObj::get_dvertices_count() const
{
	return __dvertices.size();
}

const size_t BfDrawObj::get_indices_count() const
{
	return __indices.size();
}

size_t BfDrawObj::get_vertex_data_size()
{
	this->check_ok();
	
	return sizeof(BfVertex3) * __vertices.size();
}

size_t BfDrawObj::get_index_data_size()
{
	this->check_ok();
	return sizeof(uint16_t) * __indices.size();
}

VkPipeline* BfDrawObj::get_bound_pPipeline()
{
	return __pPipeline;
}

void BfDrawObj::set_obj_data(BfObjectData obj_data)
{
	__obj_data = obj_data;
}

void BfDrawObj::bind_pipeline(VkPipeline* pPipeline)
{
	__pPipeline = pPipeline;
}

void BfDrawObj::create_indices()
{

}

void BfDrawObj::create_vertices()
{
}

std::unordered_set<unsigned int> BfObjID::__existing_values;

BfObjID::BfObjID()
{
	static uint32_t value = 0;
	__value = ++value;
	BfObjID::__existing_values.insert(__value);
}

BfObjID::~BfObjID()
{
	BfObjID::__existing_values.erase(__value);
}

const uint32_t BfObjID::get() const
{
	return __value;
}

bool BfObjID::is_id_exists(uint32_t id)
{
	if (__existing_values.contains(id))
		return true;
	else
		return false;
}

bool BfObjID::is_id_exists(BfObjID id)
{
	return BfObjID::is_id_exists(id.get());
}

BfDrawLayer::BfDrawLayer(VmaAllocator allocator, 
						 size_t vertex_size, 
						 size_t max_vertex_count, 
						 size_t max_reserved_count)

	: __reserved_n{ static_cast<uint32_t>(max_reserved_count) }
	, __buffer{ allocator, vertex_size, max_vertex_count, max_reserved_count }

{
	__objects.reserve(__reserved_n);
	__vertex_offsets.resize(__reserved_n, 0);
	__index_offsets.resize(__reserved_n, 0);
}

const size_t BfDrawLayer::get_whole_vertex_count() const noexcept
{
	size_t count = 0;
	for (auto& it : __objects) {
		count += it->get_vertices_count();
	}
	return count;
}

const size_t BfDrawLayer::get_whole_index_count() const noexcept
{
	size_t count = 0;
	for (auto& it : __objects) {
		count += it->get_indices_count();
	}
	return count;
}

const size_t BfDrawLayer::get_obj_count() const noexcept
{
	return __objects.size();
}

const std::vector<BfObjectData> BfDrawLayer::get_obj_model_matrices() const noexcept
{
	std::vector<BfObjectData> obj_data;
	obj_data.reserve(this->get_obj_count());

	for (const auto& obj : __objects) {
		obj_data.push_back(obj->get_obj_data());
	}
	
	return obj_data;
}

void BfDrawLayer::add(std::shared_ptr<BfDrawObj> obj)
{
	__objects.emplace_back(obj);
}

void BfDrawLayer::update_vertex_offset()
{
	size_t growing_offset = 0;
	for (size_t i = 0; i < __objects.size(); i++) {
		__vertex_offsets[i] = growing_offset;
		growing_offset += __objects[i]->get_vertices_count();
	}
}

void BfDrawLayer::update_index_offset()
{
	size_t growing_offset = 0;
	for (size_t i = 0; i < __objects.size(); i++) {
		__index_offsets[i] = growing_offset;
		growing_offset += __objects[i]->get_indices_count();
	}
}

void BfDrawLayer::update_buffer()
{
	void* vertex_data = __buffer.map_vertex_memory();
	{
		size_t offset = 0;
		for (const auto& obj : __objects) {
			size_t size = sizeof(BfVertex3) * obj->get_vertices_count();

			memcpy(reinterpret_cast<char*>(vertex_data) + offset, 
				   obj->get_pVertices(), 
				   size);

			offset += size;
		}
	}
	__buffer.unmap_vertex_memory();


	void* index_data = __buffer.map_index_memory();
	{
		size_t offset = 0;
		for (const auto& obj : __objects) {
			size_t size = sizeof(uint16_t) * obj->get_indices_count();

			memcpy(reinterpret_cast<char*>(index_data) + offset,
				   obj->get_pIndices(),
				   size);

			offset += size;
		}
	}
	__buffer.unmap_index_memory();
}

void BfDrawLayer::draw(VkCommandBuffer combuffer, VkPipeline pipeline)
{
	vkCmdBindVertexBuffers(combuffer, 0, 1, __buffer.get_p_vertex_buffer(), nullptr);
	vkCmdBindIndexBuffer(combuffer, *__buffer.get_p_index_buffer(), 0, VK_INDEX_TYPE_UINT16);
	
	for (size_t i = 0; i < __objects.size(); i++) {
		vkCmdDrawIndexed(
			combuffer,
			__objects[i]->get_indices_count(),
			1,
			__index_offsets[i],
			__vertex_offsets[i],
			i
		);
	}
}

void BfDrawLayer::check_element_ready(size_t element_index)
{
	//vkCmdDrawIndexed(command_buffer, (uint32_t)pData->indices_count, 1, (uint32_t)pData->index_offset, 0, i + obj_data_index_offset);
	

	for (size_t i = 0; i < __objects.size(); i++) {
		BfDrawVar var{};
		var.index_count = __objects[i]->get_indices_count();
		var.instance_count = 1;
		var.first_index = __index_offsets[i];
		var.vertex_offset = __vertex_offsets[i];
		var.first_instance = 1;

		std::cout << var << "\n";
		/*for (size_t j = 0; j < __objects[i]->get_vertices_count(); j++) {
			
		}*/
	}
}

BfPlane::BfPlane(std::vector<BfVertex3> d_vertices)
	: BfDrawObj()
{
	__dvertices = d_vertices;
}

void BfPlane::create_vertices()
{
	__vertices.reserve(__dvertices.size());
	for (const auto& dvert : __dvertices) {
		__vertices.emplace_back(dvert);
	}
}

void BfPlane::create_indices()
{
	__indices.reserve(__vertices.size() * 2);
	for (size_t i = 0; i < __vertices.size(); ++i) {
		__indices.emplace_back(i);
	}
}
