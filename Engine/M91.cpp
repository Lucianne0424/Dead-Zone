#include "pch.h"
#include "M91.h"

#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneManager.h"

Vec3 M91::_basePosition = { 50.f, -30.f, 200.f };
Vec3 M91::_baseRotation = { 0.f, 175.f, 0.f };
Vec3 M91::_baseScale = { 3.f, 3.f, 3.f };

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
}

void M91::LateUpdate()
{
}
