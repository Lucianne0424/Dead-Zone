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

	// �� �߻� �� ī�޶� �ݵ� ó��
	POINT delta = INPUT->GetDeltaPos();

	float sensitivity = 0.1f; // ī�޶� ����
	_mousePitch += delta.y * sensitivity;
	_mouseYaw += delta.x * sensitivity;

	// �ݵ� ����
	_recoilPitch = Lerp(_recoilPitch, 0.f, 2.0f * DELTA_TIME);
	_recoilYaw = Lerp(_recoilYaw, 0.f, 2.0f * DELTA_TIME);

	// ���� ȸ�� = ���콺 ȸ�� + �ݵ�
	float finalPitch = _mousePitch - _recoilPitch;
	float finalYaw = _mouseYaw - _recoilYaw;

	// ���� ī�޶� ȸ�� ����
	shared_ptr<Camera> camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	camera->GetTransform()->SetLocalRotation(Vec3(finalPitch, finalYaw, 0));
	
	

}

void M4A1::LateUpdate()
{
}