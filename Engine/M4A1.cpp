#include "pch.h"
#include "M4A1.h"
#include "EnginePch.h"

#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneManager.h"

#include "EnginePch.h"
#include "Input.h"
#include "MuzzleFlashParticle.h"
#include "GameObject.h"
#include "Gun.h"
#include "Timer.h"


Vec3 M4A1::_basePosition = { 10.f, -10.f, 20.f };
Vec3 M4A1::_baseRotation = { 270.f, 0.f, 0.f };
Vec3 M4A1::_baseScale = { 4.0f, 4.0f, 4.0f };

template<typename T>
T Lerp(const T& a, const T& b, float t)
{
	return a * (1.0f - t) + b * t;
}

M4A1::M4A1()
{
	_name = L"M4A1";
}

M4A1::~M4A1()
{
}

void M4A1::Awake()
{
	GetTransform()->SetLocalPosition(_basePosition);
	GetTransform()->SetLocalRotation(_baseRotation);
	GetTransform()->SetLocalScale(_baseScale);

	shared_ptr<Camera> camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	shared_ptr<Transform> parentTransform = camera->GetTransform();
	GetTransform()->SetParent(parentTransform);

	shared_ptr<GunInfo> info = GET_SINGLE(GameInfo)->Get<GunInfo>(L"M4A1");
	_info = *info;

	if (GetInitialized())
	{
		InitializeParticle();
	}
	_particle->GetTransform()->SetParent(GetTransform());
	_particle->GetTransform()->SetLocalPosition(Vec3(0.0f, -4.3f, 1.35f));
}

void M4A1::Update()
{
	input(); // 임시 총기와 관련된 입력 처리

	// 총 발사 시 총기 반동 처리
	if (_gunRecoilTime > 0)
	{
		_gunRecoilTime -= DELTA_TIME;
		float recoilOffset = sin(_gunRecoilTime * 20.f) * 3;

		// 총 모델을 위로 살짝 움직이거나 기울이기
		GetTransform()->SetLocalRotation(_baseRotation + Vec3(-recoilOffset, 0, 0));
	}
	else
	{
		// 복구
		GetTransform()->SetLocalRotation(_baseRotation);
	}

	// 총 발사 시 카메라 반동 처리
	POINT delta = INPUT->GetDeltaPos();

	float sensitivity = 0.1f; // 카메라 감도
	_mousePitch += delta.y * sensitivity;
	_mouseYaw += delta.x * sensitivity;

	// 반동 감쇠
	_recoilPitch = Lerp(_recoilPitch, 0.f, 2.0f * DELTA_TIME);
	_recoilYaw = Lerp(_recoilYaw, 0.f, 2.0f * DELTA_TIME);

	// 최종 회전 = 마우스 회전 + 반동
	float finalPitch = _mousePitch - _recoilPitch;
	float finalYaw = _mouseYaw - _recoilYaw;

	// 메인 카메라 회전 적용
	shared_ptr<Camera> camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	camera->GetTransform()->SetLocalRotation(Vec3(finalPitch, finalYaw, 0));
	
	

}

void M4A1::LateUpdate()
{
}