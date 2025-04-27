#ifndef BF_DRAWOBJECT2_H
#define BF_DRAWOBJECT2_H

#include <vulkan/vulkan_core.h>

#include <memory>
#include <random>
#include <stdexcept>

#include "bfDrawObjectBuffer.h"
#include "bfObjectId.h"
#include "bfTypeManager.h"
#include "bfUniforms.h"
#include "bfVertex2.hpp"

// clang-format off


/**
 * @def BF_PIPELINE(type)
 * @brief Retrieves a pipeline of the given type if pipeline support is enabled;
 *        otherwise returns nullptr.
 *
 * If @c BF_CURVES4_NO_PIPELINE is defined, then @c BF_PIPELINE(...) evaluates
 * to @c nullptr. Otherwise, it includes the @c bfPipeline.h header and defines
 * @c BF_PIPELINE(type) to dereference the result of
 * @c BfPipelineHandler::instance()->getPipeline(type).
 */
#if defined(BF_CURVES4_NO_PIPELINE)
#   define BF_PIPELINE(...) nullptr
#else
#   include "bfPipeline.h"
#   define BF_PIPELINE(type) *BfPipelineHandler::instance()->getPipeline(type)
#endif
//
//
//
//
//
// clang-format on

namespace obj
{
class BfDrawObjectBase;

using BfIndex = uint32_t;
using BfObj = std::shared_ptr< BfDrawObjectBase >;
using BfObjWeak = std::weak_ptr< BfDrawObjectBase >;

/**
 * @class BfDrawControlProxy
 * @brief Proxy class to work with render/vulkan bindings
 *
 * @note Не рассчитан на использование вне контекста работы
 * с отрисовкой
 *
 */
class BfDrawControlProxy
{
public:
   BfDrawControlProxy(BfDrawObjectBase& obj);

   /**
    *
    *
    * @brief Гененирует оффсет для вертексов на конкретный
    * уровень
    *
    * @note Используется для LAYER
    */
   std::vector< int32_t > vertexOffset(size_t last = 0) const;

   /**
    *
    *
    * @brief Генерирует оффсет для индексов для конкретного уровня
    *
    * @note Используется для LAYER
    */
   std::vector< int32_t > indexOffset(size_t last = 0) const;

   /**
    *
    *
    * @brief Возвращает данную модельную матрицу
    *
    * @note Используется для OBJECT
    */
   const glm::mat4& model() const;

   /**
    *
    *
    * @brief Является ли объект буффером, то есть было ли данного
    * объекта созданы буфферы и выделана память
    *
    * @note Используется для LAYER
    *
    * @return ...
    */
   bool isBuffer() const;

   /**
    * @brief Загружает данные о модельной матрице в `data`,
    * меняет `offset` с учетом загруженных данных
    *
    * @note Используется для LAYER & OBJECT
    *
    * @param frame_index  TODO: deprecate
    * @param offset ...
    * @param data указатель на открытую для мапа память
    */
   void mapModel(size_t frame_index, size_t& offset, void* data) const;

   /**
    * @brief Обновляет содержимое буфера.
    * Создано и работает только для слоев, которые содержат буффер,
    * таких как `BfDrawRootLayer` - которые создаются как верхние
    * слои.
    *
    * @note Функция рекурсивная, поэтому для ручного использования
    * где-то - предоставлять аргументы не следует
    *
    * @note Используется для LAYER
    *
    * @param v указатель на открутую память точек
    * @param i указатель на открутую память индексов
    * @param off_v оффсет точек
    * @param off_i оффсет индексов
    */
   void updateBuffer(
       bool make = false,
       void* v = nullptr,
       void* i = nullptr,
       size_t* off_v = nullptr,
       size_t* off_i = nullptr
   );

   /**
    * @brief Запускает процесс рендера данного объекта.
    * Если это OBJECT -> рендерит геометрию
    * Если это LAYER -> рекурсивно рендерит одъекты внутри
    *
    * @note Используется для LAYER & OBJECT
    *
    * @param combuffer командный буффер открытый для рендер пасса
    * @param offset оффсет, то же самое что рендер-айди
    * @param index_offset оффсет для индексов для конкретного буффера
    * @param vertex_offset оффсет для точек для конкретного буффера
    */
   void draw(
       VkCommandBuffer combuffer,
       size_t* offset,
       size_t* index_offset,
       size_t* vertex_offset,
       uint32_t hovered_id
   ) const;

   void toggleHover(int status = -1);

private:
   BfDrawObjectBase& m_obj;
};

/**
 * @class BfDrawDebugProxy
 * @brief Создан для хранения методов, которые могут
 * потребоваться при отладке
 *
 */
class BfDrawDebugProxy
{
public:
   BfDrawDebugProxy(BfDrawObjectBase& obj);
   void printVertices();
   void printIndices();

private:
   BfDrawObjectBase& m_obj;
};

class BfGuiIntegration2
{
public:
   virtual void processDragging() {}

protected:
   bool isHovered;
};

/**
 * @class BfDrawObjectBase
 * @brief Базовый класс объектов. Объект наследует BfObjectId, который
 * содержит методы для работы с `id`
 *
 */
class BfDrawObjectBase
    : public std::enable_shared_from_this< BfDrawObjectBase >,
      public BfObjectId,
      public obj::BfGuiIntegration2
{
public:
   /**
    * @brief База объекта может быть 1 из 3 типов, которые влияют на логику
    * рендеринга
    */
   enum Type
   {
      OBJECT,     /** Содержит только точки и индексы, без детей */
      LAYER,      /** Содержит OBJECT & LAYERS, но без точек и индексов*/
      ROOT_LAYER, /** Аналогично LAYER, но имеет свой vkbuffer */
   };

   /**
    * @brief Дефолтный конструктор. Так как это базовый класс, который не
    * сглаживает углы при создании объектов, то нужно указать все параметры:
    *
    * @param typeName Тип объекта (название)
    * @param pl Пайплайн для рендеринга. Используется только для OBJECT
    * @param type Тип объекта
    * @param max_vertex Максимальное количество точек. Используется только
    * для ROOT_LAYER
    * @param max_obj Максимальное количество индексов. Используется только
    * для ROOT_LAYER
    */
   BfDrawObjectBase(
       BfOTypeName typeName,
       VkPipeline pl,
       Type type,
       size_t max_vertex = 2000,
       size_t max_obj = 20
   );

   virtual std::shared_ptr< BfDrawObjectBase > clone() const;

   /**
    *
    *
    * @brief Точки...
    *
    * @return ...
    */
   const std::vector< BfVertex3 >& vertices() const { return m_vertices; }

   /**
    *
    *
    * @brief Индексы...
    *
    * @return ...
    */
   const std::vector< BfIndex >& indices() const { return m_indices; }

   /**
    * @brief Тип объекта
    *
    * @return ...
    */
   const Type drawtype() const { return m_type; }

   /**
    * @brief Добавляет слой или объект в ЭТОТ объект, если
    * он является ROOT_LAYER | LAYER
    *
    * @param n объект
    */
   void add(BfObj n);

   /**
    * @brief Построение объекта.
    * Если OBJECT -> генерация точек и индексов
    * Если LAYER -> генерация объектов и добавление их в слой
    * Если ROOT_LAYER -> генерация (вызов make()) для всех внутренних
    * элементов слоя
    *
    */
   virtual void make();

   friend BfDrawControlProxy;
   BfDrawControlProxy control() { return BfDrawControlProxy(*this); };

   friend BfDrawDebugProxy;
   BfDrawDebugProxy debug() { return BfDrawDebugProxy(*this); };

   BfObj root();
   virtual void copy(const BfDrawObjectBase& obj)
   {
      m_root = obj.m_root;
      m_modelMatrix = obj.m_modelMatrix;
      m_pipeline = obj.m_pipeline;
   }

   virtual bool toggleRender(int sts = -1)
   {
      bool current = m_isrender;
      if (sts == -1)
      {
         m_isrender = !m_isrender;
      }
      else
      {
         m_isrender = sts;
      }
      return current;
   }

protected:
   virtual BfObjectData _objectData();
   void _assignRoots();

protected:
   VkPipeline m_pipeline;
   glm::mat4 m_modelMatrix;

   std::vector< BfVertex3 > m_vertices;
   std::vector< BfIndex > m_indices;

   BfObjWeak m_root;
   std::vector< BfObj > m_children;

private:
   std::unique_ptr< BfObjectBuffer > m_buffer;
   Type m_type;
   bool m_isrender;
};

/**
 * @class BfDrawObject
 * @brief
 *
 */
class BfDrawObject : public BfDrawObjectBase
{
public:
   BfDrawObject(BfOTypeName typeName, VkPipeline plm, uint32_t disc = 200);

   /**
    * @brief Должен быть определен для любого объекта
    */
   virtual void make() override;

   /**
    * @brief Так как объект не может иметь детей,
    * этот метод ему не нужен
    */
   void add() = delete;

   glm::vec3& color() { return m_color; }

   virtual void copy(const BfDrawObjectBase& obj) override
   {
      BfDrawObjectBase::copy(obj);

      const auto& casted = static_cast< const BfDrawObject& >(obj);
      m_color = casted.m_color;
      m_discretization = casted.m_discretization;
   }

   virtual std::shared_ptr< BfDrawObjectBase > clone() const override;

protected:
   /**
    * @brief Генерирует индексы. Для каждого вертекса
    * Создается свой индекс. Работает хорошо для случаев,
    * когда нужно будет использовать цвет.
    */
   void _genIndicesStandart();
   uint32_t m_discretization;
   glm::vec3 m_color;
};

/**
 * @class BfDrawLayer
 * @brief
 *
 */
class BfDrawLayer : public BfDrawObjectBase
{
public:
   BfDrawLayer(BfOTypeName typeName);

   virtual void make() override;

   /**
    * @brief Так как слой не может иметь точки, этот метод ему не нужен
    */
   const std::vector< BfVertex3 >& vertices() const = delete;

   /**
    * @brief Так как слой не может иметь индексы, этот метод ему не нужен
    */
   const std::vector< BfIndex >& indices() const = delete;

   std::vector< BfObj >& children();

   virtual bool toggleRender(int sts = -1) override;
};

template < class PartEnum >
class BfDrawLayerWithAccess : public obj::BfDrawLayer
{
public:
   using pObj = std::shared_ptr< BfDrawObjectBase >;

   BfDrawLayerWithAccess(BfOTypeName typeName)
       : obj::BfDrawLayer(typeName)
   {
   }

protected:
   std::unordered_map< PartEnum, BfOId > m_idMap;

   template < PartEnum part >
   void _addPart(pObj obj)
   {
      this->add(obj);
      m_idMap[part] = obj->id();

      fmt::println(
          "Added id={}, type={}, typename={}, part={}",
          obj->id(),
          obj->type(),
          obj->typeName(),
          static_cast< uint32_t >(part)
      );
   }

   template < typename T, typename... Args >
   std::shared_ptr< T > _addPartForward(PartEnum part, Args&&... args)
   {
      auto item = std::make_shared< T >(std::forward< Args >(args)...);
      _addPart< part >(item);
      return item;
   }

   template < PartEnum part, typename T, typename... Args >
   std::shared_ptr< T > _addPartForward(Args&&... args)
   {
      auto item = std::make_shared< T >(std::forward< Args >(args)...);
      _addPart< part >(item);
      return item;
   };

   pObj _findObjectById(BfOId id)
   {
      // clang-format off
      auto b = std::find_if(m_children.begin(), m_children.end(), 
         [&id](const pObj& o) {
            return o->id() == id;    
         }
      );
      if (b == m_children.end()) { 
         throw std::runtime_error("Cant find object by id");
      }
      return *b;
      // clang-format on
   }

   template < class Cast >
   std::shared_ptr< Cast > _part(PartEnum e)
   {
      auto id = m_idMap[e];
      return std::static_pointer_cast< Cast >(_findObjectById(id));
   }

   template < PartEnum part, class Cast >
   std::shared_ptr< Cast > _part()
   {
      auto id = m_idMap[part];
      return std::static_pointer_cast< Cast >(_findObjectById(id));
   }

   template < PartEnum part >
   bool _toggleRender(int sts = -1)
   {
      auto id = m_idMap[part];
      auto obj = _findObjectById(id);
      return obj->toggleRender(sts);
   }
};

/**
 * @class BfDrawRootLayer
 * @brief Слой объект для хранения других слоев.
 * Имеет свой буффер.
 */
class BfDrawRootLayer : public BfDrawObjectBase
{
public:
   /**
    * @brief Создаем слой
    *
    * @param max_vertex Максимальное количество точек в объекте
    * @param max_obj Максимальное количество объектов
    */
   BfDrawRootLayer(size_t max_vertex = 2000, size_t max_obj = 20);

   /**
    * @brief вызывает make() всех объектов внутри
    */
   virtual void make() override;

   /**
    * @brief Так как слой не может иметь точки, этот метод ему не нужен
    */
   const std::vector< BfVertex3 >& vertices() const = delete;
   /**
    * @brief Так как слой не может иметь индексы, этот метод ему не нужен
    */
   const std::vector< BfIndex >& indices() const = delete;
};

/**
 * @class TestObj
 * @brief
 *
 */
class TestObj : public BfDrawObject
{
public:
   TestObj()
       : BfDrawObject("TestObj", BF_PIPELINE(BfPipelineType_Triangles))
   {
   }

   virtual void make() override
   {
      m_vertices = {
          {-1.0f, -1.0f, 0.0f},
          {-1.0f, 1.0f, 0.0f},
          {1.0f, 1.0f, 0.0f},
          {1.0f, -1.0f, 0.0f},
      };
      m_indices = {0, 1, 3, 1, 2, 3};
   }
};

/**
 * @class TestObj2
 * @brief
 *
 */
class TestObj2 : public BfDrawObject
{
public:
   TestObj2()
       : BfDrawObject("TestObj2", BF_PIPELINE(BfPipelineType_Triangles))
   {
   }

   virtual void make() override
   {
      m_vertices = {
          {-1.0f, 0.0f, -1.0f},
          {-1.0f, 0.0f, 1.0f},
          {1.0f, 0.0f, 1.0f},
          {1.0f, 0.0f, -1.0f},
      };
      m_indices = {0, 1, 3, 1, 2, 3};
   }
};

/**
 * @class TestLayer
 * @brief
 *
 */
class TestLayer : public BfDrawLayer
{
public:
   TestLayer()
       : BfDrawLayer("Test layer")
   {
   }

   virtual void make() override
   {
      auto test1 = std::make_shared< TestObj >();
      test1->make();
      add(test1);
      auto test2 = std::make_shared< TestObj2 >();
      test2->make();
      add(test2);
   }
};

class RandomPlane : public BfDrawObject
{
public:
   RandomPlane()
       : BfDrawObject("RandomPlane", BF_PIPELINE(BfPipelineType_Triangles))
   {
   }

   virtual void make() override
   {
      static std::random_device rd;
      static std::mt19937 gen(rd());
      std::uniform_real_distribution< float > dist(-5.0f, 5.0f);
      std::uniform_real_distribution< float > angleDist(
          0.0f,
          glm::radians(360.0f)
      );

      glm::vec3 translation = {dist(gen), 0.0f, dist(gen)};
      float rotationAngle = angleDist(gen);
      glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation);
      transform =
          glm::rotate(transform, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

      std::vector< glm::vec3 > baseVertices = {
          {-0.5f, 0.0f, -0.5f},
          {-0.5f, 0.0f, 0.5f},
          {0.5f, 0.0f, 0.5f},
          {0.5f, 0.0f, -0.5f},
      };

      m_vertices.clear();
      for (const auto& v : baseVertices)
      {
         glm::vec4 transformed = transform * glm::vec4(v, 1.0f);
         m_vertices.push_back({transformed.x, transformed.y, transformed.z});
      }

      m_indices = {0, 1, 3, 1, 2, 3};
   }
};

} // namespace obj

#endif
