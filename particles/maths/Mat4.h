#ifndef FHL_MATHS_MAT4_H
#define FHL_MATHS_MAT4_H

#include <cstring>
#include <ostream>

#include "vectors.h"
#include "mathsFuncs.h"

namespace fhl
{

	template<typename _T>
	struct Mat4
	{
		constexpr explicit Mat4(_T _diagonal = _T(1));
		constexpr Mat4(const Vec4<_T> & _c1, const Vec4<_T> & _c2, const Vec4<_T> & _c3, const Vec4<_T> & _c4) : m_cols{ _c1, _c2, _c3, _c4 } {}

		Mat4<_T> operator+(const Mat4<_T> & _other) const;
		Mat4<_T> & operator+=(const Mat4<_T> & _other);

		Mat4<_T> operator-(const Mat4<_T> & _other) const;
		Mat4<_T> & operator-=(const Mat4<_T> & _other);

		Mat4<_T> operator*(_T _n) const;
		Mat4<_T> operator*(const Mat4<_T> & _other) const;

		Mat4<_T> & operator*=(_T _n);
		Mat4<_T> & operator*=(const Mat4<_T> & _other);

		Mat4<_T> operator-() const;

		bool operator==(const Mat4<_T> & _other) const;
		bool operator!=(const Mat4<_T> & _other) const;

		Vec4<_T> getRow(std::size_t _n) const;
		constexpr Vec4<_T> getCol(std::size_t _n) const;

		Mat4<_T> & setRow(std::size_t _n, Vec4<_T> _r);
		Mat4<_T> & setCol(std::size_t _n, Vec4<_T> _c);

		static Mat4<_T> identity() { return Mat4<_T>(1.f); }

		static Mat4<_T> ortho(_T _left, _T _right, _T _bottom, _T _top, _T _near, _T _far);
		static Mat4<_T> perspective(_T _fov, _T _aspectRatio, _T _near, _T _far);
		static Mat4<_T> lookAt(const Vec3<_T> & _eye, const Vec3<_T> & _center, const Vec3<_T> & _up);

		constexpr static Mat4<_T> translate(const Vec3<_T> & _t);
		static Mat4<_T> translate(const Mat4<_T> & _mat, const Vec3<_T> & _t);

		constexpr static Mat4<_T> scale(const Vec3<_T> & _s);
		static Mat4<_T> scale(const Mat4<_T> & _mat, const Vec3<_T> & _s);

		static Mat4<_T> rotate(_T _angle, const Vec3<_T> & _axe);
		static Mat4<_T> rotate(const Mat4<_T> & _mat, _T _angle, const Vec3<_T> & _axe);

		static Mat4<_T> transpose(const Mat4<_T> & _mat);
		Mat4<_T> transposed() const;

		static Mat4<_T> invert(const Mat4<_T> & _mat);
		Mat4<_T> inverted() const;

		static Vec2<_T> transform(const Mat4<_T> & _mat, const Vec2<_T> & _v);
		static Vec3<_T> transform(const Mat4<_T> & _mat, const Vec3<_T> & _v);

		_T & operator[](std::size_t _n) { return m_cols[_n / 4][_n % 4]; }
		constexpr _T operator[] (std::size_t _n) const { return m_cols[_n / 4][_n % 4]; }

		constexpr const _T * data() const { return m_cols->data(); }

	private:
		Vec4<_T> m_cols[4];
	};

	template<typename T>
	std::ostream & operator<<(std::ostream & _out, const Mat4<T> & _mat)
	{
		for (std::size_t i = 0; i < 4; ++i)
			_out << (!i ? '{' : ' ') <<  _mat.getRow(i) << (i < 3 ? '\n' : '}');
		return _out;
	}

	using Mat4i = Mat4<int>;
	using Mat4u = Mat4<unsigned>;
	using Mat4f = Mat4<float>;
	using Mat4lf = Mat4<double>;

}

#include <FHL/Maths/Mat4.inl>

#endif
