#pragma once
#include "MonoBehaviour.h"
#include "GameInfo.h"

class Gun : public MonoBehaviour
{
public:
	Gun();
	virtual ~Gun();

	virtual void Awake() override {};
	//virtual void Start() override {}
	virtual void Update() override {}
	virtual void LateUpdate() override {}

	void Fire();
	void Reload();
	void Recoil();

	void InitializeParticle();

	bool GetInitialized() { return _initialized; }

private:
	bool _initialized = true; // �ѱ� �ʱ�ȭ�� �Ǿ����� Ȯ��
	shared_ptr<class MuzzleFlashParticle> _muzzle;


	float _fireElapsedTime = 0.f; // �߻� ��� �ð�
	float _recoilTime = 0.f; // �ݵ� �ð�
	int32 _currentAmmo = 0; // ���� ������ �Ѿ� ��

protected:
	GunInfo _info;
	shared_ptr<GameObject> _particle;
	
	
};