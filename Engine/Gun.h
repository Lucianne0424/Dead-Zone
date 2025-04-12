#pragma once
#include "MonoBehaviour.h"

struct GunInfo
{
	std::string name;
	std::string type;
	int damage;
	float range;
	float fireRate;
	int ammoCapacity;
};

class Gun : public MonoBehaviour
{
public:
	Gun();
	virtual ~Gun();

	virtual void Awake() override;
	//virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

	void Fire();
	void Reload();

private:
	GunInfo info;
};