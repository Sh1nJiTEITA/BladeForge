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
{
	__objects.reserve(max_obj);
}

BfLayer::BfLayer(const BfVertex3& f, const BfVertex3& s, const BfVertex3& t, size_t max_obj)
	: BfLayer{ f.pos, s.pos, t.pos, max_obj }
{}

BfLayer::BfLayer(const glm::vec3& f, const glm::vec3& s, const glm::vec3& t, size_t max_obj)
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

uint16_t BfLayer::add_obj(std::unique_ptr<Bf2DObject> obj)
{
	uint16_t id = obj->get_id();
	__objects.push_back(std::move(obj));
	return id;
}

uint16_t BfLayer::add_obj(std::shared_ptr<Bf2DObject> obj)
{
	uint16_t id = obj->get_id();
	__objects.push_back(std::move(obj));
	return id;
}

void BfLayer::delete_obj(uint16_t id)
{
	for (auto& it : __objects) {
		if (it->get_id() == id) {
			it.reset();
			break;
		}
	}
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
