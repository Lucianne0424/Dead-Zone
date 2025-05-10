#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"


template<typename T>
T Lerp(const T& a, const T& b, float t)
{
	return a * (1.0f - t) + b * t;
}


TestCameraScript::TestCameraScript()
{
	_name = L"MainCamera";
}

TestCameraScript::~TestCameraScript()
{
}

void TestCameraScript::LateUpdate()
{
	if (INPUT->GetButton(KEY_TYPE::KEY_F5))
		SET_DEBUG_MODE(!DEBUG_MODE);

	if (INPUT->GetButton(KEY_TYPE::KEY_F4))
		INPUT->LockCursor(!INPUT->IsCursorLocked());

	Vec3 pos = GetTransform()->GetLocalPosition();

	if (INPUT->GetButton(KEY_TYPE::W))
	{
		Vec3 look = GetTransform()->GetLook();
		look = Vec3(look.x, 0.f, look.z);
		pos += look * _speed * DELTA_TIME;
	}

	if (INPUT->GetButton(KEY_TYPE::S))
	{
		Vec3 look = GetTransform()->GetLook();
		look = Vec3(look.x, 0.f, look.z);
		pos -= look * _speed * DELTA_TIME;
	}

	if (INPUT->GetButton(KEY_TYPE::A))
	{
		Vec3 right = GetTransform()->GetRight();
		right = Vec3(right.x, 0.f, right.z);
		pos -= right * _speed * DELTA_TIME;
	}

	if (INPUT->GetButton(KEY_TYPE::D))
	{
		Vec3 right = GetTransform()->GetRight();
		right = Vec3(right.x, 0.f, right.z);
		pos += right * _speed * DELTA_TIME;
	}

	if (INPUT->GetButton(KEY_TYPE::Q))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += DELTA_TIME * 15.0f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::E))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x -= DELTA_TIME * 15.0f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::Z))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y += DELTA_TIME * 15.0f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::C))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y -= DELTA_TIME * 15.0f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButtonDown(MOUSE_TYPE::RBUTTON))
	{
		const POINT& pos = INPUT->GetMousePos();
		GET_SINGLE(SceneManager)->Pick(pos.x, pos.y);
	}

	POINT deltaPos = INPUT->GetDeltaPos();

	_mouseYaw += deltaPos.x * DELTA_TIME * _sensitivity;
	_mousePitch += deltaPos.y * DELTA_TIME * _sensitivity;

	// 피치 제한 (위아래)
	// _mousePitch = std::clamp(_mousePitch, -89.f, 89.f);

	// 반동 감쇠
	_recoilPitch = Lerp(_recoilPitch, 0.f, 2.f * DELTA_TIME);
	_recoilYaw = Lerp(_recoilYaw, 0.f, 2.f * DELTA_TIME);

	// 최종 회전 = 마우스 + 반동
	float finalPitch = _mousePitch - _recoilPitch;
	float finalYaw = _mouseYaw - _recoilYaw;

	GetTransform()->SetLocalRotation(Vec3(finalPitch, finalYaw, 0.f));

	GetTransform()->SetLocalPosition(pos);
}

void TestCameraScript::Recoil(float pitchAmount, float yawAmount)
{
	// 카메라 반동 설정
	_recoilPitch += pitchAmount;	// 수직
	_recoilYaw += yawAmount;		// 수평
}