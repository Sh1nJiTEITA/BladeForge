#ifndef BF_DRAWOBJECT_H
#define BF_DRAWOBJECT_H

#include <algorithm>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "bfBuffer.h"
#include "bfUniforms.h"
#include "bfVertex2.hpp"

class BfObjID
{
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
   void change_type(uint32_t type);
   static bool is_id_exists(uint32_t id);
   static bool is_id_exists(BfObjID &id);
   static uint32_t find_type(uint32_t);
};

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

class BfDrawObj;
class BfLayerHandler;

struct BfDrawLayerCreateInfo
{
   VmaAllocator allocator;
   size_t vertex_size;
   size_t max_vertex_count;
   size_t max_reserved_count;
   bool is_nested;
   uint32_t id_type;
};

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

class BfDrawLayer : public std::enable_shared_from_this<BfDrawLayer>
{
   using ptrLayer_t = std::shared_ptr<BfDrawLayer>;
   using ptrObj_t = std::shared_ptr<BfDrawObj>;
   using ptrVar_t = std::variant<ptrLayer_t, ptrObj_t>;

   using itptrLayer_t = std::vector<ptrLayer_t>::iterator;
   using itptrObj_t = std::vector<ptrObj_t>::iterator;
   using itptrVar_t = std::variant<itptrLayer_t, itptrObj_t>;

   using layerPair = std::pair<ptrLayer_t, std::optional<itptrLayer_t>>;
   using objPair = std::pair<ptrLayer_t, std::optional<itptrObj_t>>;
   using varPair = std::pair<ptrLayer_t, std::optional<itptrVar_t>>;

   // struct __varTransaction
   // {
   //    std::optional<itptrVar_t> what;
   //    std::optional<itptrVar_t> where;
   //    std::optional<ptrLayer_t> root;
   // };

   uint32_t __reserved_n;
   std::vector<ptrObj_t> __objects;
   std::vector<ptrLayer_t> __layers;

   // offset of index of vertex in vertex buffer
   std::vector<int32_t> __vertex_offsets;

   // offset of index of index in index buffer
   std::vector<int32_t> __index_offsets;

   BfLayerBuffer __buffer;

public:
   BfObjID id;

   BfDrawLayer(
       VmaAllocator allocator = nullptr,
       size_t vertex_size = 0,
       size_t max_vertex_count = 0,
       size_t max_reserved_count = 0,
       bool is_nested = true,
       uint32_t id_type = 0
   );

   BfDrawLayer(const BfDrawLayerCreateInfo &info);
   virtual ~BfDrawLayer();

   bool is_nested() const noexcept;

   itptrObj_t begin();
   itptrObj_t end();

   const size_t get_whole_vertex_count() const noexcept;
   const size_t get_whole_index_count() const noexcept;
   const size_t get_obj_count() const noexcept;
   const size_t get_layer_count() const noexcept;

   const size_t get_obj_count_downside() const noexcept;

   const std::vector<BfObjectData> get_obj_model_matrices() const noexcept;

   void add(ptrObj_t obj);
   void add(ptrLayer_t layer);
   void add_l(ptrObj_t obj);

   virtual void generate_draw_data();

   void del(uint32_t id, bool total = true);
   void del(const std::vector<uint32_t> &id);

   virtual void del_all();
   virtual void remake();

   std::vector<int32_t> &update_vertex_offset();
   std::vector<int32_t> &update_index_offset();

   void update_buffer();
   void update_nested(void *v, void *i, size_t &off_v, size_t &off_i);

   void clear_buffer();
   void set_color(glm::vec3 c);

   // TODO: global model-matrix descriptor
   void draw(
       VkCommandBuffer combuffer,
       size_t &offset,
       size_t &index_offset,
       size_t &vertex_offset
   );

   void map_model_matrices(size_t frame_index, size_t &offset, void *data);

   ptrObj_t get_object_by_index(size_t index);
   ptrObj_t get_object_by_id(size_t index);
   ptrLayer_t get_layer_by_index(size_t index);
   ptrLayer_t get_layer_by_id(size_t index);

   friend BfLayerHandler;

private:
   ptrObj_t &__ref_find_obj_by_id(size_t id);
   ptrLayer_t &__ref_find_layer_by_id(size_t id);
   objPair __it_find_obj_by_id(size_t id);
   layerPair __it_find_layer_by_id(size_t id);
   varPair __it_find_var_by_id(size_t id);
};

//
//
//
//
//
//
//
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
//
//
//
//
//
//
//
//
//

using pObj = std::shared_ptr<BfDrawObj>;
using pLay = std::shared_ptr<BfDrawLayer>;
using pVar = std::variant<pObj, pLay>;

template <class PartType>
class BfDrawLayerAccessed : public BfDrawLayer
{
protected:
   std::unordered_map<PartType, uint32_t> m_idMap;

   void _addPart(pVar var, PartType part)
   {
      std::visit(
          [this, &part](auto &&input) {
             using T = std::decay_t<decltype(input)>;
             uint32_t id = input->id.get();

             if constexpr (std::is_same_v<T, pObj>)
             {
                m_idMap[part] = id;
                this->add_l(input);
             }
             else if constexpr (std::is_same_v<T, pLay>)
             {
                m_idMap[part] = id;
                this->add(input);
             }
             else
             {
                std::cerr << "Invalid type in variant (_addPart)\n";
                abort();
             }
          },
          var
      );
   }

   template <class T, class... Ts>
   void _addPartForward(PartType part, Ts &&...args)
   {
      auto item = std::make_shared<T>(std::forward<Ts>(args)...);
      _addPart(item, part);
   };

   template <class T, PartType part, class... Ts>
   void _addPartForward(Ts &&...args)
   {
      auto item = std::make_shared<T>(std::forward<Ts>(args)...);
      _addPart(item, part);
   };

   template <class Cast>
   std::shared_ptr<Cast> _part(PartType e)
   {
      auto id = m_idMap[e];
      if (pObj found = this->get_object_by_id(id))
      {
         return std::dynamic_pointer_cast<Cast>(found);
      }
      else if (pLay found = this->get_layer_by_id(id))
      {
         return std::dynamic_pointer_cast<Cast>(found);
      }
      else
      {
         std::cerr << "Invalid 'id' (no obj or layer with such id) (_part)\n";
         abort();
      }
   }
};

//
//
//
//
//
//
//
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
//
//
//
//
//
//
//
//

class BfLayerKiller
{
   std::vector<std::shared_ptr<BfDrawLayer>> __layers;

   static BfLayerKiller *__p;

public:
   BfLayerKiller();
   ~BfLayerKiller();

   static BfLayerKiller *get_root();
   static void set_root(BfLayerKiller *k);

   void add(std::shared_ptr<BfDrawLayer> layer);
   void kill();
};

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

class BfGuiIntegration
{
protected:
   bool __is_selected = false;

public:
   bool *get_pSelection();
   bool is_draw = true;
};

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

class BfDrawObj : public BfGuiIntegration
{
protected:
   std::vector<BfVertex3> __vertices;
   std::vector<BfVertex3> __dvertices;
   std::vector<uint32_t> __indices;

   VkPipeline *__pPipeline = nullptr;
   glm::mat4 __model_matrix = glm::mat4(1.0f);
   glm::vec3 __main_color = glm::vec3(1.0f);
   float __line_thickness = 0.00025;

public:
   BfDrawObj();
   BfDrawObj(uint32_t type);
   BfDrawObj(const std::vector<BfVertex3> &dvert, uint32_t type = 0);

   BfObjID id;

   std::vector<BfVertex3> &vertices();
   std::vector<BfVertex3> &dVertices();
   std::vector<uint32_t> &indices();

   BfObjectData genObjData();

   VkPipeline *get_bound_pPipeline();
   glm::mat4 &modelMatrix();

   void bind_pipeline(VkPipeline *pPipeline);
   void set_color(glm::vec3 c);

   virtual bool is_ok();
   virtual void createIndices();
   virtual void createVertices();
};

#endif
