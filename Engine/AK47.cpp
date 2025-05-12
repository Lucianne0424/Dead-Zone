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


//fov 60�� ���� ( ���� GunCamera�� fov 60�� )
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

	// ��ƼŬ ��ġ ����
	setParticlePos(_ParticlePosition);

	//TODO
	/*_startFov = camera->GetNormalFOV();
	_startPos = _basePosition;*/
}

void AK47::Update()
{
	input(); // �ӽ� �ѱ�� ���õ� �Է� ó��

	// �� �߻� �� �ѱ� �ݵ� ó��
	if (_gunRecoilTime > 0)
	{
		_gunRecoilTime -= DELTA_TIME;
		float recoilOffset = sin(_gunRecoilTime * 20.f) * 3;

		// �� ���� ���� ��¦ �����̰ų� ����̱�
		GetTransform()->SetLocalRotation(_baseRotation + Vec3(-recoilOffset, 0, 0));
	}
	else
	{
		// ����
		GetTransform()->SetLocalRotation(_baseRotation);
	}

	// ������ ó��
	// fov ����
	float fov = IsAiming() ? _info.fov : GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera()->GetNormalFOV();
	Vec3 pos = IsAiming() ? _aimingPosition : _basePosition;
	Aiming(fov, pos);

}

void AK47::LateUpdate()
{
}