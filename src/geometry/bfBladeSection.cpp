#include "bfBladeSection.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include <variant>

#include "bfCurves3.h"
#include "bfDrawObject.h"
#include "bfDrawObjectDefineType.h"
#include "bfLayerHandler.h"
#include "bfMath.hpp"
#include "bfPipeline.h"
#include "bfVertex2.hpp"

void
bfFillBladeSectionStandart(BfBladeSectionCreateInfo* info)
{
   *info = {
       .layer_create_info =
           {.allocator = BfLayerHandler::instance()
                             ? *BfLayerHandler::instance()->allocator()
                             : nullptr,
            .vertex_size = sizeof(BfVertex3),
            .max_vertex_count = 10000,
            .max_reserved_count = 1000,
            .is_nested = false,
            .id_type = BF_DRAW_LAYER_TYPE_BLADE_SECTION},

       .width = 1.0f,
       .install_angle = 102.0f,

       .inlet_angle = 25.0f,
       .outlet_angle = 42.0f,

       .inlet_surface_angle = 15.0f,
       .outlet_surface_angle = 15.0f,

       .inlet_radius = 0.025f,
       .outlet_radius = 0.005f,

       .border_length = 20.0f,

       .is_triangulate = false,
       .is_center = true,
       // clang-format off
       .l_pipeline = BfLayerHandler::instance()
                         ? *BfPipelineHandler::instance()->getPipeline(BfPipelineType_Lines)
                         : nullptr,
       .t_pipeline = BfLayerHandler::instance()
                         ? *BfPipelineHandler::instance()->getPipeline(BfPipelineType_Triangles)
                         : nullptr
       // clang-format on
   };
};

BfBladeSection::BfBladeSection(BfBladeSectionCreateInfo* info)
    : BfDrawLayer(info->layer_create_info), __info{info}
{
   __generate_outer_elements();
   __generate_average_line_geometry();
   __generate_surface_elements();
   __generate_obj_frames();

   if (info->is_center)
      __generate_center_circle();
   if (info->is_triangulate)
      __generate_triangular_shape();

   this->generate_draw_data();
}

void
BfBladeSection::remake(BfBladeSectionCreateInfo* info)
{
   __clear_draw_data();

   __info = info;

   __generate_outer_elements();
   __generate_average_line_geometry();
   __generate_surface_elements();
   __generate_obj_frames();

   if (info->is_center)
      __generate_center_circle();
   if (info->is_triangulate)
      __generate_triangular_shape();

   this->generate_draw_data();
}

const BfVertex3&
BfBladeSection::get_inlet_center() const noexcept
{
   return __chord_inlet_center;
}
const BfVertex3&
BfBladeSection::get_outlet_center() const noexcept
{
   return __chord_outlet_center;
}

std::shared_ptr< BfSingleLine >
BfBladeSection::get_top_border()
{
   return std::dynamic_pointer_cast< BfSingleLine >(this->get_object_by_index(0)
   );
}
std::shared_ptr< BfSingleLine >
BfBladeSection::get_bot_border()
{
   return std::dynamic_pointer_cast< BfSingleLine >(this->get_object_by_index(1)
   );
}
std::shared_ptr< BfSingleLine >
BfBladeSection::get_chord()
{
   return std::dynamic_pointer_cast< BfSingleLine >(this->get_object_by_index(2)
   );
}
std::shared_ptr< BfCircle >
BfBladeSection::get_inlet_edge()
{
   return std::dynamic_pointer_cast< BfCircle >(this->get_object_by_index(3));
}
std::shared_ptr< BfCircle >
BfBladeSection::get_outlet_edge()
{
   return std::dynamic_pointer_cast< BfCircle >(this->get_object_by_index(4));
}
std::shared_ptr< BfSingleLine >
BfBladeSection::get_inlet_vector()
{
   return std::dynamic_pointer_cast< BfSingleLine >(this->get_object_by_index(5)
   );
}
std::shared_ptr< BfSingleLine >
BfBladeSection::get_outlet_vector()
{
   return std::dynamic_pointer_cast< BfSingleLine >(this->get_object_by_index(6)
   );
}
std::shared_ptr< BfBezierCurve >
BfBladeSection::get_ave_curve()
{
   return std::dynamic_pointer_cast< BfBezierCurve >(
       this->get_object_by_index(7)
   );
}
std::shared_ptr< BfBezierCurve >
BfBladeSection::get_back_curve()
{
   return std::dynamic_pointer_cast< BfBezierCurve >(
       this->get_object_by_index(8)
   );
}
std::shared_ptr< BfBezierCurve >
BfBladeSection::get_face_curve()
{
   return std::dynamic_pointer_cast< BfBezierCurve >(
       this->get_object_by_index(9)
   );
}
std::shared_ptr< BfArc >
BfBladeSection::get_inlet_edge_a()
{
   return std::dynamic_pointer_cast< BfArc >(this->get_object_by_index(10));
}
std::shared_ptr< BfArc >
BfBladeSection::get_outlet_edge_a()
{
   return std::dynamic_pointer_cast< BfArc >(this->get_object_by_index(11));
}

std::shared_ptr< BfBezierCurveFrame >
BfBladeSection::get_ave_curve_frame()
{
   return std::dynamic_pointer_cast< BfBezierCurveFrame >(
       this->get_layer_by_index(0)
   );
}
std::shared_ptr< BfBezierCurveFrame >
BfBladeSection::get_back_curve_frame()
{
   return std::dynamic_pointer_cast< BfBezierCurveFrame >(
       this->get_layer_by_index(1)
   );
}
std::shared_ptr< BfBezierCurveFrame >
BfBladeSection::get_face_curve_frame()
{
   return std::dynamic_pointer_cast< BfBezierCurveFrame >(
       this->get_layer_by_index(2)
   );
}

std::vector< std::shared_ptr< BfTriangle > >
BfBladeSection::get_triangulated_shape()
{
   this->get_back_curve()->createVertices();
   this->get_face_curve()->createVertices();
   auto back = this->get_back_curve()->vertices();
   auto face = this->get_face_curve()->vertices();
   if (back.size() != face.size())
      abort();

   std::vector< std::shared_ptr< BfTriangle > > triangles;
   triangles.reserve(back.size() * 4);

   for (size_t i = 1; i < back.size() - 2; i++)
   {
      triangles.emplace_back(
          std::make_shared< BfTriangle >(back[i], face[i], face[i + 1])
      );
   }
   for (size_t i = 2; i < back.size() - 1; i++)
   {
      triangles.emplace_back(
          std::make_shared< BfTriangle >(face[i], back[i], back[i - 1])
      );
   }

   this->get_inlet_edge_a()->createVertices();
   auto iedge = this->get_inlet_edge_a()->vertices();
   auto icenter = this->get_inlet_edge_a()->get_center();
   for (size_t i = 0; i < iedge.size() - 1; i++)
   {
      triangles.emplace_back(
          std::make_shared< BfTriangle >(icenter, iedge[i], iedge[i + 1])
      );
   }

   this->get_outlet_edge_a()->createVertices();
   auto oedge = this->get_outlet_edge_a()->vertices();
   auto ocenter = this->get_outlet_edge_a()->get_center();
   for (size_t i = 0; i < iedge.size() - 1; i++)
   {
      triangles.emplace_back(
          std::make_shared< BfTriangle >(ocenter, oedge[i], oedge[i + 1])
      );
   }

   { // Inlet
      triangles.emplace_back(
          std::make_shared< BfTriangle >(icenter, face[0], face[1])
      );
      triangles.emplace_back(
          std::make_shared< BfTriangle >(icenter, back[0], back[1])
      );
      triangles.emplace_back(
          std::make_shared< BfTriangle >(icenter, back[1], face[1])
      );
   }

   { // Outlet
      triangles.emplace_back(
          std::make_shared< BfTriangle >(
              ocenter,
              face[back.size() - 1],
              face[back.size() - 2]
          )
      );
      triangles.emplace_back(
          std::make_shared< BfTriangle >(
              ocenter,
              back[back.size() - 1],
              back[back.size() - 2]
          )
      );
      triangles.emplace_back(
          std::make_shared< BfTriangle >(
              ocenter,
              back[back.size() - 2],
              face[back.size() - 2]
          )
      );
   }

   return triangles;
}

BfVertex3
BfBladeSection::get_center()
{
   auto t = this->get_triangulated_shape();

   float total_area = 0;
   glm::vec3 center(0.0f);

   for (auto& tr : t)
   {
      float area = tr->get_area();
      total_area += area;
      center += area * tr->get_center().pos;
   }

   return {
       center / total_area,
       {1.0f, 0.5f, 0.0f},
       this->__chord_inlet_center.normals
   };
}

void
BfBladeSection::del_all()
{
   std::vector< uint32_t > ids;
   ids.reserve(this->get_obj_count());

   for (size_t i = 0; i < this->get_obj_count(); i++)
   {
      ids.push_back(this->get_object_by_index(i)->id.get());
   }

   /*for (auto& o : __objects) {
           ids.push_back(o->id.get());
   }
   for (auto& l : __layers) {
           ids.push_back(l->id.get());
   }*/
   this->del(ids);
}

std::vector< BfVertex3 >
BfBladeSection::get_contour()
{
   if (!get_back_curve()->is_ok())
      get_back_curve()->createVertices();
   if (!get_inlet_edge_a()->is_ok())
      get_inlet_edge_a()->createVertices();
   if (!get_face_curve()->is_ok())
      get_face_curve()->createVertices();
   if (!get_outlet_edge_a()->is_ok())
      get_outlet_edge_a()->createVertices();

   auto vert = bfMathStickObjVertices(
       {std::dynamic_pointer_cast< BfDrawObj >(get_back_curve()),
        std::dynamic_pointer_cast< BfDrawObj >(get_inlet_edge_a()),
        std::dynamic_pointer_cast< BfDrawObj >(get_face_curve()),
        std::dynamic_pointer_cast< BfDrawObj >(get_outlet_edge_a())}
   );

   auto last = std::unique(
       vert.begin(),
       vert.end(),
       [](const BfVertex3& a, const BfVertex3& b) {
          return CHECK_FLOAT_EQUALITY(a.pos.x, b.pos.x) &&
                 CHECK_FLOAT_EQUALITY(a.pos.y, b.pos.y) &&
                 CHECK_FLOAT_EQUALITY(a.pos.z, b.pos.z);
       }
   );

   vert.erase(last, vert.end());
   return vert;
}

void
BfBladeSection::generate_draw_data()
{
   for (size_t i = 0; i < this->get_obj_count(); ++i)
   {
      auto obj = this->get_object_by_index(i);
      // obj->set_color({ 1.0f, 1.0f, 1.0f });
      obj->createVertices();
      obj->createIndices();
      obj->bind_pipeline(&__info->l_pipeline);
   }
   this->update_buffer();
}

std::vector< std::shared_ptr< BfDrawObj > >
BfBladeSection::get_shape_parts()
{
   return {
       this->get_inlet_edge_a(),
       this->get_back_curve(),
       this->get_outlet_edge_a(),
       this->get_face_curve()
   };
}

//
// const BfBladeSectionCreateInfo& BfBladeSection::get_info() {
//	return this->__info;
//}
//
// BfBladeSectionCreateInfo* BfBladeSection::get_pInfo() {
//	return &__info;
//}

void
BfBladeSection::__generate_outer_elements()
{
   __chord_inlet_center =
       {__info->start_vertex, {1.0f, 1.0f, 1.0f}, __info->up_direction};

   glm::vec3 left_border_dir =
       glm::normalize(glm::cross(__info->up_direction, __info->grow_direction));
   glm::vec3 right_border_dir = -left_border_dir;

   { // TOP BORDER
      auto top_border = std::make_shared< BfSingleLine >(
          BfVertex3{
              __chord_inlet_center.pos +
                  left_border_dir * __info->border_length / 2.0f,
              {1.0f, 1.0f, 1.0f},
              __info->up_direction
          },
          BfVertex3{
              __chord_inlet_center.pos +
                  right_border_dir * __info->border_length / 2.0f,
              {1.0f, 1.0f, 1.0f},
              __info->up_direction
          }
      );
      this->add_l(top_border);
   }

   { // BOT BORDER
      auto bot_border = std::make_shared< BfSingleLine >(
          BfVertex3{
              __chord_inlet_center.pos +
                  __info->grow_direction * __info->width +
                  left_border_dir * __info->border_length / 2.0f,
              {1.0f, 1.0f, 1.0f},
              __info->up_direction
          },
          BfVertex3{
              __chord_inlet_center.pos +
                  __info->grow_direction * __info->width +
                  right_border_dir * __info->border_length / 2.0f,
              {1.0f, 1.0f, 1.0f},
              __info->up_direction
          }
      );
      this->add_l(bot_border);
   }

   glm::vec4 chord_dir =
       (glm::rotate(
            glm::mat4(1.0f),
            glm::radians(90.0f - __info->install_angle),
            __info->up_direction
        ) *
        glm::vec4(__info->grow_direction, 1.0f));

   __chord_outlet_center = BfVertex3{
       __chord_inlet_center.pos +
           glm::vec3(chord_dir) * __info->width /
               glm::sin(glm::radians(__info->install_angle)),
       {0.5f, 0.5f, 1.0f},
       __info->up_direction
   };

   { // CHORD
      auto chord = std::make_shared< BfSingleLine >(
          __chord_inlet_center,
          __chord_outlet_center
      );
      this->add_l(chord);
   }
}

void
BfBladeSection::__generate_average_line_geometry()
{
   // INLET-OUTLET potencial intersection
   glm::vec3 _inlet_dir =
       (glm::rotate(
            glm::mat4(1.0f),
            glm::radians(90.0f + __info->inlet_angle),
            __info->up_direction
        ) *
        glm::vec4(__info->grow_direction, 1.0f));
   auto _inlet_vector = std::make_shared< BfSingleLine >(
       __chord_inlet_center,
       __chord_inlet_center.pos + _inlet_dir
   );

   glm::vec3 _outlet_dir =
       (glm::rotate(
            glm::mat4(1.0f),
            glm::radians(90.0f - __info->outlet_angle),
            __info->up_direction
        ) *
        glm::vec4(__info->grow_direction, 1.0f));
   auto _outlet_vector = std::make_shared< BfSingleLine >(
       __chord_outlet_center,
       __chord_outlet_center.pos + _outlet_dir
   );

   glm::vec3 potencial_io_intersection = bfMathFindLinesIntersection(
       *_inlet_vector.get(),
       *_outlet_vector.get(),
       BF_MATH_FIND_LINES_INTERSECTION_ANY
   );

   glm::vec3 section_center_line_vert_1 =
       (get_top_border()->get_first().pos + get_bot_border()->get_first().pos) /
       2.0f;

   glm::vec3 section_center_line_vert_2 = (get_top_border()->get_second().pos +
                                           get_bot_border()->get_second().pos) /
                                          2.0f;

   auto center_line = std::make_shared< BfSingleLine >(
       BfVertex3{
           section_center_line_vert_1,
           {1.0f, 1.0f, 1.0f},
           {get_inlet_center().normals}
       },
       BfVertex3{
           section_center_line_vert_2,
           {1.0f, 1.0f, 1.0f},
           {get_inlet_center().normals}
       }
   );

   { // INLET EDGE
      std::vector< BfCircle > c = bfMathGetInscribedCircles(
          BF_BEZIER_CURVE_VERT_COUNT,
          *get_top_border().get(),
          *get_chord().get(),
          __info->inlet_radius
      );

      std::vector< std::pair< BfCircle*, float > > distances_to_center_line;
      for (size_t i = 0; i < c.size(); i++)
      {
         distances_to_center_line.push_back(
             std::make_pair(
                 &c[i],
                 bfMathGetDistanceToLine(*center_line, c[i].get_center().pos)
             )
         );
      }

      std::sort(
          distances_to_center_line.begin(),
          distances_to_center_line.end(),
          [](const auto& a, const auto& b) { return a.second < b.second; }
      );

      distances_to_center_line.pop_back();
      distances_to_center_line.pop_back();

      float min_distance_to_intersection = FLT_MAX;
      BfCircle* circle = nullptr;

      for (auto& pair : distances_to_center_line)
      {
         float distance_to_intersection = glm::distance(
             potencial_io_intersection,
             pair.first->get_center().pos
         );

         if (distance_to_intersection < min_distance_to_intersection)
         {
            min_distance_to_intersection = distance_to_intersection;
            circle = pair.first;
         }
      }

      if (circle != nullptr)
      {
         circle->is_draw = false;
         this->add_l(std::make_shared< BfCircle >(*circle));
      }
      else
         abort();
   }

   { // OUTLET EDGE
      std::vector< BfCircle > c = bfMathGetInscribedCircles(
          BF_BEZIER_CURVE_VERT_COUNT,
          *get_bot_border().get(),
          *get_chord().get(),
          __info->outlet_radius
      );

      std::vector< std::pair< BfCircle*, float > > distances_to_center_line;
      for (size_t i = 0; i < c.size(); i++)
      {
         distances_to_center_line.push_back(
             std::make_pair(
                 &c[i],
                 bfMathGetDistanceToLine(*center_line, c[i].get_center().pos)
             )
         );
      }

      std::sort(
          distances_to_center_line.begin(),
          distances_to_center_line.end(),
          [](const auto& a, const auto& b) { return a.second < b.second; }
      );

      distances_to_center_line.pop_back();
      distances_to_center_line.pop_back();

      float min_distance_to_intersection = FLT_MAX;
      BfCircle* circle = nullptr;

      for (auto& pair : distances_to_center_line)
      {
         float distance_to_intersection = glm::distance(
             potencial_io_intersection,
             pair.first->get_center().pos
         );

         if (distance_to_intersection < min_distance_to_intersection)
         {
            min_distance_to_intersection = distance_to_intersection;
            circle = pair.first;
         }
      }

      if (circle != nullptr)
      {
         circle->is_draw = false;
         this->add_l(std::make_shared< BfCircle >(*circle));
      }
      else
         abort();
   }

   { // INLET VECTOR
      glm::vec3 inlet_dir =
          (glm::rotate(
               glm::mat4(1.0f),
               glm::radians(90.0f + __info->inlet_angle),
               __info->up_direction
           ) *
           glm::vec4(__info->grow_direction, 1.0f));
      auto inlet_vector = std::make_shared< BfSingleLine >(
          this->get_inlet_edge()->get_center(),
          this->get_inlet_edge()->get_center().pos + glm::normalize(inlet_dir)
      );
      inlet_vector->createVertices();
      this->add_l(inlet_vector);
   }

   { // OUTLET VECTOR
      glm::vec3 outlet_dir =
          (glm::rotate(
               glm::mat4(1.0f),
               glm::radians(90.0f - __info->outlet_angle),
               __info->up_direction
           )
           //*glm::vec4(this->get_outlet_edge()->get_center().pos +
           //__info->grow_direction, 1.0f)
           * glm::vec4(__info->grow_direction, 1.0f));
      auto outlet_vector = std::make_shared< BfSingleLine >(
          this->get_outlet_edge()->get_center(),
          this->get_outlet_edge()->get_center().pos + glm::normalize(outlet_dir)
      );
      outlet_vector->createVertices();
      this->add_l(outlet_vector);
   }

   glm::vec3 io_intersection = bfMathFindLinesIntersection(
       *this->get_inlet_vector().get(),
       *this->get_outlet_vector().get(),
       BF_MATH_FIND_LINES_INTERSECTION_ANY
   );

   //{ // INLET IO-LINE
   //	auto inlet_io = std::make_shared<BfSingleLine>(
   //		this->get_inlet_edge()->get_center(),
   //		BfVertex3{
   //			io_intersection,
   //			{1.0f, 1.0f, 1.0f},
   //			this->get_inlet_edge()->get_center().normals
   //		}
   //	);
   //
   //	this->add_l(inlet_io);
   //}

   //{ // OUTLET IO-LINE
   //	auto outlet_io = std::make_shared<BfSingleLine>(
   //		BfVertex3{
   //			io_intersection,
   //			{1.0f, 1.0f, 1.0f},
   //			this->get_outlet_edge()->get_center().normals
   //		},
   //		this->get_outlet_edge()->get_center()
   //	);
   //	this->add_l(outlet_io);
   //}

   { // AVE_CURVE
      auto ave_curve = std::make_shared< BfBezierCurve >(
          2,
          BF_BEZIER_CURVE_VERT_COUNT,
          std::vector< BfVertex3 >{
              this->get_inlet_edge()->get_center(),
              BfVertex3(
                  io_intersection,
                  {1.0f, 1.0f, 1.0f},
                  this->get_inlet_center().normals
              ),
              this->get_outlet_edge()->get_center()
          }
      );
      ave_curve->set_color(BF_BLADESECTION_AVE_COLOR);
      this->add_l(ave_curve);
   }

   // this->add_l(center_line);

   //{ // BACK
   //	glm::vec3 back_intersection = io_intersection +
   // this->get_top_border()->get_direction_from_start() * 0.4f;
   //	// INLET
   //
   //	auto back_inlet_tangents =
   // this->get_inlet_edge()->get_tangent_vert(back_intersection); 	auto
   // back_inlet_io_1 =
   // std::make_shared<BfSingleLine>(back_inlet_tangents[0],
   //														  back_intersection);
   //

   //	auto back_inlet_io_2 =
   // std::make_shared<BfSingleLine>(back_inlet_tangents[1],
   // back_intersection);
   //

   //	BfVertex3 back_inlet_tangent;
   //	if (bfMathGetDistanceToLine(*get_top_border().get(),
   // back_inlet_tangents[0]) <
   // bfMathGetDistanceToLine(*get_top_border().get(),
   // back_inlet_tangents[1]))
   //	{
   //		back_inlet_tangent = back_inlet_tangents[0];
   //		this->add_l(back_inlet_io_1);
   //	}
   //	else
   //	{
   //		back_inlet_tangent = back_inlet_tangents[1];
   //		this->add_l(back_inlet_io_2);
   //	}

   //	// OUTLET

   //	auto back_outlet_tangents =
   // this->get_outlet_edge()->get_tangent_vert(back_intersection); 	auto
   // back_outlet_io_1 =
   // std::make_shared<BfSingleLine>(back_outlet_tangents[0],
   //														   back_intersection);
   //
   //	auto back_outlet_io_2 =
   // std::make_shared<BfSingleLine>(back_outlet_tangents[1],
   // back_intersection);
   //

   //	BfVertex3 back_outlet_tangent;
   //	if (bfMathGetDistanceToLine(*get_bot_border().get(),
   // back_outlet_tangents[0]) <
   // bfMathGetDistanceToLine(*get_bot_border().get(),
   // back_outlet_tangents[1]))
   //	{
   //		back_outlet_tangent = back_outlet_tangents[0];
   //		this->add_l(back_outlet_io_1);
   //	}
   //	else
   //	{
   //		back_outlet_tangent = back_outlet_tangents[1];
   //		this->add_l(back_outlet_io_2);
   //	}

   //	auto back_curve = std::make_shared<BfBezierCurve>(
   //		2,
   //		100,
   //		std::vector<BfVertex3>{
   //			back_inlet_tangent,
   //			BfVertex3(back_intersection, { 1.0f, 1.0f, 1.0f
   //},
   // this->get_inlet_center().normals), back_outlet_tangent
   //		}
   //	);
   //	back_curve->set_color({ .0f, 1.0f, 1.0f });
   //	this->add_l(back_curve);
   //}

   //{ // FACE
   //	glm::vec3 face_intersection = io_intersection -
   // this->get_top_border()->get_direction_from_start() * 0.4f;
   //	// INLET

   //	auto face_inlet_tangents =
   // this->get_inlet_edge()->get_tangent_vert(face_intersection); 	auto
   // face_inlet_io_1 =
   // std::make_shared<BfSingleLine>(face_inlet_tangents[0],
   //														  face_intersection);

   //	auto face_inlet_io_2 =
   // std::make_shared<BfSingleLine>(face_inlet_tangents[1],
   // face_intersection);

   //	BfVertex3 face_inlet_tangent;
   //	if (bfMathGetDistanceToLine(*get_top_border().get(),
   // face_inlet_tangents[0]) >
   // bfMathGetDistanceToLine(*get_top_border().get(),
   // face_inlet_tangents[1]))
   //	{
   //		face_inlet_tangent = face_inlet_tangents[0];
   //		this->add_l(face_inlet_io_1);
   //	}
   //	else
   //	{
   //		face_inlet_tangent = face_inlet_tangents[1];
   //		this->add_l(face_inlet_io_2);
   //	}

   //	// OUTLET

   //	auto face_outlet_tangents =
   // this->get_outlet_edge()->get_tangent_vert(face_intersection); 	auto
   // face_outlet_io_1 =
   // std::make_shared<BfSingleLine>(face_outlet_tangents[0],
   //														   face_intersection);

   //	auto face_outlet_io_2 =
   // std::make_shared<BfSingleLine>(face_outlet_tangents[1],
   // face_intersection);

   //	BfVertex3 face_outlet_tangent;
   //	if (bfMathGetDistanceToLine(*get_bot_border().get(),
   // face_outlet_tangents[0]) >
   // bfMathGetDistanceToLine(*get_bot_border().get(),
   // face_outlet_tangents[1]))
   //	{
   //		face_outlet_tangent = face_outlet_tangents[0];
   //		this->add_l(face_outlet_io_1);
   //	}
   //	else
   //	{
   //		face_outlet_tangent = face_outlet_tangents[1];
   //		this->add_l(face_outlet_io_2);
   //	}

   //	auto face_curve = std::make_shared<BfBezierCurve>(
   //		2,
   //		100,
   //		std::vector<BfVertex3>{
   //		face_inlet_tangent,
   //			BfVertex3(face_intersection, { 1.0f, 1.0f, 1.0f
   //},
   // this->get_inlet_center().normals), 		face_outlet_tangent
   //	}
   //	);
   //	face_curve->set_color({ .0f, 1.0f, 1.0f });
   //	this->add_l(face_curve);
   //}
}

void
BfBladeSection::__generate_surface_elements()
{
   std::array< BfVertex3, 2 > inlet_surface_tangents;
   glm::vec3 inlet_surface_vert;
   { // INLET DEFINE SURFACE VERTEX
      glm::vec3 dir = get_inlet_vector()->get_direction_from_start();
      inlet_surface_vert =
          get_inlet_edge()->get_center().pos +
          dir * __info->inlet_radius /
              glm::sin(glm::radians(__info->inlet_surface_angle / 2.0f));

      inlet_surface_tangents =
          get_inlet_edge()->get_tangent_vert(inlet_surface_vert);

      /*

      this->add_l(t_1);
      this->add_l(t_2);*/
   }
   auto inlet_t_1 = std::make_shared< BfSingleLine >(
       BfVertex3{
           inlet_surface_vert,
           {1.0f, 0.0f, 0.2f},
           get_inlet_center().normals
       },
       BfVertex3{
           inlet_surface_tangents[0].pos,
           {1.0f, 0.0f, 0.2f},
           get_inlet_center().normals
       }
   );
   auto inlet_t_2 = std::make_shared< BfSingleLine >(
       BfVertex3{
           inlet_surface_vert,
           {1.0f, 0.0f, 0.2f},
           get_inlet_center().normals
       },
       BfVertex3{
           inlet_surface_tangents[1].pos,
           {1.0f, 0.0f, 0.2f},
           get_inlet_center().normals
       }
   );

   std::array< BfVertex3, 2 > outlet_surface_tangents;
   glm::vec3 outlet_surface_vert;
   { // OUTLET DEFINE SURFACE VERTEX
      glm::vec3 dir = get_outlet_vector()->get_direction_from_start();
      outlet_surface_vert =
          get_outlet_edge()->get_center().pos +
          dir * __info->outlet_radius /
              glm::sin(glm::radians(__info->outlet_surface_angle / 2.0f));

      outlet_surface_tangents =
          get_outlet_edge()->get_tangent_vert(outlet_surface_vert);

      /*
      this->add_l(t_1);
      this->add_l(t_2);*/
   }

   auto outlet_t_1 = std::make_shared< BfSingleLine >(
       BfVertex3{
           outlet_surface_vert,
           {1.0f, 0.0f, 0.2f},
           get_outlet_center().normals
       },
       BfVertex3{
           outlet_surface_tangents[0].pos,
           {1.0f, 0.0f, 0.2f},
           get_outlet_center().normals
       }
   );
   auto outlet_t_2 = std::make_shared< BfSingleLine >(
       BfVertex3{
           outlet_surface_vert,
           {1.0f, 0.0f, 0.2f},
           get_outlet_center().normals
       },
       BfVertex3{
           outlet_surface_tangents[1].pos,
           {1.0f, 0.0f, 0.2f},
           get_outlet_center().normals
       }
   );

   std::vector< std::pair<
       std::shared_ptr< BfSingleLine >,
       std::shared_ptr< BfSingleLine > > >
       line_com{
           {inlet_t_1, outlet_t_1},
           {inlet_t_1, outlet_t_2},
           {inlet_t_2, outlet_t_1},
           {inlet_t_2, outlet_t_2},
       };

   std::vector< glm::vec3 > intersections;
   intersections.reserve(line_com.size());

   for (auto& pair : line_com)
   {
      glm::vec3 inter = bfMathFindLinesIntersection(
          *pair.first,
          *pair.second,
          BF_MATH_FIND_LINES_INTERSECTION_ANY
      );
      intersections.emplace_back(inter);
   }

   std::vector< std::pair< size_t, std::pair< float, float > > > distances;
   distances.reserve(line_com.size());

   for (size_t i = 0; i < intersections.size(); i++)
   {
      distances.emplace_back(
          std::pair(
              i,
              std::pair(
                  glm::distance(
                      this->get_inlet_edge()->get_center().pos,
                      intersections[i]
                  ),
                  glm::distance(
                      this->get_outlet_edge()->get_center().pos,
                      intersections[i]
                  )
              )
          )
      );
   }

   std::sort(
       distances.begin(),
       distances.end(),
       [](const auto& a, const auto& b) {
          float sum_a = a.second.first + a.second.second;
          float sum_b = b.second.first + b.second.second;
          return sum_a < sum_b;
       }
   );

   size_t back_index = distances.rbegin()->first;
   auto back_curve = std::make_shared< BfBezierCurve >(
       2,
       BF_BEZIER_CURVE_VERT_COUNT,
       std::vector< BfVertex3 >{
           line_com[back_index].first->get_second(),
           BfVertex3(
               intersections[back_index],
               {1.0f, 1.0f, 1.0f},
               this->get_inlet_edge()->get_center().normals
           ),
           line_com[back_index].second->get_second(),
       }
   );
   back_curve->set_color(BF_BLADESECTION_BACK_COLOR);
   this->add_l(back_curve);

   size_t face_index = distances.begin()->first;
   auto face_curve = std::make_shared< BfBezierCurve >(
       2,
       BF_BEZIER_CURVE_VERT_COUNT,
       std::vector< BfVertex3 >{
           line_com[face_index].first->get_second(),
           BfVertex3(
               intersections[face_index],
               {1.0f, 1.0f, 1.0f},
               this->get_inlet_edge()->get_center().normals
           ),
           line_com[face_index].second->get_second(),
       }
   );
   face_curve->set_color(BF_BLADESECTION_FACE_COLOR);
   this->add_l(face_curve);

   { // INLET_EDGE_A
      auto edge = std::make_shared< BfArc >(
          BF_BEZIER_CURVE_VERT_COUNT,
          line_com[back_index].first->get_second(),
          BfVertex3(
              get_inlet_edge()->get_center().pos +
                  get_inlet_vector()->get_direction_from_start() *
                      __info->inlet_radius,
              {1.0f, 1.0f, 1.0f},
              get_inlet_edge()->get_center().normals
          ),
          line_com[face_index].first->get_second()
      );
      this->add_l(edge);
   }

   { // OUTLET_EDGE_A
      auto edge = std::make_shared< BfArc >(
          BF_BEZIER_CURVE_VERT_COUNT,
          line_com[back_index].second->get_second(),
          BfVertex3(
              get_outlet_edge()->get_center().pos +
                  get_outlet_vector()->get_direction_from_start() *
                      __info->outlet_radius,
              {1.0f, 1.0f, 1.0f},
              get_inlet_edge()->get_center().normals
          ),
          line_com[face_index].second->get_second()
      );
      this->add_l(edge);
   }
}

void
BfBladeSection::__generate_obj_frames()
{
   if (get_ave_curve_frame() == nullptr)
   {
      // AVE
      auto ave_curve_frame = std::make_shared< BfBezierCurveFrame >(
          this->get_ave_curve(),
          __info->layer_create_info.allocator,
          __info->l_pipeline,
          __info->t_pipeline
      );
      ave_curve_frame->set_color(BF_BLADESECTION_AVE_COLOR);
      ave_curve_frame->update_buffer();
      this->add(ave_curve_frame);

      // BACK
      auto back_curve_frame = std::make_shared< BfBezierCurveFrame >(
          this->get_back_curve(),
          __info->layer_create_info.allocator,
          __info->l_pipeline,
          __info->t_pipeline
      );
      back_curve_frame->set_color(BF_BLADESECTION_BACK_COLOR);
      back_curve_frame->update_buffer();
      this->add(back_curve_frame);

      // FACe
      auto face_curve_frame = std::make_shared< BfBezierCurveFrame >(
          this->get_face_curve(),
          __info->layer_create_info.allocator,
          __info->l_pipeline,
          __info->t_pipeline
      );
      face_curve_frame->set_color(BF_BLADESECTION_FACE_COLOR);
      face_curve_frame->update_buffer();
      this->add(face_curve_frame);
   }
   else
   {
      get_ave_curve_frame()->remake(get_ave_curve(), BF_BLADESECTION_AVE_COLOR);
      get_back_curve_frame()->remake(
          get_back_curve(),
          BF_BLADESECTION_BACK_COLOR
      );
      get_face_curve_frame()->remake(
          get_face_curve(),
          BF_BLADESECTION_FACE_COLOR
      );
   }
}
void
BfBladeSection::__generate_center_circle()
{
   BfVertex3 center = this->get_center();

   auto c_circle = std::make_shared< BfCircle >(20, center, 0.02f);

   this->add_l(c_circle);
}

void
BfBladeSection::__generate_triangular_shape()
{
   auto t = this->get_triangulated_shape();
   for (auto& it : t)
   {
      this->add_l(it);
   }
}

void
BfBladeSection::__clear_draw_data()
{
   this->del_all();
}

void
bfFillBladeBaseStandart(BfBladeBaseCreateInfo* info)
{
   *info = {
       .layer_create_info =
           {.allocator = BfLayerHandler::instance()
                             ? *BfLayerHandler::instance()->allocator()
                             : nullptr,
            .vertex_size = sizeof(BfVertex3),
            .max_vertex_count = 10000,
            .max_reserved_count = 1000,
            .is_nested = false,
            .id_type = BF_DRAW_LAYER_TYPE_BLADE_BASE},

       // .l_pipeline = *BfLayerHandler::instance()->line_pipeline(),
       // .t_pipeline = *BfLayerHandler::instance()->trinagle_pipeline()
   };
}

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

void
bfFillBladeSectionStandart2(BfBladeSectionCreateInfo2* info)
{
   *info = {
       .layer_create_info =
           {.allocator = BfLayerHandler::instance()
                             ? *BfLayerHandler::instance()->allocator()
                             : nullptr,
            .vertex_size = sizeof(BfVertex3),
            .max_vertex_count = 10000,
            .max_reserved_count = 1000,
            .is_nested = false,
            .id_type = BF_DRAW_LAYER_TYPE_BLADE_SECTION},

       .chord = 0.3f,
       .installAngle = 50.0f,
       .inletAngle = 25.0f,
       .outletAngle = 42.0f,
       .inletEdgeRadius = 0.025,
       .outletEdgeRadius = 0.005,
       .inletEdgeAngle = 15.0f,
       .outletEdgeAngle = 10.0f,

       .cmax = {{0.04f, 0.25f}, {0.025f, 0.6}},

       // clang-format off
       .l_pipeline = BfLayerHandler::instance()
                         ? *BfPipelineHandler::instance()->getPipeline(BfPipelineType_Lines)
                         : nullptr,
       .t_pipeline = BfLayerHandler::instance()
                         ? *BfPipelineHandler::instance()->getPipeline(BfPipelineType_Triangles)
                         : nullptr
       // clang-format on
   };
}

BfBladeSection2::BfBladeSection2(BfBladeSectionCreateInfo2* info)
    : m_info{info}
{
   _createAverageCurve();
   _createCmax();
   _createInitialEdges();
   _createCmaxLines();
   _createCircleSkeleton();
   // _createOutShape();
   createVertices();
}

float
BfBladeSection2::equivalentInletAngle()
{
   return 90.0f - m_info->inletAngle;
}
float
BfBladeSection2::equivalentOutletAngle()
{
   return 90.0f - m_info->outletAngle;
}

void
BfBladeSection2::createVertices()
{
   for (size_t i = 0; i < this->get_obj_count(); ++i)
   {
      auto obj = this->get_object_by_index(i);
      obj->createVertices();
      obj->createIndices();
      obj->bind_pipeline(&m_info->l_pipeline);
   }
   this->update_buffer();
}

void
BfBladeSection2::remake()
{ // clang-format off
   { // CMAX
      auto aveCurve = _part<BfBezierCurveWithHandles, BfBladeSection2_Part_Average>()->curve();
      auto cmaxs = _part<BfDrawLayer, BfBladeSection2_Part_Cmax>();
      bfAssert(cmaxs->get_obj_count() == m_info->cmax.size());
      // NOTE: Если количество точек увеличится то это надо будет сделать тут
      for (size_t i = 0; i < m_info->cmax.size(); ++i)
      {
         m_cmaxCenterVertices.at(i) = aveCurve->calcBfV3(m_info->cmax[i].relativeCoordinate);
      }
   }

   { // CMAX LINES
      auto circles = this->circles(); 
      auto lines_layer = _part<BfDrawLayer, BfBladeSection2_Part_CmaxLines_Skeleton>();
      for (size_t i = 0; i < circles.size() - 1; ++i) { 
         std::array<glm::vec3, 4> tangent = bfMathFindTangentLines(
            *circles[i], 
            *circles[i + 1]
         );
         auto line_1 = lines_layer->get_object_by_index(2 * i);
         line_1->dVertices()[0] = tangent[0];
         line_1->dVertices()[1] = tangent[1];

         auto line_2 = lines_layer->get_object_by_index(2 * i + 1);
         line_2->dVertices()[0] = tangent[2];
         line_2->dVertices()[1] = tangent[3];
      }
   }

   {  // TANGENTS & NORMALS
      auto ave_curve = _part<BfBezierCurveWithHandles, BfBladeSection2_Part_Average>()->curve();
      
      // BfBladeSectionCmax
      auto nlines_layer_front = _part<BfDrawLayer, BfBladeSection2_Part_CmaxLines_Normals_Front>();
      auto nlines_layer_back = _part<BfDrawLayer, BfBladeSection2_Part_CmaxLines_Normals_Back>();
      auto tlines_layer_front = _part<BfDrawLayer, BfBladeSection2_Part_CmaxLines_Tangets_Front>();
      auto tlines_layer_back = _part<BfDrawLayer, BfBladeSection2_Part_CmaxLines_Tangets_Back>();
         
      std::vector<BfBladeSectionCmax> cmaxes = m_info->cmax;
      cmaxes.insert(cmaxes.begin(), { m_info->inletEdgeRadius, 0.f });
      cmaxes.insert(cmaxes.end(), { m_info->outletEdgeRadius, 1.f });

      int i = 0;
      for (auto cmax : cmaxes) { 
         auto bezier_vert = ave_curve->calcBfV3(cmax.relativeCoordinate);
         std::array<BfVertex3, 2> normals;
         bfMathFindBezierNormals(ave_curve.get(), cmax.relativeCoordinate, cmax.radius, normals);

         auto old_normal_1 = std::dynamic_pointer_cast<BfSingleLine>(
            nlines_layer_front->get_object_by_index(i)
         );
         old_normal_1->dVertices().at(0) = bezier_vert;
         old_normal_1->dVertices().at(1) = normals[0];
         
         auto old_normal_2 = std::dynamic_pointer_cast<BfSingleLine>(
            nlines_layer_back->get_object_by_index(i)
         );
         old_normal_2->dVertices().at(0) = bezier_vert;
         old_normal_2->dVertices().at(1) = normals[1];


         auto bline_direction_1 = bezier_vert.pos - normals[0].pos;
         auto bline_direction_2 = bezier_vert.pos - normals[1].pos;

         auto tline_direction_1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.f), bezier_vert.normals) * 
                                  glm::vec4(bline_direction_1, 1.f);
         auto tline_direction_2 = glm::rotate(glm::mat4(1.0f), glm::radians(90.f), bezier_vert.normals) * 
                                  glm::vec4(bline_direction_2, 1.f);
         
         auto old_tangent_1 = std::dynamic_pointer_cast<BfSingleLine>(
            tlines_layer_front->get_object_by_index(i)
         );
         old_tangent_1->dVertices().at(0) =  glm::vec3(tline_direction_1) + normals[0].pos;
         old_tangent_1->dVertices().at(1) = glm::vec3(-tline_direction_1) + normals[0].pos;
         
         auto old_tangent_2 = std::dynamic_pointer_cast<BfSingleLine>(
            tlines_layer_back->get_object_by_index(i)
         );
         old_tangent_2->dVertices().at(0) =  glm::vec3(tline_direction_2) + normals[1].pos;
         old_tangent_2->dVertices().at(1) = -glm::vec3(tline_direction_2) + normals[1].pos;

         i++;
      }

   }
   {  // OUTSHAPE BEZIER PARTS
      auto remakeBezierParts = [this](pLay tangent_l, pLay bezier_l) {
         for (size_t i = 0; i < tangent_l->get_obj_count() - 1; ++i)
         {
            auto start = std::dynamic_pointer_cast<BfSingleLine>(tangent_l->get_object_by_index(i));
            auto stop = std::dynamic_pointer_cast<BfSingleLine>(tangent_l->get_object_by_index(i + 1));
            auto bezier = std::dynamic_pointer_cast<BfBezierCurve>(bezier_l->get_object_by_index(i));
            if (!bfMathCreateBezier2From2Lines(*start, *stop, *bezier))
            {
               abort();
            }
         }
      };
      remakeBezierParts(_part<BfDrawLayer, BfBladeSection2_Part_CmaxLines_Tangets_Back>(),
                        _part<BfDrawLayer, BfBladeSection2_Part_Back>());

      remakeBezierParts(_part<BfDrawLayer, BfBladeSection2_Part_CmaxLines_Tangets_Front>(),
                        _part<BfDrawLayer, BfBladeSection2_Part_Front>());
   }

   BfDrawLayer::remake();
} // clang-format on

std::vector< std::shared_ptr< BfCircle > >
BfBladeSection2::circles()
{
   std::vector< std::shared_ptr< BfCircle > > circles;
   circles.push_back(
       _part< BfCircle, BfBladeSection2_Part_InitialInletEdge >()
   );
   auto cmax_layer = _part< BfDrawLayer, BfBladeSection2_Part_Cmax >();
   for (size_t i = 0; i < cmax_layer->get_obj_count(); ++i)
   {
      circles.push_back(
          std::dynamic_pointer_cast< BfCircle >(
              cmax_layer->get_object_by_index(i)
          )
      );
   }
   circles.push_back(
       _part< BfCircle, BfBladeSection2_Part_InitialOutletEdge >()
   );
   return circles;
}

void
BfBladeSection2::_createAverageCurve()
{
   glm::vec3 inletP = {0, 0, 0};
   glm::vec3 outletP = {m_info->chord, 0, 0};

   glm::vec4 inletDirection = glm::rotate(
                                  glm::mat4(1.0f),
                                  glm::radians(equivalentInletAngle()),
                                  glm::vec3(0.0f, 0.0, 1.0f)
                              ) *
                              glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

   glm::vec4 outletDirection = glm::rotate(
                                   glm::mat4(1.0f),
                                   glm::radians(-equivalentOutletAngle()),
                                   glm::vec3(0.0f, 0.0, 1.0f)
                               ) *
                               glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

   glm::vec3 intersectionP = bfMathFindLinesIntersection(
       BfSingleLine(inletP, inletP + glm::vec3(inletDirection)),
       BfSingleLine(outletP, outletP + glm::vec3(outletDirection)),
       BF_MATH_FIND_LINES_INTERSECTION_ANY
   );

   _addPartForward< BfBezierCurveWithHandles, BfBladeSection2_Part_Average >(
       BF_BEZIER_CURVE_VERT_COUNT,
       std::vector< BfVertex3 >{
           BfVertex3{inletP, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
           BfVertex3{intersectionP, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
           BfVertex3{outletP, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}
       }
   );
}

void
BfBladeSection2::_createCmax()
{ // clang-format off
   auto aveCurve = _part<BfBezierCurveWithHandles, BfBladeSection2_Part_Average>()->curve();
   auto cmaxLayer = std::make_shared<BfDrawLayer>(
      BfDrawLayerCreateInfo{.is_nested = true}
   );

   for (const auto &cmax : m_info->cmax)
   {
      auto v = m_cmaxCenterVertices.insert(
         m_cmaxCenterVertices.end(), 
         aveCurve->calcBfV3(cmax.relativeCoordinate)
      );

      // TODO: Make defines for default number of vertices
      auto cmax_obj = std::make_shared<BfCircleFilledWithHandles>(
         100, 
         &(*v), cmax.radius
      );

      // BFBS2_LOG(
      //     "Added cmax with aveCoo '" << BFVEC3_STR(m_cmaxCenterVertices.rbegin()->pos) << "' radius '"
      //                                << cmax.radius << "'"
      // );
      cmax_obj->set_color(BF_BLADESECTION_CMAX_COLOR);
      cmax_obj->createVertices();
      // cmax_obj->createIndices();
      // cmax_obj->bind_pipeline(&m_info->l_pipeline);
      cmaxLayer->add(cmax_obj);
   }
   _addPart(cmaxLayer, BfBladeSection2_Part_Cmax);
} // clang-format on

void
BfBladeSection2::_createInitialEdges()
{ // clang-format off
   auto ave_curve = _part<BfBezierCurveWithHandles, BfBladeSection2_Part_Average>()
   ->curve();

   auto inlet_edge = _addPartForward<BfCircle, BfBladeSection2_Part_InitialInletEdge>(
       200,
       &(*ave_curve->dVertices().begin()),
       m_info->inletEdgeRadius
   );
   inlet_edge->bind_pipeline(&m_info->l_pipeline);
   auto outlet_edge = _addPartForward<BfCircle, BfBladeSection2_Part_InitialOutletEdge>(
       200,
       &(*ave_curve->dVertices().rbegin()),
       m_info->outletEdgeRadius
   );
   outlet_edge->bind_pipeline(&m_info->l_pipeline);

}; //clang-format on


void BfBladeSection2::_createCircleSkeleton() { 
auto circles = this->circles(); 
   
   auto lines_layer = std::make_shared<BfDrawLayer>(
      BfDrawLayerCreateInfo{.is_nested = true}
   );

   for (size_t i = 0; i < circles.size() - 1; ++i) { 
      std::array<glm::vec3, 4> tangent = bfMathFindTangentLines(
         *circles[i], 
         *circles[i + 1]
      );
      // clang-format off
      auto line_1 = std::make_shared<BfSingleLine>(tangent[0], tangent[1]);
      line_1->bind_pipeline(&m_info->l_pipeline);
      line_1->createVertices();
      line_1->createIndices();
      line_1->set_color({0.8, 0.1, 0.2});

      auto line_2 = std::make_shared<BfSingleLine>(tangent[2], tangent[3]);
      line_2->bind_pipeline(&m_info->l_pipeline);
      line_2->createVertices();
      line_2->createIndices();
      line_2->set_color({0.8, 0.1, 0.2});

      // clang-format on
      lines_layer->add_l(line_1);
      lines_layer->add_l(line_2);
   }

   _addPart(lines_layer, BfBladeSection2_Part_CmaxLines_Skeleton);
}

void
BfBladeSection2::_createCmaxLines()
{ // clang-format off
   // INLET
   auto ave_curve = _part<BfBezierCurveWithHandles, BfBladeSection2_Part_Average>()->curve();
   
   auto nlines_layer_front = std::make_shared<BfDrawLayer>(BfDrawLayerCreateInfo{.is_nested = true}); 
   auto nlines_layer_back = std::make_shared<BfDrawLayer>(BfDrawLayerCreateInfo{.is_nested = true}); 

   auto tlines_layer_front = std::make_shared<BfDrawLayer>(BfDrawLayerCreateInfo{.is_nested = true}); 
   auto tlines_layer_back = std::make_shared<BfDrawLayer>(BfDrawLayerCreateInfo{.is_nested = true}); 
      
   std::vector<BfBladeSectionCmax> cmaxes = m_info->cmax;
   cmaxes.insert(cmaxes.begin(), { m_info->inletEdgeRadius, 0.f });
   cmaxes.insert(cmaxes.end(), { m_info->outletEdgeRadius, 1.f });

   for (auto cmax : cmaxes) { 
      auto bezier_vert = ave_curve->calcBfV3(cmax.relativeCoordinate);
      std::array<BfVertex3, 2> normals;
      bfMathFindBezierNormals(ave_curve.get(), cmax.relativeCoordinate, cmax.radius, normals);
      
      auto nline_1 = std::make_shared<BfSingleLine>( bezier_vert, normals[0]);
      nline_1->bind_pipeline(&m_info->l_pipeline);
      nline_1->set_color({1.0f, 0.0f, 0.0f});
      nline_1->createVertices();
      nline_1->createIndices();
      nlines_layer_front->add_l(nline_1);

      auto nline_2 = std::make_shared<BfSingleLine>(bezier_vert, normals[1]);
      nline_2->bind_pipeline(&m_info->l_pipeline);
      nline_2->set_color({0.0f, 1.f, 0.0f});
      nline_2->createVertices();
      nline_2->createIndices();
      nlines_layer_back->add_l(nline_2);

      auto bline_direction_1 = bezier_vert.pos - normals[0].pos;
      auto bline_direction_2 = bezier_vert.pos - normals[1].pos;
   
      auto tline_direction_1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.f), bezier_vert.normals) * 
                               glm::vec4(bline_direction_1, 1.f);
      auto tline_direction_2 = glm::rotate(glm::mat4(1.0f), glm::radians(90.f), bezier_vert.normals) * 
                               glm::vec4(bline_direction_2, 1.f);
      
      // auto tline_1 = std::make_shared<BfSingleLine>(tline_direction_1.xyz() * normals[0].pos + 1.f, 
      //                                               tline_direction_1.xyz() * normals[0].pos - 1.f);
      auto tline_1 = std::make_shared<BfSingleLine>( glm::vec3(tline_direction_1) + normals[0].pos, 
                                                    -glm::vec3(tline_direction_1) + normals[0].pos);
      // auto tline_1 = std::make_shared<BfSingleLine>(normals[0].pos, 
      //                                               -tline_direction_1.xyz() + normals[0].pos);
      tline_1->bind_pipeline(&m_info->l_pipeline);
      tline_1->set_color({1.f, 0.5f, 0.3f});
      tline_1->createVertices();
      tline_1->createIndices();
      tlines_layer_front->add_l(tline_1);

      auto tline_2 = std::make_shared<BfSingleLine>( glm::vec3(tline_direction_2) + normals[1].pos,
                                                    -glm::vec3(tline_direction_2) + normals[1].pos);
      tline_2->bind_pipeline(&m_info->l_pipeline);
      tline_2->set_color({0.0f, 1.0f, 0.0f});
      tline_2->createVertices();
      tline_2->createIndices();
      tlines_layer_back->add_l(tline_2);
   }
   _addPart(nlines_layer_front, BfBladeSection2_Part_CmaxLines_Normals_Front);
   _addPart(nlines_layer_back, BfBladeSection2_Part_CmaxLines_Normals_Back);
   _addPart(tlines_layer_front, BfBladeSection2_Part_CmaxLines_Tangets_Front);
   _addPart(tlines_layer_back, BfBladeSection2_Part_CmaxLines_Tangets_Back);
} // clang-format on

void
BfBladeSection2::_createOutShape()
{ // clang-format off
   auto createBezierParts = [this](std::shared_ptr<BfDrawLayer> tangent_l) { 
      auto bezier_l = std::make_shared<BfDrawLayer>(BfDrawLayerCreateInfo{.is_nested = true});
      for (size_t i = 0; i < tangent_l->get_obj_count() - 1; ++i) 
      { 
         auto start = std::dynamic_pointer_cast<BfSingleLine>(tangent_l->get_object_by_index(i));
         auto stop  = std::dynamic_pointer_cast<BfSingleLine>(tangent_l->get_object_by_index(i + 1));
         auto bezier = std::make_shared<BfBezierCurve>(2, BF_BEZIER_CURVE_VERT_COUNT);
         if (bfMathCreateBezier2From2Lines(*start, *stop, *bezier)) { 
            bezier->bind_pipeline(&m_info->l_pipeline);
            bezier->set_color({1.f, 0.5f, 0.3f});
            bezier->createVertices();
            bezier->createIndices();
            bezier_l->add_l(bezier);   
         } else { 
            abort();
         }
      }
      return bezier_l;
   };

   _addPart(createBezierParts(_part<BfDrawLayer, BfBladeSection2_Part_CmaxLines_Tangets_Back>()), 
            BfBladeSection2_Part_Back);
   _addPart(createBezierParts(_part<BfDrawLayer, BfBladeSection2_Part_CmaxLines_Tangets_Front>()), 
            BfBladeSection2_Part_Front);

} // clang-format on

// === === === === === === === === === === === === === === === === === === ===
// === === === === === === === === === === === === === === === === === === ===

BfBladeBase::BfBladeBase(const BfBladeBaseCreateInfo& info)
    : BfDrawLayer(info.layer_create_info), __info{info}
{
   for (auto& it : __info.section_infos)
   {
      auto sec = std::make_shared< BfBladeSection >(&it);
      this->add(sec);
   }
}

std::shared_ptr< BfBladeBaseSurface >
BfBladeBase::get_shape()
{
   return std::static_pointer_cast< BfBladeBaseSurface >(
       this->get_object_by_index(0)
   );
}

const BfBladeBaseCreateInfo&
BfBladeBase::get_info()
{
   return __info;
}

// size_t BfBladeBase::add_section(const BfBladeSectionCreateInfo& info) {
//	if (!info.layer_create_info.is_nested)
//		throw std::runtime_error("Input blade section is not nested");
//
//	auto sec = std::make_shared<BfBladeSection>(
//		info
//	);
//	this->add(sec);
//	this->update_buffer();
//	return sec->id.get();
// }
//
// void BfBladeBase::del_section(size_t id) {
//	this->del(id);
// }

// std::vector<BfBladeSectionCreateInfo*> BfBladeBase::get_pInfos() {
//
//	std::vector<BfBladeSectionCreateInfo*> infos;
//	infos.reserve(this->get_layer_count() * 2);
//
//	for (size_t i = 0; i < this->get_layer_count(); i++) {
//
//		auto l =
// std::dynamic_pointer_cast<BfBladeSection>(this->get_layer_by_index(i));
//		infos.push_back(l->get_pInfo());
//	}
//
//	return infos;
// }

BfBladeBaseSurface::BfBladeBaseSurface(
    const std::vector< std::shared_ptr< BfBladeSection > >& secs,
    size_t inner_sections_count,
    size_t skip_vert
)
    : __secs{secs}, __slices_count{inner_sections_count}, __skip_vert{skip_vert}
{
   this->createVertices();
   this->createIndices();

   // for (auto it = __indices.end() - 1; it != __indices.end() - 400; --it)
   //    std::cout << *it << "\n";

   // std::cout << "Vertices count: " << __vertices.size() << "\n";
   // std::cout << "Indices count: " << __indices.size() << "\n";
}

void
BfBladeBaseSurface::createVertices()
{
   using T_parts = std::vector< std::shared_ptr< BfDrawObj > >;

   std::vector< size_t > total_vertices;
   std::vector< T_parts > parts;

   total_vertices.reserve(__secs.size());
   parts.reserve(__secs.size());
   /*
    * Возвращает набор из:
    * 1) Входная кромка
    * 2) Спинка
    * 3) Выходная кромка
    * 4) Корыто
    *
    */
   for (auto sec = __secs.begin(); sec != __secs.end(); ++sec)
   {
      T_parts p = sec->get()->get_shape_parts();
      parts.emplace_back(p);
      size_t total_part_vert_count = 0;
      for (auto part = p.begin(); part != p.end(); ++part)
      {
         total_part_vert_count += part->get()->vertices().size();
      }
      total_vertices.emplace_back(total_part_vert_count);
   }

   // Проверяем, одинаковое ли количество точек во всех сечениях
   for (auto count = total_vertices.begin() + 1; count != total_vertices.end();
        ++count)
   {
      if (*count != *(count - 1))
      {
         // std::cout << "count are not the same" << "\n";
      }
   }

   size_t parts_count = parts[0].size();
   // Идем по каждый части сечения по очереди
   for (int p_index = 0; p_index < parts_count; ++p_index)
   {
      // Идем по каждой точке во всех сечениях
      size_t vert_count = parts[0][p_index]->vertices().size();

      std::vector< std::vector< glm::vec3 > > v_part;
      v_part.resize((__slices_count) * (__secs.size() - 1));

      for (auto it = v_part.begin(); it != v_part.end(); ++it)
      {
         it->resize(vert_count);
      }

      for (size_t v_index = 0; v_index < vert_count; ++v_index)
      {
         std::vector< BfVertex3 > spl_v;
         spl_v.reserve(parts.size());

         for (auto part = parts.begin(); part != parts.end(); ++part)
         {
            // Получаем все точки для составляющей сечения с индексом p_index
            spl_v.emplace_back((*part)[p_index]->vertices()[v_index]);
         }

         std::vector< SplineLib::cSpline3 > spls =
             bfMathSplineFitExternal3D(spl_v);
         size_t spl_index = 0;
         for (auto spl = spls.begin(); spl != spls.end(); ++spl)
         {
            // std::cout << "Spline index:" << spl_index << "\n";
            std::vector< BfVertex3 > local_part_v;
            local_part_v.reserve(__slices_count);
            for (size_t i = 0; i < __slices_count; ++i)
            {
               float t = (float)i / (__slices_count - 1);
               SplineLib::Vec3f __fff = SplineLib::Position(*spl, t);
               v_part[spl_index + i][v_index] = {__fff.x, __fff.y, __fff.z};
               // local_part_v.emplace_back(BfVertex3({__fff.x, __fff.y,
               // __fff.z}));
               // __vertices.push_back(BfVertex3({__fff.x, __fff.y,
               // __fff.z})); std::sort(__vertices.begin(), __vertices.end(),
               // [](const BfVertex3& a, const BfVertex3& b) { return a.pos.z
               // < b.pos.z; });
            }
            spl_index += __slices_count;
         }
         // Triangulate across part
      }
      // std::cout << "Slices: " << v_part.size() << "\n";
      // for (size_t i = 0; i < v_part.size(); ++i) {
      // 	std::cout << "V:" << v_part[i].size() << "\n";
      // 	for (size_t j = 0; j < v_part[j].size(); ++j) {
      // 		__vertices.push_back(v_part[i][j]);
      // 	}
      // }

      glm::vec3 color{1.0f, 1.0f, 1.0f};

      for (size_t slice_index = 0; slice_index < v_part.size() - 1;
           ++slice_index)
      {
         for (size_t v_index = 0; v_index < v_part[slice_index].size() - 1;
              ++v_index)
         {
            glm::vec3 n_1 = bfMathGetNormal(
                v_part[slice_index][v_index],
                v_part[slice_index][v_index + 1],
                v_part[slice_index + 1][v_index]
            );
            glm::vec3 n_2 = bfMathGetNormal(
                v_part[slice_index + 1][v_index],
                v_part[slice_index + 1][v_index + 1],
                v_part[slice_index][v_index]
            );

            __vertices.emplace_back(
                BfVertex3{{v_part[slice_index][v_index]}, color, n_1}
            );
            __vertices.emplace_back(
                BfVertex3{{v_part[slice_index][v_index + 1]}, color, n_1}
            );
            __vertices.emplace_back(
                BfVertex3{{v_part[slice_index + 1][v_index]}, color, n_1}
            );
            __vertices.emplace_back(
                BfVertex3{{v_part[slice_index + 1][v_index]}, color, n_1}
            );
            __vertices.emplace_back(
                BfVertex3{{v_part[slice_index + 1][v_index + 1]}, color, n_1}
            );
            __vertices.emplace_back(
                BfVertex3{{v_part[slice_index][v_index + 1]}, color, n_1}
            );
            // __vertices.emplace_back(v_part[slice_index][v_index+1]);
            // __vertices.emplace_back(v_part[slice_index + 1][v_index]);
            // __vertices.emplace_back(v_part[slice_index + 1][v_index]);
            // __vertices.emplace_back(v_part[slice_index + 1][v_index+1]);
            // __vertices.emplace_back(v_part[slice_index][v_index+1]);

            // if (v_index == v_part[slice_index].size() - 2 && slice_index
            // == v_part.size() - 2) { 	con
            // }
            // 	__vertices.emplace_back(v_part[slice_index][v_index]);
            // 	__vertices.emplace_back(v_part[slice_index][v_index+1]);
            // 	__vertices.emplace_back(v_part[slice_index +
            // 1][v_index]);
            // 	__vertices.emplace_back(v_part[slice_index +
            // 1][v_index]);
            // 	__vertices.emplace_back(v_part[slice_index +
            // 1][v_index+1]);
            // 	__vertices.emplace_back(v_part[slice_index][v_index+1]);
            // }
         }
      }
      // __vertices.emplace_back(v_part[v_part.size()-1][v_part[0].size()-1]);
      // __vertices.emplace_back(v_part[v_part.size()-1][v_part[0].size()-1+1]);
      // __vertices.emplace_back(v_part[v_part.size()-1][v_part[0].size()-1]);
   }
}

std::shared_ptr< BfBladeBaseSurface >
BfBladeBase::create_shape()
{
   std::vector< std::shared_ptr< BfBladeSection > > secs;
   secs.reserve(this->get_layer_count());

   for (size_t i = 0; i < this->get_layer_count(); ++i)
   {
      secs.emplace_back(
          std::dynamic_pointer_cast< BfBladeSection >(
              this->get_layer_by_index(i)
          )
      );
   }

   auto surface = std::make_shared< BfBladeBaseSurface >(secs, 20, 10);
   return surface;
}
