#ifndef FHL_MATHS_MATHS_FUNCS
#define FHL_MATHS_MATHS_FUNCS

#include <algorithm>

#include "Constants.h"

namespace fhl
{
	template<typename T = double>
	constexpr T toRadians(T _deg) { return _deg * Constants<T>::Pi() / 180.; }
	template<typename T = double>
	constexpr T toDegrees(T _rad) { return _rad * 180. / Constants<T>::Pi(); }

	template<typename T>
	constexpr T abs(T n) { return n < T{} ? -n : n; }

	template <typename T>
	constexpr T clamp(T n, T lo, T hi)
	{
		return std::max(lo, std::min(n, hi));
	}
}

#endif
