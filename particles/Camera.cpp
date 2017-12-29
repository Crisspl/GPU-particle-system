#include "Camera.h"

#include "maths/swizzle.h"

#include <utility>

fhl::Vec3f Camera::getDirectionVector() const
{
	using namespace fhl::swizzle;
	return fhl::swizzle::get<X, Y, Z>(m_view.getRow(2)).normalized();
}

void Camera::updateViewMatrix()
{
	m_view = m_quat.toMat4f() * fhl::Mat4f::translate(-m_position);
}

void Camera::mouseMove(fhl::Vec2f _offset)
{
	std::swap(_offset.x(), _offset.y());
	_offset *= 0.02f; // sensitivity

	fhl::Quaternion quat(fhl::Vec3f{_offset, 0.f});
	m_quat = quat * m_quat;
	m_quat = m_quat.normalized();
}

void Camera::translate(float _strafe, float _forward)
{
	using namespace fhl::swizzle;
	const fhl::Vec3f strafeVec = fhl::swizzle::get<X, Y, Z>(m_view.getRow(0));
	const fhl::Vec3f forwardVec = fhl::swizzle::get<X, Y, Z>(m_view.getRow(2));

	m_position += strafeVec * _strafe + forwardVec * _forward;
}
