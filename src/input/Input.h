/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <memory>
#include "glm/glm.hpp"
#include "input/Gamepad.h"
#include "utility/Timer.h"


namespace nhahn
{
#	define MAX_KEYS		1024
#	define MAX_BUTTONS	32

	enum INPUT_KEY
	{
		IK_UNKNOWN		= -1,
		IK_SPACE		= 32,
		IK_APOSTROPHE   = 39,
		IK_COMMA		= 44,
		IK_MINUS		= 45,
		IK_PERIOD		= 46,
		IK_SLASH		= 47,
		IK_0			= 48,
		IK_1			= 49,
		IK_2			= 50,
		IK_3			= 51,
		IK_4			= 52,
		IK_5			= 53,
		IK_6			= 54,
		IK_7			= 55,
		IK_8			= 56,
		IK_9			= 57,
		IK_SEMICOLON    = 59,
		IK_EQUAL		= 61,
		IK_A			= 65,
		IK_B			= 66,
		IK_C			= 67,
		IK_D			= 68,
		IK_E			= 69,
		IK_F			= 70,
		IK_G			= 71,
		IK_H			= 72,
		IK_I			= 73,
		IK_J			= 74,
		IK_K			= 75,
		IK_L			= 76,
		IK_M			= 77,
		IK_N			= 78,
		IK_O			= 79,
		IK_P			= 80,
		IK_Q			= 81,
		IK_R			= 82,
		IK_S			= 83,
		IK_T			= 84,
		IK_U			= 85,
		IK_V			= 86,
		IK_W			= 87,
		IK_X			= 88,
		IK_Y			= 89,
		IK_Z			= 90,
		IK_LEFT_BRACKET = 91,
		IK_BACKSLASH	= 92,
		IK_RIGHT_BRACKET = 93,
		IK_GRAVE_ACCENT = 96,
		IK_WORLD_1		= 161, /* non-US #1 */
		IK_WORLD_2		= 162, /* non-US #2 */
		IK_ESCAPE		= 256,
		IK_ENTER		= 257,
		IK_TAB			= 258,
		IK_BACKSPACE	= 259,
		IK_INSERT		= 260,
		IK_DELETE		= 261,
		IK_RIGHT		= 262,
		IK_LEFT			= 263,
		IK_DOWN			= 264,
		IK_UP			= 265,
		IK_PAGE_UP		= 266,
		IK_PAGE_DOWN	= 267,
		IK_HOME			= 268,
		IK_END			= 269,
		IK_CAPS_LOCK	= 280,
		IK_SCROLL_LOCK  = 281,
		IK_NUM_LOCK		= 282,
		IK_PRINT_SCREEN = 283,
		IK_PAUSE		= 284,
		IK_F1			= 290,
		IK_F2			= 291,
		IK_F3			= 292,
		IK_F4			= 293,
		IK_F5			= 294,
		IK_F6			= 295,
		IK_F7			= 296,
		IK_F8			= 297,
		IK_F9			= 298,
		IK_F10			= 299,
		IK_F11			= 300,
		IK_F12			= 301,
		IK_F13			= 302,
		IK_F14			= 303,
		IK_F15			= 304,
		IK_F16			= 305,
		IK_F17			= 306,
		IK_F18			= 307,
		IK_F19			= 308,
		IK_F20			= 309,
		IK_F21			= 310,
		IK_F22			= 311,
		IK_F23			= 312,
		IK_F24			= 313,
		IK_F25			= 314,
		IK_KP_0			= 320,
		IK_KP_1			= 321,
		IK_KP_2			= 322,
		IK_KP_3			= 323,
		IK_KP_4			= 324,
		IK_KP_5			= 325,
		IK_KP_6			= 326,
		IK_KP_7			= 327,
		IK_KP_8			= 328,
		IK_KP_9			= 329,
		IK_KP_DECIMAL	= 330,
		IK_KP_DIVIDE	= 331,
		IK_KP_MULTIPLY  = 332,
		IK_KP_SUBTRACT  = 333,
		IK_KP_ADD		= 334,
		IK_KP_ENTER		= 335,
		IK_KP_EQUAL		= 336,
		IK_LEFT_SHIFT   = 340,
		IK_LEFT_CONTROL = 341,
		IK_LEFT_ALT		= 342,
		IK_LEFT_SUPER   = 343,
		IK_RIGHT_SHIFT  = 344,
		IK_RIGHT_CONTROL = 345,
		IK_RIGHT_ALT	= 346,
		IK_RIGHT_SUPER  = 347,
		IK_MENU			= 348,
		IK_LAST			= IK_MENU
	};

	enum INPUT_KEY_ACTION
	{
		IKA_RELEASE = 0,
		IKA_PRESS = 1,
		IKA_REPEAT = 2
	};

	enum INPUT_KEY_MOD
	{
		IKM_SHIFT = 0x0001,
		IKM_CONTROL = 0x0002,
		IKM_MOD_ALT = 0x0004,
		IKM_MOD_SUPER = 0x0008,
		IKM_MOD_CAPS_LOCK = 0x0010,
		IKM_MOD_NUM_LOCK = 0x0020
	};


	/** Primary module used for dealing with input. */
	class Input
	{
	public:
		~Input() {}
		static Input& instance();

		void update(double dt);
		void setPollRate(double rate);

		glm::vec2 getMousePosition() const;
		int getMouseWheelPosition() const;
		bool isMouseButtonPressed(unsigned button) const;

		bool isKeyPressed(unsigned keycode) const;

		bool isGamepadButtonChanged(unsigned buttoncode, int action) const;
		std::shared_ptr<Gamepad> getGamepad() const { return _gpadDevice; }

		void connectionRecheck(bool value) { _gpadConnectionRecheck = value; }
		void setConnectionRecheckTime(double seconds) { _gpadConnectionRecheckTime = seconds; }

		static void _notifyKeyChanged(int key, int scancode, int action, int mods);
		static void _notifyMouseButtonPressed(int button, int action, int mods);
		static void _notifyCursorMoved(double xpos, double ypos);
		static void _notifyMouseWheelScrolled(double xoffset, double yoffset);
		static void _notifyGamepadStateChanged(GAMEPAD_STATE newState, GAMEPAD_STATE_DIFF change);

	private:
		Input();

	private:
		double _pollRate = 0.0;
		double _nextPoll = 0.0;

		bool m_Keys[MAX_KEYS];
		bool m_MouseButtons[MAX_BUTTONS];
		double mx, my;
		int m_MouseWheelPos;

		std::shared_ptr<Gamepad> _gpadDevice = nullptr;
		bool _gpadConnectionRecheck = true;
		double _gpadConnectionRecheckTime = 3.0;
		double _nextConnectionCheck = 0.0;
	};

	/** Provides global access to Input. */
	Input& gInput();
}
