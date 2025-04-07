#pragma once
#include "GameObject.h"

struct GunInfo
{
	std::string name;
	std::string type;
	int damage;
	float range;
	float fireRate;
	int ammoCapacity;
};

class Gun : public GameObject
{
public:
	Gun();
	virtual ~Gun();

private:
	GunInfo info;
};