#include "pch.h"
#include "Gun.h"

#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneManager.h"

#include "EnginePch.h"
#include "Input.h"
#include "MuzzleFlashParticle.h"
#include "GameObject.h"
#include "EnginePch.h"
#include "Timer.h"
#include "TestCameraScript.h"

bool Gun::_initialized = true;
shared_ptr<GameObject> Gun::_particle = nullptr;
shared_ptr<MuzzleFlashParticle> Gun::_muzzle = nullptr;

Gun::Gun()
{
}

Gun::~Gun()
{
}

void Gun::Fire()
{
	if ((1 /_info.fireRate) <= _fireElapsedTime)
	{
		if (_currentAmmo > 0)
		{
			_currentAmmo--; // 장탄수 감소
			_fireElapsedTime = 0.f; // 경과 시간 초기화
			_muzzle->SetActive(true); // 파티클 활성화
			Recoil(_info.recoil, 0.f); // 반동 처리

			_gunRecoilTime = 0.1f; // 반동 시간 초기화
			_cameraRecoilTime = 0.5f; // 카메라 반동 시간 초기화

			// TODO : 사운드 출력

		}

		if (_currentAmmo <= 0)
		{
			Reload(); // 장탄수 0이면 장전
			return;
		}
		
		
	}
	else
	{
		_fireElapsedTime += DELTA_TIME;
	}
	
}

void Gun::Reload()
{
	// TODO : 남은 탄수 확인 후에 장전
	// TODO : 현재 장전되어 있는 탄을 남은 탄수에 더하기
	_currentAmmo = 0; // 장전된 총알 수 초기화
	
	// TODD : 장전 시간 검사 후에 장전
	_currentAmmo = _info.ammoCapacity; // 장전된 총알 수를 최대 장전 수로 초기화

	// TODO : 사운드 출력
}

void Gun::Recoil(float pitchAmount, float yawAmount)
{
	shared_ptr<Camera> camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	static_pointer_cast<TestCameraScript>(camera->GetGameObject()->GetMonoBehaviour(L"MainCamera"))->Recoil(pitchAmount, yawAmount); // 카메라 반동 처리
}

void Gun::input()
{
	// 발사 버튼이 눌렸을 때
	if (INPUT->GetButton(MOUSE_TYPE::LBUTTON))
	{
		Fire();
	}

	// 장전 버튼이 눌렸을 때
	if (INPUT->GetButtonDown(KEY_TYPE::R))
	{
		Reload();
	}
}

void Gun::InitializeParticle()
{
	// 파티클 생성
	_particle = make_shared<GameObject>();
	_particle->AddComponent(make_shared<Transform>());
	_muzzle = make_shared<MuzzleFlashParticle>();
	_particle->AddComponent(_muzzle);
	_particle->GetTransform()->SetLocalPosition(Vec3(0.f,0.f,0.f));
	_particle->SetCheckFrustum(false);
	GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(_particle);
	_initialized = false;
}