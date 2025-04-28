#include "pch.h"
#include "BaseCollider.h"

BaseCollider::BaseCollider(ColliderType colliderType)
	: Component(COMPONENT_TYPE::COLLIDER), _colliderType(colliderType)
{

}

BaseCollider::~BaseCollider()
{

}

void BaseCollider::SetOrientation(Vec3 orientation)
{
	_orientation = SimpleMath::Quaternion::CreateFromYawPitchRoll(
		XMConvertToRadians(orientation.x),
		XMConvertToRadians(orientation.y),
		XMConvertToRadians(orientation.z));
}