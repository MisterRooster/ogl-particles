/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "Input.h"

#include "utility/Debug.h"


namespace nhahn
{
	Input::Input() : m_MouseWheelPos(0), mx(0), my(0), _pollRate(0.01)
	{
		DBG("Input", DebugLevel::INFO, "Start input manager\n");

		for (int i = 0; i < MAX_KEYS; i++)
		{
			m_Keys[i] = false;
		}

		for (int i = 0; i < MAX_BUTTONS; i++)
		{
			m_MouseButtons[i] = false;
		}

		// connect hardware gamepad
		_gpadDevice = std::make_shared<Gamepad>();
		_gpadDevice->setStateChangedCallback(_notifyGamepadStateChanged);
	}

	Input& Input::instance()
	{
		static Input *instance = new Input();

		return *instance;
	}

	void Input::update(double dt)
	{
		// poll hardware inputs
		if (_gpadDevice->isConnected())
		{
			if (_nextPoll <= 0)
			{
				_nextPoll = _pollRate;
				if (_gpadDevice->stateChanged())
					_gpadDevice->getState();
			}
			else
				_nextPoll -= dt;
		}

		// recheck gamepad connection
		if (_gpadConnectionRecheck)
		{
			if (_nextConnectionCheck <= 0)
			{
				_nextConnectionCheck = _gpadConnectionRecheckTime;
				_gpadDevice->checkConnection();
			}
			else
				_nextConnectionCheck -= dt;
		}

	}

	void Input::setPollRate(double rate)
	{
		DBG("Input", DebugLevel::DEBUG, "Set poll rate to %f\n", rate);

		_pollRate = 1.0 / rate;
	}

	glm::vec2 Input::getMousePosition() const
	{
		return glm::vec2(mx, my);
	}

	int Input::getMouseWheelPosition() const
	{
		return m_MouseWheelPos;
	}

	bool Input::isKeyPressed(unsigned keycode) const
	{
		if (keycode >= MAX_KEYS)
			return false;

		return m_Keys[keycode];
	}

	bool Input::isMouseButtonPressed(unsigned button) const
	{
		if (button >= MAX_BUTTONS)
			return false;

		return m_MouseButtons[button];
	}

	bool Input::isGamepadButtonChanged(unsigned buttoncode, int action) const
	{
		if ((_gpadDevice->latestChange().wButtonsDiff & buttoncode) == 0)
			return false;

		if (action == IKA_REPEAT) action = IKA_PRESS;
		return ((_gpadDevice->getState().gamepadState.wButtons & buttoncode) != action);
		return false;

		return true;
	}

	void Input::_notifyKeyChanged(int key, int scancode, int action, int mods)
	{
		DBG("Input", DebugLevel::VERBOSE, "Key with code 0x%X %s\n", key, (action == IKA_RELEASE) ? "released" : "pressed");
		gInput().m_Keys[key] = action == IKA_PRESS;
	}

	void Input::_notifyMouseButtonPressed(int button, int action, int mods)
	{
		DBG("Input", DebugLevel::VERBOSE, "Mousebutton %i %s\n", button, (action == IKA_RELEASE) ? "released" : "pressed");
		gInput().m_MouseButtons[button] = action == IKA_PRESS;
	}

	void Input::_notifyCursorMoved(double xpos, double ypos)
	{
		gInput().mx = xpos;
		gInput().my = ypos;
	}

	void Input::_notifyMouseWheelScrolled(double xoffset, double yoffset)
	{
		DBG("Input", DebugLevel::VERBOSE, "Mousewheel scrolled with offset [%f,%f]\n", xoffset, yoffset);
		gInput().m_MouseWheelPos += (int)yoffset;
	}

	void Input::_notifyGamepadStateChanged(GAMEPAD_STATE newState, GAMEPAD_STATE_DIFF change)
	{
		if (DEBUG_LEVEL == DebugLevel::VERBOSE)
		{
			for (unsigned i = 1; i <= GAMEPAD_BUTTON_LAST; i = i * 2)
			{
				if ((change.wButtonsDiff & i) != 0)
					DBG("Input", DebugLevel::VERBOSE, "Gamepad button with code 0x%X %s\n", i, ((newState.wButtons & i) == i) ? "pressed" : "released");
			}
			if (change.sLeftTriggerDiff != 0)
				DBG("Input", DebugLevel::VERBOSE, "Gamepad left trigger changed by %i\n", change.sLeftTriggerDiff);
			if (change.sRightTriggerDiff != 0)
				DBG("Input", DebugLevel::VERBOSE, "Gamepad right trigger changed by %i\n", change.sRightTriggerDiff);
			if (change.iThumbLXDiff != 0 || change.iThumbLYDiff != 0)
				DBG("Input", DebugLevel::VERBOSE, "Gamepad left thumb changed by [%i,%i]\n", change.iThumbLXDiff, change.iThumbLYDiff);
			if (change.iThumbRXDiff != 0 || change.iThumbRYDiff != 0)
				DBG("Input", DebugLevel::VERBOSE, "Gamepad right thumb changed by [%i,%i]\n", change.iThumbRXDiff, change.iThumbRYDiff);
		}
	}

	Input& gInput()
	{
		return Input::instance();
	}
}

