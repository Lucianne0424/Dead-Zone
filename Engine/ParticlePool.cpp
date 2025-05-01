#include "pch.h"
#include "ParticlePool.h"
#include "GameObject.h"
#include "Transform.h"
#include "MuzzleFlashParticle.h"
#include "EnginePch.h"
#include "SceneManager.h"
#include "Scene.h"



void ParticlePool::Init(uint32 maxCount)
{
	_pool.reserve(maxCount);

	for (uint32 i = 0; i < maxCount; ++i)
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->AddComponent(make_shared<Transform>());

		shared_ptr<MuzzleFlashParticle> particle = make_shared<MuzzleFlashParticle>();
		obj->AddComponent(particle);

		obj->SetCheckFrustum(false);
		obj->SetActive(false); // �ʱ⿡�� ��Ȱ��ȭ

		//GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(obj);

		_pool.push_back(obj);
	}
}

shared_ptr<GameObject> ParticlePool::Get()
{
	for (uint32 i = 0; i < _pool.size(); ++i)
	{
		uint32 index = (_curIndex + i) % _pool.size();
		if (!_pool[index]->IsActive())
		{
			_pool[index]->SetActive(true);
			_curIndex = (index + 1) % _pool.size();
			return _pool[index];
		}
	}

	// Ǯ�� ���� ��ü�� ������ nullptr ��ȯ
	return nullptr;
}

void ParticlePool::Return(shared_ptr<GameObject> obj)
{
	obj->SetActive(false);
}

shared_ptr<GameObject> ParticlePool::GetFromPool()
{
	for (const auto& obj : _pool)
	{
		if (!obj->IsActive())
		{
			obj->SetActive(true); // ���� �� Ȱ��ȭ
			return obj;
		}
	}

	return nullptr; // �� �� �ִ� ��ü ����
}