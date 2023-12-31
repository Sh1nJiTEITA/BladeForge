#ifndef BF_CURVES_2_H
#define BF_CURVES_2_H

#include <array>

#include "bfVariative.hpp"
#include "bfVertex2.hpp"

using pVecVert3 = std::shared_ptr<std::vector<BfVertex3>>;
using pVecUint = std::shared_ptr<std::vector<uint16_t>>;


class Bf2DObject;


#define BF_BFLAYER_STD_MAX_OBJECTS 1000

class BfLayer {
	


	glm::vec3 __direction; // A B C
	float __center_offset; // D
	
	std::vector<std::shared_ptr<Bf2DObject>> __objects;

public:

	BfLayer(const glm::vec3& d, float offset, size_t max_obj = BF_BFLAYER_STD_MAX_OBJECTS);
	BfLayer(const BfVertex3& f, const BfVertex3& s, const BfVertex3& t, size_t max_obj = BF_BFLAYER_STD_MAX_OBJECTS);
	BfLayer(const glm::vec3& f, const glm::vec3& s, const glm::vec3& t, size_t max_obj = BF_BFLAYER_STD_MAX_OBJECTS);
	
	uint16_t add_obj(std::unique_ptr<Bf2DObject> obj); // returns object ID
	uint16_t add_obj(std::shared_ptr<Bf2DObject> obj);
	void delete_obj(uint16_t id);

	std::shared_ptr<Bf2DObject> get_obj(uint16_t);

	static bool is_layers_equal(const BfLayer& f, const BfLayer& s);
};






class BfID {
private:
	uint16_t value;
public:
	BfID();
	~BfID() {};

	const uint16_t get() const;
};





class BfDrawObject {
protected:

	

	pVecVert3	__def_vertices = nullptr;
	pVecVert3	__vertices = nullptr;
	pVecUint	__indices = nullptr;
	bool		__is_3D = false;
	BfID		__id;

	//bool __check_is_3D(pVecVert3 vert) const;
	//bool __check_is_3D(std::initializer_list<BfVertex3> list) const;
	glm::vec3 __get_average_color(pVecVert3 vert) const;
	glm::vec3 __get_average_color(std::initializer_list<BfVertex3> list) const;

public:
	uint16_t get_id();
	virtual ~BfDrawObject() = default;
	

	virtual const pVecVert3 get_pVertices() = 0;
	virtual const pVecUint get_pIndices() = 0;
};

class Bf2DObject : public BfDrawObject {

protected:
	
	bool __check_if_all_vertices_in_same_plane(pVecVert3 vert) const;
	bool __check_if_all_vertices_in_same_plane(std::initializer_list<BfVertex3> vert) const;
	
public:

	virtual ~Bf2DObject() {};
	virtual const pVecVert3 get_pVertices();
	virtual const pVecUint get_pIndices();
};





// 1 2
// 0 3
#define BF_RECTANGLE_DEF_VERTICES_COUNT 4
class BfRectangle : public Bf2DObject {


public:
	BfRectangle(const BfVertex3& left_bot, const BfVertex3& right_top);
	~BfRectangle() {};
protected:
	bool __check_collineare(pVecVert3 vert) const;
	bool __check_collineare(std::initializer_list<BfVertex3> list) const;

};







#endif 