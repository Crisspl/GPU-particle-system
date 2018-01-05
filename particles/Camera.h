#ifndef CAMERA_H
#define CAMERA_H

#include "maths/Mat4.h"
#include "maths/Quaternion.h"

class Camera
{
public:
	Camera(const fhl::Vec3f & _pos) : m_position{_pos}, m_sensitivity{1.f} {}

	const fhl::Mat4f & getView() const { return m_view; }
	fhl::Vec3f getDirectionVector() const;
	fhl::Vec3f getPosition() const { return m_position; }

	void setSensitivity(float _s) { m_sensitivity = _s; }

	void updateViewMatrix();
	void mouseMove(fhl::Vec2f _offset);
	void translate(float _strafe, float _forward);

private:
	fhl::Quaternion m_quat;
	fhl::Mat4f m_view;
	fhl::Vec3f m_position;
	float m_sensitivity;
};

#endif
