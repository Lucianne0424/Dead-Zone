#include "pch.h"
#include "Gun.h"

#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneManager.h"

#include "EnginePch.h"
#include "Input.h"
#include "MuzzleFlashParticle.h"
#include "GameObject.h"
#include "EnginePch.h"
#include "Timer.h"

bool Gun::_initialized = true;
shared_ptr<GameObject> Gun::_particle = make_shared<GameObject>();

Gun::Gun()
{
}

Gun::~Gun()
{
}

void Gun::Fire()
{
	if ((1 /_info.fireRate) <= _fireElapsedTime)
	{
		if (_currentAmmo > 0)
		{
			_currentAmmo--; // ��ź�� ����
			_fireElapsedTime = 0.f; // ��� �ð� �ʱ�ȭ
			_muzzle->SetActive(true); // ��ƼŬ Ȱ��ȭ
			Recoil(); // �ݵ� ó��

			// TODO : ���� ���

		}
		else
		{
			Reload(); // ��ź�� 0�̸� ����
			return;
		}
		
		
	}
	else
	{
		_fireElapsedTime += DELTA_TIME;
	}
	
}

void Gun::Reload()
{
	// TODO : ���� ź�� Ȯ�� �Ŀ� ����
	// TODO : ���� �����Ǿ� �ִ� ź�� ���� ź���� ���ϱ�
	_currentAmmo = 0; // ������ �Ѿ� �� �ʱ�ȭ
	
	// TODD : ���� �ð� �˻� �Ŀ� ����
	_currentAmmo = _info.ammoCapacity; // ������ �Ѿ� ���� �ִ� ���� ���� �ʱ�ȭ

	// TODO : ���� ���
}

void Gun::Recoil()
{
	shared_ptr<Camera> camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();

	Vec3 rotation = camera->GetTransform()->GetLocalRotation();
	rotation.x -= DELTA_TIME * _info.recoil;
	camera->GetTransform()->SetLocalRotation(rotation);
}

void Gun::InitializeParticle()
{
	// ��ƼŬ ����
	_particle = make_shared<GameObject>();
	_particle->AddComponent(make_shared<Transform>());
	_muzzle = make_shared<MuzzleFlashParticle>();
	_particle->AddComponent(_muzzle);
	_particle->GetTransform()->SetLocalPosition(Vec3(0.f,0.f,0.f));
	_particle->SetCheckFrustum(false);
	GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(_particle);
	_initialized = false;
}
