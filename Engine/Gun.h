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
	void Aiming(float aimFov, Vec3 aimPos);

	float IsAiming() { return _isAiming; }

	void input(); // �ӽ�, ���߿� ��ġ ���� �� ����
	
	// ��ƼŬ ���� �Լ�
	void InitializeParticle();
	void setParticlePos(Vec3 pos);

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
	float _gunRecoilTime; // �ѱ� �ݵ� �ð�
	float _cameraRecoilTime; // ī�޶� �ݵ� �ð�


	// ������
	bool _isAiming = false;

	//TODO
	//float _aimElapsed = 0.f;
	//float _startFov = 90.f;
	//Vec3 _startPos = Vec3(0.f, 0.f, 0.f);
};