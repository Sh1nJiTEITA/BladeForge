#ifndef BF_BLADE_SECTION_H
#define BF_BLADE_SECTION_H

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include "bfCurves3.h"
#include "bfDrawObject.h"
#include "bfDrawObjectDefineType.h"
// #include "bfLayerKiller.h"

struct BfBladeSectionCreateInfo
{
   BfDrawLayerCreateInfo layer_create_info{
       .id_type = BF_DRAW_LAYER_TYPE_BLADE_SECTION,
   };

   float width;

   float install_angle;

   float inlet_angle;
   float outlet_angle;

   float inlet_surface_angle;
   float outlet_surface_angle;

   float inlet_radius;
   float outlet_radius;

   float border_length;

   bool is_triangulate = false;
   bool is_center = true;

   // Other
   glm::vec3 start_vertex{0.0f, 0.0f, 0.0f};
   glm::vec3 grow_direction{0.0f, 1.0f, 0.0f};
   glm::vec3 up_direction{0.0f, 0.0f, 1.0f};

   VkPipeline l_pipeline = nullptr;
   VkPipeline t_pipeline = nullptr;
};

bool bfCheckBladeSectionCreateInfoEquality(
    const BfBladeSectionCreateInfo& i_1, const BfBladeSectionCreateInfo& i_2
);
void bfFillBladeSectionStandart(BfBladeSectionCreateInfo* info);

#define BF_BLADESECTION_AVE_COLOR glm::vec3(1.0, 0.5, 0.0)
#define BF_BLADESECTION_BACK_COLOR glm::vec3(0.5, 0.0, 1.0)
#define BF_BLADESECTION_FACE_COLOR glm::vec3(0.0, 0.8, 0.8)

class BfBladeSection : public BfDrawLayer
{
protected:
   BfBladeSectionCreateInfo* __info;

   BfVertex3 __chord_inlet_center;
   BfVertex3 __chord_outlet_center;

public:
   BfBladeSection(BfBladeSectionCreateInfo* info);

   /*
           __objects[..]
           1. Top border
           2. Bot border
           3. Chord
           4. Inlet edge circle
           5. Outlet edge circle
           4. Inlet vector
           5. Outlet vector
           6. Inlet io-line
           7. Outlet io_line
   */

   const BfVertex3& get_inlet_center() const noexcept;
   const BfVertex3& get_outlet_center() const noexcept;

   //
   std::shared_ptr<BfSingleLine> get_top_border();
   std::shared_ptr<BfSingleLine> get_bot_border();
   std::shared_ptr<BfSingleLine> get_chord();
   std::shared_ptr<BfCircle> get_inlet_edge();
   std::shared_ptr<BfCircle> get_outlet_edge();
   std::shared_ptr<BfSingleLine> get_inlet_vector();
   std::shared_ptr<BfSingleLine> get_outlet_vector();

   std::shared_ptr<BfBezierCurve> get_ave_curve();
   std::shared_ptr<BfBezierCurve> get_back_curve();
   std::shared_ptr<BfBezierCurve> get_face_curve();

   std::shared_ptr<BfArc> get_inlet_edge_a();
   std::shared_ptr<BfArc> get_outlet_edge_a();

   std::shared_ptr<BfBezierCurveFrame> get_ave_curve_frame();
   std::shared_ptr<BfBezierCurveFrame> get_back_curve_frame();
   std::shared_ptr<BfBezierCurveFrame> get_face_curve_frame();

   std::vector<std::shared_ptr<BfTriangle>> get_triangulated_shape();
   BfVertex3 get_center();

   virtual void remake(BfBladeSectionCreateInfo* info);
   virtual void del_all() override;

   std::vector<BfVertex3> get_contour();

   void generate_draw_data() override;

   /*
    * Возвращает набор из:
    * 1) Входная кромка
    * 2) Спинка
    * 3) Выходная кромка
    * 4) Корыто
    *
    */
   std::vector<std::shared_ptr<BfDrawObj>> get_shape_parts();
   /*const BfBladeSectionCreateInfo& get_info();
   BfBladeSectionCreateInfo* get_pInfo();*/

private:
   void __generate_outer_elements();
   void __generate_average_line_geometry();
   void __generate_surface_elements();
   void __generate_obj_frames();

   void __generate_center_circle();
   void __generate_triangular_shape();

   void __clear_draw_data();
};

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
 * @class BfBladeSectionCmax
 * @brief Описывает как должны располагаться окружности,
 * между которыми необходимо будет проводить кривые Безье
 *
 */
struct BfBladeSectionCmax
{
   float radius;             /*! Радиус окружности */
   float relativeCoordinate; /*! Относительная координата (от начала) */
};

/**
 * @class BfBladeSectionCreateInfo2
 * @brief Описывает каким образом сечение должно быть
 * создано
 *
 */
struct BfBladeSectionCreateInfo2
{
   /*! Как создать слой (заполняется автоматически) */
   BfDrawLayerCreateInfo layer_create_info;

   float chord;            /*! Длина хорды */
   float installAngle;     /*! Угол установки */
   float inletAngle;       /*! Входной угол */
   float outletAngle;      /*! Выходной угол */
   float inletEdgeRadius;  /*! Радиус входной кромки */
   float outletEdgeRadius; /*! Радиус выходной кромки */

   std::vector<BfBladeSectionCmax> cmax; /*! Все окружности */

   VkPipeline l_pipeline = nullptr;
   VkPipeline t_pipeline = nullptr;
};

/**
 * @brief Заполняет `BfBladeSectionCreateInfo2`
 * стандартными значениями
 * @param info
 */
void bfFillBladeSectionStandart2(BfBladeSectionCreateInfo2* info);

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
 * @defgroup BfBladeSection2_Defines Некоторые отладочные макросы
 * для `BfBladeSection2`
 * @{
 */

/** Transform vector to string with `<<` operator, (for std::cout) */
#define BFVEC3_STR(VEC) VEC.x << " " << VEC.y << " " << VEC.z
//
//
//
/** Short log */
#define BFBS2_LOG(MSG) std::cout << MSG << "\n"
//
//
//
/** Default color for cmax object */
#define BF_BLADESECTION_CMAX_COLOR glm::vec3(1.0f, 0.2f, 0.5f)

/**@} end of BfBladeSection2_Defines */

//
//
//
//
//
//
//
//

typedef uint32_t BfBladeSection2_Part;
enum BfBladeSection2_Part_ : BfBladeSection2_Part
{
   BfBladeSection2_Part_Average,
   BfBladeSection2_Part_AverageFrame,
   BfBladeSection2_Part_Cmax,
   BfBladeSection2_Part_Back,
   BfBladeSection2_Part_Front,
};

//
//
//
//
//
//
//

/**
 * @class BfBladeSection2
 * @brief Слой для создания сечения лопатки 2.0
 *
 */
class BfBladeSection2 : public BfDrawLayerAccessed<BfBladeSection2_Part>
{
   /*! Входное инфо для создания */
   BfBladeSectionCreateInfo2* m_info;

   /*! Хранение `id` элементов для их поиска */
   std::unordered_map<BfBladeSection2_Part, uint32_t> m_idMap;

public:
   /**
    * @brief Дефолтный конструктор
    * @param info Инфо как создать сечение
    */
   BfBladeSection2(BfBladeSectionCreateInfo2* info);

   //
   //
   //
   /** @brief Эквивалетный входной угол */
   float equivalentInletAngle();

   //
   //
   //
   /** @brief Эквивалетный выходной угол */
   float equivalentOutletAngle();

   //
   //
   //
   /** @brief Генерирует точки для 3D визуализации */
   virtual void createVertices();

private:
   void _createAverageCurve();
   void _createCmax();
   void _createInitialEdges();
   void _createBack();
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
//
//
//
//
//
//
//

struct BfBladeBaseCreateInfo
{
   BfDrawLayerCreateInfo layer_create_info{
       .id_type = BF_DRAW_LAYER_TYPE_BLADE_BASE
   };

   std::vector<BfBladeSectionCreateInfo> section_infos;
};

void bfFillBladeBaseStandart(BfBladeBaseCreateInfo* info);

class BfBladeBaseSurface;

class BfBladeBase : public BfDrawLayer
{
   BfBladeBaseCreateInfo __info;

public:
   BfBladeBase(const BfBladeBaseCreateInfo& info);

   // size_t add_section(const BfBladeSectionCreateInfo& info);
   // void del_section(size_t id);

   // std::vector<BfBladeSectionCreateInfo*> get_pInfos();
   const BfBladeBaseCreateInfo& get_info();
   std::shared_ptr<BfBladeBaseSurface> get_shape();
   std::shared_ptr<BfBladeBaseSurface> create_shape();

private:
};

class BfBladeBaseSurface : public BfDrawObj
{
   std::vector<std::shared_ptr<BfBladeSection>> __secs;
   size_t __slices_count;
   size_t __skip_vert;

public:
   BfBladeBaseSurface(
       const std::vector<std::shared_ptr<BfBladeSection>>& secs,
       size_t inner_sections_count,
       size_t skip_vert = 0
   );

   virtual void createVertices() override;
};

#endif
