#pragma once
#include "Component.h"

enum class ColliderType
{
	SPHERE,
	AABB,
	OBB,
};

class BaseCollider : public Component
{
public:
	BaseCollider(ColliderType colliderType);
	virtual ~BaseCollider();

	ColliderType GetColliderType() { return _colliderType; }

	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance) = 0;
	virtual bool Intersects(shared_ptr<BoundingSphere> boundingSphere) = 0;
	virtual bool Intersects(shared_ptr<BoundingBox> boundingBox) = 0;
	virtual bool Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox) = 0;

	void SetRadius(float radius) { _radius = radius; }
	void SetCenter(Vec3 center) { _center = center; }
	void SetExtent(Vec3 extents) { _extents = extents; }
	void SetOrientation(Vec3 rotation);

	shared_ptr<GameObject> GetDebugCollider() { return _debugCollider; }

protected:
	Vec3						_center = {}; // �ݶ��̴��� ���� �߽� ��ġ
	float						_radius = {}; // �ݶ��̴� ������
	Vec3						_extents = {}; // �ݶ��̴� ũ��
	SimpleMath::Quaternion		_orientation = {}; // �ݶ��̴� ȸ��

	shared_ptr<GameObject>		_debugCollider;

private:
	ColliderType _colliderType = {};
};