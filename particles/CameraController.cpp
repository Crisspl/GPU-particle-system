#include "CameraController.h"

#include <SDL.h>

void CameraController::processKeyStates(const std::map<int, int> & _states)
{
	static std::map<int, fhl::Vec2f> dirs {
		std::make_pair(SDL_SCANCODE_W, fhl::Vec2f::down()),
		std::make_pair(SDL_SCANCODE_S, fhl::Vec2f::up()),
		std::make_pair(SDL_SCANCODE_A, fhl::Vec2f::right()),
		std::make_pair(SDL_SCANCODE_D, fhl::Vec2f::left())
	};
	for (const auto & s : _states)
		if (s.second != 0)
			translateAllCams(dirs[s.first] * m_translationSpeed);
}

void CameraController::processMousePosition(const fhl::Vec2f & _mpos)
{
	fhl::Vec2f offset = _mpos - m_prevMousePosition;
	m_prevMousePosition = _mpos;
	for (Camera * c : m_cams)
		c->mouseMove(offset);
}

void CameraController::updateAll()
{
	for (Camera * c : m_cams)
		c->updateViewMatrix();
}

void CameraController::translateAllCams(const fhl::Vec2f & _v)
{
	for (Camera * c : m_cams)
		c->translate(_v.x(), _v.y());
}
