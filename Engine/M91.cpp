#include "pch.h"
#include "M91.h"

#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneManager.h"

#include "EnginePch.h"
#include "Input.h"
#include "MuzzleFlashParticle.h"
#include "GameObject.h"


Vec3 M91::_basePosition = { 5.f, -3.f, 20.f };
Vec3 M91::_baseRotation = { 0.f, 175.f, 0.f };
Vec3 M91::_baseScale = { 0.3f, 0.3f, 0.3f };

M91::M91()
{
}

M91::~M91()
{
}

void M91::Awake()
{
	GetTransform()->SetLocalPosition(_basePosition);
	GetTransform()->SetLocalRotation(_baseRotation);
	GetTransform()->SetLocalScale(_baseScale);

	shared_ptr<Camera> camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	shared_ptr<Transform> parentTransform = camera->GetTransform();
	GetTransform()->SetParent(parentTransform);
}

void M91::Update()
{
	// Å×½ºÆ®
	if (INPUT->GetButtonDown(KEY_TYPE::Q))
	{
		int32 index = static_cast<int32>(KEY_TYPE::Q);

		shared_ptr<GameObject> particle = make_shared<GameObject>();
		particle->AddComponent(make_shared<Transform>());

		shared_ptr<class MuzzleFlashParticle> _muzzle = make_shared<MuzzleFlashParticle>();
		_muzzle->EmitOnce();

		particle->AddComponent(_muzzle);
		particle->SetCheckFrustum(false);

		particle->GetTransform()->SetParent(GetTransform());
		particle->GetTransform()->SetLocalPosition(Vec3(0.f,0.f,-100.f));
		//particle->GetTransform()->SetLocalRotation(Vec3(0.f,0.f,-180.f));

		GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(particle);
	}
}

void M91::LateUpdate()
{
}
