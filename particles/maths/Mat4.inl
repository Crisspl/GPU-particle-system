namespace fhl
{

	template<typename _T>
	constexpr inline Mat4<_T>::Mat4(_T _diagonal) :
		m_cols {
			Vec4<_T>{ Vec3<_T>::right(_diagonal), _T(0) },
			Vec4<_T>{ Vec3<_T>::up(_diagonal), _T(0) },
			Vec4<_T>{ Vec3<_T>::forward(_diagonal), _T(0) },
			Vec4<_T>::wAxis(_diagonal)
		}
	{}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::operator+(const Mat4<_T> & _other) const
	{
		Mat4<_T> ret(*this);

		for (int i = 0; i < 16; i++)
			ret[i] += _other[i];

		return ret;
	}

	template<typename _T>
	inline Mat4<_T> & Mat4<_T>::operator+=(const Mat4<_T> & _other)
	{
		for (int i = 0; i < 16; i++)
			(*this)[i] += _other[i];

		return *this;
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::operator-(const Mat4<_T> & _other) const
	{
		Mat4<_T> ret(*this);

		for (int i = 0; i < 16; i++)
			ret[i] -= _other[i];

		return ret;
	}

	template<typename _T>
	inline Mat4<_T> & Mat4<_T>::operator-=(const Mat4<_T> & _other)
	{
		for (int i = 0; i < 16; i++)
			(*this)[i] -= _other[i];

		return *this;
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::operator*(_T _n) const
	{
		Mat4<_T> ret(*this);

		for (int i = 0; i < 16; i++)
			ret[i] *= _n;

		return ret;
	}

	template<typename _T>
	inline Mat4<_T> operator*(_T _n, const Mat4<_T> & _m)
	{
		return _m * _n;
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::operator*(const Mat4<_T> & _other) const
	{
		Mat4<_T> ret;
		for (int j = 0; j < 4; j++)
		{
			for (int i = 0; i < 4; i++)
			{
				_T sum = 0;
				for (int e = 0; e < 4; e++)
					sum += (*this)[i + e * 4] * _other[e + j * 4];
				ret[i + j * 4] = sum;
			}
		}
		return ret;
	}

	template<typename _T>
	inline Mat4<_T> & Mat4<_T>::operator*=(_T _n)
	{
		return (*this = *this * _n);
	}

	template<typename _T>
	inline Mat4<_T> & Mat4<_T>::operator*=(const Mat4<_T> & _other)
	{
		return (*this = *this * _other);
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::operator-() const
	{
		return *this * -1.f;
	}

	template<typename _T>
	inline bool Mat4<_T>::operator==(const Mat4<_T> & _other) const
	{
		for (int i = 0; i < 4; i++)
			if (m_cols[i] != _other.m_cols[i])
				return false;
		return true;
	}

	template<typename _T>
	inline bool Mat4<_T>::operator!=(const Mat4<_T> & _other) const
	{
		return !(*this == _other);
	}

	template<typename _T>
	inline Vec4<_T> Mat4<_T>::getRow(std::size_t _n) const
	{
		Vec4<_T> row;
		for (int i = 0; i < 4; i++)
			row[i] = (*this)[_n + i * 4];

		return row;
	}

	template<typename _T>
	constexpr inline Vec4<_T> Mat4<_T>::getCol(std::size_t _n) const
	{
		return m_cols[_n];
	}

	template<typename _T>
	inline Mat4<_T> & Mat4<_T>::setRow(std::size_t _n, Vec4<_T> _r)
	{
		for (int i = 0; i < 4; i++)
			(*this)[_n + i * 4] = _r[i];

		return *this;
	}

	template<typename _T>
	inline Mat4<_T> & Mat4<_T>::setCol(std::size_t _n, Vec4<_T> _c)
	{
		m_cols[_n] = _c;
		return *this;
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::ortho(_T _left, _T _right, _T _bottom, _T _top, _T _near, _T _far)
	{
		Mat4<_T> ret;

		ret[0 + 0 * 4] = 2.f / (_right - _left);
		ret[1 + 1 * 4] = 2.f / (_top - _bottom);
		ret[2 + 2 * 4] = -2.f / (_far - _near);
		ret.setCol(3, {
		   -((_right + _left) / (_right - _left)),
		   (_top + _bottom) / (_top - _bottom),
		   -((_far + _near) / (_far - _near)),
		   1.f }
		);

		return ret;
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::perspective(_T _fov, _T _aspectRatio, _T _near, _T _far)
	{
		Mat4<_T> ret(0.f);

		_T t = std::tan(toRadians(_fov / 2.f));

		ret[0 + 0 * 4] = 1.f / (_aspectRatio * t);
		ret[1 + 1 * 4] = 1.f / t;
		ret[2 + 2 * 4] = -(_far + _near) / (_far - _near);
		ret[3 + 2 * 4] = -1.0f;
		ret[2 + 3 * 4] = -2.f * _far * _near / (_far - _near);

		return ret;
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::lookAt(const Vec3<_T> & _eye, const Vec3<_T> & _center, const Vec3<_T> & _up)
	{
		Mat4<_T> ret(1.f);

		const Vec3<_T> f = (_center - _eye).normalized();
		const Vec3<_T> s = f.cross(_up).normalized();
		const Vec3<_T> u = s.cross(f);
		const Vec3<_T> t = Vec3<_T>(-(s.dot(_eye)), -(u.dot(_eye)), f.dot(_eye));

		ret.setRow(0, Vec4<_T>(s, 0));
		ret.setRow(1, Vec4<_T>(u, 0));
		ret.setRow(2, Vec4<_T>(-f, 0));
		ret.setCol(3, Vec4<_T>(t, 1));

		return ret;
	}

	template<typename _T>
	constexpr inline Mat4<_T> Mat4<_T>::translate(const Vec3<_T> & _t)
	{
		return Mat4<_T> {
			Vec4<_T>{ Vec3<_T>::right(), _T(0) },
			Vec4<_T>{ Vec3<_T>::up(), _T(0) },
			Vec4<_T>{ Vec3<_T>::forward(), _T(0) },
			Vec4<_T>{ _t, _T(1) }
		};
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::translate(const Mat4<_T> & _mat, const Vec3<_T> & _t)
	{
		return _mat * translate(_t);
	}

	template<typename _T>
	constexpr inline Mat4<_T> Mat4<_T>::scale(const Vec3<_T> & _s)
	{
		return Mat4<_T> {
			Vec4<_T>{ Vec3<_T>::right(_s.x()), _T(0) },
			Vec4<_T>{ Vec3<_T>::up(_s.y()), _T(0) },
			Vec4<_T>{ Vec3<_T>::forward(_s.z()), _T(0) },
			Vec4<_T>::wAxis()
		};
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::scale(const Mat4<_T> & _mat, const Vec3<_T> & _s)
	{
		return _mat * scale(_s);
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::rotate(_T _angle, const Vec3<_T> & _axe)
	{
		Mat4<_T> ret(1.f);

		_T x = _axe.x();
		_T y = _axe.y();
		_T z = _axe.z();

		_T angle = toRadians(_angle);
		_T s = std::sin(angle);
		_T c = std::cos(angle);
		_T omc = 1.f - c;


		ret.setRow(0,
		{
			x * x * omc + c,
			x * y * omc - z * s,
			x * z * omc + y * s,
			0
		});
		ret.setRow(1,
		{
			x * y * omc + z * s,
			y * y * omc + c,
			y * z * omc - x * s,
			0
		});
		ret.setRow(2,
		{
			x * z * omc - y * s,
			y * z * omc - y * s,
			z * z * omc + c,
			0
		});

		return ret;
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::rotate(const Mat4<_T> & _mat, _T _angle, const Vec3<_T> & _axe)
	{
		return _mat * rotate(_angle, _axe);
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::transpose(const Mat4<_T> & _mat)
	{
		Mat4<_T> trans;
		for (int i = 0; i < 4; i++)
			trans.setCol(i, _mat.getRow(i));

		return trans;
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::transposed() const
	{
		return transpose(*this);
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::invert(const Mat4<_T> & _mat)
	{
		Mat4<_T> inv;
		_T * const e = const_cast<_T *>(inv.data());
		const _T * const m = _mat.data();

		e[0] = m[5] * m[10] * m[15] + m[9] * m[14] * m[7] + m[13] * m[6] * m[11] - (m[13] * m[10] * m[7] + m[9] * m[6] * m[15] + m[5] * m[14] * m[11]);
		e[4] = -(m[1] * m[10] * m[15] + m[9] * m[14] * m[3] + m[13] * m[2] * m[11] - (m[13] * m[10] * m[3] + m[9] * m[2] * m[15] + m[1] * m[14] * m[11]));
		e[8] = m[1] * m[6] * m[15] + m[5] * m[14] * m[3] + m[13] * m[2] * m[7] - (m[13] * m[6] * m[3] + m[5] * m[2] * m[15] + m[1] * m[14] * m[7]);
		e[12] = -(m[1] * m[6] * m[11] + m[5] * m[10] * m[3] + m[9] * m[2] * m[7] - (m[9] * m[6] * m[3] + m[5] * m[2] * m[11] + m[1] * m[10] * m[7]));
		e[1] = -(m[4] * m[10] * m[15] + m[8] * m[14] * m[7] + m[12] * m[6] * m[11] - (m[12] * m[10] * m[7] + m[8] * m[6] * m[15] + m[4] * m[14] * m[11]));
		e[5] = m[0] * m[10] * m[15] + m[8] * m[14] * m[3] + m[12] * m[2] * m[11] - (m[12] * m[10] * m[3] + m[8] * m[2] * m[15] + m[0] * m[14] * m[11]);
		e[9] = -(m[0] * m[6] * m[15] + m[4] * m[14] * m[3] + m[12] * m[2] * m[7] - (m[12] * m[6] * m[3] + m[4] * m[2] * m[15] + m[0] * m[14] * m[7]));
		e[13] = m[0] * m[6] * m[11] + m[4] * m[10] * m[3] + m[8] * m[2] * m[7] - (m[8] * m[6] * m[3] + m[4] * m[2] * m[11] + m[0] * m[10] * m[7]);
		e[2] = m[4] * m[9] * m[15] + m[8] * m[13] * m[7] + m[12] * m[5] * m[11] - (m[12] * m[9] * m[7] + m[8] * m[5] * m[15] + m[4] * m[13] * m[11]);
		e[6] = -(m[0] * m[9] * m[15] + m[8] * m[13] * m[3] + m[12] * m[1] * m[11] - (m[12] * m[9] * m[3] + m[8] * m[1] * m[15] + m[0] * m[13] * m[3]));
		e[10] = m[0] * m[5] * m[15] + m[4] * m[13] * m[3] + m[12] * m[1] * m[7] - (m[12] * m[5] * m[3] + m[4] * m[1] * m[15] + m[0] * m[13] * m[7]);
		e[14] = -(m[0] * m[5] * m[11] + m[4] * m[9] * m[3] + m[8] * m[1] * m[7] - (m[8] * m[5] * m[3] + m[4] * m[1] * m[11] + m[0] * m[9] * m[7]));
		e[3] = -(m[4] * m[9] * m[14] + m[8] * m[13] * m[6] + m[12] * m[5] * m[10] - (m[12] * m[9] * m[6] + m[8] * m[5] * m[14] + m[4] * m[13] * m[10]));
		e[7] = m[0] * m[9] * m[14] + m[8] * m[13] * m[2] + m[12] * m[1] * m[10] - (m[12] * m[9] * m[2] + m[8] * m[1] * m[14] + m[0] * m[13] * m[10]);
		e[11] = -(m[0] * m[5] * m[14] + m[4] * m[13] * m[2] + m[12] * m[1] * m[6] - (m[12] * m[5] * m[2] + m[4] * m[1] * m[14] + m[0] * m[13] * m[6]));
		e[15] = m[0] * m[5] * m[10] + m[4] * m[9] * m[2] + m[8] * m[1] * m[6] - (m[8] * m[5] * m[2] + m[4] * m[1] * m[10] + m[0] * m[9] * m[6]);

		_T det = m[0] * e[0] + m[1] * e[1] + m[2] * e[2] + m[3] * e[3];

		return inv.transposed() * _T(1. / det);
	}

	template<typename _T>
	inline Mat4<_T> Mat4<_T>::inverted() const
	{
		return invert(*this);
	}

	template<typename _T>
	inline Vec2<_T> Mat4<_T>::transform(const Mat4<_T> & _mat, const Vec2<_T> & _v)
	{
		auto calc = [&_mat, &_v](std::size_t _n) -> _T {
			return _mat.getRow(_n).dot(Vec4<_T>(_v, fhl::Vec2f::one()));
		};

		return Vec2<_T>(calc(0), calc(1));
	}


	template<typename _T>
	inline Vec3<_T> Mat4<_T>::transform(const Mat4<_T> & _mat, const Vec3<_T> & _v)
	{
		auto calc = [&_mat, &_v](std::size_t _n) -> _T {
			return _mat.getRow(_n).dot(Vec4<_T>(_v, 1));
		};

		return Vec3<_T>(calc(0), calc(1), calc(2));
	}

}