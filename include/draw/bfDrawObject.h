#ifndef BF_DRAWOBJECT_H
#define BF_DRAWOBJECT_H

#include <algorithm>
#include <functional>
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
class BfDrawLayer;
class BfLayerHandler;

/**
 * @defgroup bfDrawElementsShortcuts Короткие
 * названия для стандартных элементов
 * @{
 */

/*! Для объекта отрисовки */
using pObj = std::shared_ptr<BfDrawObj>;
/*! Для слоя отрисовки */
using pLay = std::shared_ptr<BfDrawLayer>;
/*! Для варианта (объект или слой) */
using pVar = std::variant<pObj, pLay>;

/**@} end of BfBladeSection2_Defines */
//
//
//
//
//

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

   uint32_t __reserved_n;
   std::vector<ptrObj_t> __objects;
   std::vector<ptrLayer_t> __layers;

   // offset of index of vertex in vertex buffer
   std::vector<int32_t> __vertex_offsets;

   // offset of index of index in index buffer
   std::vector<int32_t> __index_offsets;

   BfLayerBuffer __buffer;

   BfDrawLayer *m_root = nullptr;

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

/**
 * @brief Класс слоя с возможностью
 * получения элементов через список
 *
 * Для правильной работы класса необходимо
 * в качестве шаблонного аргумента предоставить
 * `enum`, на основе которого и будет осуществляться
 * логика хранения и поиска
 *
 * @tparam PartType Тип списка, по которому будут
 * записываться и получаться объекты или слои.
 * Должен быть списком, который наследует `uint32_t`
 *
 */
template <class PartType>
class BfDrawLayerAccessed : public BfDrawLayer
{
protected:
   /**
    * @brief Мапа, на основе которой будет
    * происходить поиск объектов
    *
    * @note В будущем для большей оптимизации
    * можно заменить `id` объектов и слоев
    * на указатели, но тогда экземляр класса
    * будет хранить дупликаты этих указателей,
    * это не страшно, так как вся логика
    * объектов и слоев основывается на использовании
    * `std::shared_ptr`
    */
   std::unordered_map<PartType, uint32_t> m_idMap;
   //
   //
   //
   //
   /**
    * @brief Добавляет объекты
    * @note Замена для `add(ptrObj_t obj)` и `add(ptrLayer_t layer)`
    *
    * @param var Вариант из слоя или объекта
    * @param part Элемент списка, к которому необходимо
    * привязать объект
    */
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
   //
   //
   //
   //
   //
   /**
    * @brief Добавление объекта через аргументы.
    * Для создания объекта внутри функции
    * передается тип и аргументы
    *
    * @note Функция существует для динамического добавления
    * элементов по время `runtime`
    *
    * @tparam T Тип создаваемого объекта или слоя
    * @tparam Ts Типы aргументов создаваемого объекта или слоя
    * @param part Элемент списка к которому нужно будет привязать объект или
    * слой
    * @param args Аргументы создаваемого объекта или слоя
    * @return Shared-указатель на созданный объект
    */
   template <class T, class... Ts>
   std::shared_ptr<T> _addPartForward(PartType part, Ts &&...args)
   {
      auto item = std::make_shared<T>(std::forward<Ts>(args)...);
      _addPart(item, part);
      return item;
   };
   //
   //
   //
   //
   //
   /**
    * @brief Добавление объекта через аргументы.
    * Для создания объекта внутри функции
    * передается тип и аргументы
    *
    * @note Функция существует для статического добавления
    * элементов, так как компилятор может оптимизировать
    * шаблонные функции
    *
    * @tparam T Тип создаваемого объекта или слоя
    * @tparam Ts Типы aргументов создаваемого объекта или слоя
    * @tparam part Элемент списка к которому нужно будет привязать объект или
    * слой
    * @param args Аргументы создаваемого объекта или слоя
    * @return Shared-указатель на созданный объект
    */
   template <class T, PartType part, class... Ts>
   std::shared_ptr<T> _addPartForward(Ts &&...args)
   {
      auto item = std::make_shared<T>(std::forward<Ts>(args)...);
      _addPart(item, part);
      return item;
   };
   //
   //
   //
   //
   //
   /**
    * @brief Возвращает casted-указатель на элемент слоя.
    * Если такого объекта нет, то происходит аварийный выход
    *
    * @param Для динамического использования
    *
    * @tparam Cast Тип к которому нужно скастить найденный объект
    * @param e Элемент списка который по которому нужно искать
    * @return Casted-shared-указатель на объект
    */
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
   //
   //
   //
   //
   //
   /**
    * @brief Возвращает casted-указатель на элемент слоя.
    * Если такого объекта нет, то происходит аварийный выход
    *
    * @param Для статического использования
    *
    * @tparam Cast Элемент списка который по которому нужно искать
    * @return Casted-shared-указатель на объект
    */
   template <class Cast, PartType part>
   std::shared_ptr<Cast> _part()
   {
      auto id = m_idMap[part];
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
public:
   bool is_draw = true;
   bool is_hovered = false;
   bool is_dragged = false;

   virtual void update();
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

   BfDrawLayer *m_root = nullptr;

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

   friend BfDrawLayer;
   friend BfLayerHandler;
};

#endif
