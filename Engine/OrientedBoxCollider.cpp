#include "pch.h"
#include "OrientedBoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Resources.h"
#include "MeshRenderer.h"

OrientedBoxCollider::OrientedBoxCollider() : BaseCollider(ColliderType::OBB)
{
	_boundingOrientedBox = make_shared<BoundingOrientedBox>(); // 바운딩 박스 생성

	// 디버그용 콜라이더 생성
	_debugCollider = make_shared<GameObject>();
	_debugCollider->AddComponent(make_shared<Transform>());
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> cubeMesh = GET_SINGLE(Resources)->LoadCubeMesh();
		meshRenderer->SetMesh(cubeMesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"WireFrame");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		meshRenderer->SetMaterial(material);
	}
	_debugCollider->AddComponent(meshRenderer);

	_debugCollider->Awake();
	_debugCollider->Start();
}

OrientedBoxCollider::~OrientedBoxCollider()
{
}

void OrientedBoxCollider::FinalUpdate()
{
	Matrix worldMatrix = GetTransform()->GetWorldMatrix();
	Vec3 scale{};
	Vec3 rotation{};
	Vec3 translation{};
	SimpleMath::Quaternion orientation{};
	worldMatrix.Decompose(scale, orientation, translation);

	_boundingOrientedBox->Center = _center + translation;
	_boundingOrientedBox->Extents = _extents * scale;
	_boundingOrientedBox->Orientation = orientation;

	// 디버그용 콜라이더 위치 설정
	shared_ptr<Transform> debugColliderTransform = _debugCollider->GetTransform();
	if (debugColliderTransform->GetParent().lock() == nullptr)
		debugColliderTransform->SetParent(GetTransform()); // 트랜스폼 부모 설정
	
	debugColliderTransform->SetLocalPosition(_center);
	debugColliderTransform->SetLocalScale(Vec3(_boundingOrientedBox->Extents) * 2.f);

	_debugCollider->Update();
	_debugCollider->LateUpdate();
	_debugCollider->FinalUpdate();
}

bool OrientedBoxCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingOrientedBox->Intersects(rayOrigin, rayDir, OUT distance);
}

bool OrientedBoxCollider::Intersects(shared_ptr<BoundingSphere> boundingSphere)
{
	return _boundingOrientedBox->Intersects(*boundingSphere);
}

bool OrientedBoxCollider::Intersects(shared_ptr<BoundingBox> boundingBox)
{
	return _boundingOrientedBox->Intersects(*boundingBox);
}

bool OrientedBoxCollider::Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox)
{
	return _boundingOrientedBox->Intersects(*boundingOrientedBox);
}
