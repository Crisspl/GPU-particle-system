#ifndef FHL_MATHS_VEC2
#define FHL_MATHS_VEC2

#include "VecBase.h"

namespace fhl
{

	template<typename _T>
	class Vec2 : public detail::VecBase<2, _T>
	{
	public:
		using valueType = _T;
		enum { Dimensions = 2 };

		constexpr explicit Vec2(_T _value = _T(0)) : detail::VecBase<2, _T>(_value) {}
		constexpr Vec2(_T _x, _T _y) : detail::VecBase<2, _T>(_x, _y) {}
		template<typename _U>
		Vec2(const Vec2<_U> & _other) : detail::VecBase<2, _T>(static_cast<const detail::VecBase<2, _U> &>(_other)) {}
		Vec2(const detail::VecBase<2, _T> & _other) : detail::VecBase<2, _T>(_other) {}

		template<typename _U>
		Vec2<_T> & operator=(const Vec2<_U> & _other) { detail::VecBase<2, _T>::operator=(_other); return *this; }

		static constexpr Vec2<_T> up(_T n = _T(1)) { return{ 0, n }; }
		static constexpr Vec2<_T> down(_T n = _T(1)) { return{ 0, -n }; }
		static constexpr Vec2<_T> right(_T n = _T(1)) { return{ n, 0 }; }
		static constexpr Vec2<_T> left(_T n = _T(1)) { return{ -n, 0 }; }
		static constexpr Vec2<_T> zero(_T = _T(1)) { return Vec2<_T>(_T(0)); }
		static constexpr Vec2<_T> one(_T n = _T(1)) { return Vec2<_T>(n); }

		constexpr Vec2<_T> perpendicular() const { return {y(), -x()}; }

		_T & x() { return (*this)[0]; }
		constexpr _T x() const { return (*this)[0]; }

		_T & y() { return (*this)[1]; }
		constexpr _T y() const { return (*this)[1]; }

		_FHL_VECTOR_OPERATORS_IMPLEMENTATION(_T, Vec2)
	};

	template<typename T>
	Vec2<T> operator*(T scalar, const Vec2<T> & vec) { return vec * scalar; }

	using Vec2f = Vec2<float>;
	using Vec2lf = Vec2<double>;
	using Vec2i = Vec2<int>;
	using Vec2u = Vec2<unsigned>;

}

#endif
