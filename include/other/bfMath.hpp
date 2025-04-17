/*

⠄⠄⠄⣠⢴⢴⡴⣤⢤⣄⠄⠄⢀⠄⣀⡤⣴⣺⡽⣯⡷⣦⣄⠄⠄⠄
⠄⣔⢞⢝⢝⠽⡽⣽⣳⢿⡽⣏⣗⢗⢯⢯⣗⡯⡿⣽⢽⣷⣟⣷⣄ ⠄
⠄⡗⡟⡼⣸⣁⢋⠎⠎⢯⢯⡧⡫⣎⡽⡹⠊⢍⠙⠜⠽⣳⢯⣿⣳ ⠄
⠄⢕⠕⠁⣁⢬⢬⣌⠆⠅⢯⡻⣜⢷⠁⠌⡼⠲⠺⢮⡆⡉⢹⣺⣽ ⠄
⠄⠄⡀⢐⠄⠄⠄⠈⠳⠁⡂⢟⣞⡏⠄⡹⠄⠄⠄⠄⠈⣺⡐⣞⣾ ⠄
⠄⢰⡳⡹⢦⣀⣠⡠⠤⠄⡐⢝⣾⣳⣐⣌⠳⠦⠤⠤⣞⢼⢽⣻⡷ ⠄
⠄⢸⣚⢆⢄⣈⠨⢊⢐⢌⠞⣞⣞⡗⡟⡾⣝⢦⣳⡳⣯⢿⣻⣽⣟ ⠄
⠄⠘⡢⡫⢒⠒⣘⠰⣨⢴⣸⣺⣳⢥⢷⣳⣽⣳⢮⢝⢽⡯⣿⣺⡽ ⠄
⠄⠄⠁⠪⠤⢑⢄⢽⡙⢽⣺⢾⢽⢯⡟⡽⣾⣎⡿⣮⡳⣹⣳⣗⠇ ⠄
⠄⠄⠄⠁⠄⡸⡡⠑⠤⣠⡑⠙⠍⡩⡴⣽⡗⣗⣟⣷⣫⢳⢕⡏ ⠄⠄
⠄⠄⠄⠄⢈⡇⡇⡆⡌⡀⡉⠫⡯⢯⡫⡷⣽⣺⣗⣟⡾⡼⡺ ⠄⠄⠄
⠄⠄⠄⠄⡮⡎⡎⡎⣞⢲⡹⡵⡕⣇⡿⣽⣳⣟⣾⣳⡯⠉ ⠄⠄⠄⠄
⠄⠄⠄⠄⢯⡣⡣⡣⡣⡣⣗⡽⣽⣳⢯⢷⣳⣻⣺⣗⡇ ⠄⠄⠄⠄⠄
⠄⠄⠄⠄⠰⡙⠺⢪⢪⡺⡵⣯⣗⡯⡿⣽⢽⢾⣳⠏ ⠄⠄⠄⠄⠄⠄
⠄⠄⠄⠄⠄⠐⠢⠄⣀⣀⢉⠊⣊⣉⡬⡶⡻⣝⡞ ⠄⠄⠄⠄⠄⠄⠄
⠄⠄⠄⠄⠄⠄⠄⠄⠄⠈⠙⢙⢑⢹⣘⠮⠛⠈ ⠄⠄⠄⠄⠄⠄⠄⠄
⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠂⠁⠑⠁⠄

*/

#ifndef BF_MATH_HPP
#define BF_MATH_HPP

#include <stdlib.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <initializer_list>
#include <iterator>
#include <vector>

#define REL_ACC 1e-6
#define ABS_ACC 1e-6
#define MAX(x, y) (((x) < (y)) ? (y) : (x))
#define CHECK_FLOAT_EQUALITY_TO_NULL(v1, v2) (std::abs(v1 - v2) < ABS_ACC)

#define CHECK_FLOAT_EQUALITY(v1, v2)                                           \
   (((v1) == 0 && (v2) == 0) ? true                                            \
                             : (std::abs((v1 - v2) / MAX(v1, v2)) < REL_ACC))

#define fori_cw(end, beg) for (size_t i = end - 1; i > beg; i--)
#define forj_cw(end, beg) for (size_t i = end - 1; i > beg; i--)

#define fori(beg, end) for (size_t i = beg; i < end; i++)
#define forj(beg, end) for (size_t j = beg; j < end; j++)
#define fork_(beg, end) for (size_t k = beg; k < end; k++)

#define forit(container) for (auto& it : container)
#define forit1(container) for (auto& it1 : container)
#define forit2(container) for (auto& it2 : container)

#define CLEAR_VEC                                                              \
   if (!__data.empty())                                                        \
   __data.clear()

#define CHECK_DIMENSION(v1, v2)                                                \
   if ((v1).size() != (v2).size())                                             \
   abort()
#define CHECK_DIMENSION_NUM(n1, n2)                                            \
   if (n1 != n2)                                                               \
   abort()
#define CHECK_CAPACITY_NUM_LESS(n1, n2)                                        \
   if (n1 > n2)                                                                \
   abort()
#define CHECK_CAPACITY_NUM_LESS_EQ(n1, n2)                                     \
   if (n1 > n2)                                                                \
   abort()

#define IT_PAIR                                                                \
   std::pair< std::vector< float >::iterator, std::vector< float >::iterator >

template < class T >
class BfMtxColumnIterator
{
   std::vector< T >::iterator __b;
   std::vector< T >::iterator __e;
   const size_t __n;
   const size_t __m;

public:
   BfMtxColumnIterator(
       std::vector< T >::iterator b, const size_t n, const size_t m
   );

   BfMtxColumnIterator< T > operator++();
};

template < class TI >
class BfMatrixIterator
{
public:
   using value_type = TI;
   using difference_type = std::ptrdiff_t;
   using pointer = TI*;
   using reference = TI&;
   using iterator_category = std::random_access_iterator_tag;

   enum it_type
   {
      ROW_ITERATOR,
      COL_ITERATOR,
      ROW_ITERATOR_R,
      COL_ITERATOR_R,
      LEFT_DIAGONAL_ITERATOR,
      RIGHT_DIAGONAL_ITERATOR
   };

   BfMatrixIterator()
       : __ptr(nullptr), __n{1}, __m{1} {};

   BfMatrixIterator(pointer ptr, size_t n, size_t m, it_type t = ROW_ITERATOR)
       : __ptr(ptr), __n{n}, __m{m}, __t{t} {};

   reference operator*() const { return *__ptr; }
   pointer operator->() const { return __ptr; }
   reference operator[](difference_type n) const { return *(__ptr + n); }

   BfMatrixIterator& operator++()
   {
      switch (__t)
      {
      case ROW_ITERATOR:
         __ptr++;
         break;
      case COL_ITERATOR:
         __ptr += __m;
         break;
      case ROW_ITERATOR_R:
         __ptr--;
         break;
      case COL_ITERATOR_R:
         __ptr -= __m;
         break;
      case LEFT_DIAGONAL_ITERATOR:
         __ptr += __m + 1;
         break;
      case RIGHT_DIAGONAL_ITERATOR:
         __ptr += __m - 1;
         break;
      }
      return *this;
   }

   BfMatrixIterator operator++(int)
   {
      BfMatrixIterator temp = *this;
      switch (__t)
      {
      case ROW_ITERATOR:
         __ptr++;
         break;
      case COL_ITERATOR:
         __ptr += __m;
         break;
      case ROW_ITERATOR_R:
         __ptr--;
         break;
      case COL_ITERATOR_R:
         __ptr -= __m;
         break;
      case LEFT_DIAGONAL_ITERATOR:
         __ptr += __m + 1;
         break;
      case RIGHT_DIAGONAL_ITERATOR:
         __ptr += __m - 1;
         break;
      }
      return temp;
   }

   BfMatrixIterator& operator--()
   {
      switch (__t)
      {
      case ROW_ITERATOR:
         __ptr--;
         break;
      case COL_ITERATOR:
         __ptr -= __m;
         break;
      case ROW_ITERATOR_R:
         __ptr++;
         break;
      case COL_ITERATOR_R:
         __ptr += __m;
         break;
      case LEFT_DIAGONAL_ITERATOR:
         __ptr -= __m - 1;
         break;
      case RIGHT_DIAGONAL_ITERATOR:
         __ptr -= __m + 1;
         break;
      }
      return *this;
   }

   BfMatrixIterator operator--(int)
   {
      BfMatrixIterator temp = *this;
      switch (__t)
      {
      case ROW_ITERATOR:
         __ptr--;
         break;
      case COL_ITERATOR:
         __ptr -= __m;
         break;
      case ROW_ITERATOR_R:
         __ptr++;
         break;
      case COL_ITERATOR_R:
         __ptr += __m;
         break;
      case LEFT_DIAGONAL_ITERATOR:
         __ptr -= __m - 1;
         break;
      case RIGHT_DIAGONAL_ITERATOR:
         __ptr -= __m + 1;
         break;
      }
      return temp;
   }

   BfMatrixIterator& operator+=(difference_type n)
   {
      switch (__t)
      {
      case ROW_ITERATOR:
         __ptr += n;
         break;
      case COL_ITERATOR:
         __ptr += n * __m;
         break;
      case ROW_ITERATOR_R:
         __ptr -= n;
         break;
      case COL_ITERATOR_R:
         __ptr -= __m;
         break;
      case LEFT_DIAGONAL_ITERATOR:
         __ptr += n * __m + 1 * n;
         break;
      case RIGHT_DIAGONAL_ITERATOR:
         __ptr += n * __m - 1 * n;
         break;
      }
      return *this;
   }
   BfMatrixIterator& operator-=(difference_type n)
   {
      switch (__t)
      {
      case ROW_ITERATOR:
         __ptr -= n;
         break;
      case COL_ITERATOR:
         __ptr -= n * __m;
         break;
      case ROW_ITERATOR_R:
         __ptr += n;
         break;
      case COL_ITERATOR_R:
         __ptr += n * __m;
         break;
      case LEFT_DIAGONAL_ITERATOR:
         __ptr -= n * __m - 1 * n;
         break;
         // case RIGHT_DIAGONAL_ITERATOR: __ptr += ; break;
      }
      return *this;
   }

   friend bool
   operator==(const BfMatrixIterator& lhs, const BfMatrixIterator& rhs)
   {
      return lhs.__ptr == rhs.__ptr;
   }

   friend bool
   operator!=(const BfMatrixIterator& lhs, const BfMatrixIterator& rhs)
   {
      return !(lhs == rhs);
   }

   friend bool
   operator<(const BfMatrixIterator& lhs, const BfMatrixIterator& rhs)
   {
      return lhs.__ptr < rhs.__ptr;
   }

   friend bool
   operator>(const BfMatrixIterator& lhs, const BfMatrixIterator& rhs)
   {
      return rhs < lhs;
   }

   friend bool
   operator<=(const BfMatrixIterator& lhs, const BfMatrixIterator& rhs)
   {
      return !(rhs < lhs);
   }

   friend bool
   operator>=(const BfMatrixIterator& lhs, const BfMatrixIterator& rhs)
   {
      return !(lhs < rhs);
   }

   friend BfMatrixIterator
   operator+(const BfMatrixIterator& it, difference_type n)
   {
      BfMatrixIterator temp = it;
      temp += n;
      return temp;
   }

   friend BfMatrixIterator
   operator+(difference_type n, const BfMatrixIterator& it)
   {
      return it + n;
   }

   friend BfMatrixIterator
   operator-(const BfMatrixIterator& it, difference_type n)
   {
      BfMatrixIterator temp = it;
      temp -= n;
      return temp;
   }

   friend difference_type
   operator-(const BfMatrixIterator& lhs, const BfMatrixIterator& rhs)
   {
      return lhs.m_ptr - rhs.m_ptr;
   }

private:
   pointer __ptr;
   size_t __n;
   size_t __m;
   it_type __t;
};

template < class T >
class BfMatrix2
{
   std::vector< T > __v;

   const size_t __n;
   const size_t __m;

public:
   // Constructors
   BfMatrix2(const std::vector< T >& v, size_t n, size_t m)
       : __v{v}, __n{n}, __m{m} {};

   BfMatrix2(std::initializer_list< T > v, size_t n, size_t m)
       : __v{v}, __n{n}, __m{m} {
          // if (v.size() != __n * __m) abort();
          // __v.reserve(__m * __n);
          // for (auto it = v.begin(); it != v.end(); ++it) {
          //     __v.emplace_back(*it);
          // }
       };

   BfMatrix2(size_t n, size_t m, T value)
       : __n{n}, __m{m}
   {
      __v.reserve(__n * __m);
      fori(0, __m * __n) { __v.emplace_back(value); }
   }

   // Other operations
   void fill(T v)
   {
      fori(0, __m * __n) { (*this)[i] = v; }
   }

   size_t rows() const noexcept { return __n; }
   size_t cols() const noexcept { return __m; }

   // Iterators
   std::vector< T >::iterator begin() { return __v.begin(); }
   std::vector< T >::iterator end() { return __v.end(); }
   std::vector< T >::iterator rbegin() { return __v.rbegin(); }
   std::vector< T >::iterator rend() { return __v.rend(); }

   T& operator()(size_t i, size_t j) { return __v[i * __m + j]; }
   const T& operator()(size_t i, size_t j) const { return __v[i * __m + j]; }
   T& operator[](size_t i) { return __v[i]; }

   BfMatrixIterator< T > begin_row(size_t i)
   {
      return BfMatrixIterator< T >(
          &__v[i * __m],
          __n,
          __m,
          BfMatrixIterator< T >::ROW_ITERATOR
      );
   }

   BfMatrixIterator< T > begin_col(size_t j)
   {
      return BfMatrixIterator< T >(
          &__v[j],
          __n,
          __m,
          BfMatrixIterator< T >::COL_ITERATOR
      );
   }

   BfMatrixIterator< T > end_row(size_t i)
   {
      return BfMatrixIterator< T >(
          &(*this)(i, __m),
          __n,
          __m,
          BfMatrixIterator< T >::ROW_ITERATOR
      );
   }

   BfMatrixIterator< T > end_col(size_t j)
   {
      return BfMatrixIterator< T >(
          &(*this)(__n, j),
          __n,
          __m,
          BfMatrixIterator< T >::COL_ITERATOR
      );
   }

   BfMatrixIterator< T > rbegin_row(size_t i)
   {
      return BfMatrixIterator< T >(
          &(*this)(i, __m - 1),
          __n,
          __m,
          BfMatrixIterator< T >::ROW_ITERATOR_R
      );
   }

   BfMatrixIterator< T > rbegin_col(size_t j)
   {
      return BfMatrixIterator< T >(
          &(*this)(__n - 1, j),
          __n,
          __m,
          BfMatrixIterator< T >::COL_ITERATOR_R
      );
   }

   BfMatrixIterator< T > rend_row(size_t i)
   {
      return BfMatrixIterator< T >(
          &(*this)(i, 0),
          __n,
          __m,
          BfMatrixIterator< T >::ROW_ITERATOR_R
      );
   }

   BfMatrixIterator< T > rend_col(size_t j)
   {
      return BfMatrixIterator< T >(
          &(*this)(0, j),
          __n,
          __m,
          BfMatrixIterator< T >::COL_ITERATOR_R
      );
   }

   BfMatrixIterator< T > begin_dim_left(int i)
   {
      if (i == 0)
      {
         return BfMatrixIterator< T >(
             &(*__v.begin()),
             __n,
             __m,
             BfMatrixIterator< T >::LEFT_DIAGONAL_ITERATOR
         );
      }
      else if (i > 0)
      {
         if (i == __m)
            abort();
         return BfMatrixIterator< T >(
             &(*__v.begin()) + i,
             __n,
             __m,
             BfMatrixIterator< T >::LEFT_DIAGONAL_ITERATOR
         );
      }
      else
      {
         if (-i == __n)
            abort();
         return BfMatrixIterator< T >(
             &(*__v.begin()) - i * __m,
             __n,
             __m,
             BfMatrixIterator< T >::LEFT_DIAGONAL_ITERATOR
         );
      }
   }

   BfMatrixIterator< T > end_dim_left(int i)
   {
      if (__n != __m)
         abort();
      if (i >= 0)
      {
         if (i == __m)
            abort();
         return this->begin_dim_left(i) + (__n - i);
      }
      else
      {
         if (-i == __n)
            abort();
         return this->begin_dim_left(i) + (__n + i);
      }
   }

   // Main math operators

   BfMatrix2< T > operator+(const BfMatrix2< T >& o)
   {
      if (__n != o.__n || __m != o.__m)
         abort();
      BfMatrix2< T > temp = *this;
      fori(0, __n * __m) { temp[i] += o[i]; }
      return temp;
   }

   BfMatrix2< T > operator-(const BfMatrix2< T >& o)
   {
      if (__n != o.__n || __m != o.__m)
         abort();
      BfMatrix2< T > temp = *this;
      fori(0, __n * __m) { temp[i] -= o[i]; }
      return temp;
   }

   BfMatrix2< T > operator*(const BfMatrix2< T >& o)
   {
      if (__m != o.__n)
         abort();

      size_t r = __n;
      size_t c = o.__m;
      size_t com = __m;

      BfMatrix2< T > res(r, c, 0);

      fori(0, r)
      {
         forj(0, c)
         {
            fork_(0, com) { res(i, j) += (*this)(i, k) * o(k, j); }
         }
      }
      return res;
   }

   BfMatrix2< T > operator*(T v)
   {
      BfMatrix2< T > temp = *this;
      fori(0, __n * __m) { temp[i] *= v; }
      return temp;
   }

   BfMatrix2< T > operator/(T v)
   {
      BfMatrix2< T > temp = *this;
      fori(0, __n * __m) { temp[i] /= v; }
      return temp;
   }

   bool operator==(const BfMatrix2< T >& rhs)
   {
      if (this->rows() != rhs.rows() || this->cols() != rhs.cols())
      {
         return false;
      }

      for (size_t i = 0; i < this->rows(); ++i)
      {
         for (size_t j = 0; j < this->cols(); ++j)
         {
            T diff = std::abs((*this)(i, j) - rhs(i, j));
            T max_val = std::max(std::abs((*this)(i, j)), std::abs(rhs(i, j)));
            if (max_val > 0 && diff / max_val > REL_ACC)
            {
               return false;
            }
         }
      }
      return true;
   }

   // friend std::ostream&
   // operator<<(std::ostream& os, const BfMatrix2< T >& matrix)
   // {
   //    os << "BfMatrix2(";
   //    for (size_t i = 0; i < matrix.rows(); ++i)
   //    {
   //       os << "[";
   //       for (size_t j = 0; j < matrix.cols(); ++j)
   //       {
   //          if (j != matrix.cols() - 1)
   //             os << matrix(i, j) << " ";
   //          else
   //             os << matrix(i, j);
   //       }
   //       if (i != matrix.rows() - 1)
   //          os << "], ";
   //       else
   //          os << "])";
   //    }
   //    return os;
   // }

   void swap_rows(size_t i_1, size_t i_2)
   {
      BfMatrixIterator< T > b_r1 = this->begin_row(i_1);
      BfMatrixIterator< T > e_r1 = this->end_row(i_1);
      BfMatrixIterator< T > b_r2 = this->begin_row(i_2);

      std::swap_ranges(b_r1, e_r1, b_r2);
   }

   void swap_cols(size_t j_1, size_t j_2)
   {
      BfMatrixIterator< T > b_c1 = this->begin_col(j_1);
      BfMatrixIterator< T > e_c1 = this->end_col(j_1);
      BfMatrixIterator< T > b_c2 = this->begin_col(j_2);

      std::swap_ranges(b_c1, e_c1, b_c2);
   }

   void swap_row_col(size_t i, size_t j)
   {
      if (__n != __m)
         abort();
      BfMatrixIterator< T > b_r1 = this->begin_row(i);
      BfMatrixIterator< T > e_r1 = this->end_row(i);
      BfMatrixIterator< T > b_c2 = this->begin_col(j);

      std::swap_ranges(b_r1, e_r1, b_c2);
   }

   BfMatrix2< T > upper_triangular_gauss()
   {
      BfMatrix2< T > o{*this};
      int m = o.rows();
      int n = o.cols();

      int h = 0;
      int k = 0;

      while (h < m && k < n)
      {
         int i_max = h;
         for (int i = h + 1; i < m; ++i)
         {
            if (std::abs(o(i, k)) > std::abs(o(i_max, k)))
            {
               i_max = i;
            }
         }
         if (o(i_max, k) == 0)
         {
            k++;
         }
         else
         {
            o.swap_rows(h, i_max);
            for (int i = h + 1; i < m; ++i)
            {
               T f = o(i, k) / o(h, k);
               o(i, k) = 0;
               for (int j = k + 1; j < n; ++j)
               {
                  o(i, j) -= f * o(h, j);
               }
            }
            h++;
            k++;
         }
      }
      return o;
   }

   T det_bareiss()
   {
      BfMatrix2< T > o{*this};
      if (__m != __n)
         abort();
      int dim = __n;

      if (dim <= 0)
      {
         return 0;
      }

      T sign = 1;

      for (int k = 0; k < dim - 1; k++)
      {
         // Pivot - row swap needed
         if (o(k, k) == 0)
         {
            int m = 0;
            for (m = k + 1; m < dim; m++)
            {
               if (o(m, k) != 0)
               {
                  o.swap_rows(m, k);
                  sign = -sign;
                  break;
               }
            }

            // No entries != 0 found in column k -> det = 0
            if (m == dim)
            {
               return 0;
            }
         }

         // Apply formula
         for (int i = k + 1; i < dim; i++)
         {
            for (int j = k + 1; j < dim; j++)
            {
               o(i, j) = o(k, k) * o(i, j) - o(i, k) * o(k, j);
               if (k != 0)
               {
                  o(i, j) /= o(k - 1, k - 1);
               }
            }
         }
      }
      return sign * o(dim - 1, dim - 1);
   }

   float det_triangular_gauss()
   {
      auto o = this->upper_triangular_gauss();
      float det = 1;
      for (auto it = o.begin_dim_left(0); it != o.end_dim_left(0); it++)
      {
         det *= *it;
      }
      return det;
   }

   std::pair< BfMatrix2< T >, BfMatrix2< T > > LU_decomposition()
   {
      if (__n != __m)
         abort();
      // if (this->det_bareiss() == 0) abort();

      BfMatrix2< T > L(__n, __m, 0);
      BfMatrix2< T > U(__n, __m, 0);

      int i = 0, j = 0, k = 0;
      for (i = 0; i < __n; i++)
      {
         for (j = 0; j < __n; j++)
         {
            if (j < i)
               L(j, i) = 0;
            else
            {
               L(j, i) = (*this)(j, i);
               for (k = 0; k < i; k++)
               {
                  L(j, i) = L(j, i) - L(j, k) * U(k, i);
               }
            }
         }

         for (j = 0; j < __n; j++)
         {
            if (j < i)
               U(i, j) = 0;
            else if (j == i)
               U(i, j) = 1;
            else
            {
               U(i, j) = (*this)(i, j) / L(i, i);
               for (k = 0; k < i; k++)
               {
                  U(i, j) = U(i, j) - ((L(i, k) * U(k, j)) / L(i, i));
               }
            }
         }
      }
      return std::make_pair(L, U);
   }

   std::array< BfMatrix2< T >, 4 > PLU_decomposition()
   {
      int n = (*this).rows();
      if (n != (*this).cols())
      {
         throw std::invalid_argument(
             "Matrix must be square for PLU decomposition"
         );
      }

      BfMatrix2< T > P(n, n, 0); // Матрица перестановок
      BfMatrix2< T > L(n, n, 0); // Нижняя треугольная матрица
      BfMatrix2< T > U(n, n, 0); // Верхняя треугольная матрица

      for (int i = 0; i < n; ++i)
      {
         P(i, i) = 1; // Единичная матрица для P
      }

      BfMatrix2< T > A_copy{*this}; // Копия исходной матрицы

      for (int k = 0; k < n; ++k)
      {
         // Ищем максимальный элемент в столбце k
         int max_row = k;
         for (int i = k + 1; i < n; ++i)
         {
            if (std::abs(A_copy(i, k)) > std::abs(A_copy(max_row, k)))
            {
               max_row = i;
            }
         }

         // Переставляем строки матрицы A, P и L, если необходимо
         if (max_row != k)
         {
            A_copy.swap_rows(max_row, k);
            P.swap_rows(max_row, k);
            L.swap_rows(max_row, k);
         }

         // Заполняем элементы матрицы L и U
         for (int i = k; i < n; ++i)
         {
            L(i, k) = A_copy(i, k);
            U(k, i) = A_copy(k, i) / L(k, k);
         }

         // Обновляем матрицу A_copy
         for (int i = k + 1; i < n; ++i)
         {
            for (int j = k + 1; j < n; ++j)
            {
               A_copy(i, j) -= L(i, k) * U(k, j);
            }
         }
      }

      return {A_copy, P, L, U};
   }

   float det_LU()
   {
      std::pair< BfMatrix2< T >, BfMatrix2< T > > LU = this->LU_decomposition();

      float L_det = 1;
      for (auto it = LU.first.begin_dim_left(0); it != LU.first.end_dim_left(0);
           ++it)
      {
         L_det *= *it;
      }
      float U_det = 1;
      for (auto it = LU.second.begin_dim_left(0);
           it != LU.second.end_dim_left(0);
           ++it)
      {
         U_det *= *it;
      }
      return L_det * U_det;
   }

   float det_PLU()
   {
      std::array< BfMatrix2< T >, 4 > PLU = this->PLU_decomposition();

      float L_det = 1;
      for (auto it = PLU[1].begin_dim_left(0); it != PLU[0].end_dim_left(0);
           ++it)
      {
         L_det *= *it;
      }

      float U_det = 1;
      for (auto it = PLU[1].begin_dim_left(0); it != PLU[1].end_dim_left(0);
           ++it)
      {
         U_det *= *it;
      }

      float P_det = 1;
      for (auto it = PLU[2].begin_dim_left(0); it != PLU[2].end_dim_left(0);
           ++it)
      {
         P_det *= *it;
      }

      return L_det * U_det * P_det;
   }
};

template < class T >
std::pair< BfMatrix2< T >, BfMatrix2< T > >
luDecomposition(const BfMatrix2< T >& A)
{
   size_t n = A.rows();
   BfMatrix2< T > L(n, n, 0);
   BfMatrix2< T > U = A;

   std::vector< size_t > P(n);
   for (size_t i = 0; i < n; ++i)
   {
      P[i] = i; // Начальная перестановка P
   }

   for (size_t k = 0; k < n - 1; ++k)
   {
      // Выбор главного элемента по столбцу
      size_t pivotRow = k;
      T maxPivot = std::abs(U(k, k));
      for (size_t i = k + 1; i < n; ++i)
      {
         if (std::abs(U(i, k)) > maxPivot)
         {
            maxPivot = std::abs(U(i, k));
            pivotRow = i;
         }
      }

      // Перестановка строк матрицы U и вектора P
      if (pivotRow != k)
      {
         std::swap_ranges(
             U.begin_row(k),
             U.end_row(k) + 1,
             U.begin_row(pivotRow)
         );
         std::swap(P[k], P[pivotRow]);
         if (k > 0)
         {
            std::swap_ranges(
                L.begin_row(k),
                L.end_row(k) + 1,
                L.begin_row(pivotRow)
            );
         }
      }

      // Проверка на вырожденность матрицы
      if (std::abs(U(k, k)) < REL_ACC)
      {
         abort();
      }

      // Вычисление элементов L и U
      for (size_t i = k + 1; i < n; ++i)
      {
         L(i, k) = U(i, k) / U(k, k);
         for (size_t j = k; j < n; ++j)
         {
            U(i, j) -= L(i, k) * U(k, j);
         }
      }
   }

   // Добавление 1 на диагональ матрицы L
   for (size_t i = 0; i < n; ++i)
   {
      L(i, i) = 1;
   }

   return {L, U};
}

#endif
