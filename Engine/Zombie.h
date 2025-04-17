#pragma once
#include "MonoBehaviour.h"
#include "GameInfo.h"

enum class ZOMBIE_ANIMATION_TYPE
{
	ATTACK1,
	DIE1,
	DIE2,
	IDLE1,
	IDLE2,
	ATTACK2,
	RUN,
	SCREAM,
	WALK,
	T_POSE,
	END
};

class Zombie : public MonoBehaviour
{
public:
	Zombie();
	virtual ~Zombie();

	virtual void Awake() override;
	//virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void SetRandomDirection();

private:
	ZombieInfo _info;

private:
	Vec3 _direction;
	float _changeDirectionTime;
	float _elaspedTime;
};

