#ifndef FHL_MATHS_VEC4_H
#define FHL_MATHS_VEC4_H

#include "Vec3.h"

namespace fhl
{

	template<typename _T>
	class Vec4 : public detail::VecBase<4, _T>
	{
	public:
		using valueType = _T;
		enum { Dimensions = 4 };

		constexpr explicit Vec4(_T _value = _T(0)) : detail::VecBase<4, _T>(_value) {}
		constexpr Vec4(_T _x, _T _y, _T _z, _T _w) : detail::VecBase<4, _T>(_x, _y, _z, _w) {}
		template<typename _U>
		Vec4(const Vec4<_U> & _other) : detail::VecBase<4, _T>(static_cast<const detail::VecBase<4, _U> &>(_other)) {}
		Vec4(const detail::VecBase<4, _T> & _other) : detail::VecBase<4, _T>(_other) {}
		constexpr Vec4(const Vec2<_T> & _v1, const Vec2<_T> & _v2) : detail::VecBase<4, _T>(_v1.x(), _v1.y(), _v2.x(), _v2.y()) {}
		constexpr Vec4(const Vec3<_T> & _v, _T _n) : detail::VecBase<4, _T>(_v.x(), _v.y(), _v.z(), _n) {}
		constexpr Vec4(_T _n, const Vec3<_T> & _v) : detail::VecBase<4, _T>(_n, _v.x(), _v.y(), _v.z()) {}

		template<typename _U>
		Vec4<_T> & operator=(const Vec4<_U> & _other) { detail::VecBase<4, _T>::operator=(_other); return *this; }

		static constexpr Vec4<_T> xAxis(_T n = _T(1)) { return{ n, 0, 0, 0 }; }
		static constexpr Vec4<_T> yAxis(_T n = _T(1)) { return{ 0, n, 0, 0 }; }
		static constexpr Vec4<_T> zAxis(_T n = _T(1)) { return{ 0, 0, n, 0 }; }
		static constexpr Vec4<_T> wAxis(_T n = _T(1)) { return{ 0, 0, 0, n }; }
		static constexpr Vec4<_T> zero(_T = _T(1)) { return Vec4<_T>(_T(0)); }
		static constexpr Vec4<_T> one(_T n = _T(1)) { return Vec4<_T>(n); }

		_T & x() { return (*this)[0]; }
		constexpr _T x() const { return (*this)[0]; }

		_T & y() { return (*this)[1]; }
		constexpr _T y() const { return (*this)[1]; }

		_T & z() { return (*this)[2]; }
		constexpr _T z() const { return (*this)[2]; }

		_T & w() { return (*this)[3]; }
		constexpr _T w() const { return (*this)[3]; }

		_FHL_VECTOR_OPERATORS_IMPLEMENTATION(_T, Vec4)
	};

	template<typename T>
	Vec4<T> operator*(T scalar, const Vec4<T> & vec) { return vec * scalar; }

	using Vec4f = Vec4<float>;
	using Vec4lf = Vec4<double>;
	using Vec4i = Vec4<int>;
	using Vec4u = Vec4<unsigned>;

}

#endif