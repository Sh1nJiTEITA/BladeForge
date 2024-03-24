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

BfDrawObj::BfDrawObj() 
	: BfDrawObj(0)
{}

BfDrawObj::BfDrawObj(uint32_t type) 
	: id(type)
{ 
}

BfDrawObj::BfDrawObj(const std::vector<BfVertex3>& dvert, uint32_t type)
	: __dvertices{dvert}
	, id(type)
{

}

bool BfDrawObj::is_ok()
{
	bool decision = !__vertices.empty() * !__indices.empty() * BfObjID::is_id_exists(id);


	return decision;
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

BfObjectData BfDrawObj::get_obj_data()
{
	BfObjectData data{};
	data.id = this->id.get();
	data.model_matrix = __model_matrix;
	data.line_thickness = __line_thickness;

	if (__is_selected)
		data.select_color = glm::vec3(1.0f, 0.5f, 0.0f);
	else
		data.select_color = glm::vec3(1.0f, 1.0f, 1.0f);
	return data;
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
	return sizeof(BfVertex3) * __vertices.size();
}

size_t BfDrawObj::get_index_data_size()
{
	return sizeof(uint16_t) * __indices.size();
}

VkPipeline* BfDrawObj::get_bound_pPipeline()
{
	return __pPipeline;
}

//void BfDrawObj::set_obj_data(BfObjectData obj_data)
//{
//	__obj_data = obj_data;
//}

glm::mat4& BfDrawObj::get_model_matrix()
{
	return __model_matrix;
}

void BfDrawObj::bind_pipeline(VkPipeline* pPipeline)
{
	__pPipeline = pPipeline;
}

void BfDrawObj::set_color(glm::vec3 c)
{
	__main_color = c;
}

void BfDrawObj::create_indices()
{
	
	
	// TODO
	if (!__indices.empty())
		__indices.clear();

	for (int i = 0; i < __vertices.size(); ++i) {
		__indices.emplace_back(i);
	}
}

void BfDrawObj::create_vertices()
{
}

std::unordered_set<unsigned int> BfObjID::__existing_values;
std::map<uint32_t, uint32_t> BfObjID::__existing_pairs;

BfObjID::BfObjID()
	: BfObjID(0)
{}

BfObjID::BfObjID(uint32_t type) 
	: __type{type}
{
	static uint32_t value = 0;
	__value = ++value;
	BfObjID::__existing_values.insert(__value);
	BfObjID::__existing_pairs.insert(std::make_pair(__value, __type));
}

BfObjID::~BfObjID()
{
	//std::cout << "\nID DECONSTRUCTOR\n";
	BfObjID::__existing_values.erase(__value);
	BfObjID::__existing_pairs.erase(__value);
}

const uint32_t BfObjID::get() const
{
	return __value;
}

const uint32_t BfObjID::get_type() const {
	return __type;
}

bool BfObjID::is_id_exists(uint32_t id)
{
	if (__existing_values.contains(id))
		return true;
	else
		return false;
}

bool BfObjID::is_id_exists(BfObjID& id)
{
	return BfObjID::is_id_exists(id.get());
}

uint32_t BfObjID::find_type(uint32_t type) {
	return BfObjID::__existing_pairs[type];
}

BfDrawLayer::BfDrawLayer(VmaAllocator allocator, 
						 size_t vertex_size, 
						 size_t max_vertex_count, 
						 size_t max_reserved_count)

	: __reserved_n{ static_cast<uint32_t>(max_reserved_count) }
	, __buffer{ allocator, vertex_size, max_vertex_count, max_reserved_count }
	, id {}

{
	__objects.reserve(__reserved_n);
	__vertex_offsets.resize(__reserved_n, 0);
	__index_offsets.resize(__reserved_n, 0);
}

BfDrawLayer::BfDrawLayer(const BfDrawLayerCreateInfo& info)
	: BfDrawLayer(
		info.allocator,
		info.vertex_size,
		info.max_vertex_count,
		info.max_reserved_count
	)
{}

std::vector<std::shared_ptr<BfDrawObj>>::iterator BfDrawLayer::begin() {
	return __objects.begin();
}
std::vector<std::shared_ptr<BfDrawObj>>::iterator BfDrawLayer::end() {
	return __objects.end();
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
	if (!obj->is_ok())
		throw std::runtime_error("object is incorrect");
	__objects.emplace_back(obj);
}

void BfDrawLayer::add(std::shared_ptr<BfDrawLayer> layer) {
	__layers.emplace_back(layer);
}

void BfDrawLayer::add_l(std::shared_ptr<BfDrawObj> obj) {
	__objects.emplace_back(obj);
}

void BfDrawLayer::del(uint32_t id) {
	__objects.erase(
		std::remove_if(__objects.begin(), __objects.end(),
			[id](const auto& obj) { return obj->id.get() == id; }),
		__objects.end()
	);
	__layers.erase(
		std::remove_if(__layers.begin(), __layers.end(),
			[id](const auto& layer) { return layer->id.get() == id; }),
		__layers.end()
	);

	this->update_buffer();
}

void BfDrawLayer::del(const std::vector<uint32_t>& id) {
	if (id.size() == 0) return;
	
	auto con_o = [&id](std::shared_ptr<BfDrawObj> obj) {
		auto it = std::find(id.begin(), id.end(), obj->id.get());

		if (it != id.end()) {
			return true;
		}
		else
			return false;
	};

	auto con_l = [&id](std::shared_ptr<BfDrawLayer> obj) {
		auto it = std::find(id.begin(), id.end(), obj->id.get());

		if (it != id.end()) {
			return true;
		}
		else
			return false;
		};

	auto rem_o = std::remove_if(__objects.begin(), __objects.end(), con_o);
	__objects.erase(rem_o, __objects.end());
	auto rem_l = std::remove_if(__layers.begin(), __layers.end(), con_l);
	__layers.erase(rem_l, __layers.end());

	this->update_buffer();
}

void BfDrawLayer::generate_draw_data() {
	for (size_t i = 0; i < this->get_obj_count(); ++i) {
		auto obj = this->get_object_by_index(i);
		//obj->set_color({ 1.0f, 1.0f, 1.0f });
		obj->create_vertices();
		obj->create_indices();
		//obj->bind_pipeline(&__info.pipeline);
	}
	this->update_buffer();
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
	for (size_t i = 0; i < __layers.size(); i++) {
		__layers[i]->draw(combuffer, pipeline);
	}
	
	// Local elements
	vkCmdBindVertexBuffers(combuffer, 
						   0, 
						   1, 
						   __buffer.get_p_vertex_buffer(), 
						   nullptr);
	vkCmdBindIndexBuffer(combuffer, 
						 *__buffer.get_p_index_buffer(), 
						 0, 
						 VK_INDEX_TYPE_UINT16);
	
	for (size_t i = 0; i < __objects.size(); i++) {
		if (__objects[i] == nullptr) continue;
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

std::shared_ptr<BfDrawObj> BfDrawLayer::get_object_by_index(size_t index)
{
	if (index > __objects.size())
		throw std::runtime_error("input object index > objects in layer");
	return __objects.at(index);
}


bool* BfGuiIntegration::get_pSelection()
{
	return &__is_selected;
}
