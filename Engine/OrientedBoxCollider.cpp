#include "pch.h"
#include "OrientedBoxCollider.h"
#include "Transform.h"

OrientedBoxCollider::OrientedBoxCollider() : BaseCollider(ColliderType::OrientedBox)
{
}

OrientedBoxCollider::~OrientedBoxCollider()
{
}

void OrientedBoxCollider::FinalUpdate()
{
	shared_ptr<Transform> originTransform = GetTransform();

	_boundingOrientedBox->Center = _center + originTransform->GetWorldPosition();
	_boundingOrientedBox->Extents = _extents * originTransform->GetLocalScale();
	_boundingOrientedBox->Orientation = _orientation;
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
