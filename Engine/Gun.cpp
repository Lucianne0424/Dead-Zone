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
#include "TestCameraScript.h"

bool Gun::_initialized = true;
shared_ptr<GameObject> Gun::_particle = nullptr;
shared_ptr<MuzzleFlashParticle> Gun::_muzzle = nullptr;

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
			Recoil(_info.recoil, 0.f); // �ݵ� ó��

			_gunRecoilTime = 0.1f; // �ݵ� �ð� �ʱ�ȭ
			_cameraRecoilTime = 0.5f; // ī�޶� �ݵ� �ð� �ʱ�ȭ

			// TODO : ���� ���

		}

		if (_currentAmmo <= 0)
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

void Gun::Recoil(float pitchAmount, float yawAmount)
{
	shared_ptr<Camera> camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();
	static_pointer_cast<TestCameraScript>(camera->GetGameObject()->GetMonoBehaviour(L"MainCamera"))->Recoil(pitchAmount, yawAmount); // ī�޶� �ݵ� ó��
}

void Gun::input()
{
	// �߻� ��ư�� ������ ��
	if (INPUT->GetButton(MOUSE_TYPE::LBUTTON))
	{
		Fire();
	}

	// ���� ��ư�� ������ ��
	if (INPUT->GetButtonDown(KEY_TYPE::R))
	{
		Reload();
	}
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