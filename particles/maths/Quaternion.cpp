#include "Quaternion.h"
#include "mathsFuncs.h"
#include "swizzle.h"

namespace fhl
{

	Quaternion::Quaternion(float _x, float _y, float _z, float _angle) : m_xyzw(_x, _y, _z, _angle)
	{
		const float radAngle = toRadians(_angle) * 0.5f;
		const float sine = std::sin(radAngle);

		m_xyzw *= sine;
		m_xyzw.w() = std::cos(radAngle);
	}

	Quaternion::Quaternion(Vec3f _eulerAngles)
	{
		for (int i = 0; i < 3; ++i)
			_eulerAngles[i] = toRadians(_eulerAngles[i]);

		const Vec3f c{std::cos(_eulerAngles[0]), std::cos(_eulerAngles[1]), std::cos(_eulerAngles[2]) };
		const Vec3f s{std::sin(_eulerAngles[0]), std::sin(_eulerAngles[1]), std::sin(_eulerAngles[2])};

		m_xyzw = {
			s.x() * c.y() * c.z() - c.x() * s.y() * s.z(),
			c.x() * s.y() * c.z() + s.x() * c.y() * s.z(),
			c.x() * c.y() * s.z() - s.x() * s.y() * c.z(),
			c.x() * c.y() * c.z() + s.x() * s.y() * s.z()
		};
	}

	Quaternion Quaternion::operator*(Quaternion _other) const
	{
		const Vec4f & q1 = this->m_xyzw, &q2 = _other.m_xyzw;
		_other.m_xyzw =
		{
			q2.w() * q1.x() + q2.x() * q1.w() + q2.y() * q1.z() - q2.z() * q1.y(),
			q2.w() * q1.y() + q2.y() * q1.w() + q2.z() * q1.x() - q2.x() * q1.z(),
			q2.w() * q1.z() + q2.z() * q1.w() + q2.x() * q1.y() - q2.y() * q1.x(),
			q2.w() * q1.w() - q2.x() * q1.x() - q2.y() * q1.y() - q2.z() * q1.z()
		};

		return _other;
	}

	Quaternion & Quaternion::operator*=(const Quaternion & _other)
	{
		return *this = *this * _other;
	}

	Vec3f Quaternion::getAxis() const
	{
		const float sqrSine = 1.f - m_xyzw.w() * m_xyzw.w();
		if (sqrSine <= 0)
			return Vec3f::zero();
		const float sine = std::sqrt(sqrSine);

		using namespace swizzle;
		return get<X, Y, Z>(m_xyzw) / sine;
	}

	float Quaternion::getAngle() const
	{
		auto arccos = [](float _a) -> float {
			if (_a < -1.f) return Constants<float>::Pi();
			if (_a > 1.f) return 0.f;
			return std::acos(_a);
		};

		return toDegrees(arccos(m_xyzw.w())) * 2;
	}

	Mat4f Quaternion::toMat4f() const
	{
		Mat4f ret;

		const Vec4f & q = m_xyzw;
		float xx = q.x() * q.x(), yy = q.y() * q.y(), zz = q.z() * q.z();
		float xy = q.x() * q.y(), xz = q.x() * q.z();
		float yz = q.y() * q.z(), wx = q.w() * q.x();
		float wy = q.w() * q.y(), wz = q.w() * q.z();

		ret.setRow(0,
		{
			  1.f - 2 * (yy + zz),
			  2 * (xy + wz),
			  2 * (xz - wy),
			  0
		});
		ret.setRow(1,
		{
			  2 * (xy - wz),
			  1.f - 2 * (xx + zz),
			  2 * (yz + wx),
			  0
		});
		ret.setRow(2,
		{
			  2 * (xz + wy),
			  2 * (yz - wx),
			  1.f - 2 * (xx + yy),
			  0
		});
		ret.setRow(3, { 0, 0, 0, 1 });

		return ret;
	}

	float Quaternion::length() const
	{
		return std::sqrt(dot(*this));
	}

	float Quaternion::dot(const Quaternion & _other) const
	{
		return m_xyzw.dot(_other.m_xyzw);
	}

	Quaternion Quaternion::normalized() const
	{
		Quaternion ret = *this;
		ret.m_xyzw /= length();

		return ret;
	}

	Quaternion Quaternion::inverted() const
	{
		Quaternion ret;
		ret.m_xyzw = this->m_xyzw * (1.f / length()) * Vec4f(-1, -1, -1, 1);

		return ret;
	}

	Quaternion Quaternion::conjugate() const
	{
		Quaternion ret = *this;
		ret.m_xyzw *= Vec4f(-1, -1, -1, 1);

		return ret;
	}

}