#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ParticleSystem.h"
#include "Gun.h"
#include "M4A1.h"
#include "AK47.h"

TestCameraScript::TestCameraScript()
{
	_name = L"MainCamera";
}

TestCameraScript::~TestCameraScript()
{
}

void TestCameraScript::LateUpdate()
{
	if(INPUT->GetButton(KEY_TYPE::KEY_F5))
		SET_DEBUG_MODE(!DEBUG_MODE);

	if (INPUT->GetButton(KEY_TYPE::KEY_F4))
		INPUT->LockCursor(!INPUT->IsCursorLocked());

	Vec3 pos = GetTransform()->GetLocalPosition();

	if (INPUT->GetButton(KEY_TYPE::W))
		pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::S))
		pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::A))
		pos -= GetTransform()->GetRight() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::D))
		pos += GetTransform()->GetRight() * _speed * DELTA_TIME;

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

	if (INPUT->GetButtonDown(KEY_TYPE::F))
	{
		// 테스트용 임시로 대충 만듦
		_GunType = (_GunType + 1) % _MaxGunType;
		if (_GunType == 0)
		{
			GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"M4A1")->SetActive(true);
			GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"AK47")->SetActive(false);

			auto gun = GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"M4A1");
			gun->SetActive(true);
			Vec3 pos = static_pointer_cast<M4A1>(gun->GetMonoBehaviour(L"M4A1"))->GetNomalParticlePos();
			static_pointer_cast<M4A1>(gun->GetMonoBehaviour(L"M4A1"))->setParticlePos(pos);

		}
		else if (_GunType == 1)
		{
			GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"M4A1")->SetActive(false);
			auto gun = GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"AK47");
			gun->SetActive(true);
			Vec3 pos = static_pointer_cast<AK47>(gun->GetMonoBehaviour(L"AK47"))->GetNomalParticlePos();
			static_pointer_cast<AK47>(gun->GetMonoBehaviour(L"AK47"))->setParticlePos(pos);
		}
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