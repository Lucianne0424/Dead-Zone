#pragma once
#include "Component.h"

enum class ColliderType
{
	NONE,
	SPHERE,
	AABB, // 없는거
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
	void SetExtents(Vec3 extents) { _extents = extents; }
	void SetRotation(Vec3 rotation) { _rotation = rotation; }

	shared_ptr<GameObject> GetDebugCollider() { return _debugCollider; }

protected:
	Vec3						_center = {}; // 콜라이더의 로컬 중심 위치
	float						_radius = {}; // 콜라이더 반지름
	Vec3						_extents = {}; // 콜라이더 크기
	Vec3						_rotation = { 0.f, 0.f, 0.f }; // 콜라이더 회전

	Matrix						_matLocal;
	Matrix						_matWorld;

	shared_ptr<GameObject>		_debugCollider;

private:
	ColliderType _colliderType = {};
};