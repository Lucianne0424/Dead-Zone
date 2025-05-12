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

void Gun::Aiming(float aimFov, Vec3 aimPos)
{
	/*
	//TODO
	const float aimSpeed = _info.aimSpeed;

	if (_isAiming)
		_aimElapsed = min(_aimElapsed + DELTA_TIME, aimSpeed);
	else
		_aimElapsed = max(_aimElapsed - DELTA_TIME, 0.f);

	float t = _aimElapsed / aimSpeed;
	
	*/

	// 카메라 FOV 보간
	shared_ptr<Camera> camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	float currentFov = camera->GetFOV();
	float newFov = Lerp(currentFov, aimFov, DELTA_TIME * 5.f);
	camera->SetFOV(newFov);

	// 총기 위치 보간
	Vec3 curPos = GetTransform()->GetLocalPosition();
	Vec3 newPos = Lerp(curPos, aimPos, DELTA_TIME * 5.f);
	GetTransform()->SetLocalPosition(newPos);
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

	if (INPUT->GetButton(MOUSE_TYPE::RBUTTON))
	{
		_isAiming = true;
		GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"Crosshair")->SetActive(false); // 조준선 비활성화
	}
	else
	{
		_isAiming = false;
		GET_SINGLE(SceneManager)->GetActiveScene()->FindGameObject(L"Crosshair")->SetActive(true); // 조준선 활성화
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
	uint8 gunLayer = GET_SINGLE(SceneManager)->LayerNameToIndex(L"Gun");
	_particle->SetLayerIndex(gunLayer);
	_particle->SetCheckFrustum(false);
	GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(_particle);
	_initialized = false;
}