#include "pch.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"

SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere)
{

}

SphereCollider::~SphereCollider()
{

}

void SphereCollider::FinalUpdate()
{
	shared_ptr<Transform> originTransform = GetTransform();

	_boundingSphere->Center = _center + originTransform->GetWorldPosition();

	Vec3 scale = originTransform->GetLocalScale();
	_boundingSphere->Radius = _radius * max(max(scale.x, scale.y), scale.z);
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