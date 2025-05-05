#include "pch.h"
#include "OrientedBoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Resources.h"
#include "MeshRenderer.h"

OrientedBoxCollider::OrientedBoxCollider() : BaseCollider(ColliderType::OBB)
{
	_boundingOrientedBox = make_shared<BoundingOrientedBox>(); // �ٿ�� �ڽ� ����

	// ����׿� �ݶ��̴� ����
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
	// ���� ��� �����
	Matrix matScale = Matrix::CreateScale(Vec3(1.f, 1.f, 1.f));

	SimpleMath::Quaternion q;

	float sp = sinf(_rotation.x * 0.5f);
	float cp = cosf(_rotation.x * 0.5f);

	float sy = sinf(_rotation.y * 0.5f);
	float cy = cosf(_rotation.y * 0.5f);

	float sr = sinf(_rotation.z * 0.5f);
	float cr = cosf(_rotation.z * 0.5f);

	q.w = cy * cp * cr + sy * sp * sr;
	q.x = cy * sp * cr + sy * cp * sr;
	q.y = sy * cp * cr - cy * sp * sr;
	q.z = cy * cp * sr - sy * sp * cr;

	Matrix matRotation = Matrix::CreateFromQuaternion(q);
	Matrix matTranslation = Matrix::CreateTranslation(_center);

	_matLocal = matScale * matRotation * matTranslation;

	Matrix worldMatrix = GetTransform()->GetWorldMatrix();

	// ���� ��� ���
	_matWorld = _matLocal * worldMatrix;

	Vec3 scale{};
	Vec3 rotation{};
	Vec3 translation{};
	SimpleMath::Quaternion orientation{};
	_matWorld.Decompose(scale, orientation, translation);

	_boundingOrientedBox->Center = translation;
	_boundingOrientedBox->Extents = _extents * scale;
	_boundingOrientedBox->Orientation = orientation;

	// ����׿� �ݶ��̴� ��ġ ����
	shared_ptr<Transform> debugColliderTransform = _debugCollider->GetTransform();
	if (debugColliderTransform->GetParent().lock() == nullptr)
		debugColliderTransform->SetParent(GetTransform()); // Ʈ������ �θ� ����
	
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
