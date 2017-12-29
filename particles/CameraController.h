#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <vector>
#include <utility>
#include <map>

#include "Camera.h"

class CameraController
{
public:
	explicit CameraController(std::vector<Camera *> _cams) : m_cams{std::move(_cams)}, m_translationSpeed{1.f} {}

	void setTranslationSpeed(float _speed) { m_translationSpeed = _speed; }
	void processKeyStates(const std::map<int, int> & _states);
	void processMousePosition(const fhl::Vec2f & _mpos);
	void updateAll();

private:
	void translateAllCams(const fhl::Vec2f & _v);

private:
	std::vector<Camera *> m_cams;
	float m_translationSpeed;
	fhl::Vec2f m_prevMousePosition;
};

#endif