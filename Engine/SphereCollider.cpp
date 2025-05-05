#include "pch.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Resources.h"
#include "MeshRenderer.h"

SphereCollider::SphereCollider() : BaseCollider(ColliderType::SPHERE)
{
	_boundingSphere = make_shared<BoundingSphere>(); // 바운딩 박스 생성

	// 디버그용 콜라이더 생성
	_debugCollider = make_shared<GameObject>();
	_debugCollider->AddComponent(make_shared<Transform>());
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
		meshRenderer->SetMesh(sphereMesh);
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

SphereCollider::~SphereCollider()
{

}

void SphereCollider::FinalUpdate()
{
	Matrix worldMatrix = GetTransform()->GetWorldMatrix();
	Vec3 scale{};
	Vec3 rotation{};
	Vec3 translation{};
	SimpleMath::Quaternion orientation{};
	worldMatrix.Decompose(scale, orientation, translation);

	_boundingSphere->Center = _center + translation;
	_boundingSphere->Radius = _radius * max(max(scale.x, scale.y), scale.z);

	// 디버그용 콜라이더 위치 설정
	shared_ptr<Transform> debugColliderTransform = _debugCollider->GetTransform();
	if (debugColliderTransform->GetParent().lock() == nullptr)
		_debugCollider->GetTransform()->SetParent(GetTransform()); // 트랜스폼 부모 설정
	
	debugColliderTransform->SetLocalPosition(_center);
	debugColliderTransform->SetLocalScale(Vec3(_boundingSphere->Radius) * 2.f);

	_debugCollider->Update();
	_debugCollider->LateUpdate();
	_debugCollider->FinalUpdate();
}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
	return _boundingSphere->Intersects(rayOrigin, rayDir, OUT distance);
}

bool SphereCollider::Intersects(shared_ptr<BoundingSphere> boundingSphere)
{
	return _boundingSphere->Intersects(*boundingSphere);
}

bool SphereCollider::Intersects(shared_ptr<BoundingBox> boundingBox)
{
	return _boundingSphere->Intersects(*boundingBox);
}

bool SphereCollider::Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox)
{
	return _boundingSphere->Intersects(*boundingOrientedBox);
}