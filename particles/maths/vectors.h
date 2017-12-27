#ifndef FHL_MATHS_VECTORS_H
#define FHL_MATHS_VECTORS_H

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

namespace fhl
{

	template<std::size_t DimsCount, typename ValT>
	struct VectorTypeForSize {};

	template<typename ValT>
	struct VectorTypeForSize<2, ValT> { using Type = Vec2<ValT>; };

	template<typename ValT>
	struct VectorTypeForSize<3, ValT> { using Type = Vec3<ValT>; };

	template<typename ValT>
	struct VectorTypeForSize<4, ValT> { using Type = Vec4<ValT>; };

}

#endif
