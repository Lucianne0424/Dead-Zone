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
	void Recoil(float pitchAmount, float yawAmount);

	void input(); // �ӽ�, ���߿� ��ġ ���� �� ����
	
	void InitializeParticle();

	bool GetInitialized() { return _initialized; }

	int32 GetCurrentAmmo() { return _currentAmmo; }

private:
	static bool _initialized; // �ѱ� �ʱ�ȭ�� �Ǿ����� Ȯ��
	static shared_ptr<class MuzzleFlashParticle> _muzzle;


	float _fireElapsedTime = 0.f; // �߻� ��� �ð�
	float _recoilTime = 0.f; // �ݵ� �ð�
	int32 _currentAmmo = 0; // ���� ������ �Ѿ� ��

protected:
	GunInfo _info;
	static shared_ptr<GameObject> _particle;
	float _gunRecoilTime;


	float _recoilPitch = 0.f;		// ���Ʒ� ��鸲
	float _recoilYaw = 0.f;			// �¿� ��鸲
	float _mousePitch = 0.f;		// ���콺 ��ġ (ȸ��)
	float _mouseYaw = 0.f;			// ���콺 �� (ȸ��)
	
	
};