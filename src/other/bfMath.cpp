#include "bfMath.hpp"

template<class T>
BfMtxColumnIterator<T>::BfMtxColumnIterator(std::vector<T>::iterator b,
					 const size_t n,
					 const size_t m)
	: __b{b}
	, __e{__b + n * m}
	, __n{n}
	, __m{m}
{

}

