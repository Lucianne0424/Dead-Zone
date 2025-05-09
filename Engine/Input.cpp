#include "pch.h"
#include "Input.h"
#include "Engine.h"

void Input::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_hInst = (HINSTANCE)::GetWindowLongPtr(_hwnd, GWLP_HINSTANCE);
	_mouseStates.resize(3, KEY_STATE::NONE);
	_keyStates.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
	_centerScreenPos = { GEngine->GetWindow().width / 2, GEngine->GetWindow().height / 2 };

	LockCursor(false);

	// DirectInput 초기화
	HRESULT hr = S_OK;
	if (FAILED(hr = DirectInput8Create(_hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(&_directInput),
		nullptr)))
	{
		return;
	}

	// 키보드 Direct Input 설정
	if (FAILED(hr = _directInput->CreateDevice(GUID_SysKeyboard,
		&_keyboard,
		nullptr)))
	{
		return;
	}

	if (FAILED(hr = _keyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return;
	}

	if (FAILED(hr = _keyboard->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		return;
	}

	if (FAILED(hr = _keyboard->Acquire()))
	{
		return;
	}

	// 마우스 Direct Input 설정
	if (FAILED(hr = _directInput->CreateDevice(GUID_SysMouse,
		&_mouse,
		nullptr)))
	{
		return;
	}

	if (FAILED(hr = _mouse->SetDataFormat(&c_dfDIMouse)))
	{
		return;
	}

	if (FAILED(hr = _mouse->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		return;
	}

	if (FAILED(hr = _mouse->Acquire()))
	{
		return;
	}
}

void Input::Shutdown()
{
	// 마우스 반환
	if (_mouse)
	{
		_mouse->Unacquire();
		_mouse->Release();
		_mouse = nullptr;
	}

	// 키보드 반환
	if (_keyboard)
	{
		_keyboard->Unacquire();
		_keyboard->Release();
		_keyboard = nullptr;
	}

	// DirectInput 반환
	if (_directInput)
	{
		_directInput->Release();
		_directInput = nullptr;
	}
}

void Input::Update()
{
	if (GetForegroundWindow() != _hwnd)
	{
		return;
	}

	HRESULT hr;
	if (_mouse == nullptr || _keyboard == nullptr) return;

	if (FAILED(hr = _mouse->GetDeviceState(sizeof(DIMOUSESTATE), &_DIMouseState)))
	{
		while (_mouse->Acquire() == DIERR_INPUTLOST);
	}

	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (FAILED(hr = _keyboard->GetDeviceState(KEY_TYPE_COUNT, &asciiKeys)))
	{
		while (_keyboard->Acquire() == DIERR_INPUTLOST);
	}

	BYTE mouseButtons[3];
	for (uint32 i = 0; i < 3; i++)
	{
		mouseButtons[i] = _DIMouseState.rgbButtons[i];
	}
	for (uint32 i = 0; i < 3; i++) {
		// 키가 눌려 있으면 true
		if (mouseButtons[i] & 0x80)
		{
			KEY_STATE& state = _mouseStates[i];

			// 이전 프레임에 키를 누른 상태라면 PRESS
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else
		{
			KEY_STATE& state = _mouseStates[i];

			// 이전 프레임에 키를 누른 상태라면 UP
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}

	// 클라이언트 기준으로 좌표 가져옴
	_deltaPos.x = _DIMouseState.lX;
	_deltaPos.y = _DIMouseState.lY;

	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{
		// 키가 눌려 있으면 true
		if (asciiKeys[key] & 0x80)
		{
			KEY_STATE& state = _keyStates[key];

			// 이전 프레임에 키를 누른 상태라면 PRESS
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else
		{
			KEY_STATE& state = _keyStates[key];

			// 이전 프레임에 키를 누른 상태라면 UP
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}

	_mousePos.x += _deltaPos.x;
	_mousePos.y += _deltaPos.y;

	GetCursorPos(&_mousePos);
	ScreenToClient(_hwnd, &_mousePos);

	if (_lockCursor)
	{
		// 마우스 위치를 화면 중앙으로 이동
		_mousePos = _centerScreenPos;
		ClientToScreen(_hwnd, &_mousePos);
		SetCursorPos(_mousePos.x, _mousePos.y);
	}
}

void Input::LockCursor(bool flag)
{
	_lockCursor = flag;

	if (_lockCursor)
		while (ShowCursor(FALSE) >= 0);
	else
		while (ShowCursor(TRUE) < 0);
}

