#ifndef BF_CURVES_2_H
#define BF_CURVES_2_H

#include "bfVariative.hpp"
#include "bfVertex2.hpp"
#include <array>
#include <memory>

using pVecVert3 = std::shared_ptr<std::vector<BfVertex3>>;
using pVecUint = std::shared_ptr<std::vector<uint16_t>>;

using VecV3 = std::vector<BfVertex3>;
using VecUi = std::vector<BfVertex3>;

class Bf2DObject;

class BfID {
private:
  uint16_t value;

public:
  BfID();
  ~BfID(){};

  const uint16_t get() const;
};

#define BF_BFLAYER_STD_MAX_OBJECTS 1000

class BfLayer {

  BfID __id;

  // Geometry parameters
  glm::vec3 __direction; // A B C
  float __center_offset; // D

  // Drawing parameters
  std::vector<std::shared_ptr<Bf2DObject>> __objects;
  size_t __reserved_objects_count;

  // Memory management
  std::shared_ptr<VmaAllocator> __pAllocator = nullptr;
  bool __is_allocated = false;
  size_t __allocated_objects_count = 0;

  BfAllocatedBuffer __vertex_buffer;
  BfAllocatedBuffer __index_buffer;

public:
  // Initialization
  BfLayer(const glm::vec3 &d, float offset,
          size_t max_obj = BF_BFLAYER_STD_MAX_OBJECTS);
  BfLayer(const BfVertex3 &f, const BfVertex3 &s, const BfVertex3 &t,
          size_t max_obj = BF_BFLAYER_STD_MAX_OBJECTS);
  BfLayer(const glm::vec3 &f, const glm::vec3 &s, const glm::vec3 &t,
          size_t max_obj = BF_BFLAYER_STD_MAX_OBJECTS);

  BfEvent bind_allocator(std::shared_ptr<VmaAllocator> allocator);
  BfEvent bind_allocator(std::unique_ptr<VmaAllocator> allocator);
  BfEvent bind_allocator(VmaAllocator *allocator);

  BfEvent allocate_buffers(size_t max_vertices);
  void *write_to_buffers();

  // Object management
  uint16_t add_obj(std::unique_ptr<Bf2DObject> obj); // returns object ID
  uint16_t add_obj(std::shared_ptr<Bf2DObject> obj);
  std::shared_ptr<Bf2DObject> get_obj(uint16_t id);
  void delete_obj(uint16_t id);

  const uint16_t get_id() const;
  const uint16_t get_total_vertex_size() const;

  static bool is_layers_equal(const BfLayer &f, const BfLayer &s);
};

#define BF_DRAW_OBJECT_TRIANGLE_MODE_BIT 0x1
#define BF_DRAW_OBJECT_LINE_MODE_BIT 0x2

class BfDrawObject {
protected:
  std::shared_ptr<VkPipeline> __pPipeline = nullptr;
  /*std::vector<BfVertex3> __def_vertices;
  std::vector<BfVertex3> __vertices;
  std::vector<uint16_t>  __indices;*/
  pVecVert3 __def_vertices;
  pVecVert3 __vertices;
  pVecUint __indices;

  bool __is_3D = false;
  bool __is_changed = false;
  BfID __id;

  // bool __check_is_3D(pVecVert3 vert) const;
  // bool __check_is_3D(std::initializer_list<BfVertex3> list) const;
  glm::vec3 __get_average_color(pVecVert3 vert) const;
  glm::vec3 __get_average_color(std::initializer_list<BfVertex3> list) const;

public:
  virtual ~BfDrawObject() = default;

  uint16_t get_id();

  void bind_pipeline(std::shared_ptr<VkPipeline> pPipeline);
  void bind_pipeline(std::unique_ptr<VkPipeline> pPipeline);

  virtual const pVecVert3 get_pVertices() = 0;
  virtual const pVecUint get_pIndices() = 0;
};

class Bf2DObject : public BfDrawObject {

protected:
  bool __check_if_all_vertices_in_same_plane(pVecVert3 vert) const;
  bool __check_if_all_vertices_in_same_plane(
      std::initializer_list<BfVertex3> vert) const;

public:
  virtual ~Bf2DObject(){};
  virtual void calculate_vertices(uint16_t flags = 0) {};
  virtual void calculate_indices(uint16_t flags = 0) {};
  void calculate_geometry(uint16_t flags = 0);

  virtual const pVecVert3 get_pVertices();
  virtual const pVecUint get_pIndices();
};

// 1 2
// 0 3
#define BF_RECTANGLE_DEF_VERTICES_COUNT 4
#define BF_RECTANGLE_TRIANGLE_MODE_VERTICES_COUNT 6
class BfRectangle : public Bf2DObject {

public:
  BfRectangle(const BfVertex3 &left_bot, const BfVertex3 &right_top);
  void calculate_vertices(uint16_t flags = 0) override;
  void calculate_indices(uint16_t flags = 0) override;

  ~BfRectangle(){};

protected:
  bool __check_collineare(pVecVert3 vert) const;
  bool __check_collineare(std::initializer_list<BfVertex3> list) const;
};

#endif
