#ifndef BF_OBJECT_MATH_H
#define BF_OBJECT_MATH_H

#include <cmath>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <stdexcept>
#include <utility>

#include "bfVertex2.hpp"

namespace obj
{
namespace curves
{
namespace math
{
#define BF_MATH_FIND_LINES_INTERSECTION_BETWEEN_VERTICES 0x1
#define BF_MATH_FIND_LINES_INTERSECTION_ANY 0x2

/**
 * @brief Вычисляет вершины круга в 3D-пространстве.
 *
 * Эта функция генерирует вершины для круга, расположенного в плоскости,
 * определяемой нормалью (нормаль задает направление плоскости). Круг
 * генерируется с использованием параметрического уравнения окружности. Вершины
 * круга равномерно распределяются по окружности и сохраняются в вектор.
 *
 * @param center Центр окружности (тип BfVertex3, содержит позицию и нормаль).
 * @param radius Радиус окружности.
 * @param m_discretization Количество точек для аппроксимации окружности
 * (большее значение дает более гладкую окружность).
 * @param color Цвет для всех вершин окружности.
 *
 * @note ВАЖНО, для построения окружности должен быть задан валидный вектор
 * нормали центра.
 *
 * @return std::vector<BfVertex3> Вектор вершин, представляющих окружность.
 * с прописанными нормалями
 *
 *
 */
std::vector< BfVertex3 > calcCircleVertices(
    const BfVertex3& center,
    float radius,
    uint32_t m_discretization,
    const glm::vec3& color
);

/**
 * @brief Вычисляет коэффициенты плоскости, определяемой тремя точками.
 *
 * Функция вычисляет коэффициенты плоскости, заданной тремя точками, используя
 * метод детерминантов для нахождения нормали плоскости и её уравнения в виде
 * Ax + By + Cz + D = 0.
 *
 * @tparam T (glm::vec3 | BfVertex3) Тип первой точки (f).
 * @tparam U (glm::vec3 | BfVertex3) Тип второй точки (s).
 * @tparam B (glm::vec3 | BfVertex3) Тип третьей точки (t).
 * @param _f Первая точка плоскости.
 * @param _s Вторая точка плоскости.
 * @param _t Третья точка плоскости.
 *
 * @note В качестве входных типов могут быть использованы любые типы, для
 * которых прописан конструктор BfVertex3.
 *
 * @return Коэффициенты плоскости в формате glm::vec4:
 *         - Первые три компоненты: нормаль плоскости (A, B, C).
 *         - Четвёртая компонента: константа D уравнения плоскости.
 */
template < typename T, typename U, typename B >
glm::vec4
calcPlaneCoeffs(T&& _f, U&& _s, B&& _t)
{
   auto f = BfVertex3{std::forward< T >(_f)}.pos;
   auto s = BfVertex3{std::forward< U >(_s)}.pos;
   auto t = BfVertex3{std::forward< B >(_t)}.pos;

   glm::mat3 xd = {f.y, f.z, 1, s.y, s.z, 1, t.y, t.z, 1};
   glm::mat3 yd = {f.x, f.z, 1, s.x, s.z, 1, t.x, t.z, 1};
   glm::mat3 zd = {f.x, f.y, 1, s.x, s.y, 1, t.x, t.y, 1};

   auto direction = glm::vec3(
       glm::determinant(xd),
       -glm::determinant(yd),
       glm::determinant(zd)
   );

   glm::mat3 D = {f.x, f.y, f.z, s.x, s.y, s.z, t.x, t.y, t.z};

   return {direction, -glm::determinant(D)};
}

#define BF_MATH_ABS_ACCURACY 10e-5
#define BF_MATH_DEFAULT_DERIVATIVE_STEP 10e-5

/**
 * @brief Проверяет, находятся ли вершины в одной плоскости.
 *
 * Функция проверяет, расположены ли все переданные вершины (точки) в одной
 * плоскости, используя векторное произведение для вычисления нормали и
 * скалярное произведение для проверки отношения между точками.
 * Функция поддерживает рекурсивную проверку для произвольного числа точек.
 *
 * Для базового случая (трех точек) возвращается `true` (по умолчанию).
 * Для расширенного случая (большее количество точек) происходит рекурсивный
 * вызов, и если все точки лежат в одной плоскости, функция возвращает `true`.
 *
 * @tparam T (glm::vec3 | BfVertex3) Тип первой точки (f).
 * @tparam U (glm::vec3 | BfVertex3) Тип второй точки (s).
 * @tparam B (glm::vec3 | BfVertex3) Тип третьей точки (t).
 * @tparam A (glm::vec3 | BfVertex3) Тип нормали плоскости (np).
 * @tparam Args Дополнительные точки для проверки, поддерживается произвольное
 * количество точек.
 *
 * @param _f Первая точка (вершина).
 * @param _s Вторая точка (вершина).
 * @param _t Третья точка (вершина).
 * @param _np Нормаль плоскости, определяющая ориентацию плоскости.
 * @param args Дополнительные вершины, которые также будут проверяться на
 * нахождение в той же плоскости.
 *
 * @note В качестве входных типов могут быть использованы любые типы, для
 * которых прописан конструктор BfVertex3.
 *
 * @return Возвращает `true`, если все переданные точки лежат в одной плоскости,
 * иначе `false`.
 */
template < typename T, typename U, typename B >
bool
isVerticesInPlain(T&& _f, U&& _s, B&& _t)
{
   return true;
}
template < typename T, typename U, typename B, typename A, typename... Args >
bool
isVerticesInPlain(T&& _f, U&& _s, B&& _t, A&& _np, Args&&... args)
{
   auto np = BfVertex3{std::forward< A >(_np)};
   auto f = BfVertex3{std::forward< T >(_f)};
   auto s = BfVertex3{std::forward< U >(_s)};
   auto t = BfVertex3{std::forward< B >(_t)};

   // clang-format off
   return (glm::abs(
               glm::dot(
                  glm::cross(s.pos - f.pos, 
                             t.pos - f.pos),
                  np.pos - f.pos
               )
          ) < BF_MATH_ABS_ACCURACY) && 
   isVerticesInPlain(std::move(_s), 
                     std::move(_t), 
                     std::move(_np), 
                     std::forward<Args>(args)...);
   // clang-format on
}

/**
 * @brief Вычисляет факториал числа с использованием предвычисленных значений.
 *
 * Функция вычисляет факториал числа n с использованием карты предвычисленных
 * значений для чисел от 1 до 10. Для чисел, больших 10, выбрасывается
 * исключение, так как для них факториал не поддерживается в текущей реализации.
 * Факториал числа n (обозначается как n!) равен произведению всех целых чисел
 * от 1 до n. Например, факториал 5 равен 5! = 5 * 4 * 3 * 2 * 1 = 120.
 *
 * @param n Число, для которого необходимо вычислить факториал. Должно быть от 1
 * до 10.
 * @return Возвращает факториал числа n.
 *
 * @throw std::runtime_error Выбрасывает исключение, если n > 10, так как в
 * текущей реализации поддерживаются только значения факториалов до 10.
 *
 * @note Если n равно 0 или меньше, возвращается 1, так как 0! = 1 по
 * определению. Используется карта предвычисленных значений для оптимизации.
 */
uint32_t factorial(uint32_t n);

/**
 * @brief Вычисляет биномиальный коэффициент.
 *
 * Функция вычисляет биномиальный коэффициент C(n, k), который равен
 * количеству способов выбрать k элементов из n возможных.
 * Он также известен как "n по k" или "число сочетаний".
 * Формула вычисления биномиального коэффициента: C(n, k) = n! / (k! * (n -
 * k)!).
 *
 * @param n Общее количество элементов.
 * @param k Количество элементов для выбора.
 * @return Возвращает биномиальный коэффициент C(n, k).
 *
 * @note Для значений k > n биномиальный коэффициент равен 0.
 */
uint32_t binomial(uint32_t n, uint32_t k);

/**
 * @brief Вычисляет нормаль плоскости, заданной тремя точками.
 *
 * Функция вычисляет нормаль плоскости, проходящей через три точки (p1, p2, p3),
 * используя векторное произведение векторов, направленных вдоль двух сторон
 * треугольника, образованного этими точками. Результирующий вектор будет
 * перпендикулярным этой плоскости.
 *
 * @param p1 Первая точка плоскости.
 * @param p2 Вторая точка плоскости.
 * @param p3 Третья точка плоскости.
 * @return Возвращает нормаль плоскости, как вектор glm::vec3.
 *
 * @note Векторы, образующие стороны треугольника, вычисляются как разности
 * между координатами точек, после чего вычисляется их векторное произведение.
 */
template < typename T, typename U, typename A >
glm::vec3
calcPlaneNormal(T&& _p1, U&& _p2, A&& _p3)
{
   auto p1 = BfVertex3(std::forward< T >(_p1));
   auto p2 = BfVertex3(std::forward< T >(_p2));
   auto p3 = BfVertex3(std::forward< T >(_p3));

   glm::vec3 _v1{p2.pos - p1.pos};
   glm::vec3 _v2{p3.pos - p1.pos};
   glm::vec3 normal = glm::cross(_v1, _v2);

   if (normal == glm::vec3(0.0f))
      return normal;
   else
      return normal / glm::length(normal);
};

/**
 * @brief Находит точку пересечения двух прямых, заданных двумя отрезками.
 *
 * Функция находит точку пересечения двух прямых, представленных двумя
 * отрезками, с использованием метода параметрического представления прямых.
 * Прямые определяются четырьмя точками: началом и концом двух отрезков. Функция
 * также позволяет выбирать режим вычисления пересечения: либо между вершинами,
 * либо в любом месте прямых.
 *
 * @tparam T (glm::vec3 | BfVertex3) Тип первой начальной точки первого отрезка.
 * @tparam U (glm::vec3 | BfVertex3) Тип конечной точки первого отрезка.
 * @tparam B (glm::vec3 | BfVertex3) Тип первой начальной точки второго отрезка.
 * @tparam A (glm::vec3 | BfVertex3) Тип конечной точки второго отрезка.
 * @param l1begin Начало первого отрезка.
 * @param l1end Конец первого отрезка.
 * @param l2begin Начало второго отрезка.
 * @param l2end Конец второго отрезка.
 * @param mode Режим поиска пересечения:
 *             - BF_MATH_FIND_LINES_INTERSECTION_BETWEEN_VERTICES: пересечение
 * между отрезками.
 *             - BF_MATH_FIND_LINES_INTERSECTION_ANY: пересечение на любом
 * участке прямых.
 *
 * @note В качестве входных типов могут выступать любые типы, для которых
 * прописан конструктор BfVertex3
 *
 * @return Точка пересечения двух прямых (вектор 3D). Если пересечение не
 * найдено, возвращается NaN.
 */
template < typename T, typename U, typename B, typename A >
glm::vec3
findLinesIntersection(T&& l1begin, U&& l1end, B&& l2begin, A&& l2end, int mode)
{
   auto _a1 = BfVertex3(std::forward< T >(l1begin));
   auto _b1 = BfVertex3(std::forward< U >(l1end));
   auto _a2 = BfVertex3(std::forward< B >(l2begin));
   auto _b2 = BfVertex3(std::forward< A >(l2end));

   if (isVerticesInPlain(l1begin, l1end, l2begin, l2end))
   {
      glm::vec3 a1 = _a1.pos;
      glm::vec3 b1 = glm::normalize(_b1.pos - _a1.pos);

      glm::vec3 a2 = _a2.pos;
      glm::vec3 b2 = glm::normalize(_b2.pos - _a2.pos);

      float frac = b1.x * b2.y - b1.y * b2.x;

      float t1 =
          (a1.x * b2.y - a1.y * b2.x - a2.x * b2.y + a2.y * b2.x) / (-frac);
      float t2 =
          (a1.x * b1.y - a1.y * b1.x - a2.x * b1.y + a2.y * b1.x) / (-frac);

      // if lines are parallel
      if (glm::isnan(t1) || glm::isnan(t2))
      {
         return glm::vec3(std::nan(""), std::nan(""), std::nan(""));
      }
      if (mode == BF_MATH_FIND_LINES_INTERSECTION_BETWEEN_VERTICES)
      {
         if ((t1 >= 0.0f) && (t1 <= 1.0f) && (t2 >= 0.0f) && (t2 <= 1.0f))
         {
            return a1 + b1 * t1;
         }
         else
         {
            return glm::vec3(std::nan(""), std::nan(""), std::nan(""));
         }
      }
      else if (mode == BF_MATH_FIND_LINES_INTERSECTION_ANY)
      {
         return a1 + b1 * t1;
      }
      else
      {
         throw std::runtime_error(
             "Invalid bfMathFindLinesIntersection mode inputed"
         );
      }
   }
   else
   {
      return glm::vec3(std::nan(""), std::nan(""), std::nan(""));
   }
}

/**
 * @brief Вычисляет угол между тремя вершинами, расположенными в одной
 * плоскости.
 *
 * Эта функция принимает три вершины, вычисляет два вектора, направленные от
 * второй вершины (вектора `v1` и `v2`), и возвращает угол между ними в
 * градусах.
 *
 * @tparam V1 Тип первого аргумента (вершина 1).
 * @tparam V2 Тип второго аргумента (вершина 2, вершина, относительно которой
 * вычисляется угол).
 * @tparam V3 Тип третьего аргумента (вершина 3).
 *
 * @param _v1 Первая вершина (вектор 1).
 * @param _v2 Вторая вершина (вектор 2, центральная точка).
 * @param _v3 Третья вершина (вектор 3).
 *
 * @return Угол между двумя векторами в градусах.
 *
 * @note Угол вычисляется с использованием скалярного произведения, и результат
 * находится в диапазоне от 0 до 180 градусов.
 */
template < typename V1, typename V2, typename V3 >
float
angleBetween3Vertices(V1&& _v1, V2&& _v2, V3&& _v3)
{
   auto v1 = BfVertex3(std::forward< V1 >(_v1));
   auto v2 = BfVertex3(std::forward< V2 >(_v2));
   auto v3 = BfVertex3(std::forward< V3 >(_v3));

   glm::vec3 d1 = glm::normalize(v1.pos - v2.pos);
   glm::vec3 d2 = glm::normalize(v3.pos - v2.pos);

   return glm::degrees(glm::acos(glm::clamp(glm::dot(d1, d2), -1.0f, 1.0f)));
}

/**
 * @brief Calculates the perpendicular distance from a point to a line defined
 * by two points.
 *
 * @param point The point from which the distance is calculated.
 * @param p1 First point on the line.
 * @param p2 Second point on the line.
 * @return float Distance from `point` to the line.
 */
template < typename V, typename L1, typename L2 >
float
distanceToLine(V&& _v, L1&& _l1, L2&& _l2)
{
   auto vert = BfVertex3(std::forward< V >(_v));
   auto l1 = BfVertex3(std::forward< L1 >(_l1));
   auto l2 = BfVertex3(std::forward< L2 >(_l2));

   glm::vec3 lineDir = l2.pos - l1.pos;
   glm::vec3 pointVec = vert.pos - l1.pos;

   glm::vec3 crossProd = glm::cross(lineDir, pointVec);
   float distance = glm::length(crossProd) / glm::length(lineDir);

   return distance;
}

/**
 * @brief Находит ближайшую точку на прямой, заданной двумя вершинами, к
 * произвольной вершине.
 *
 * Эта функция вычисляет точку на прямой, заданной вершинами `line1` и `line2`,
 * которая находится на минимальном расстоянии до заданной вершины `free`.
 * Расчёт выполняется с использованием проекции вектора.
 *
 * @tparam F Тип произвольной вершины.
 * @tparam L1 Тип первой вершины прямой.
 * @tparam L2 Тип второй вершины прямой.
 *
 * @param free Произвольная вершина, не обязательно лежащая на прямой.
 * @param line1 Первая вершина, определяющая прямую.
 * @param line2 Вторая вершина, определяющая прямую.
 *
 * @return Вершина (glm::vec3), лежащая на прямой line1-line2 и ближайшая к
 * free.
 */
template < typename F, typename L1, typename L2 >
glm::vec3
closestPointOnLine(F&& free, L1&& line1, L2&& line2)
{
   auto p = BfVertex3(std::forward< F >(free));
   auto a = BfVertex3(std::forward< L1 >(line1));
   auto b = BfVertex3(std::forward< L2 >(line2));

   glm::vec3 ap = p.pos - a.pos;
   glm::vec3 ab = b.pos - a.pos;
   glm::vec3 abNorm = glm::normalize(ab);

   float t = glm::dot(ap, abNorm);

   return a.pos + abNorm * t;
}

/**
 * @brief Тип, определяющий, является ли тип указателя или объекта BfVertex3.
 *
 * Используется для проверки типа на соответствие BfVertex3.
 */
template < typename T >
using IsBfVertex3Variation = std::enable_if_t<
    std::is_same_v< std::remove_pointer_t< T >, BfVertex3 >,
    bool >;

/**
 * @brief Базовый класс для работы с кривыми Безье.
 *
 * Класс предоставляет статические методы для вычислений, связанных с кривыми
 * Безье: нормали, касательные, производные, длина кривой и вычисление точек.
 *
 * Методы могут работать с BfVertex3 | BfVertex3*
 */
class BfBezierBase
{
public:
   BfBezierBase() = delete;

   /**
    * @brief Вычисляет нормаль к кривой Безье в точке t.
    *
    * @tparam T Тип элементов в векторе данных.
    * @param data Вектор данных, представляющих кривую Безье.
    * @param t Параметр для вычисления точки на кривой.
    * @return Нормаль к кривой в точке t.
    */
   template < typename T, IsBfVertex3Variation< T > = true >
   static glm::vec3 calcNormal(const std::vector< T >& data, float t)
   {
      return glm::rotate(
                 glm::mat4(1.0f),
                 glm::radians(90.0f),
                 calc(data, t).normals
             ) *
             glm::vec4(glm::normalize(calcTangent(data, t)), 1.0f);
   }

   /**
    * @brief Вычисляет касательную вектору кривой Безье в точке t.
    *
    * @tparam T Тип элементов в векторе данных.
    * @param data Вектор данных, представляющих кривую Безье.
    * @param t Параметр для вычисления точки на кривой.
    * @return Касательная вектору кривой в точке t.
    */
   template < typename T, IsBfVertex3Variation< T > = true >
   static glm::vec3 calcTangent(const std::vector< T >& data, float t)
   {
      return calcDerivative(data, t);
   }

   /**
    * @brief Вычисляет производную кривой Безье в точке t.
    *
    * @tparam T Тип элементов в векторе данных.
    * @param data Вектор данных, представляющих кривую Безье.
    * @param t Параметр для вычисления точки на кривой.
    * @return Производная кривой в точке t.
    */
   template < typename T, IsBfVertex3Variation< T > = true >
   static glm::vec3 calcDerivative(const std::vector< T >& data, float t)
   {
      const auto step = 10e-05f;
      glm::vec3 left = calc(data, t - step).pos;
      glm::vec3 right = calc(data, t + step).pos;
      return (right - left) * 0.5f / step;
   }

   /**
    * @brief Вычисляет длину кривой Безье.
    *
    * @tparam T Тип элементов в векторе данных.
    * @param data Вектор данных, представляющих кривую Безье.
    * @return Длину кривой.
    */
   template < typename T, IsBfVertex3Variation< T > = true >
   static float length(const std::vector< T >& data)
   {
      using _T = std::decay_t< T >;
      if constexpr (std::is_pointer_v< _T >)
      {
         float len = 0;
         for (size_t i = 0; i < data.size() - 1; ++i)
         {
            len += glm::length(data[i + 1]->pos - data[i]->pos);
         }

         return len;
      }
      else
      {
         float len = 0;
         for (size_t i = 0; i < data.size() - 1; ++i)
         {
            len += glm::length(data[i + 1].pos - data[i].pos);
         }
         return len;
      }
   }

   /**
    * @brief Вычисляет точку на кривой Безье в параметрической форме.
    *
    * @tparam T Тип элементов в векторе данных.
    * @param data Вектор данных, представляющих кривую Безье.
    * @param t Параметр для вычисления точки на кривой.
    * @return Точку на кривой.
    */
   template < typename T, IsBfVertex3Variation< T > = true >
   static BfVertex3 calc(const std::vector< T >& data, float t)
   {
      // clang-format off
      using _T = std::decay_t<T>;
      glm::vec3 _v{0.0f};
      uint8_t n = data.size() - 1;

      if constexpr (std::is_pointer_v<_T>)
      {
         for (size_t i = 0; i <= n; ++i)
         {
            _v += static_cast<float>(
                      math::binomial(n, i) * std::pow(1 - t, n - i) * std::pow(t, i)
                  ) * data.at(i)->pos;
         }

         return BfVertex3{
             std::move(_v),
             glm::vec3{1.0f},
             curves::math::calcPlaneNormal(data.at(0)->pos,
                                           data.at(1)->pos,
                                           data.at(2)->pos)
         };
      }
      else
      {
         for (size_t i = 0; i <= n; ++i)
         {
            _v += static_cast<float>(
                      math::binomial(n, i) * std::pow(1 - t, n - i) *
                      std::pow(t, i)
                  ) * data.at(i).pos;
         }

         return BfVertex3{
             std::move(_v),
             glm::vec3{1.0f},
             curves::math::calcPlaneNormal(data.at(0).pos,
                                           data.at(1).pos,
                                           data.at(2).pos)
         };
      }
   }
};

}  // namespace math
}  // namespace curves
}  // namespace obj

#endif
