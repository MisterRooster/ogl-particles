/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <string>
#include <functional>
#include <windows.h>


namespace nhahn
{
	struct GAMEPAD_STATE
	{
		WORD  wButtons = 0x000;
		BYTE  bLeftTrigger = 0;
		BYTE  bRightTrigger = 0;
		SHORT sThumbLX = 0;
		SHORT sThumbLY = 0;
		SHORT sThumbRX = 0;
		SHORT sThumbRY = 0;

		bool operator==(const GAMEPAD_STATE& other) const
		{
			return wButtons == other.wButtons 
				&& bLeftTrigger == other.bLeftTrigger
				&& bRightTrigger == other.bRightTrigger
				&& sThumbLX == other.sThumbLX
				&& sThumbLY == other.sThumbLY
				&& sThumbRX == other.sThumbRX
				&& sThumbRY == other.sThumbRY;
		}

		bool operator!=(const GAMEPAD_STATE& other)
		{
			return !(operator==(other));
		}
	};

	struct GAMEPAD_STATE_DIFF
	{
		WORD wButtonsDiff;
		SHORT sLeftTriggerDiff;
		SHORT sRightTriggerDiff;
		INT iThumbLXDiff;
		INT iThumbLYDiff;
		INT iThumbRXDiff;
		INT iThumbRYDiff;
	};

	struct INPUT_STATE
	{
		DWORD dwPacketNumber;
		GAMEPAD_STATE gamepadState;
	};

	enum GAMEPAD_BUTTONS
	{
		GAMEPAD_DPAD_UP = 0x0001,
		GAMEPAD_DPAD_DOWN = 0x0002,
		GAMEPAD_DPAD_LEFT = 0x0004,
		GAMEPAD_DPAD_RIGHT = 0x0008,
		GAMEPAD_START = 0x0010,
		GAMEPAD_BACK = 0x0020,
		GAMEPAD_LEFT_THUMB = 0x0040,
		GAMEPAD_RIGHT_THUMB = 0x0080,
		GAMEPAD_LEFT_SHOULDER = 0x0100,
		GAMEPAD_RIGHT_SHOULDER = 0x0200,
		GAMEPAD_A = 0x1000,
		GAMEPAD_B = 0x2000,
		GAMEPAD_X = 0x4000,
		GAMEPAD_Y = 0x8000,
		GAMEPAD_BUTTON_LAST = GAMEPAD_Y
	};

    /** Represents a single hardware gamepad. */
	class Gamepad
	{
	public:
		Gamepad();
		~Gamepad();

		bool isConnected() const { return _sourceConnected; }
		bool checkConnection();
		
		INPUT_STATE getState() const { return _currentState; }
		GAMEPAD_STATE_DIFF latestChange() const { return _latestDiff; }
		bool stateChanged();
		void setStateChangedCallback(std::function<void(GAMEPAD_STATE, GAMEPAD_STATE_DIFF)> cb)
		{
			_onStateChangedCallback = cb;
		}

		void setDeadzones(WORD left, WORD right) { _leftDeadzone = left; _rightDeadzone = right; }

		void vibrate(int leftval = 0, int rightval = 0);

	private:
		GAMEPAD_STATE_DIFF _calcStateDiff(GAMEPAD_STATE a, GAMEPAD_STATE b);
		INPUT_STATE _applyDeadzones(INPUT_STATE& a);

	public:
		/** Minimum allowed value as reported by the axis movement events. */
		static constexpr int MIN_AXIS = -32768;
		/** Maximum allowed value as reported by the axis movement events. */
		static constexpr int MAX_AXIS = 32767;

	private:
		int _id;
		static int _next_id;
		bool _sourceConnected = false;

		INPUT_STATE _currentState;
		GAMEPAD_STATE_DIFF _latestDiff;

		WORD _leftDeadzone;
		WORD _rightDeadzone;

		std::function<void(GAMEPAD_STATE, GAMEPAD_STATE_DIFF)> _onStateChangedCallback = {};
	};
}
