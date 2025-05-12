#include "pch.h"
#include "AK47.h"
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


//fov 60도 기준 ( 현재 GunCamera가 fov 60도 )
Vec3 AK47::_basePosition = { 10.f, -8.f, 18.f };
Vec3 AK47::_baseRotation = { 270.f, 180.f, 0.f };
Vec3 AK47::_baseScale = { 0.1f, 0.1f, 0.1f };
Vec3 AK47::_aimingPosition = { 0.f, -4.5f, 12.f };
Vec3 AK47::_ParticlePosition = { 0.0f, 180.0f, 33.0f };


AK47::AK47()
{
	_name = L"AK47";
}

AK47::~AK47()
{
}

void AK47::Awake()
{
	GetTransform()->SetLocalPosition(_basePosition);
	GetTransform()->SetLocalRotation(_baseRotation);
	GetTransform()->SetLocalScale(_baseScale);

	shared_ptr<Camera> camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetGunCamera();
	shared_ptr<Transform> parentTransform = camera->GetTransform();
	GetTransform()->SetParent(parentTransform);

	shared_ptr<GunInfo> info = GET_SINGLE(GameInfo)->Get<GunInfo>(L"AK47");
	_info = *info;

	if (GetInitialized())
	{
		InitializeParticle();
	}

	// 파티클 위치 설정
	setParticlePos(_ParticlePosition);

	//TODO
	/*_startFov = camera->GetNormalFOV();
	_startPos = _basePosition;*/
}

void AK47::Update()
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

	// 정조준 처리
	// fov 설정
	float fov = IsAiming() ? _info.fov : GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetNormalFOV();
	Vec3 pos = IsAiming() ? _aimingPosition : _basePosition;
	Aiming(fov, pos);

}

void AK47::LateUpdate()
{
}