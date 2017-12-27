#ifndef FHL_MATHS_SWIZZLE_H
#define FHL_MATHS_SWIZZLE_H

#include <type_traits>

#include "vectors.h"

namespace fhl { namespace swizzle
{

	namespace impl
	{	
		/** @todo Use C++17 fold expressions instead */
		template<bool...>
		struct AllTrue : std::false_type {};

		template<bool FirstCond, bool... Conds>
		struct AllTrue<FirstCond, Conds...> : std::integral_constant<bool, FirstCond && AllTrue<Conds...>::value> {};

		template<bool Cond>
		struct AllTrue<Cond> : std::integral_constant<bool, Cond> {};

		template<>
		struct AllTrue<> : std::true_type {};
	}

	enum VecDim
	{
		X = 0, x = 0, R = 0, r = 0,
		Y = 1, y = 1, G = 1, g = 1,
		Z = 2, z = 2, B = 2, b = 2,
		W = 3, w = 3, A = 3, a = 3
	};

	template<VecDim ...Dims, typename VecT>
	constexpr typename VectorTypeForSize<sizeof...(Dims), typename VecT::valueType>::Type
	get(const VecT & _v)
	{
		static_assert(impl::AllTrue<(Dims < VecT::Dimensions)...>::value, "Not enough vector dimensions to get the one(s) demanded");

		return{ _v[Dims]... };
	}

}}

#endif
