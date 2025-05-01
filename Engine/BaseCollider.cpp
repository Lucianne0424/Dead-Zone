#include "pch.h"
#include "BaseCollider.h"

BaseCollider::BaseCollider(ColliderType colliderType)
	: Component(COMPONENT_TYPE::COLLIDER), _colliderType(colliderType)
{

}

BaseCollider::~BaseCollider()
{

}

void BaseCollider::SetOrientation(Vec3 rotation)
{
	_orientation = SimpleMath::Quaternion::CreateFromYawPitchRoll(
		XMConvertToRadians(rotation.x),
		XMConvertToRadians(rotation.y),
		XMConvertToRadians(rotation.z));
}