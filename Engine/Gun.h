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

	int32 GetCurrentAmmo() { return _currentAmmo; }

private:
	static bool _initialized; // 총기 초기화가 되었는지 확인
	static shared_ptr<class MuzzleFlashParticle> _muzzle;


	float _fireElapsedTime = 0.f; // 발사 경과 시간
	float _recoilTime = 0.f; // 반동 시간
	int32 _currentAmmo = 0; // 현재 장전된 총알 수

protected:
	GunInfo _info;
	static shared_ptr<GameObject> _particle;
	
	
};