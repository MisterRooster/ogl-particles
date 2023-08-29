/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma comment(lib, "XInput.lib")

#include "Gamepad.h"
#include "utility/Debug.h"
#include <Xinput.h>


namespace nhahn
{

	GAMEPAD_STATE convert_XINPUT_GAMEPAD(XINPUT_GAMEPAD state)
	{
		return {state.wButtons, state.bLeftTrigger, state.bRightTrigger,
			state.sThumbLX, state.sThumbLY, state.sThumbRX, state.sThumbRY};
	}

	INPUT_STATE convert_XINPUT_STATE(XINPUT_STATE state)
	{
		return { state.dwPacketNumber, convert_XINPUT_GAMEPAD(state.Gamepad) };
	}

	int Gamepad::_next_id = 0;

	Gamepad::Gamepad() : _id(_next_id++), _leftDeadzone(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE),
		_rightDeadzone(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		// check if pad is connected
		if (!checkConnection())
			DBG("Gamepad", DebugLevel::DEBUG, "No hardware device is connected\n");
	}

	Gamepad::~Gamepad()
	{
		_next_id--;
	}

	bool Gamepad::checkConnection()
	{
		XINPUT_CAPABILITIES caps;
		bool connected = (XInputGetCapabilities(_id, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS);
		if (_sourceConnected != connected)
		{
			if (connected) DBG("Gamepad", DebugLevel::INFO, "Device with id %i connected\n", _id);
			else DBG("Gamepad", DebugLevel::INFO, "Device with id %i disconnected\n", _id);
		}
		_sourceConnected = connected;
		return connected;
	}

	bool Gamepad::stateChanged()
	{
		if (!_sourceConnected)
			return false;

		XINPUT_STATE state;
		
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		// get the state of the controller from XInput.
		if (XInputGetState(_id, &state) != ERROR_SUCCESS)
		{
			checkConnection();
			return false;
		}

		if (state.dwPacketNumber != _currentState.dwPacketNumber)
		{
			GAMEPAD_STATE oldState = _currentState.gamepadState;
			INPUT_STATE newState = convert_XINPUT_STATE(state);
			_currentState = _applyDeadzones(newState);

			if (_currentState.gamepadState != oldState)
			{
				_latestDiff = _calcStateDiff(oldState, _currentState.gamepadState);
				if (_onStateChangedCallback)
				{
					_onStateChangedCallback(_currentState.gamepadState, _latestDiff);

				}
				return true;
			}
		}
		return false;
	}

	GAMEPAD_STATE_DIFF Gamepad::_calcStateDiff(GAMEPAD_STATE a, GAMEPAD_STATE b)
	{
		GAMEPAD_STATE_DIFF res;
		res.wButtonsDiff = a.wButtons ^ b.wButtons;
		res.sLeftTriggerDiff = b.bLeftTrigger - a.bLeftTrigger;
		res.sRightTriggerDiff = b.bRightTrigger - a.bRightTrigger;
		res.iThumbLXDiff = b.sThumbLX - a.sThumbLX;
		res.iThumbLYDiff = b.sThumbLY - a.sThumbLY;
		res.iThumbRXDiff = b.sThumbRX - a.sThumbRX;
		res.iThumbRYDiff = b.sThumbRY - a.sThumbRY;
		return res;
	}

	INPUT_STATE Gamepad::_applyDeadzones(INPUT_STATE& a)
	{
		if (std::abs(a.gamepadState.sThumbLX) <= _leftDeadzone) a.gamepadState.sThumbLX = 0;
		if (std::abs(a.gamepadState.sThumbLY) <= _leftDeadzone) a.gamepadState.sThumbLY = 0;
		if (std::abs(a.gamepadState.sThumbRX) <= _rightDeadzone) a.gamepadState.sThumbRX = 0;
		if (std::abs(a.gamepadState.sThumbRY) <= _rightDeadzone) a.gamepadState.sThumbRY = 0;
		return a;
	}

	void Gamepad::vibrate(int leftVal, int rightVal)
	{
		if (_sourceConnected)
		{
			// create a vibraton State
			XINPUT_VIBRATION Vibration;
			ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

			// Set the Vibration Values
			Vibration.wLeftMotorSpeed = leftVal;
			Vibration.wRightMotorSpeed = rightVal;

			// Vibrate the controller
			XInputSetState(_id, &Vibration);
		}
	}
}

