#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "Material.h"
#include "Shader.h"
#include "ParticleSystem.h"
#include "InstancingManager.h"

#include "BaseCollider.h"
#include "SphereCollider.h"
#include "OrientedBoxCollider.h"

Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

Camera::Camera() : Component(COMPONENT_TYPE::CAMERA)
{
	_width = static_cast<float>(GEngine->GetWindow().width);
	_height = static_cast<float>(GEngine->GetWindow().height);
}

Camera::~Camera()
{
}

void Camera::FinalUpdate()
{
	_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	if (_type == PROJECTION_TYPE::PERSPECTIVE)
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, _width / _height, _near, _far);
	else
		_matProjection = ::XMMatrixOrthographicLH(_width * _scale, _height * _scale, _near, _far);

	Camera::S_MatView = _matView;
	Camera::S_MatProjection = _matProjection;

	//_frustum.FinalUpdate();
	BoundingFrustum frustum;
	BoundingFrustum::CreateFromMatrix(frustum, Camera::S_MatProjection);
	Matrix viewInv = Camera::S_MatView.Invert();
	frustum.Transform(frustum, viewInv);
	_frustum = frustum;
}

void Camera::SortGameObject()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_vecForward.clear();
	_vecDeferred.clear();
	_vecParticle.clear();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr)
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())
		{
			if (FrustumCulling(gameObject) == DISJOINT)
			{
				continue;
			}
		}

		if(gameObject->GetMeshRenderer())
		{
			SHADER_TYPE shaderType = gameObject->GetMeshRenderer()->GetMaterial()->GetShader()->GetShaderType();
			switch (shaderType)
			{
			case SHADER_TYPE::DEFERRED:
				_vecDeferred.push_back(gameObject);
				{
					shared_ptr<BaseCollider> collider = gameObject->GetCollider();
					if (collider)
					{
						_vecDeferred.push_back(collider->GetDebugCollider());
					}
				}
				break;
			case SHADER_TYPE::FORWARD:
				_vecForward.push_back(gameObject);
				break;
			}
		}
		else
		{
			_vecParticle.push_back(gameObject);
		}
	}
}

void Camera::SortShadowObject()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_vecShadow.clear();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		if (gameObject->IsStatic())
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())
		{
			if (FrustumCulling(gameObject) == DISJOINT)
			{
				continue;
			}
		}

		_vecShadow.push_back(gameObject);
	}
}

void Camera::Render_Deferred()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	GET_SINGLE(InstancingManager)->Render(_vecDeferred);
}

void Camera::Render_Forward()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	GET_SINGLE(InstancingManager)->Render(_vecForward);

	for (auto& gameObject : _vecParticle)
	{
		gameObject->GetParticleSystem()->Render();
	}
}

void Camera::Render_Shadow()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	for (auto& gameObject : _vecShadow)
	{
		gameObject->GetMeshRenderer()->RenderShadow();
	}
}

ContainmentType Camera::FrustumCulling(shared_ptr<GameObject> gameObject)
{
	if (gameObject->GetCollider() == nullptr)
		return ContainmentType::CONTAINS;

	ColliderType colliderType = gameObject->GetCollider()->GetColliderType();

	if (colliderType == ColliderType::SPHERE)
	{
		shared_ptr<SphereCollider> sphere = static_pointer_cast<SphereCollider>(gameObject->GetCollider());
		return _frustum.Contains(*sphere->GetBoundingSphere());
	}
	else if (colliderType == ColliderType::OBB)
	{
		shared_ptr<OrientedBoxCollider> obb = static_pointer_cast<OrientedBoxCollider>(gameObject->GetCollider());
		return _frustum.Contains(*obb->GetBoundingOrientedBox());
	}
	else
	{
		return ContainmentType::CONTAINS;
	}
}
