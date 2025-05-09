#pragma once
#include <dinput.h>

enum class MOUSE_TYPE
{
	LBUTTON = 0,
	MBUTTON = 1,
	RBUTTON = 2,
};

enum class KEY_TYPE
{
	UP = DIK_UP,
	DOWN = DIK_DOWN,
	LEFT = DIK_LEFT,
	RIGHT = DIK_RIGHT,

	W = DIK_W,
	A = DIK_A,
	S = DIK_S,
	D = DIK_D,

	Q = DIK_Q,
	E = DIK_E,
	Z = DIK_Z,
	C = DIK_C,

	R = DIK_R,

	KEY_1 = DIK_1,
	KEY_2 = DIK_2,
	KEY_3 = DIK_3,
	KEY_4 = DIK_4,

	KEY_F5 = DIK_F5,

	SPACE = DIK_SPACE,
};

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	END
};

enum
{
	MOUSE_TYPE_COUNT = 3,
	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<int32>(KEY_STATE::END),
};

class Input
{
	DECLARE_SINGLE(Input);

public:
	void Init(HWND hwnd);
	void Shutdown();
	void Update();

	// 누르고 있을 때
	bool GetButton(MOUSE_TYPE key) { return GetMouseState(key) == KEY_STATE::PRESS; }
	bool GetButton(KEY_TYPE key) { return GetKeyState(key) == KEY_STATE::PRESS; }
	// 맨 처음 눌렀을 때
	bool GetButtonDown(MOUSE_TYPE key) { return GetMouseState(key) == KEY_STATE::DOWN; }
	bool GetButtonDown(KEY_TYPE key) { return GetKeyState(key) == KEY_STATE::DOWN; }
	// 맨 처음 눌렀다 뗐을 때
	bool GetButtonUp(MOUSE_TYPE key) { return GetMouseState(key) == KEY_STATE::UP; }
	bool GetButtonUp(KEY_TYPE key) { return GetKeyState(key) == KEY_STATE::UP; }

	const POINT& GetMousePos() { return _mousePos; }

private:
	inline KEY_STATE GetMouseState(MOUSE_TYPE key) { return _mouseStates[static_cast<uint8>(key)]; }
	inline KEY_STATE GetKeyState(KEY_TYPE key) { return _keyStates[static_cast<uint8>(key)]; }

private:
	HWND _hwnd;
	HINSTANCE _hInst;
	POINT _mousePos = {};

	DIMOUSESTATE			_DIMouseState;
	LPDIRECTINPUT8			_directInput;

	LPDIRECTINPUTDEVICE8	_mouse = nullptr;
	LPDIRECTINPUTDEVICE8	_keyboard = nullptr;

	vector<KEY_STATE>		_mouseStates;
	vector<KEY_STATE>		_keyStates;
};

