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
	_debugCollider->GetTransform()->SetLocalPosition(_boundingSphere->Center);
	Matrix m = Matrix::CreateFromQuaternion(orientation);
	{
		Vec3 rotation = Transform::QuaternionToEuler(orientation);
		rotation = RadianToDegree(rotation);
		_debugCollider->GetTransform()->SetLocalRotation(rotation);
	}
	_debugCollider->GetTransform()->SetLocalScale(scale);

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