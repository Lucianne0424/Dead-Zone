#pragma once
#include "MonoBehaviour.h"
#include "GameInfo.h"

class Zombie : public MonoBehaviour
{
public:
	Zombie();
	virtual ~Zombie();

	virtual void Awake() override;
	//virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	ZombieInfo _info;
};

