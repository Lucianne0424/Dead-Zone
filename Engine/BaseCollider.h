#pragma once
#include "Component.h"

enum class ColliderType
{
	Sphere,
	Box,
	OrientedBox,
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
	void SetOrientation(Vec3 orientation);

protected:
	Vec3						_center = {}; // 콜라이더의 로컬 중심 위치
	float						_radius = {}; // 콜라이더 반지름
	Vec3						_extents = {}; // 콜라이더 크기
	SimpleMath::Quaternion		_orientation = {}; // 콜라이더 회전

private:
	ColliderType _colliderType = {};
};