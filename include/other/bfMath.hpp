#ifndef BF_MATH_HPP
#define BF_MATH_HPP

#define REL_ACC 1e-6
#define ABS_ACC 1e-6
#define MAX(x, y) (((x) < (y)) ? (y) : (x))
#define CHECK_FLOAT_EQUALITY_TO_NULL(v1, v2) (std::abs(v1 - v2) < ABS_ACC)

#define CHECK_FLOAT_EQUALITY(v1, v2) \
    (((v1) == 0 && (v2) == 0) ? true : \
    (std::abs( (v1 - v2)/MAX(v1, v2) ) < REL_ACC))

#define fori_cw(end, beg) for (size_t i = end - 1; i > beg; i--)
#define forj_cw(end, beg) for (size_t i = end - 1; i > beg; i--)

#define fori(beg, end) for(size_t i = beg; i < end; i++)
#define forj(beg, end) for(size_t j = beg; j < end; j++)
#define fork(beg, end) for(size_t k = beg; k < end; k++)


#define forit(container) for(auto& it : container)
#define forit1(container) for(auto& it1 : container)
#define forit2(container) for(auto& it2 : container)

#define CLEAR_VEC if (!__data.empty()) __data.clear()

#define CHECK_DIMENSION(v1, v2) if((v1).size() != (v2).size()) abort()
#define CHECK_DIMENSION_NUM(n1, n2) if(n1 != n2) abort()
#define CHECK_CAPACITY_NUM_LESS(n1, n2) if(n1 > n2) abort()
#define CHECK_CAPACITY_NUM_LESS_EQ(n1, n2) if(n1 > n2) abort()

#endif