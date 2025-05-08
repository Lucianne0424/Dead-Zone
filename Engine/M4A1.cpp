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


Vec3 M4A1::_basePosition = { 5.f, -3.f, 20.f };
Vec3 M4A1::_baseRotation = { 0.f, 175.f, 0.f };
Vec3 M4A1::_baseScale = { 0.3f, 0.3f, 0.3f };

M4A1::M4A1()
{
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

	//if (GetInitialized())
	//{
	//	InitializeParticle();
	//}
	//_particle->GetTransform()->SetParent(GetTransform());
	//_particle->GetTransform()->SetLocalPosition(Vec3(-10.f, 5.f, -100.f));
}

void M4A1::Update()
{
	if (INPUT->GetButton(MOUSE_TYPE::LBUTTON))
	{
		Fire();
	}
}

void M4A1::LateUpdate()
{
}
