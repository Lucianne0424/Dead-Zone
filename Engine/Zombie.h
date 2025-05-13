#pragma once
#include "MonoBehaviour.h"
#include "GameInfo.h"

enum class ZOMBIE_STATE
{
	T_POSE,
	IDLE,
	WALK,
	RUN,
	ATTACK,
	DIE,

	END
};

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
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	virtual void SetState(ZOMBIE_STATE);

public:
	void SetRandomDirection();
	void SetPauseDuration();
	void Move();

private:
	ZOMBIE_STATE _state;
	ZombieInfo _info;
	shared_ptr<class GameObject> _particle;
	shared_ptr<class BloodParticle> _blood;

private:
	bool _initialized = false;
	float _changeDirectionTime;
	float _elapsedTime;
	bool _moving;
	float _pauseDuration{};

	bool _start;
};

