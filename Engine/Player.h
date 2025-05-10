#pragma once
#include "MonoBehaviour.h"
#include "GameInfo.h"

enum class PLAYER_STATE
{
	T_POSE,
	IDLE,
	RUN_FORWARD,
	RUN_BACKWARD,
	RUN_LEFT,
	RUN_RIGHT,
	FIRE,
};

enum class PLAYER_ANIMATION_TYPE
{
	T_POSE,
	IDLE,
	RUN_FORWARD,
	RUN_BACKWARD,
	RUN_LEFT,
	RUN_RIGHT,
	FIRE,

	END,
};

class Player : public MonoBehaviour
{
public:
	Player();
	virtual ~Player();

	virtual void LateUpdate() override;

	virtual void SetState(PLAYER_STATE state) {}

	void Awake();
	void Start();
	void Update();
	
protected:
	PLAYER_STATE _state;
	PlayerInfo _info;
};

